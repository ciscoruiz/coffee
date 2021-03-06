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
#ifndef _coffee_networking_Socket_hpp_
#define _coffee_networking_Socket_hpp_

#include <memory>
#include <zmq.hpp>

#include <coffee/basis/StreamString.hpp>
#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/DataBlock.hpp>

#include <coffee/networking/SocketArguments.hpp>

namespace coffee {

namespace xml {
   class Node;
}

namespace networking {

class NetworkingService;

class Socket {
public:
   virtual ~Socket();

   bool isValid() const noexcept { return (bool) m_zmqSocket; }

   const EndPoints& getEndPoints() const noexcept { return m_endPoints; }

   virtual basis::StreamString asString() const noexcept;

protected:
   Socket(NetworkingService &networkingService, const SocketArguments& socketArguments, const int socketType);

   std::shared_ptr<zmq::socket_t>& getZmqSocket() { return m_zmqSocket; }

   /**
    * This method will be called from MessageBroker once the socket is created and MessageBroker is
    * prepared to work
    */
   virtual void initialize() throw(basis::RuntimeException) = 0;

   /**
    * This method will be called from MessageBroker once the socket is created and MessageBroker is
    * prepared to work
    */
   virtual void destroy() noexcept = 0;

   void bind() throw(basis::RuntimeException);
   void unbind() noexcept ;
   void connect() throw(basis::RuntimeException);
   void disconnect() noexcept ;

   /**
    * \return Summarize information of this instance in a coffee::xml::Node.
    */
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const throw(basis::RuntimeException);

protected:
   std::shared_ptr<zmq::socket_t> m_zmqSocket;

private:
   const EndPoints m_endPoints;
   const int m_socketType;

   friend class NetworkingService;
};

}
}

#endif //_coffee_networking_Socket_hpp_
