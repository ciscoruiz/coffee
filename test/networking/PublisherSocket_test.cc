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
#include <coffee/networking/ClientSocket.hpp>

#include "NetworkingFixture.hpp"

using namespace coffee;

BOOST_FIXTURE_TEST_CASE(publishersocket_without_endpoints, NetworkingFixture)
{
   networking::SocketArguments arguments;
   arguments.addSubscription("12345");
   BOOST_REQUIRE_THROW(networkingService->createPublisherSocket(arguments), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(publishersocket_without_handler, NetworkingFixture)
{
   networking::SocketArguments arguments;
   arguments.addSubscription("12345");
   BOOST_REQUIRE_NO_THROW(networkingService->createPublisherSocket(arguments.addEndPoint("tcp://*:5566")));
}

BOOST_FIXTURE_TEST_CASE(publishersocket_bad_address, NetworkingFixture)
{
   networking::SocketArguments arguments;

   arguments.addSubscription("123").addEndPoint("bad-address").setMessageHandler(UpperStringHandler::instantiate());
   BOOST_REQUIRE_THROW(networkingService->createPublisherSocket(arguments), basis::RuntimeException);
}
