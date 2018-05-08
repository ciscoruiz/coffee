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

#include <coffee/logger/Logger.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/Socket.hpp>
#include <coffee/networking/SocketArguments.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Node.hpp>

using namespace coffee;

networking::Socket::Socket(networking::NetworkingService& networkingService, const SocketArguments& socketArguments, const int socketType) :
   m_endPoints(socketArguments.getEndPoints()),
   m_socketType(socketType)
{
   zmq::context_t& context = *networkingService.getContext();
   try {
      m_zmqSocket = std::make_shared<zmq::socket_t>(context, m_socketType);
   }
   catch (zmq::error_t& ex) {
      LOG_ERROR(asString() << ", Error=" << ex.what());
   }
}

networking::Socket::~Socket()
{
   m_zmqSocket->close();
   m_zmqSocket.reset();
}

void networking::Socket::Socket::bind()
   throw(basis::RuntimeException)
{
   if (m_endPoints.empty()) {
      COFFEE_THROW_EXCEPTION(this->asString() << " does not have any end point");
   }

   if (!m_zmqSocket) {
      COFFEE_THROW_EXCEPTION("Socket was not initialized successfully");
   }

   bool someWorks = false;

   for (auto& endPoint : m_endPoints) {
      try {
         m_zmqSocket->bind(endPoint.c_str());
         someWorks = true;
      }
      catch(const zmq::error_t& ex) {
         LOG_ERROR(this->asString() << ", Error=" << ex.what());
      }
   }

   if (!someWorks) {
      COFFEE_THROW_EXCEPTION(asString() << " does not have any valid end point");
   }
}

void networking::Socket::unbind()
   noexcept
{
   if (!m_zmqSocket)
      return;

   for (auto& endPoint : m_endPoints) {
      try {
         m_zmqSocket->unbind(endPoint.c_str());
      }
      catch (const zmq::error_t& ex) {
         LOG_WARN(asString() << ", Error=" << ex.what());
      }
   }
}

void networking::Socket::connect()
   throw(basis::RuntimeException)
{
   if (m_endPoints.empty()) {
      COFFEE_THROW_EXCEPTION(this->asString() << " does not have any end point");
   }

   if (!m_zmqSocket) {
      COFFEE_THROW_EXCEPTION("Socket was not initialized successfully");
   }

   try {
      for (auto& endPoint : m_endPoints) {
         m_zmqSocket->connect(endPoint.c_str());
      }
   }
   catch(zmq::error_t& ex) {
      COFFEE_THROW_EXCEPTION(asString () << ", Error=" << ex.what());
   }
}

void networking::Socket::disconnect()
   noexcept
{
   if (!m_zmqSocket)
      return;

   for (auto& endPoint : m_endPoints) {
      try {
         m_zmqSocket->disconnect(endPoint.c_str());
      }
      catch (zmq::error_t& ex) {
         LOG_WARN(asString() << ", Error=" << ex.what());
      }
   }
}

//virtual
basis::StreamString networking::Socket::asString() const
   noexcept
{
   basis::StreamString result("Socket {");

   result << "IsValid=" << isValid();
   result << ", Type=" << m_socketType;
   result << ", EndPoints=[";
   bool first = true;
   for (auto& ii : m_endPoints) {
      if (!first)
         result << ",";
      result << ii;
      first = false;
   }
   result << "]";


   return result << "}";
}

//virtual
std::shared_ptr<xml::Node> networking::Socket::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   auto xmlNode = parent->createChild("Socket");
   xmlNode->createAttribute("IsValid", basis::AsString::apply(isValid()));
   xmlNode->createAttribute("Type", m_socketType);

   auto xmlEndPoints = xmlNode->createChild("EndPoints");
   for (auto& endPoint : m_endPoints) {
      xmlEndPoints->createChild("EndPoint")->createText(endPoint);
   }

   return xmlNode;
}
