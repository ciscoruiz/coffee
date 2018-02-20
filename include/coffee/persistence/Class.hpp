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

#ifndef __coffee_persistence_Class_hpp
#define __coffee_persistence_Class_hpp

#include <mutex>
#include <memory>
#include <vector>

#include <coffee/adt/NamedObject.hpp>
#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>
#include <coffee/dbms/datatype/Set.hpp>

namespace coffee {

namespace xml {
   class Node;
}

namespace persistence {

class Object;
class Accessor;
class ClassBuilder;
class PrimaryKey;

class Class : public adt::NamedObject {
public:
   explicit Class(const ClassBuilder& classBuilder);

   virtual ~Class() {;}

   adt::StreamString asString() const noexcept;
      
   std::shared_ptr<PrimaryKey> createPrimaryKey() const throw ();
   std::shared_ptr<Object> createObject(const std::shared_ptr<PrimaryKey>& primaryKey) const throw(adt::RuntimeException);

   std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   Class(const Class&) = delete;

private:
   std::shared_ptr<PrimaryKey> m_primaryKey;
   dbms::datatype::Set m_members;

};

} /* namespace persistence */
} /* namespace coffee */
#endif
