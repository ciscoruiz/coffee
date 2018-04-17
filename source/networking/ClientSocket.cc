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
#include <coffee/networking/MessageHandler.hpp>
#include <coffee/logger/Logger.hpp>

using namespace coffee;

networking::ClientSocket::ClientSocket(networking::NetworkingService& networkingService, const SocketArguments& socketArguments) :
   networking::Socket(networkingService, socketArguments, ZMQ_REQ)
{
}

void networking::ClientSocket::initialize()
   throw(basis::RuntimeException)
{
   static const std::chrono::milliseconds handshakeTime(50);

   connect();
   m_zmqSocket->setsockopt(ZMQ_SNDTIMEO, (int) handshakeTime.count());
   m_zmqSocket->setsockopt(ZMQ_RCVTIMEO, (int) handshakeTime.count());
   m_zmqSocket->setsockopt(ZMQ_LINGER, (int) handshakeTime.count());
}

void networking::ClientSocket::destroy()
   noexcept
{
   disconnect();
}

basis::DataBlock networking::ClientSocket::send(const basis::DataBlock& request)
   throw(basis::RuntimeException)
{
   zmq::message_t zmqResponse;

   try {
      zmq::message_t zmqRequest(request.size());
      coffee_memcpy(zmqRequest.data(), request.data(), request.size());
      if (!m_zmqSocket->send(zmqRequest, ZMQ_DONTWAIT)) {
         COFFEE_THROW_EXCEPTION(asString() << ",Error=Socket could not send the message");
      }

      if (!m_zmqSocket->recv(&zmqResponse)) {
         m_zmqSocket->close();
         COFFEE_THROW_EXCEPTION(asString() << " did not receive any response");
      }
   }
   catch(zmq::error_t& ex) {
      COFFEE_THROW_EXCEPTION(asString() << ",Error=" << ex.what());
   }

   return basis::DataBlock ((char*) zmqResponse.data(), zmqResponse.size());
}

basis::StreamString networking::ClientSocket::asString() const
   noexcept
{
   basis::StreamString result("networking.ClientSocket {");

   result << Socket::asString();
   result << ",IsConnected=" << m_zmqSocket->connected();

   return result << "}";
}
