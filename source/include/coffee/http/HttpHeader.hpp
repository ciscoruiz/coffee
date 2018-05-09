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

#ifndef _coffee_http_HttpHeader_hpp_
#define _coffee_http_HttpHeader_hpp_

#include <memory>

#include <coffee/basis/AsString.hpp>
#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace http {

/**
 * This class models the structure of an predefined HTTP header.
 */
class HttpHeader {
public:
   /**
      HTTP predefined types.
      \see RFC 2616.
      \author francisco.ruiz.rayo@gmail.com.
      \see Header.
   */
   struct Type {
      enum _v {
         Custom = -1,
         /* Common */
            CacheControl, Connection, Date, Pragma, Trailer, TransferEncoding, Upgrade, Via,
         Warning,
         /* Request  */
            Accept, AcceptCharset, AcceptEncoding, AcceptLanguage, Authorization, Expect, From,
         Host, IfMatch, IfModifiedSince, IfNoneMatch, IfRange, IfUnmodifiedSince, MaxForwards,
         ProxyAuthorization, Range, Referer, TE, UserAgent,
         /* Entity Header */
            Allow, ContentEncoding, ContentLanguage, ContentLength, ContentLocation, ContentMD5,
         ContentRange, ContentType, Expires, LastModified,
         /* Response Header */
            AcceptRanges, Age, ETAG, Location, ProxyAuthenticate, RetryAfter, Server, Vary,
         WWWAuthenticate
      };

      static const std::string& asString(const Type::_v type) noexcept;
   };

   static std::shared_ptr<HttpHeader> instantiate(const Type::_v type) noexcept {
      return std::make_shared<HttpHeader>(type);
   }

   /**
    * Constructor.
    * @param type The predefined type of this header.
    */
   explicit HttpHeader(const Type::_v type) : m_type (type) {;}

   HttpHeader(const HttpHeader&) = delete;

   /**
    * Destructor
    */
   virtual ~HttpHeader() {;}

   /**
    * @return The type of this header.
    */
   const Type::_v getType() const noexcept { return m_type; }

   /**
    * @return the literal with the name of this header.
    */
   virtual const std::string& getName () const throw(basis::RuntimeException);

   /**
    * @return the value of this header.
    */
   const std::string& getValue() const noexcept { return m_value; }

   /**
    * Sets the value of this header.
    * \param value the value of this header.
    */
   void setValue(const std::string& value) noexcept {
      if (m_value.empty()) {
         m_value = value;
      }
      else {
         m_value.append(",").append(value);
      }
   }

   /**
    * Sets the value of this header.
    * \param value the value of this header.
    */
   void setValue(const uint32_t value) noexcept { setValue(basis::AsString::apply(value)); }

   /**
    * @return The line with encoded information which will be use while generate the HTTP message.
    */
   std::string encode() const
      throw(basis::RuntimeException)
   {
      std::string result(getName());
      return result.append(":").append(m_value);
   }

private:
   const Type::_v m_type;
   std::string m_value;
};

}
}

#endif 
