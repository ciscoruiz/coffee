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
   networking::AsyncSocket(networkingService, socketArguments, ZMQ_REP)
{
}

void networking::ServerSocket::initialize()
   throw(basis::RuntimeException)
{
   AsyncSocket::initialize();
   bind();
}

void networking::ServerSocket::destroy()
   noexcept
{
   unbind();
}

void networking::ServerSocket::send(const basis::DataBlock& response)
   throw(basis::RuntimeException)
{
   zmq::message_t zmqMessage(response.size());
   coffee_memcpy(zmqMessage.data(), response.data(), response.size());
   try {
      m_zmqSocket->send(zmqMessage);
   }
   catch (const zmq::error_t& ex) {
      COFFEE_THROW_EXCEPTION(asString() << ", Error=" << ex.what());
   }
}

basis::StreamString networking::ServerSocket::asString() const
   noexcept
{
   basis::StreamString result("networking.ServerSocket {");

   result << AsyncSocket::asString();

   return result << "}";
}
