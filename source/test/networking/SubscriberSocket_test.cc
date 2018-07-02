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

#include <gtest/gtest.h>

#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/SubscriberSocket.hpp>

#include "NetworkingFixture.hpp"

using namespace coffee;

struct SubscriberSocketTest : public NetworkingFixture {;};

TEST_F(SubscriberSocketTest, subscribersocket_without_endpoints)
{
   networking::SocketArguments arguments;
   arguments.addSubscription("12345").setMessageHandler(UpperStringHandler::instantiate());
   ASSERT_THROW(networkingService->createSubscriberSocket(arguments), basis::RuntimeException);
}

TEST_F(SubscriberSocketTest, subscribersocket_without_handler)
{
   networking::SocketArguments arguments;
   arguments.addSubscription("12345").addEndPoint("tcp://localhost:5566");
   ASSERT_THROW(networkingService->createSubscriberSocket(arguments), basis::RuntimeException);
}

TEST_F(SubscriberSocketTest, subscribersocket_without_subscription)
{
   networking::SocketArguments arguments;
   arguments.addEndPoint("tcp://localhost:5566").setMessageHandler(UpperStringHandler::instantiate());
   ASSERT_THROW(networkingService->createSubscriberSocket(arguments), basis::RuntimeException);
}

TEST_F(SubscriberSocketTest, subscribersocket_bad_address)
{
   networking::SocketArguments arguments;

   arguments.addSubscription("123").addEndPoint("bad-address").setMessageHandler(UpperStringHandler::instantiate());
   ASSERT_THROW(networkingService->createSubscriberSocket(arguments), basis::RuntimeException);
}

TEST_F(SubscriberSocketTest, subscribersocket_must_not_send)
{
   networking::SocketArguments arguments;

   arguments.addSubscription("123").addEndPoint("tcp://localhost:5566").setMessageHandler(UpperStringHandler::instantiate());
   auto subscriber = networkingService->createSubscriberSocket(arguments);
   ASSERT_TRUE(subscriber != nullptr);
   ASSERT_THROW(subscriber->send(basis::DataBlock("world", 5)), basis::RuntimeException);
}
