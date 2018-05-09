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

#ifndef _wepm_dbms_ErrorCodeInterpreter_h
#define _wepm_dbms_ErrorCodeInterpreter_h

#include <string>
#include <memory>

#include <string.h>

#include <stdlib.h>

#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace dbms {

class Database;

/**
 * This class should translate numeric values returned by the RDBMS into
 * concepts or states that should be evaluated for our application.
 */
class ErrorCodeInterpreter {
public:
   virtual ~ErrorCodeInterpreter() {;}

   /**
    * \return \b true if the search conditions was not find any record or \b false otherwise.
    */
   virtual bool notFound(const int errorCode) const throw(basis::RuntimeException) = 0;
   
   /**
    * \return \b true if the sentence was run successfully or \b false otherwise.
    */
   virtual bool successful(const int errorCode) const throw(basis::RuntimeException) = 0;

   /**
    * \return \b true if the sentence fault due to inter-locked detection or \b false otherwise.
    */
   virtual bool locked(const int errorCode) const throw(basis::RuntimeException) = 0;

   /**
    * \return \b true if the sentence fault due to lost connection or \b false otherwise.
    */
   virtual bool lostConnection(const int errorCode) const throw(basis::RuntimeException) = 0;
};

}
}

#endif

