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

#ifndef _coffee_http_HttpResponse_hpp_
#define _coffee_http_HttpResponse_hpp_

#include <coffee/http/HttpMessage.hpp>

namespace coffee {
namespace http {

class HttpRequest;

/**
 * General definition for HTTP requests following RFC 2616.
 *
 * The first line of any HTTP-request will be:
 * /code
 *    <METHOD> <URI> HTTP/<MAYOR VERSION>.<MINOR VERSION>
 * /endcode
 *
 * \author francisco.ruiz.rayo@gmail.com
 */
class HttpResponse : public HttpMessage {
public:
   /**
    * Constructor.
    */
   HttpResponse(const int statusCode, const std::shared_ptr<HttpRequest>& request);

   static std::shared_ptr<HttpResponse> instantiate(const int statusCode, const std::shared_ptr<HttpRequest>& request)
      noexcept
   {
      return std::make_shared<HttpResponse>(statusCode, request);
   }

   int getStatusCode() const noexcept { return m_statusCode; }

protected:
      /**
    * @return the first line in the HTTP message.
    */
   std::string encodeFirstLine() const throw(basis::RuntimeException);

private:
   const int m_statusCode;
   std::shared_ptr<HttpRequest> m_request;
};

}
}

#endif // _coffee_http_HttpResponse_hpp_