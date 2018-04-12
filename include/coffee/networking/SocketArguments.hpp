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

#ifndef _coffee_networking_SocketArguments_hpp_
#define _coffee_networking_SocketArguments_hpp_

#include <vector>

namespace coffee {

namespace networking {

typedef std::vector<std::string> EndPoints;

class MessageHandler;

class SocketArguments {
public:
   SocketArguments() {;}
   ~SocketArguments() { m_endPoints.clear(); }

   SocketArguments& addEndPoint(const EndPoints::value_type& endPoint) noexcept { m_endPoints.push_back(endPoint); return *this; }
   SocketArguments& setName(const std::string& name) noexcept { m_name = name; return *this; }
   SocketArguments& setMessageHandler(const std::shared_ptr<MessageHandler> messageHandler) { m_messageHandler = messageHandler; return *this; }

   const EndPoints& getEndPoints() const noexcept { return m_endPoints; }
   const std::string& getName() const noexcept { return m_name; }
   std::shared_ptr<MessageHandler> getMessageHandler() const noexcept { return m_messageHandler; }

private:
   EndPoints m_endPoints;
   std::shared_ptr<MessageHandler> m_messageHandler;
   std::string m_name;
};

}
}

#endif //_coffee_networking_SocketArguments_hpp_
