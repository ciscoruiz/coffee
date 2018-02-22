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

#ifndef __coffee_adt_Exception_hpp
#define __coffee_adt_Exception_hpp

#include <stdexcept>

namespace coffee {

namespace adt {

/**
 * Base exception for all exception defined in this framework.
 */
class Exception : std::logic_error {
public:
   using std::logic_error::what;

   /**
    * Destructor.
    */
   virtual ~Exception () noexcept {;}

   /**
    * \return the method name from this exception was thrown.
    */
   const char* getMethod () const noexcept { return m_fromMethod; }

   /**
    * \return the line number in file from this exception was thrown.
    */
   const char* getFile () const noexcept { return m_fromFile; }

   /**
    * \return the line number in file from this exception was thrown.
    */
   const unsigned getLine () const noexcept { return m_fromLine; }

protected:
   /**
    * Constructor
    * \param str text associated to this exception.
    * \param fromMethod the method name from this exception was thrown.
    * \param fromFile the line number in file from this exception was thrown.
    * \param fromLine the line number in file from this exception was thrown.
    */
   Exception (const std::string& str, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      std::logic_error (str),
      m_fromMethod (fromMethod),
      m_fromFile (fromFile),
      m_fromLine (fromLine)
   {;}

   /**
    * Copy Constructor
    */
   Exception (const Exception& other) :
      std::logic_error (other),
      m_fromMethod (other.m_fromMethod),
      m_fromFile (other.m_fromFile),
      m_fromLine (other.m_fromLine)
   {;}

   /**
    * \return Combines method, file and number of line on that file to locate were the exception was thrown.
    */
   std::string filePosition () const noexcept;

private:
   const char* m_fromMethod;
   const char* m_fromFile;
   const unsigned m_fromLine;
};

}
}

#endif
