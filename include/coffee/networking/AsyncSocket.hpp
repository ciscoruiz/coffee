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
#ifndef _coffee_networking_AsyncSocket_hpp_
#define _coffee_networking_AsyncSocket_hpp_

#include <vector>
#include <memory>

#include <coffee/networking/Socket.hpp>

namespace coffee {

namespace networking {

class MessageHandler;

class AsyncSocket : public Socket {
public:
   virtual ~AsyncSocket() { m_messageHandler.reset(); }
   virtual basis::StreamString asString() const noexcept;
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;
   virtual void send(const basis::DataBlock& response) throw(basis::RuntimeException) = 0;

protected:
   AsyncSocket(NetworkingService& networkingService, const SocketArguments& socketArguments, const int socketType);

   virtual void initialize() throw(basis::RuntimeException);

private:
   std::shared_ptr<MessageHandler> m_messageHandler;

   void handle(const basis::DataBlock& message) throw(basis::RuntimeException);

   friend class NetworkingService;
};

}
}

#endif // _coffee_networking_AsyncSocket_hpp_
