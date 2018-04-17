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

#include <boost/test/unit_test.hpp>

#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/PublisherSocket.hpp>
#include <coffee/logger/Logger.hpp>

#include "NetworkingFixture.hpp"

using namespace coffee;

class Counter : public networking::MessageHandler {
public:
   Counter() : MessageHandler("Counter"), m_counter(0) {}

   int getCounter() const noexcept { return m_counter; }

protected:
   virtual void apply(const basis::DataBlock& message, networking::AsyncSocket& socket)
      throw(basis::RuntimeException)
   {
      std::lock_guard<std::mutex> guard(m_mutex);
      m_counter ++;
   }

private:
   std::mutex m_mutex;
   int m_counter;

};

BOOST_FIXTURE_TEST_CASE(publish_subscribers, NetworkingFixture)
{
   std::shared_ptr<networking::PublisherSocket> publisherSocket;

   {
      networking::SocketArguments arguments;
      publisherSocket = networkingService->createPublisherSocket(arguments.addEndPoint("tcp://*:5566"));
      BOOST_REQUIRE(publisherSocket);
   }

   auto counterHandler = std::make_shared<Counter>();

   const int maxSubscribers = 5;

   for (int ii = 0; ii < maxSubscribers; ++ ii) {
      networking::SocketArguments arguments;
      arguments.addSubscription("message").setMessageHandler(counterHandler);
      auto subscriberSocket = networkingService->createSubscriberSocket(arguments.addEndPoint("tcp://127.0.0.1:5566"));
      BOOST_REQUIRE(subscriberSocket);
   }

   // To give time to NetworkingService to detect new subscribers
   usleep(100000);


   const int maxMessages = 100;
   for (int ii = 0; ii < maxMessages; ++ ii) {
      basis::StreamString str;
      str << (ii % 2 ? "message=": "other=") << ii;
      publisherSocket->send(basis::DataBlock(str.c_str()));
      usleep(1000);
   }

   BOOST_REQUIRE_EQUAL(counterHandler->getCounter(), maxMessages / 2 * maxSubscribers);
}
