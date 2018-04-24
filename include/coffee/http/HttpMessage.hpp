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

#ifndef _coffee_http_HttpMessage_hpp_
#define _coffee_http_HttpMessage_hpp_

#include <list>
#include <memory>
#include <unordered_map>

#include <coffee/basis/DataBlock.hpp>
#include <coffee/http/HttpHeader.hpp>

namespace coffee {

namespace http {

namespace protocol {
class HttpProtocolEncoder;
}

/**
 * General definition for HTTP messages following RFC 2616.
 * \author francisco.ruiz.rayo@gmail.com
 */
class HttpMessage {
public:
   /**
    * Destructor
    */
   virtual ~HttpMessage() { clear(); }

   bool hasHeader(const HttpHeader::Type::_v type) const noexcept {
      return m_directory.find(HttpHeader::Type::asString(type)) != m_directory.end();
   }
   HttpMessage& setHeader(const HttpHeader::Type::_v type, const std::string& value) throw(basis::RuntimeException);
   HttpMessage& setCustomHeader(const std::string& headerName, const std::string& value) noexcept;

   /**
    * @return The body of this HTTP message
    */
   const basis::DataBlock& getBody() const noexcept  { return m_body; }

   /**
    * Sets the body of this Message.
    * @param body The new body of this message.
    */
   HttpMessage& setBody(const basis::DataBlock& body) throw () { m_body = body; return *this; }

   /**
    * Clear the body of this Message.
    */
   HttpMessage& clearBody() throw () { m_body.clear(); return *this; }

   /**
    * Resets all components of this message.
    */
   void clear() throw () {
      m_directory.clear();
      m_sequentialHeaders.clear();
      m_body.clear();
   }

   uint16_t getMajorVersion() const noexcept { return m_majorVersion; }

   uint16_t getMinorVersion() const noexcept { return m_minorVersion; }

protected:
   /**
    * Constructor.
    */
   HttpMessage(const uint16_t majorVersion, const uint16_t minorVersion) :
      m_majorVersion(majorVersion),
      m_minorVersion(minorVersion)
   {}

   /**
    * @return the expression HTTP/<mayor version>.<minor version> which will be used on the first line.
    */
   std::string encodeVersion() const noexcept ;

   /**
    * @return the first line in the HTTP message.
    */
   virtual std::string encodeFirstLine() const throw(basis::RuntimeException) = 0;

private:
   const uint16_t m_majorVersion;
   const uint16_t m_minorVersion;
   typedef std::list<std::shared_ptr<HttpHeader> > SequentialHeaders;
   SequentialHeaders m_sequentialHeaders;
   std::unordered_map<std::string, SequentialHeaders::iterator> m_directory;
   basis::DataBlock m_body;

   friend class protocol::HttpProtocolEncoder;
};

}
}

#endif // _coffee_http_HttpMessage_hpp_