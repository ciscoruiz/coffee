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

#include <csignal>

#include <coffee/networking/ClientSocket.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>
#include <coffee/logger/Logger.hpp>

#include "NetworkingFixture.hpp"

using namespace coffee;

BOOST_FIXTURE_TEST_CASE(networking_single_ip, NetworkingFixture)
{
   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555"));
      BOOST_REQUIRE(clientSocket);
      basis::DataBlock request("work");
      auto response = clientSocket->send(request);
      BOOST_REQUIRE_EQUAL(std::string(response.data()), "WORK");
   }

   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://127.0.0.1:5556"));
      BOOST_REQUIRE(clientSocket);
      basis::DataBlock request("this is other");
      auto response = clientSocket->send(request);
      BOOST_REQUIRE_EQUAL(std::string(response.data()), "THIS IS OTHER");
   }
}

BOOST_FIXTURE_TEST_CASE(networking_multiple_ip, NetworkingFixture)
{
   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555").addEndPoint("tcp://localhost:5556"));
   BOOST_REQUIRE(clientSocket);

   basis::DataBlock request("work");
   auto response = clientSocket->send(request);
   BOOST_REQUIRE_EQUAL(std::string(response.data()), "WORK");

   {
      basis::DataBlock request("this is other");
      auto response = clientSocket->send(request);
      BOOST_REQUIRE_EQUAL(std::string(response.data()), "THIS IS OTHER");
   }
}

BOOST_FIXTURE_TEST_CASE(networking_ipv6, NetworkingFixture)
{
   {
      networking::SocketArguments arguments;
      arguments.setMessageHandler(UpperStringHandler::instantiate()).addEndPoint("tcp://[::1]:5557").activateIPv6();
      auto ipv6Server = networkingService->createServerSocket(arguments);
      BOOST_REQUIRE(ipv6Server);
   }

   // To give time to NetworkingService to detect new server socket
   usleep(100000);

   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://[0:0:0:0:0:0:0:1]:5557").activateIPv6());
   BOOST_REQUIRE(clientSocket);

   basis::DataBlock request("work");
   auto response = clientSocket->send(request);
   BOOST_REQUIRE_EQUAL(std::string(response.data()), "WORK");
}

class MuteHandler : public coffee::networking::MessageHandler {
public:
   MuteHandler() : coffee::networking::MessageHandler("MuteHandler") {;}

   static std::shared_ptr<MuteHandler> instantiate() { return std::make_shared<MuteHandler>(); }

protected:
   void apply(const coffee::basis::DataBlock& message, coffee::networking::AsyncSocket& serverSocket)
      throw(coffee::basis::RuntimeException) { LOG_DEBUG("Ignore message"); }
};

BOOST_FIXTURE_TEST_CASE(networking_mute_server, NetworkingFixture) {
   {
      networking::SocketArguments arguments;
      arguments.setMessageHandler(MuteHandler::instantiate()).addEndPoint("tcp://[::1]:5557").activateIPv6();
      auto ipv6Server = networkingService->createServerSocket(arguments);
      BOOST_REQUIRE(ipv6Server);
   }

   // To give time to NetworkingService to detect new server socket
   usleep(100000);

   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://[0:0:0:0:0:0:0:1]:5557").activateIPv6());
   BOOST_REQUIRE(clientSocket);

   try {
      basis::DataBlock request("work");
      clientSocket->send(request);
      BOOST_REQUIRE(false);
   }
   catch(const basis::RuntimeException& ex) {
      BOOST_REQUIRE(ex.asString().find("did not receive any response"));
   }
}

BOOST_FIXTURE_TEST_CASE(networking_create_new_server, NetworkingFixture)
{
   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555"));
      BOOST_REQUIRE(clientSocket);
      basis::DataBlock request("work");
      auto response = clientSocket->send(request);
      BOOST_REQUIRE_EQUAL(std::string(response.data()), "WORK");
   }

   networking::SocketArguments arguments;
   arguments.setMessageHandler(LowerStringHandler::instantiate()).addEndPoint("tcp://*:6666");
   auto newServerSocket = networkingService->createServerSocket(arguments);
   BOOST_REQUIRE(newServerSocket->isValid());

   // To give time to NetworkingService to detect new server socket
   usleep(100000);

   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:6666"));
      BOOST_REQUIRE(clientSocket);
      basis::DataBlock request("This is the NEW server");
      auto response = clientSocket->send(request);
      BOOST_REQUIRE_EQUAL(std::string(response.data()), "this is the new server");
   }
}

BOOST_FIXTURE_TEST_CASE(networking_large_message, NetworkingFixture)
{
   networking::SocketArguments arguments;
   arguments.setMessageHandler(EchoHandler::instantiate()).addEndPoint("tcp://*:6667");
   auto echoServer = networkingService->createServerSocket(arguments);
   BOOST_REQUIRE(echoServer);

   // To give time to NetworkingService to detect new server socket
   usleep(100000);

   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:6667"));
      BOOST_REQUIRE(clientSocket);

      static const int largeSize = 16384;

      char* memory = new char[largeSize];

      basis::DataBlock request(memory, largeSize);
      auto response = clientSocket->send(request);
      BOOST_REQUIRE_EQUAL(response.size(), largeSize);
      for(int ii = 0; ii < largeSize; ++ ii) {
         BOOST_REQUIRE(memory[ii] == response[ii]);
      }

      delete []memory;
   }
}

BOOST_FIXTURE_TEST_CASE(networking_write_xml, NetworkingFixture)
{
   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555").addEndPoint("tcp://localhost:5556"));
      BOOST_REQUIRE(clientSocket);
   }
   {
      networking::SocketArguments arguments;
      auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555"));
      BOOST_REQUIRE(clientSocket);
   }

   app.setOutputContextFilename("source/test/networking/context.xml");

   std::raise(SIGUSR1);

   xml::Document document;

   BOOST_CHECK_NO_THROW(document.parse(app.getOutputContextFilename()));

   auto root = document.getRoot();

   auto app = root->lookupChild("app.Application");
   auto services = app->lookupChild("Services");

   auto node = services->childAt(0);
   BOOST_REQUIRE(node);

   node = node->lookupChild("app.Service");
   BOOST_REQUIRE(node);

   BOOST_CHECK_EQUAL(node->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "Networking:ZeroMQ");
}
