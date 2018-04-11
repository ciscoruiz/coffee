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
#ifndef _coffee_networking_ClientSocket_hpp_
#define _coffee_networking_ClientSocket_hpp_

#include <memory>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/DataBlock.hpp>
#include <coffee/networking/Socket.hpp>

namespace coffee {

namespace networking {

class MessageHandler;

class ClientSocket : public Socket {
public:
   std::shared_ptr<MessageHandler>& getMessageHandler() noexcept { return m_messageHandler;}
   void send(const basis::DataBlock& message) throw(basis::RuntimeException);
   basis::StreamString asString() const noexcept;

protected:
   virtual void initialize() throw(basis::RuntimeException);
   virtual void destroy() noexcept;

private:
   std::shared_ptr<MessageHandler> m_messageHandler;

   ClientSocket(NetworkingService& networkingService, const SocketArguments& socketArguments, std::shared_ptr<MessageHandler> messageHandler);

   friend class NetworkingService;
};

}
}

#endif // _coffee_networking_ClientSocket_hpp_
