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
#ifndef _coffee_networking_MessageHandler_hpp_
#define _coffee_networking_MessageHandler_hpp_

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/DataBlock.hpp>
#include <coffee/basis/NamedObject.hpp>

namespace coffee {

namespace networking {

class Socket;
class NetworkingService;

class MessageHandler : public basis::NamedObject {
protected:
public:
   explicit MessageHandler(const std::string &name) : NamedObject(name) {}

protected:
   virtual void apply(const basis::DataBlock& message, Socket& socket) throw(basis::RuntimeException) = 0;

   friend class NetworkingService;
};

}
}
#endif //_coffee_networking_MessageHandler_hpp_
