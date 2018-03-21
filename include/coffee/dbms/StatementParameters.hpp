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
#ifndef _coffee_dbms_StatementParameter_h
#define _coffee_dbms_StatementParameter_h

#include <coffee/adt/StreamString.hpp>
#include <coffee/dbms/ActionOnError.hpp>

namespace coffee {

namespace dbms {

class StatementParameters {
public:
   StatementParameters() : actionOnError(ActionOnError::Rollback) {;}
   explicit StatementParameters(const ActionOnError::_v _actionOnError ) : actionOnError(_actionOnError) {;}
   virtual ~StatementParameters() { ; }

   const ActionOnError::_v getActionOnError() const noexcept { return actionOnError; }

   virtual adt::StreamString asString() const noexcept {
      adt::StreamString result("dbms.StatementParameters { ");
      result << "ActionOnError=" << (actionOnError == ActionOnError::Ignore ? "Ignore": "Rollback");
      return result << " }";
   }

private:
   const ActionOnError::_v actionOnError;
};


}
}


#endif /* _coffee_dbms_StatementParameter_h */
