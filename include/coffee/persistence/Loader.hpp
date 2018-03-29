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

#ifndef __coffee_persistence_Loader_hpp
#define __coffee_persistence_Loader_hpp

#include <coffee/persistence/Accessor.hpp>

#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace dbms {
   class GuardStatement;
}

namespace persistence {



class Loader : public Accessor {
public:
   virtual ~Loader () {;}

   virtual dbms::ResultCode apply(dbms::GuardStatement& statement, TheObject& object)
      throw(basis::RuntimeException, dbms::DatabaseException) = 0;

   virtual bool hasToRefresh (dbms::GuardStatement& statement, TheObject& object)
      throw (basis::RuntimeException, dbms::DatabaseException) = 0;

   const TheClass& getClass() const noexcept { return m_class; }
   
protected:
   Loader (const char* name, TheStatement& statement, const ThePrimaryKey& primaryKey, const TheClass& clazz) :
      Accessor(name, statement, primaryKey),
      m_class(clazz)
   {;}
   
private:
   TheClass m_class;
};

} /* namespace persistence */
} /* namespace coffee */
#endif
