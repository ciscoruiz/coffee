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

#include <zmq.hpp>

#include <coffee/basis/DataBlock.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/app/Application.hpp>

#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/ServerSocket.hpp>
#include <coffee/networking/ClientSocket.hpp>
#include <coffee/networking/SocketArguments.hpp>
#include <coffee/networking/MessageHandler.hpp>

using namespace coffee;

// static
std::shared_ptr<networking::NetworkingService> networking::NetworkingService::instantiate(app::Application& application, const int zeroMQThreads)
   noexcept
{
   std::shared_ptr<NetworkingService> result(new NetworkingService(application, zeroMQThreads));
   application.attach(result);
   return result;
}

networking::NetworkingService::NetworkingService(app::Application &app, const int zeroMQThreads) :
   app::Service(app, "networking.NetworkingService")
{
   m_context = std::make_shared<zmq::context_t>(zeroMQThreads);
}

networking::NetworkingService::~NetworkingService()
{
   m_sockets.clear();
   m_serverSockets.clear();
   m_namedClientSockets.clear();
}

void networking::NetworkingService::do_initialize()
   throw(basis::RuntimeException)
{
   for (auto socket : m_sockets) {
      LOG_DEBUG("Initializing " << socket->asString());
      socket->initialize();
   }

   m_broker = std::thread(broker, std::ref(*this));
}

//static
void networking::NetworkingService::broker(NetworkingService& networkingService)
   noexcept
{
   LOG_THIS_METHOD();

   while (networkingService.isStarting());

   zmq::pollitem_t* items = createPollItems(networkingService);
   size_t nitems = networkingService.m_serverSockets.size();
   const std::chrono::milliseconds timeout(250);

   networkingService.notifyEffectiveRunning();

   while (networkingService.isRunning()) {
      zmq::message_t message;

      LOG_DEBUG("Polling n-items=" << nitems);
      int rpoll = zmq_poll(items, nitems, timeout.count());

      if (rpoll == 0)
         continue;

      if (rpoll == -1) {
         LOG_ERROR("Error=" << strerror(errno));
         break;
      }

      // See http://zguide.zeromq.org/cpp:mspoller
      for (size_t index = 0; index < nitems; ++ index) {
         if (items[index].revents & ZMQ_POLLIN) {
            auto socket = networkingService.m_serverSockets[index];
            auto zmqSocket = socket->getZmqSocket();
            zmqSocket->recv(&message);
            basis::DataBlock ww((const char*) message.data(), message.size());
            socket->getMessageHandler()->apply(ww, *socket);
         }
      }

      if (networkingService.m_serverSockets.size() != nitems) {
         delete [] items;
         items = createPollItems(networkingService);
         nitems = networkingService.m_serverSockets.size();
      }
   }

   delete [] items;
}

void networking::NetworkingService::do_stop()
   throw(basis::RuntimeException)
{
   LOG_THIS_METHOD();

   statusStopped();

   LOG_DEBUG("Waiting for termination of networking broker ...");
   m_broker.join();
   LOG_DEBUG("Termination of networking broker is done");

   for (auto socket : m_sockets) {
      LOG_DEBUG("Destroying " << socket->asString());
      socket->destroy();
   }
}

std::shared_ptr<networking::ServerSocket> networking::NetworkingService::createServerSocket(const networking::SocketArguments& socketArguments)
   throw(basis::RuntimeException)
{
   std::shared_ptr<networking::ServerSocket> result(new networking::ServerSocket(*this, socketArguments));

   if (this->isRunning()) {
      LOG_DEBUG("Initializing " << result->asString());
      result->initialize();
   }

   m_sockets.push_back(result);
   m_serverSockets.push_back(result);

   return result;
}

std::shared_ptr<networking::ClientSocket> networking::NetworkingService::createClientSocket(const networking::SocketArguments& socketArguments)
   throw(basis::RuntimeException)
{
   std::shared_ptr<networking::ClientSocket> result(new networking::ClientSocket(*this, socketArguments));

   if (this->isRunning()) {
      LOG_DEBUG("Initializing " << result->asString());
      result->initialize();
   }

   m_sockets.push_back(result);

   const std::string& name = socketArguments.getName();

   if (!name.empty()) {
      m_namedClientSockets[name] = result;
   }

   return result;
}

std::shared_ptr<networking::ClientSocket> networking::NetworkingService::findClientSocket(const std::string& name)
   throw(basis::RuntimeException)
{
   auto ii = m_namedClientSockets.find(name);

   if (ii == m_namedClientSockets.end()) {
      COFFEE_THROW_EXCEPTION("Client socket named as '" << name << " is not defined");
   }

   return ii->second;
}

zmq_pollitem_t* networking::NetworkingService::createPollItems(networking::NetworkingService &broker) {
   const size_t maxii = broker.m_serverSockets.size();
   zmq_pollitem_t* result = new zmq_pollitem_t[maxii];

   for (size_t ii = 0; ii < maxii; ++ ii) {
      coffee_memset(&result[ii], 0, sizeof(zmq_pollitem_t));
      zmq::socket_t* socket = broker.m_serverSockets[ii]->getZmqSocket().get();
      result[ii].socket = (void*) *socket;
      result[ii].events = ZMQ_POLLIN;
   }

   return result;
}
