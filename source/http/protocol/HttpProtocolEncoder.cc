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

#include <coffee/http/protocol/HttpProtocolEncoder.hpp>

#include <coffee/http/HttpMessage.hpp>
#include <coffee/http/HttpHeader.hpp>
#include <coffee/http/protocol/defines.hpp>

using namespace coffee;

const basis::DataBlock& http::protocol::HttpProtocolEncoder::apply(std::shared_ptr<HttpMessage> message) const
   throw(basis::RuntimeException)
{
   m_buffer.clear();

   m_buffer.append(message->encodeFirstLine()).append(newLineCharacters);

   const basis::DataBlock& body (message->m_body);
   const bool hasBody = body.size() > 0;

   if (hasBody && !message->hasHeader(HttpHeader::Type::ContentLength)) {
      message->setHeader(HttpHeader::Type::ContentLength, basis::AsString::apply(body.size()));
   }

   for (auto header : message->m_sequentialHeaders) {
      m_buffer.append(header->encode()).append(newLineCharacters);
   }

   m_buffer.append(newLineCharacters);

   if (hasBody) {
      m_buffer.append(body);
   }

   return m_buffer;
}

