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

#ifndef _coffee_dbms_binder_Binder_h
#define _coffee_dbms_binder_Binder_h

#include <functional>
#include <memory>

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/DatabaseException.hpp>

namespace coffee {

namespace dbms {

namespace datatype {
   class Abstract;
}

class Connection;
class Statement;

namespace binder {

/**
 * Bind one C++ variable to the some other DBMS variable.
 *
 * \include test/dbms/Definition_test.cc
 */
class Binder {
public:
   /**
    * Destructor.
    */
   virtual ~Binder() { m_data.reset(); }

   /**
    * \return the C++ variable bond to this instance.
    */
   std::shared_ptr<datatype::Abstract>& getData() noexcept { return std::ref(m_data); }

   /**
    * \return the C++ variable bond to this instance.
    */
   const std::shared_ptr<datatype::Abstract>& getData() const noexcept { return std::ref(m_data); }

protected:
   /**
    * Constructor.
    */
   explicit Binder(std::shared_ptr<datatype::Abstract>& data) : m_data(data) {;}

   /**
    * \return Summarize information of this instance in a coffee::adt::StreamString.
    */
   virtual adt::StreamString asString() const noexcept = 0;

private:
   std::shared_ptr<datatype::Abstract> m_data;

   virtual void do_prepare(Statement& statement, const int pos) throw(adt::RuntimeException, DatabaseException) = 0;

   friend class coffee::dbms::Statement;
};

}
}
}

#endif

