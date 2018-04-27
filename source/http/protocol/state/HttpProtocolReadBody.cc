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

#include <string.h>
#include <algorithm>

#include <coffee/http/protocol/state/HttpProtocolReadBody.hpp>
#include <coffee/http/protocol/HttpProtocolDecoder.hpp>
#include <coffee/http/protocol/defines.hpp>
#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/url/URLParser.hpp>
#include <coffee/logger/Logger.hpp>

using namespace coffee;
using namespace coffee::http::protocol::state;

HttpProtocolState::ProcessResult::_v HttpProtocolReadBody::process(HttpProtocolDecoder& context, const Token& token) const
   throw(basis::RuntimeException)
{
   if (token.value.size() != context.m_bodyExpectedSize) {
      COFFEE_THROW_EXCEPTION("Body length received (" << context.m_bodyExpectedSize << ") does not match the received (" << token.value.size() << ")");
   }

   context.m_result->setBody(token.value);
   return ProcessResult::Completed;
}

