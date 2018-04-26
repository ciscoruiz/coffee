// MIT License
//
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
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

#include <coffee/app/Application.hpp>
#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/HttpService.hpp>
#include <coffee/http/HttpServlet.hpp>
#include <coffee/http/protocol/HttpProtocolDecoder.hpp>
#include <coffee/http/protocol/HttpProtocolEncoder.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/networking/AsyncSocket.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/SocketArguments.hpp>

using namespace coffee;

//static
const std::string http::HttpService::Implementation("HTTP");

namespace coffee {
namespace http {

class HttpRequestHandler : public networking::MessageHandler {
public:
   HttpRequestHandler(http::HttpService& httpService) :
      coffee::networking::MessageHandler("HttpRequestHandler"),
      m_httpService(httpService)
   {;}

private:
   http::HttpService& m_httpService;

   void apply(const basis::DataBlock& message, networking::AsyncSocket& serverSocket)
      throw(basis::RuntimeException);
};

}
}

//static
std::shared_ptr<http::HttpService> http::HttpService::instantiate(app::Application& app)
   noexcept
{
   std::shared_ptr<http::HttpService> result(new http::HttpService(app));
   app.attach(result);
   return result;
}

http::HttpService::HttpService(coffee::app::Application& app) :
   app::Service(app, app::Feature::Networking, Implementation)
{
   require(app::Feature::Networking);
}

void http::HttpService::do_initialize()
   throw(basis::RuntimeException)
{
   m_networkingService = std::dynamic_pointer_cast<networking::NetworkingService>(getRequirement(app::Feature::Networking));
}

void http::HttpService::createServer(std::shared_ptr<http::url::URL> url)
   throw(basis::RuntimeException) {
   basis::StreamString endpoint;

   endpoint << "tcp://" << url->getComponent(http::url::ComponentName::Host);

   if (url->hasComponent(http::url::ComponentName::Port)) {
      endpoint << ":" << url->getComponent(http::url::ComponentName::Port);
   } else {
      endpoint << ":" << DefaultHttpPort;
   }

   networking::SocketArguments arguments;

   auto handler = std::make_shared<HttpRequestHandler>(*this);
   m_networkingService->createServerSocket(arguments.setMessageHandler(handler).addEndPoint(endpoint));
}

void http::HttpRequestHandler::apply(const basis::DataBlock& dataBlock, networking::AsyncSocket& serverSocket)
   throw(basis::RuntimeException)
{
   http::protocol::HttpProtocolDecoder decoder;
   protocol::HttpProtocolEncoder encoder;
   std::shared_ptr<http::HttpMessage> httpMessage;

   try {
      httpMessage = decoder.apply(dataBlock);
   }
   catch (basis::RuntimeException& ex) {
      logger::Logger::write(ex);
      serverSocket.send(encoder.apply(http::HttpResponse::instantiate(1, 1, 400, ex.what())));
      return;;
   }

   auto httpRequest = std::dynamic_pointer_cast<http::HttpRequest>(httpMessage);

   if (!httpRequest) {
      LOG_ERROR("HTTP server can not work on HTTP responses");
      serverSocket.send(encoder.apply(http::HttpResponse::instantiate(1, 1, 400, "HTTP server can not work on HTTP responses")));
      return;
   }

   std::shared_ptr<HttpServlet> servlet;

   try {
      servlet = m_httpService.findServlet(httpRequest->getPath());
   }
   catch(basis::RuntimeException& ex) {
      basis::StreamString ss;
      ss << httpRequest->getPath() << " was not service for any servlet";
      LOG_ERROR(ss);
      serverSocket.send(encoder.apply(http::HttpResponse::instantiate(1, 1, 404, ss)));
      return;
   }

   try {
      auto response = servlet->service(httpRequest);
      serverSocket.send(encoder.apply(response));
   }
   catch(basis::RuntimeException& ex) {
      logger::Logger::write(ex);
      serverSocket.send(encoder.apply(http::HttpResponse::instantiate(1, 1, 500, ex.what())));
   }
}
