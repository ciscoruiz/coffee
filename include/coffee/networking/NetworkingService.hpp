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
#ifndef _coffee_networking_MessageBroker_hpp_
#define _coffee_networking_MessageBroker_hpp_

#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>

#include <zmq.hpp>

#include <coffee/app/Service.hpp>
#include <coffee/basis/NamedObject.hpp>

namespace coffee {

namespace networking {

class Socket;
class SocketArguments;
class ClientSocket;

class NetworkingService : public app::Service {
public:
   ~NetworkingService();

   static std::shared_ptr<NetworkingService> instantiate(app::Application& app, const int zeroMQThreads = 1) noexcept;

   std::shared_ptr<zmq::context_t>& getContext() noexcept { return m_context; }

   std::shared_ptr<networking::Socket> createServerSocket(const SocketArguments& socketArguments) throw(basis::RuntimeException);
   std::shared_ptr<networking::ClientSocket> createClientSocket(const SocketArguments& socketArguments) throw(basis::RuntimeException);
   std::shared_ptr<networking::ClientSocket> findClientSocket(const std::string& name) throw(basis::RuntimeException);

private:
   typedef std::vector<std::shared_ptr<Socket> > Sockets;
   typedef std::unordered_map<std::string, std::shared_ptr<ClientSocket> > ClientSockets;

   std::shared_ptr<zmq::context_t> m_context;
   Sockets m_sockets;
   ClientSockets m_clientSockets;
   std::thread m_broker;

   NetworkingService(app::Application &app, const int zeroMQThreads);
   void do_initialize() throw(basis::RuntimeException);
   static void broker(NetworkingService& networkingService) noexcept;
   void do_stop() throw(basis::RuntimeException);

   static zmq_pollitem_t *createPollItems(NetworkingService &broker);
};

}
}

#endif //_coffee_networking_MessageBroker_hpp_
