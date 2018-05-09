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

#include <coffee/balance/Resource.hpp>

#include <coffee/basis/StreamString.hpp>
#include <coffee/basis/AsString.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

using namespace coffee;

//virtual
basis::StreamString balance::Resource::asString() const
   noexcept
{
   basis::StreamString result("balance::Resource {");
   result += basis::NamedObject::asString();
   result += " } | Available = ";
   result += basis::AsString::apply(isAvailable());
   return result.append("}");
}

//virtual
std::shared_ptr<xml::Node> balance::Resource::asXML(std::shared_ptr<xml::Node>& parent) const
   throw(basis::RuntimeException)
{
   std::shared_ptr<xml::Node> result = parent->createChild("balance.Resource");
   result->createAttribute("Name", this->getName());
   result->createAttribute("IsAvailable", isAvailable());
   return result;
}
