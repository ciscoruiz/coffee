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

BOOST_FIXTURE_TEST_CASE(clientsocket_service_on, NetworkingFixture)
{
   BOOST_REQUIRE(networkingService->isRunning());
}

BOOST_FIXTURE_TEST_CASE(clientsocket_server_on, NetworkingFixture)
{
   BOOST_REQUIRE(upperServer->isValid());
}

BOOST_FIXTURE_TEST_CASE(clientsocket_without_endpoints, NetworkingFixture)
{
   networking::SocketArguments arguments;

   BOOST_REQUIRE_THROW(networkingService->createClientSocket(arguments), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(clientsocket_without_handler, NetworkingFixture)
{
   networking::SocketArguments arguments;
   arguments.addSubscription("12345");
   BOOST_REQUIRE_NO_THROW(networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555")));
}

BOOST_FIXTURE_TEST_CASE(clientsocket_bad_address, NetworkingFixture)
{
   networking::SocketArguments arguments;
   arguments.addSubscription("12345");
   BOOST_REQUIRE_THROW(networkingService->createClientSocket(arguments.addEndPoint("bad-address")), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(clientsocket_not_found, NetworkingFixture)
{
   BOOST_REQUIRE_THROW(networkingService->findClientSocket("some-name"), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(clientsocket_found, NetworkingFixture)
{
   {
      networking::SocketArguments arguments;
      arguments.setName("first").addEndPoint("tcp://localhost:5555");
      auto clientSocket = networkingService->createClientSocket(arguments);
      BOOST_REQUIRE(clientSocket);
   }

   {
      networking::SocketArguments arguments;
      arguments.setName("second").addEndPoint("tcp://127.0.0.1:5556");
      auto clientSocket = networkingService->createClientSocket(arguments);
      BOOST_REQUIRE(clientSocket);
   }

   auto clientSocket = networkingService->findClientSocket("first");
   BOOST_REQUIRE_EQUAL(clientSocket->getEndPoints().at(0), "tcp://localhost:5555");

   clientSocket = networkingService->findClientSocket("second");
   BOOST_REQUIRE_EQUAL(clientSocket->getEndPoints().at(0), "tcp://127.0.0.1:5556");
}

BOOST_FIXTURE_TEST_CASE(clientsocket_unkwown_server, NetworkingFixture)
{
   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:7777"));
   BOOST_REQUIRE(clientSocket);

   basis::DataBlock request("Send to an non existant server");
   BOOST_REQUIRE_THROW(clientSocket->send(request), basis::RuntimeException);
}
