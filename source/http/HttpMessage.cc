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

#include <coffee/http/HttpMessage.hpp>
#include <coffee/http/HttpCustomHeader.hpp>

using namespace coffee;

using http::HttpMessage;

bool HttpMessage::hasHeader(const HttpHeader::Type::_v type) const
   throw(basis::RuntimeException)
{
   if (type == HttpHeader::Type::Custom) {
      COFFEE_THROW_EXCEPTION("HttpHeader of type none can not be instantiated for users");
   }

   return m_directory.find(HttpHeader::Type::asString(type)) != m_directory.end();
}

const std::string& HttpMessage::getHeaderValue(const HttpHeader::Type::_v type) const
   throw(basis::RuntimeException)
{
   if (type == HttpHeader::Type::Custom) {
      COFFEE_THROW_EXCEPTION("HttpHeader of type none can not be instantiated for users");
   }

   return getCustomHeaderValue(HttpHeader::Type::asString(type));
}

const std::string& HttpMessage::getCustomHeaderValue(const std::string& headerName) const
   throw(basis::RuntimeException)
{
   auto dd = m_directory.find(headerName);

   if (dd == m_directory.end()) {
      COFFEE_THROW_EXCEPTION("Header " << headerName << " was not found");
   }

   return (*dd->second)->getValue();
}

HttpMessage& HttpMessage::setHeader(const HttpHeader::Type::_v type, const std::string& value)
   throw(basis::RuntimeException)
{
   if (type == HttpHeader::Type::Custom) {
      COFFEE_THROW_EXCEPTION("HttpHeader of type none can not be instantiated for users");
   }

   const std::string& headerName = HttpHeader::Type::asString(type);

   auto dd = m_directory.find(headerName);

   if (dd == m_directory.end()) {
      auto newHeader = HttpHeader::instantiate(type);
      newHeader->setValue(value);
      m_directory[headerName] = m_sequentialHeaders.insert(m_sequentialHeaders.end(), newHeader);
   }
   else {
      dd->second->get()->setValue(value);
   }

   return *this;
}

HttpMessage& HttpMessage::setCustomHeader(const std::string& headerName, const std::string& value)
   noexcept
{
   auto dd = m_directory.find(headerName);

   if (dd == m_directory.end()) {
      auto newHeader = HttpCustomHeader::instantiate(headerName);
      newHeader->setValue(value);
      m_directory[headerName] = m_sequentialHeaders.insert(m_sequentialHeaders.end(), newHeader);
   }
   else {
      dd->second->get()->setValue(value);
   }

   return *this;
}

std::string http::HttpMessage::encodeVersion() const
   noexcept
{
   basis::StreamString ss("HTTP/");
   return ss << m_majorVersion << "." << m_minorVersion;
}
