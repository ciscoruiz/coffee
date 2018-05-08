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
#include <coffee/http/HttpClient.hpp>
#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/HttpService.hpp>
#include <coffee/http/HttpServlet.hpp>
#include <coffee/http/protocol/HttpProtocolDecoder.hpp>
#include <coffee/http/protocol/HttpProtocolEncoder.hpp>
#include <coffee/http/SCCS.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/networking/AsyncSocket.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/SocketArguments.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>

using namespace coffee;

//static
const std::string http::HttpService::Implementation("HTTP");

namespace coffee {
namespace http {

class HttpRequestHandler : public networking::MessageHandler {
public:
   explicit HttpRequestHandler(http::HttpService& httpService) :
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
std::shared_ptr<http::HttpService> http::HttpService::instantiate(app::Application& app, std::shared_ptr<networking::NetworkingService> networkingService)
   throw(basis::RuntimeException)
{
   std::shared_ptr<http::HttpService> result(new http::HttpService(app, networkingService));
   app.attach(result);
   return result;
}

http::HttpService::HttpService(app::Application& app, std::shared_ptr<networking::NetworkingService> networkingService) :
   app::Service(app, app::Feature::UserDefined, Implementation),
   m_networkingService(networkingService)
{
   require(app::Feature::Networking);
   http::SCCS::activate();
}

void http::HttpService::createServer(std::shared_ptr<http::url::URL> url)
   throw(basis::RuntimeException)
{
   networking::SocketArguments arguments;
   auto handler = std::make_shared<HttpRequestHandler>(*this);
   m_networkingService->createServerSocket(arguments.setMessageHandler(handler).addEndPoint(calculateEndPoint(url)));
}

std::shared_ptr<http::HttpClient> http::HttpService::createClient(std::shared_ptr<http::url::URL> url)
   throw(basis::RuntimeException)
{
   networking::SocketArguments arguments;
   auto clientSocket = m_networkingService->createClientSocket(arguments.addEndPoint(calculateEndPoint(url)));
   return http::HttpClient::instantiate(clientSocket);
}

void http::HttpService::registerServlet(const std::string& path, std::shared_ptr<HttpServlet> servlet)
   throw(basis::RuntimeException)
{
   auto ii = m_servlets.find(path);

   if (ii != m_servlets.end()) {
      COFFEE_THROW_EXCEPTION("Path " << path << " already defined");
   }

   m_servlets.insert(Servlets::value_type(path, servlet));
}

std::shared_ptr<http::HttpServlet> http::HttpService::findServlet(const std::string& path)
   throw(basis::RuntimeException)
{
   auto ii = m_servlets.find(path);

   if (ii == m_servlets.end()) {
      COFFEE_THROW_EXCEPTION("There is not Servlet defined for path " << path);
   }

   return ii->second;
}

std::shared_ptr<xml::Node> http::HttpService::asXML(std::shared_ptr<xml::Node>& parent) const
   throw(basis::RuntimeException)
{
   std::shared_ptr<xml::Node> result = parent->createChild("http.Service");

   app::Service::asXML(result);

   auto xmlServlets = result->createChild("Servlets");
   for (auto servlet : m_servlets) {
      auto xmlServlet = xmlServlets->createChild("Servlet");
      xmlServlet->createAttribute("Path", servlet.first);
      xmlServlet->createAttribute("Operation", typeid(*(servlet.second.get())).name());
   }

   return result;
}

//static
std::string http::HttpService::calculateEndPoint(std::shared_ptr<http::url::URL> url)
   throw(basis::RuntimeException)
{
   basis::StreamString result;

   result << "tcp://" << url->getComponent(http::url::ComponentName::Host);

   if (url->hasComponent(http::url::ComponentName::Port)) {
      result << ":" << url->getComponent(http::url::ComponentName::Port);
   } else {
      result << ":" << DefaultHttpPort;
   }

   return result;
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
      serverSocket.send(encoder.apply(servlet->service(httpRequest)));
   }
   catch(basis::RuntimeException& ex) {
      logger::Logger::write(ex);
      serverSocket.send(encoder.apply(http::HttpResponse::instantiate(1, 1, 500, ex.what())));
   }
}

