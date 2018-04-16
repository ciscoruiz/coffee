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

#include <coffee/networking/AsyncSocket.hpp>
#include <coffee/networking/MessageHandler.hpp>

using namespace coffee;

networking::AsyncSocket::AsyncSocket(networking::NetworkingService& networkingService, const SocketArguments& socketArguments, const int socketType) :
   networking::Socket(networkingService, socketArguments, socketType),
   m_messageHandler(socketArguments.getMessageHandler())
{
}

void networking::AsyncSocket::initialize()
   throw(basis::RuntimeException)
{
   if (!m_messageHandler) {
      COFFEE_THROW_EXCEPTION(asString() << " requires a message handler instance");
   }
}

void networking::AsyncSocket::handle(const basis::DataBlock& message)
   throw(basis::RuntimeException)
{
   m_messageHandler->apply(message, *this);
}

basis::StreamString networking::AsyncSocket::asString() const
   noexcept
{
   basis::StreamString result("networking.AsyncSocket {");

   result << Socket::asString();

   if (m_messageHandler) {
      result << ",Handler=" << m_messageHandler->asString();
   }

   return result << "}";
}
