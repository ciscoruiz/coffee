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

#ifndef _coffee_http_HttpRequest_hpp_
#define _coffee_http_HttpRequest_hpp_

#include <coffee/http/HttpMessage.hpp>
#include <coffee/http/url/defines.hpp>
#include <coffee/http/url/URL.hpp>

namespace coffee {

namespace http {

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
class HttpRequest : public HttpMessage {
public:
   struct Method {
      enum _v {
         Port, Options, Get, Head, Put, Delete, Trace, Connect
      };
      static const char* asString(const Method::_v value) noexcept;
   };

   static std::shared_ptr<HttpRequest> instantiate(const Method::_v method, const std::string& url, const uint32_t majorVersion = 1, const uint32_t minorVersion = 1)
      throw(basis::RuntimeException);

   const Method::_v getMethod() const noexcept { return m_method; }
   std::shared_ptr<url::URL> getURL() const noexcept { m_url; }

   const std::string& getPath() const throw(basis::RuntimeException) { return m_url->getComponent(url::ComponentName::Path); }

protected:
   /**
    * Constructor.
    */
   HttpRequest(const Method::_v method, const std::shared_ptr<url::URL>& url, const uint32_t majorVersion, const uint32_t minorVersion) :
      HttpMessage(majorVersion, minorVersion),
      m_method(method),
      m_url(url)
   {}

   /**
    * @return the first line in the HTTP message.
    */
   std::string encodeFirstLine() const throw(basis::RuntimeException);

private:
   const Method::_v m_method;
   std::shared_ptr<url::URL> m_url;
};

}
}

#endif // _coffee_http_HttpRequest_hpp_
