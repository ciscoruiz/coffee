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

#ifndef _coffee_http_protocol_HttpProtocolDecoder_hpp_
#define _coffee_http_protocol_HttpProtocolDecoder_hpp_

#include <memory>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/http/protocol/state/HttpProtocolWaitingContentLength.hpp>

namespace coffee {

namespace basis {
   class DataBlock;
}

namespace http {

class HttpMessage;

namespace protocol {

struct Token;

namespace state {
   class HttpProtocolState;
   class HttpProtocolWaitingMessage;
   class HttpProtocolWaitingContentLength;
   class HttpProtocolWaitingBody;
   class HttpProtocolReadBody;
}

class HttpProtocolDecoder {
public:
   struct State {
      enum _v {
         WaitingMessage, WaitingContentLength, WaitingBody, ReadBody
      };
   };

   HttpProtocolDecoder() : m_state(nullptr), m_bodyExpectedSize(0) { setState(State::WaitingMessage); }

   static bool readToken(const basis::DataBlock& dataBlock, Token& token) throw(basis::RuntimeException);

   std::shared_ptr<HttpMessage> apply(const basis::DataBlock& dataBlock) throw(basis::RuntimeException);

private:
   const state::HttpProtocolState* m_state;
   uint32_t m_bodyExpectedSize;
   std::shared_ptr<HttpMessage> m_result;

   void setState(const State::_v state) noexcept;

   friend class state::HttpProtocolWaitingMessage;
   friend class state::HttpProtocolWaitingContentLength;
   friend class state::HttpProtocolWaitingBody;
   friend class state::HttpProtocolReadBody;

};

}
}
}

#endif // _coffee_http_protocol_HttpProtocolDecoder_hpp_
