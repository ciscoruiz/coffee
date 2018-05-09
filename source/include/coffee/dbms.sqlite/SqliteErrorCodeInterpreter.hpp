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

#ifndef _coffee_dbms_sqlite_SqliteErrorCodeInterpreter_hpp_
#define _coffee_dbms_sqlite_SqliteErrorCodeInterpreter_hpp_

#include <coffee/dbms/ErrorCodeInterpreter.hpp>

namespace coffee {
namespace dbms {
namespace sqlite {

class SqliteErrorCodeInterpreter : public ErrorCodeInterpreter {
public:
   SqliteErrorCodeInterpreter() {;}

private:
   bool notFound(const int errorCode) const throw(basis::RuntimeException);
   bool successful(const int errorCode) const throw(basis::RuntimeException);
   bool locked(const int errorCode) const throw(basis::RuntimeException);
   bool lostConnection(const int errorCode) const throw(basis::RuntimeException);
};

} /* namespace sqlite */
} /* namespace dbms */
} /* namespace coffee */

#endif
