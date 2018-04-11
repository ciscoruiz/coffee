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

#include <coffee/networking/ServerSocket.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/logger/Logger.hpp>

using namespace coffee;

networking::ServerSocket::ServerSocket(networking::NetworkingService& networkingService, const SocketArguments& socketArguments) :
   networking::Socket(networkingService, socketArguments)
{
}

void networking::ServerSocket::initialize()
   throw(basis::RuntimeException)
{
   auto& socket = getImpl();

   for (auto& endPoint : getEndPoints()) {
      socket->bind(endPoint);
   }
}

void networking::ServerSocket::destroy()
   noexcept
{
   auto& socket = getImpl();

   for (auto& endPoint : getEndPoints()) {
      try {
         socket->unbind(endPoint);
      }
      catch (zmq::error_t& ex) {
         LOG_ERROR(asString() << ", Error=" << ex.what());
      }

   }
}

basis::StreamString networking::ServerSocket::asString() const
   noexcept
{
   basis::StreamString result("networking.ServerSocket {");

   result << Socket::asString();

   return result << "}";
}
