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

#ifndef _coffee_http_HttpClient_hpp_
#define _coffee_http_HttpClient_hpp_

#include <memory>
#include <coffee/networking/ClientSocket.hpp>
#include <coffee/http/protocol/HttpProtocolEncoder.hpp>
#include <coffee/http/protocol/HttpProtocolDecoder.hpp>

namespace coffee {
namespace http {

class HttpService;
class HttpRequest;
class HttpResponse;

class HttpClient {
public:
   std::shared_ptr<HttpResponse> send(const std::shared_ptr<HttpRequest>& request) throw(basis::RuntimeException);

private:
   std::shared_ptr<networking::ClientSocket> m_clientSocket;
   http::protocol::HttpProtocolEncoder m_encoder;
   http::protocol::HttpProtocolDecoder m_decoder;

   explicit HttpClient(const std::shared_ptr<networking::ClientSocket>& clientSocket) : m_clientSocket(clientSocket) {}

   static std::shared_ptr<HttpClient> instantiate(const std::shared_ptr<networking::ClientSocket>& clientSocket) noexcept {
      std::shared_ptr<HttpClient> result(new HttpClient(clientSocket));
      return result;
   }

   friend class HttpService;
};

}
}

#endif /* _coffee_http_HttpClient_hpp_ */
