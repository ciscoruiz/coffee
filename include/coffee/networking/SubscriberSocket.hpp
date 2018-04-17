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
#ifndef _coffee_networking_SubscriberSocket_hpp_
#define _coffee_networking_SubscriberSocket_hpp_

#include <vector>
#include <memory>

#include <coffee/networking/AsyncSocket.hpp>

namespace coffee {

namespace networking {

class MessageHandler;

class SubscriberSocket : public AsyncSocket {
public:
   basis::StreamString asString() const noexcept;

   void send(const coffee::basis::DataBlock&) throw(basis::RuntimeException) {
      COFFEE_THROW_EXCEPTION(asString() << " method can not be used");
   }

private:
   Subscriptions m_subscriptions;

   SubscriberSocket(NetworkingService& networkingService, const SocketArguments& socketArguments);

   void initialize() throw(basis::RuntimeException);
   void destroy() noexcept;

   friend class NetworkingService;
};

}
}

#endif // _coffee_networking_SubscriberSocket_hpp_
