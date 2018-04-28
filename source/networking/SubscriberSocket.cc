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

#include <coffee/networking/SubscriberSocket.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/MessageHandler.hpp>

using namespace coffee;

networking::SubscriberSocket::SubscriberSocket(networking::NetworkingService& networkingService, const SocketArguments& socketArguments) :
   networking::AsyncSocket(networkingService, socketArguments, ZMQ_SUB),
   m_subscriptions(socketArguments.getSubscriptions())
{
}

void networking::SubscriberSocket::initialize()
   throw(basis::RuntimeException)
{
   AsyncSocket::initialize();

   if (m_subscriptions.empty()) {
      COFFEE_THROW_EXCEPTION(asString() << " requires one or more subscriptions");
   }

   connect();

   for (auto& subscrition : m_subscriptions) {
      m_zmqSocket->setsockopt(ZMQ_SUBSCRIBE, subscrition.c_str(), subscrition.size());
   }
}

void networking::SubscriberSocket::destroy()
   noexcept
{
   disconnect();
}

basis::StreamString networking::SubscriberSocket::asString() const
   noexcept
{
   basis::StreamString result("networking.SubscriberSocket {");

   result << AsyncSocket::asString();

   result << ",Subscriptions=[";

   bool first = true;
   for (auto& ii : m_subscriptions) {
      if (!first)
         result << ",";
      result << ii;
      first = false;
   }
   result << "]";

   return result << "}";
}
