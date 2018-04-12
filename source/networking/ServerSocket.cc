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

#include <coffee/logger/Logger.hpp>

#include <coffee/networking/ServerSocket.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/MessageHandler.hpp>

using namespace coffee;

networking::ServerSocket::ServerSocket(networking::NetworkingService& networkingService, const SocketArguments& socketArguments) :
   networking::Socket(networkingService, socketArguments, ZMQ_REP),
   m_messageHandler(socketArguments.getMessageHandler())
{
}

void networking::ServerSocket::initialize()
   throw(basis::RuntimeException)
{
   auto& socket = getZmqSocket();

   if (!m_messageHandler) {
      COFFEE_THROW_EXCEPTION(asString() << " requires message handler instance");
   }

   bool empty = true;

   try {
      for (auto& endPoint : getEndPoints()) {
         empty = false;
         socket->bind(endPoint);
      }
   }
   catch(zmq::error_t& ex) {
      COFFEE_THROW_EXCEPTION(asString () << ", Error=" << ex.what());
   }

   if (empty) {
      COFFEE_THROW_EXCEPTION(asString() << " does not have any end point");
   }
}

void networking::ServerSocket::destroy()
   noexcept
{
   auto& socket = getZmqSocket();

   for (auto& endPoint : getEndPoints()) {
      try {
         socket->unbind(endPoint);
      }
      catch (zmq::error_t& ex) {
         LOG_WARN(asString() << ", Error=" << ex.what());
      }
   }
}

void networking::ServerSocket::send(const basis::DataBlock& response)
   throw(basis::RuntimeException)
{
   zmq::message_t zmqMessage(response.size());
   coffee_memcpy(zmqMessage.data(), response.data(), response.size());
   m_zmqSocket->send(zmqMessage);
}

basis::StreamString networking::ServerSocket::asString() const
   noexcept
{
   basis::StreamString result("networking.ServerSocket {");

   result << Socket::asString();

   if (m_messageHandler) {
      result << ",Handler=" << m_messageHandler->asString();
   }

   return result << "}";
}
