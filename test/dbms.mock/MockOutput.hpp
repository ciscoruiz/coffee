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

#ifndef __coffee_mock_MockOutput_hpp
#define __coffee_mock_MockOutput_hpp

#include <coffee/dbms/binder/Output.hpp>

namespace coffee {
namespace mock {

class MockOutput : public dbms::binder::Output {
public:
   explicit MockOutput(std::shared_ptr<dbms::datatype::Abstract>& abstract) : dbms::binder::Output(abstract) {;}

private:
   void do_prepare(dbms::Statement& statement, const int pos) throw(adt::RuntimeException, dbms::DatabaseException) {;}
   void do_release(dbms::Statement& statement) noexcept {;}
   void do_decode(dbms::Statement& statement, const int pos) throw(adt::RuntimeException) {;}
   void do_write(const std::shared_ptr<dbms::datatype::LongBlock>&) throw(adt::RuntimeException, dbms::DatabaseException) {;}
};

} /* namespace mock */
} /* namespace coffee */
#endif
