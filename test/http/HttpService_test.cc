// MIT License
//
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <boost/test/unit_test.hpp>

#include <coffee/app/ApplicationServiceStarter.hpp>
#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/HttpService.hpp>
#include <coffee/http/HttpServlet.hpp>
#include <coffee/http/HttpClient.hpp>
#include <coffee/http/url/URLParser.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/UnlimitedTraceWriter.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/SocketArguments.hpp>

using namespace coffee;

struct HttpFixture {
   HttpFixture();
   ~HttpFixture();

   static void parallelRun(coffee::app::Application& app) {
      app.start();
   }

   app::ApplicationServiceStarter app;
   std::shared_ptr<networking::NetworkingService> networkingService;
   std::shared_ptr<http::HttpService> httpService;
   std::thread thr;

};

class UpperServlet : public http::HttpServlet {
public:
   std::shared_ptr<http::HttpResponse> service(std::shared_ptr<http::HttpRequest> request) throw(basis::RuntimeException);
};

class LowerServlet : public http::HttpServlet {
public:
   std::shared_ptr<http::HttpResponse> service(std::shared_ptr<http::HttpRequest> request) throw(basis::RuntimeException);
};

HttpFixture::HttpFixture() : app("AppHttpFixture")
{
   const char* logFileName = "test/http/trace.log";
   unlink (logFileName);
   logger::Logger::initialize(std::make_shared<logger::UnlimitedTraceWriter>(logFileName));
//   logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
   logger::Logger::setLevel(logger::Level::Debug);

   networkingService = networking::NetworkingService::instantiate(app);
   httpService = http::HttpService::instantiate(app);

   http::url::URLParser parser("http://localhost:8080");
   httpService->createServer(parser.build());

   httpService->registerServlet("/upper", std::make_shared<UpperServlet>());
   httpService->registerServlet("/lower", std::make_shared<LowerServlet>());
   thr = std::thread(parallelRun, std::ref(app));
   app.waitUntilRunning();
   networkingService->waitEffectiveRunning();
}

HttpFixture::~HttpFixture() {
   LOG_THIS_METHOD();
   app.stop();
   thr.join();
}

std::shared_ptr<http::HttpResponse> UpperServlet::service(std::shared_ptr<http::HttpRequest> request)
   throw(basis::RuntimeException)
{
   basis::DataBlock body(request->getBody());
   std::transform(body.begin(), body.end(), body.begin(), [](unsigned char cc) { return std::toupper(cc); });

   auto response = http::HttpResponse::instantiate(request);
   response->setBody(body);
   return response;
}

std::shared_ptr<http::HttpResponse> LowerServlet::service(std::shared_ptr<http::HttpRequest> request)
throw(basis::RuntimeException)
{
   basis::DataBlock body(request->getBody());
   std::transform(body.begin(), body.end(), body.begin(), [](unsigned char cc) { return std::tolower(cc); });

   auto response = http::HttpResponse::instantiate(request);
   response->setBody(body);
   return response;
}

BOOST_FIXTURE_TEST_CASE(http_service_send_response, HttpFixture)
{
   http::url::URLParser parser("http://localhost:8080");
   auto url = parser.build();
   auto httpClient = httpService->createClient(url);

   auto request = http::HttpRequest::instantiate(http::HttpRequest::Method::Get, "/upper");
   request->setBody(basis::DataBlock("hello world"));
   auto response = httpClient->send(request);

   BOOST_REQUIRE(response->isOk());
   BOOST_REQUIRE(response->hasBody());



}