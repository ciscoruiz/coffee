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

struct ServerSocketTest : public NetworkingFixture {;};


TEST_F(ServerSocketTest, serversocket_service_on)
{
   ASSERT_TRUE(networkingService->isRunning());
}

TEST_F(ServerSocketTest, serversocket_server_on)
{
   ASSERT_TRUE(upperServer->isValid());
}

TEST_F(ServerSocketTest, serversocket_without_endpoints)
{
   networking::SocketArguments arguments;

   ASSERT_THROW(networkingService->createServerSocket(arguments.setMessageHandler(UpperStringHandler::instantiate())), basis::RuntimeException);
}

TEST_F(ServerSocketTest, serversocket_without_handler)
{
   networking::SocketArguments arguments;

   ASSERT_THROW(networkingService->createServerSocket(arguments.addEndPoint(upperServerEndPoint)), basis::RuntimeException);
}

TEST_F(ServerSocketTest, serversocket_bad_address)
{
   networking::SocketArguments arguments;

   arguments.addEndPoint("bad-address").setMessageHandler(UpperStringHandler::instantiate());
   ASSERT_THROW(networkingService->createServerSocket(arguments), basis::RuntimeException);
}
