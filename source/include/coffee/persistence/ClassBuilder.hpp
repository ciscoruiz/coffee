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

#ifndef __coffee_persistence_ClassBuilder_hpp
#define __coffee_persistence_ClassBuilder_hpp

#include <memory>

#include <coffee/basis/RuntimeException.hpp>

#include <coffee/dbms/datatype/Set.hpp>

#include <coffee/persistence/Class.hpp>

namespace coffee {

namespace persistence {

class ClassBuilder : public dbms::datatype::Set {
public:
   explicit ClassBuilder(const std::string& className) : m_className (className) {;}
   
   ClassBuilder& set(std::shared_ptr<PrimaryKey>& primaryKey) throw (basis::RuntimeException);
   
   ClassBuilder& add(std::shared_ptr<dbms::datatype::Abstract> data) throw (basis::RuntimeException) {
      insert(data);
      return *this;
   }

   const std::string& getClassName() const noexcept { return m_className; }
   const std::shared_ptr<PrimaryKey>& getPrimaryKey() const noexcept { return m_primaryKey; }

   std::shared_ptr<Class> build() const throw (basis::RuntimeException);
   
private:
   const std::string m_className;
   std::shared_ptr<PrimaryKey> m_primaryKey;
};

}
}
#endif
