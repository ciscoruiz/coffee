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

#ifndef __coffee_persistence_Object_hpp
#define __coffee_persistence_Object_hpp

#include <functional>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/datatype/Set.hpp>

namespace coffee {
   
namespace persistence {

class PrimaryKey;
class Class;


//// See http://stackoverflow.com/questions/1319876/weird-c-templating-issues
//template <typename _T> class AutoObject;

class Object : public dbms::datatype::Set {
public:
   Object(const Class& _class, const std::shared_ptr<PrimaryKey>& primaryKey, const dbms::datatype::Set& members);

   ~Object() {
       m_primaryKey.reset();
   }

   const Class& getClass() const noexcept { return std::ref(m_class); }
   const std::shared_ptr<PrimaryKey>& getPrimaryKey() const throw(adt::RuntimeException) { return m_primaryKey; }

   std::string getInternalId() const noexcept;
   
   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;

private:
   const Class& m_class;
   std::shared_ptr<PrimaryKey> m_primaryKey;
};

} /* namespace persistence */
} /* namespace coffee */
#endif
