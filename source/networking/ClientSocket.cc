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

#include <coffee/networking/ClientSocket.hpp>

using namespace coffee;

networking::ClientSocket::ClientSocket(networking::NetworkingService& networkingunicator, const SocketArguments& socketArguments, std::shared_ptr<MessageHandler> messageHandler) :
   networking::Socket(networkingunicator, socketArguments),
   m_messageHandler(messageHandler)
{
}

void networking::ClientSocket::initialize()
   throw(basis::RuntimeException)
{
   auto& socket = getImpl();

   for (auto& endPoint : getEndPoints()) {
      socket->connect(endPoint);
   }
}

void networking::ClientSocket::destroy()
   noexcept
{
   auto& socket = getImpl();

   for (auto& endPoint : getEndPoints()) {
      socket->disconnect(endPoint);
   }
}

void networking::ClientSocket::send(const basis::DataBlock& message)
   throw(basis::RuntimeException)
{
   zmq::message_t zmqMessage(message.size());
   coffee_memcpy(zmqMessage.data(), message.data(), message.size());
   getImpl()->send(zmqMessage);
}
