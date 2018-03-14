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

#ifndef _coffee_dbms_sqlite_SqliteOutputBinder_hpp_
#define _coffee_dbms_sqlite_SqliteOutputBinder_hpp_

#include <chrono>

#include <coffee/dbms/binder/Output.hpp>

namespace coffee {
namespace dbms {
namespace sqlite {

class SqliteOutputBinder: public binder::Output {
public:
   explicit SqliteOutputBinder(std::shared_ptr<datatype::Abstract>& value) : binder::Output(value) {;}
   virtual ~SqliteOutputBinder() {;}

private:
   void do_prepare(Statement& statement, const int pos) throw(adt::RuntimeException, DatabaseException) {;}
   void do_decode(Statement& statement, const int pos) throw(adt::RuntimeException);
   void do_write(const std::shared_ptr<datatype::LongBlock>&) throw(adt::RuntimeException, dbms::DatabaseException) {;}

   static std::chrono::seconds getSeconds(const std::string& value) throw (adt::RuntimeException);
};

} /* namespace sqlite */
} /* namespace dbms */
} /* namespace coffee */

#endif
