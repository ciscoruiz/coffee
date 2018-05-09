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

#ifndef __coffee_persistence_Recorder_hpp
#define __coffee_persistence_Recorder_hpp

#include <coffee/persistence/Accessor.hpp>
#include <coffee/persistence/Object.hpp>

#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace dbms {
   class GuardStatement;
}
namespace persistence {

class Object;

class Recorder : public Accessor {
public:
   virtual ~Recorder () {;}

   const Accessor::TheObject& getObject() const noexcept { return m_object; }

   virtual dbms::ResultCode apply(dbms::GuardStatement& statement)
      throw(basis::RuntimeException, dbms::DatabaseException) = 0;

protected:
   Recorder (const char* name, TheStatement& statement, const Accessor::TheObject& object) :
      Accessor(name, statement, object->getPrimaryKey()),
      m_object(object)
   {;}

private:
   Accessor::TheObject m_object;
};

} /* namespace persistence */
} /* namespace coffee */
#endif
