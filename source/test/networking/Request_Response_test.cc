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

#include <csignal>

#include <coffee/networking/ClientSocket.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>
#include <coffee/logger/Logger.hpp>
#include <iostream>

#include "NetworkingFixture.hpp"

using namespace coffee;

struct RequestResponseTest : public NetworkingFixture {;};

TEST_F(RequestResponseTest, single_ip)
{
   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555"));
      basis::DataBlock request("work");
      auto response = clientSocket->send(request);
      ASSERT_EQ("WORK", std::string(response.data()));
   }

   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://127.0.0.1:5556"));
      basis::DataBlock request("this is other");
      auto response = clientSocket->send(request);
      ASSERT_EQ("THIS IS OTHER", std::string(response.data()));
   }
}

TEST_F(RequestResponseTest, multiple_ip)
{
   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555").addEndPoint("tcp://localhost:5556"));

   basis::DataBlock request("work");
   auto response = clientSocket->send(request);
   ASSERT_EQ("WORK", std::string(response.data()));

   {
      basis::DataBlock request("this is other");
      auto response = clientSocket->send(request);
      ASSERT_EQ("THIS IS OTHER", std::string(response.data()));
   }
}

TEST_F(RequestResponseTest, ipv6)
{
   // In next version of BOOST unit test there is a better way to do this, but we are limited
   // by the version used in TravisCI Boost 1.54.

   const char* testValue = std::getenv("TEST");

   if (testValue != nullptr && coffee_strcmp(testValue, "Codecov") == 0) {
      std::cout << "TravisCI does not offer support for IPv6 networking, case ipv6 can not be applied" << std::endl << std::endl;
      return;
   }

   {
      networking::SocketArguments arguments;
      ASSERT_NO_THROW(arguments.setMessageHandler(UpperStringHandler::instantiate()).addEndPoint("tcp://[::1]:5557").activateIPv6());
      auto ipv6Server = networkingService->createServerSocket(arguments);
      ASSERT_TRUE(ipv6Server != nullptr);
   }

   // To give time to NetworkingService to detect new server socket
   usleep(100000);

   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://[0:0:0:0:0:0:0:1]:5557").activateIPv6());
   ASSERT_TRUE(clientSocket != nullptr);

   basis::DataBlock request("work");
   auto response = clientSocket->send(request);
   ASSERT_EQ("WORK", std::string(response.data()));
}

class MuteHandler : public coffee::networking::MessageHandler {
public:
   MuteHandler() : coffee::networking::MessageHandler("MuteHandler") {;}

   static std::shared_ptr<MuteHandler> instantiate() { return std::make_shared<MuteHandler>(); }

protected:
   void apply(const coffee::basis::DataBlock& message, coffee::networking::AsyncSocket& serverSocket)
      throw(coffee::basis::RuntimeException) { LOG_DEBUG("Ignore message"); }
};

TEST_F(RequestResponseTest, mute_server) {
   {
      networking::SocketArguments arguments;
      ASSERT_NO_THROW(arguments.setMessageHandler(MuteHandler::instantiate()).addEndPoint("tcp://127.0.0.1:5557"));
      auto ipv6Server = networkingService->createServerSocket(arguments);
      ASSERT_TRUE(ipv6Server != nullptr);
   }

   // To give time to NetworkingService to detect new server socket
   usleep(100000);

   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://127.0.0.1:5557"));
   ASSERT_TRUE(clientSocket != nullptr);

   try {
      basis::DataBlock request("work");
      clientSocket->send(request);
      ASSERT_TRUE(false);
   }
   catch(const basis::RuntimeException& ex) {
      ASSERT_TRUE(ex.asString().find("did not receive any response"));
   }
}

TEST_F(RequestResponseTest, create_new_server)
{
   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555"));
      ASSERT_TRUE(clientSocket != nullptr);
      basis::DataBlock request("work");
      auto response = clientSocket->send(request);
      ASSERT_EQ("WORK", std::string(response.data()));
   }

   networking::SocketArguments arguments;
   arguments.setMessageHandler(LowerStringHandler::instantiate()).addEndPoint("tcp://*:6666");
   auto newServerSocket = networkingService->createServerSocket(arguments);
   ASSERT_TRUE(newServerSocket->isValid());

   // To give time to NetworkingService to detect new server socket
   usleep(100000);

   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:6666"));
      ASSERT_TRUE(clientSocket != nullptr);
      basis::DataBlock request("This is the NEW server");
      auto response = clientSocket->send(request);
      ASSERT_EQ("this is the new server", std::string(response.data()));
   }
}

TEST_F(RequestResponseTest, large_message)
{
   networking::SocketArguments arguments;
   arguments.setMessageHandler(EchoHandler::instantiate()).addEndPoint("tcp://*:6667");
   auto echoServer = networkingService->createServerSocket(arguments);
   ASSERT_TRUE(echoServer != nullptr);

   // To give time to NetworkingService to detect new server socket
   usleep(100000);

   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:6667"));
      ASSERT_TRUE(clientSocket != nullptr);

      static const int largeSize = 16384;

      char* memory = new char[largeSize];

      basis::DataBlock request(memory, largeSize);
      auto response = clientSocket->send(request);
      ASSERT_EQ(largeSize, response.size());
      for(int ii = 0; ii < largeSize; ++ ii) {
         ASSERT_TRUE(memory[ii] == response[ii]);
      }

      delete []memory;
   }
}

TEST_F(RequestResponseTest, write_xml)
{
   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555").addEndPoint("tcp://localhost:5556"));
      ASSERT_TRUE(clientSocket != nullptr);
   }
   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555"));
      ASSERT_TRUE(clientSocket != nullptr);
   }

   app.setOutputContextFilename("source/test/networking/context.xml");

   std::raise(SIGUSR1);

   xml::Document document;

   ASSERT_NO_THROW(document.parse(app.getOutputContextFilename()));

   auto root = document.getRoot();

   auto app = root->lookupChild("app.Application");
   auto services = app->lookupChild("Services");

   auto node = services->childAt(0);
   ASSERT_TRUE(node != nullptr);

   node = node->lookupChild("app.Service");
   ASSERT_TRUE(node != nullptr);

   ASSERT_EQ("Networking:ZeroMQ", node->lookupChild("Runnable")->lookupAttribute("Name")->getValue());
}
