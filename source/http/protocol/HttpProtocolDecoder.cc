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

#include <coffee/http/protocol/HttpProtocolDecoder.hpp>
#include <coffee/http/protocol/defines.hpp>
#include <coffee/http/protocol/state/HttpProtocolState.hpp>
#include <coffee/http/protocol/state/HttpProtocolWaitingMessage.hpp>
#include <coffee/http/protocol/state/HttpProtocolWaitingContentLength.hpp>
#include <coffee/http/protocol/state/HttpProtocolWaitingBody.hpp>
#include <coffee/http/protocol/state/HttpProtocolReadBody.hpp>

using namespace coffee;

static http::protocol::state::HttpProtocolWaitingMessage stateWaitingMessage;
static http::protocol::state::HttpProtocolWaitingContentLength stateWaitingContentLength;
static http::protocol::state::HttpProtocolWaitingBody stateWaitingBody;
static http::protocol::state::HttpProtocolReadBody stateReadBody;

std::shared_ptr<http::HttpMessage> http::protocol::HttpProtocolDecoder::apply(const basis::DataBlock& dataBlock)
   throw(basis::RuntimeException)
{
   Token token;

   setState(State::WaitingMessage);
   m_result.reset();
   m_bodyExpectedSize = 0;

   while (readToken(dataBlock, token)) {
      if (m_state->process(*this, token) == state::HttpProtocolState::ProcessResult::Completed) {
         break;
      }
   }

   if (!m_result) {
      COFFEE_THROW_EXCEPTION("Unable to allocate an HTTP message");
   }

   return m_result;
}

bool http::protocol::HttpProtocolDecoder::readToken(const basis::DataBlock& dataBlock, protocol::Token& token)
   throw(basis::RuntimeException)
{
   static const int nchars = coffee_strlen(http::protocol::newLineCharacters);

   if (token.currentPosition == dataBlock.size()) {
      return false;
   }

   auto endToken = dataBlock.find(protocol::newLineCharacters, token.currentPosition);

   if (endToken == std::string::npos) {
      // Copy from the current position to the end
      token.value = basis::DataBlock(dataBlock.data() + token.currentPosition, dataBlock.size() - token.currentPosition);
      token.currentPosition = dataBlock.size();
      return true;
   }

   if (endToken == token.currentPosition) {
      // Empty token
      token.value.clear();
      token.currentPosition += nchars;
   }
   else {
      token.value = basis::DataBlock(dataBlock.data() + token.currentPosition, endToken - token.currentPosition);
      token.currentPosition = endToken + nchars;
   }

   return true;
}

void http::protocol::HttpProtocolDecoder::setState(const State::_v state)
   noexcept
{
   static const state::HttpProtocolState* states[] = { &stateWaitingMessage, &stateWaitingContentLength, &stateWaitingBody, &stateReadBody };

   if (m_state != states[state]) {
      m_state = states[state];
   }
}
