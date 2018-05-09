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

#ifndef _coffee_dbms_bind_Input_h
#define _coffee_dbms_bind_Input_h

#include "Binder.hpp"

namespace coffee {

namespace dbms {

namespace binder {

/**
 * Input binder for a c++ variable which value will be received for some DBMS variable.
 *
 * \include test/dbms/Definition_test.cc
 */
class Input : public Binder {
public:
   /**
    * Define how to transfer information from the c++ scope to the DBMS scope.
    */
   virtual void do_encode(Statement& statement, const int pos) throw(basis::RuntimeException, DatabaseException) = 0;

   /**
    * \return Summarize information of this instance in a coffee::basis::StreamString.
    */
   basis::StreamString asString() const noexcept;

protected:
   /**
    * Constructor.
    * \param value Variable which will be use to populate a DBMS variable
    */
   explicit Input(std::shared_ptr<datatype::Abstract>& value) : Binder(value) {;}
};

}
}
}

#endif

