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

#ifndef __coffee_balance_Resource_hpp
#define __coffee_balance_Resource_hpp

#include <memory>

#include <coffee/basis/NamedObject.hpp>
#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace balance {

/**
 * Generic resource to be shared by the different strategies.
 *
 * \include test/balance/TestResource.hpp
 */
class Resource : public basis::NamedObject {
public:
   /**
    * Destructor
    */
   virtual ~Resource () {;}

   /**
    * Initialize the resource
    */
   virtual void initialize () throw (basis::RuntimeException) {;}

   /**
    * \return \b true is the resource is available or \b false otherwise.
    */
   virtual bool isAvailable () const noexcept = 0;

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   operator basis::StreamString () const noexcept { return asString (); }

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   virtual basis::StreamString asString () const noexcept;

   /**
    * \return Summarize information of this instance in a coffee::xml::Node.
    */
   virtual std::shared_ptr<xml::Node> asXML (std::shared_ptr<xml::Node>& parent) const throw(basis::RuntimeException);

protected:
   explicit Resource(const std::string& name) : basis::NamedObject(name) {;}

};

} /* namespace balance */
} /* namespace coffee */
#endif

