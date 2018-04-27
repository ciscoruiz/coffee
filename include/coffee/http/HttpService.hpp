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
#ifndef _coffee_http_HttpService_hpp_
#define _coffee_http_HttpService_hpp_

#include <unordered_map>

#include <coffee/app/Service.hpp>
#include <coffee/http/url/URL.hpp>
#include <coffee/networking/MessageHandler.hpp>

namespace coffee {

namespace networking {
   class NetworkingService;
}

namespace http {

class HttpServlet;
class HttpClient;

class HttpService : public app::Service {
public:
   static const int DefaultHttpPort = 80;
   static const std::string Implementation;

   ~HttpService() { m_servlets.clear(); }

   static std::shared_ptr<HttpService> instantiate(app::Application& app, std::shared_ptr<networking::NetworkingService> networkingService) noexcept;

   void createServer(std::shared_ptr<http::url::URL> url) throw(basis::RuntimeException);
   std::shared_ptr<HttpClient> createClient(std::shared_ptr<http::url::URL> url) throw(basis::RuntimeException);

   void registerServlet(const std::string& path, std::shared_ptr<HttpServlet> servlet) throw(basis::RuntimeException);

   template <typename _T> void registerServlet(const std::string& path) throw(basis::RuntimeException) {
      registerServlet(path, std::make_shared<_T>());
   }

   std::shared_ptr<HttpServlet> findServlet(const std::string& path) throw(basis::RuntimeException);

private:
   typedef std::unordered_map<std::string, std::shared_ptr<HttpServlet> > Servlets;

   std::shared_ptr<networking::NetworkingService> m_networkingService;
   Servlets m_servlets;

   HttpService(app::Application& app, std::shared_ptr<networking::NetworkingService> networkingService);

   void do_stop() throw(basis::RuntimeException) {;}
   void do_initialize() throw(basis::RuntimeException) {;}
   static std::string calculateEndPoint(std::shared_ptr<http::url::URL> url) throw(basis::RuntimeException);
};

}
}

#endif //_coffee_http_HttpService_hpp_
