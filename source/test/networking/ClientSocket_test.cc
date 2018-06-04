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
#include <coffee/networking/ClientSocket.hpp>

#include "NetworkingFixture.hpp"

using namespace coffee;

TEST_F(NetworkingFixture, clientsocket_service_on)
{
   ASSERT_TRUE(networkingService->isRunning());
}

TEST_F(NetworkingFixture, clientsocket_server_on)
{
   ASSERT_TRUE(upperServer->isValid());
}

TEST_F(NetworkingFixture, clientsocket_without_endpoints)
{
   networking::SocketArguments arguments;

   ASSERT_THROW(networkingService->createClientSocket(arguments), basis::RuntimeException);
}

TEST_F(NetworkingFixture, clientsocket_without_handler)
{
   networking::SocketArguments arguments;
   arguments.addSubscription("12345");
   ASSERT_NO_THROW(networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555")));
}

TEST_F(NetworkingFixture, clientsocket_bad_address)
{
   networking::SocketArguments arguments;
   arguments.addSubscription("12345");
   ASSERT_THROW(networkingService->createClientSocket(arguments.addEndPoint("bad-address")), basis::RuntimeException);
}

TEST_F(NetworkingFixture, clientsocket_not_found)
{
   ASSERT_THROW(networkingService->findClientSocket("some-name"), basis::RuntimeException);
}

TEST_F(NetworkingFixture, clientsocket_found)
{
   std::string ips[] = { "tcp://localhost:5555", "tcp://127.0.0.1:5556"};

   {
      networking::SocketArguments arguments;
      arguments.setName("first").addEndPoint(ips[0]);
      auto clientSocket = networkingService->createClientSocket(arguments);
      ASSERT_TRUE(clientSocket != nullptr);
   }

   {
      networking::SocketArguments arguments;
      arguments.setName("second").addEndPoint(ips[1]);
      auto clientSocket = networkingService->createClientSocket(arguments);
      ASSERT_TRUE(clientSocket != nullptr);
   }

   auto clientSocket = networkingService->findClientSocket("first");
   ASSERT_EQ(ips[0], clientSocket->getEndPoints().at(0));

   clientSocket = networkingService->findClientSocket("second");
   ASSERT_EQ(ips[1], clientSocket->getEndPoints().at(0));
}

TEST_F(NetworkingFixture, clientsocket_unkwown_server)
{
   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:7777"));

   basis::DataBlock request("Send to an non existant server");
   ASSERT_THROW(clientSocket->send(request), basis::RuntimeException);
}
