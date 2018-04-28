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

#include <coffee/app/Application.hpp>
#include <coffee/basis/DataBlock.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/networking/ClientSocket.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/PublisherSocket.hpp>
#include <coffee/networking/ServerSocket.hpp>
#include <coffee/networking/SubscriberSocket.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Node.hpp>

using namespace coffee;

//static
const std::string networking::NetworkingService::Implementation("ZeroMQ");

// static
std::shared_ptr<networking::NetworkingService> networking::NetworkingService::instantiate(app::Application& application, const int zeroMQThreads)
   noexcept
{
   std::shared_ptr<NetworkingService> result(new NetworkingService(application, zeroMQThreads));
   application.attach(result);
   return result;
}

networking::NetworkingService::NetworkingService(app::Application &app, const int zeroMQThreads) :
   app::Service(app, app::Feature::Networking, Implementation)
{
   m_context = std::make_shared<zmq::context_t>(zeroMQThreads);
}

networking::NetworkingService::~NetworkingService()
{
   m_sockets.clear();
   m_serverSockets.clear();
   m_namedClientSockets.clear();
   m_subscriberSockets.clear();
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

   static const std::chrono::milliseconds timeout(100);

   while (networkingService.isStarting());

   std::shared_ptr<networking::NetworkingService::Poll> poll = std::make_shared<Poll>(networkingService);

   networkingService.notifyEffectiveRunning();

   while (networkingService.isRunning()) {
      zmq::message_t zmqMessage;

      int rpoll = zmq_poll(poll->m_items, poll->m_nitems, timeout.count());

      if (rpoll == 0) {
         if (poll->isOutdated()) {
            poll = std::make_shared<Poll>(networkingService);
         }
         continue;
      }

      if (rpoll == -1) {
         LOG_ERROR("Error=" << strerror(errno));
         break;
      }

      // See http://zguide.zeromq.org/cpp:mspoller
      for (size_t index = 0; index < poll->m_nitems; ++ index) {
         if (poll->m_items[index].revents & ZMQ_POLLIN) {
            auto iisocket = poll->m_asyncSockets.find(index);

            if (iisocket != poll->m_asyncSockets.end()) {
               auto socket = iisocket->second;
               socket->getZmqSocket()->recv(&zmqMessage);
               basis::DataBlock message((const char*) zmqMessage.data(), zmqMessage.size());
               try {
                  socket->handle(message);
               }
               catch(basis::RuntimeException& ex) {
                  logger::Logger::write(ex);
               }
            }
         }
      }

      if (poll->isOutdated()) {
         poll = std::make_shared<Poll>(networkingService);
      }
   }
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

std::shared_ptr<networking::PublisherSocket> networking::NetworkingService::createPublisherSocket(const SocketArguments& socketArguments)
   throw(basis::RuntimeException)
{
   std::shared_ptr<networking::PublisherSocket> result(new networking::PublisherSocket(*this, socketArguments));

   if (this->isRunning()) {
      LOG_DEBUG("Initializing " << result->asString());
      result->initialize();
   }

   m_sockets.push_back(result);

   return result;
}

std::shared_ptr<networking::SubscriberSocket> networking::NetworkingService::createSubscriberSocket(const networking::SocketArguments& socketArguments)
   throw(basis::RuntimeException)
{
   std::shared_ptr<networking::SubscriberSocket> result(new networking::SubscriberSocket(*this, socketArguments));

   if (this->isRunning()) {
      LOG_DEBUG("Initializing " << result->asString());
      result->initialize();
   }

   m_sockets.push_back(result);
   m_subscriberSockets.push_back(result);

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

std::shared_ptr<xml::Node> networking::NetworkingService::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("networking.Service");

   app::Service::asXML(result);

   auto sockets = result->createChild("Sockets");
   for (auto socket : m_sockets) {
      socket->asXML(sockets);
   }

   return result;
}

networking::NetworkingService::Poll::Poll(networking::NetworkingService& networkingService) :
   m_networkingService(networkingService),
   m_nitems(networkingService.m_serverSockets.size() + networkingService.m_subscriberSockets.size())
{
   m_items =  new zmq_pollitem_t[m_nitems];

   size_t ii = 0;

   for (auto serverSocket : networkingService.m_serverSockets) {
      coffee_memset(&m_items[ii], 0, sizeof(zmq_pollitem_t));
      zmq::socket_t* socket = serverSocket->getZmqSocket().get();
      m_items[ii].socket = (void*) *socket;
      m_items[ii].events = ZMQ_POLLIN;
      m_asyncSockets[ii ++] = serverSocket;
   }

   for (auto subscriberSocket : networkingService.m_subscriberSockets) {
      coffee_memset(&m_items[ii], 0, sizeof(zmq_pollitem_t));
      zmq::socket_t* socket = subscriberSocket->getZmqSocket().get();
      m_items[ii].socket = (void*) *socket;
      m_items[ii].events = ZMQ_POLLIN;
      m_asyncSockets[ii ++] = subscriberSocket;
   }

   LOG_DEBUG("Polling n-items=" << m_nitems);
}

networking::NetworkingService::Poll::~Poll()
{
   m_asyncSockets.clear();
   delete []m_items;
}

bool networking::NetworkingService::Poll::isOutdated() const
   noexcept
{
   return m_nitems != m_networkingService.m_serverSockets.size() + m_networkingService.m_subscriberSockets.size();
}
