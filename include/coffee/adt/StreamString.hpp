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

#ifndef _coffee_adt_StreamString_H
#define _coffee_adt_StreamString_H

#include <string>

#include <stdio.h>

#include <coffee/config/defines.hpp>

namespace coffee  {

namespace adt {

/**
 *  This class will work with basic data as an stream of data which append to some instance.
 *  
 * \include test/adt/StreamString_test.cc
 */
class StreamString : public std::string {
public:
   /**
    * Flags to tuning behaviour 
    */
   struct Flag { enum _v { None, ShowNull }; };

   /**
    * Constructor.
    * \param flag Set how to manage the null value.
    */
   explicit StreamString (const Flag::_v flag = Flag::None) : a_flags (flag) {;}

   /**
    * Constructor.
    * \param str Buffer to initialize content of the instance. It can be NULL.
    * \param flag Set how to manage the null value.
    */
   StreamString (const char* str, const Flag::_v flag = Flag::None) :
      std::string ((str == NULL) ? ((flag & Flag::ShowNull) ? "<null>": ""): str),
      a_flags (flag)
   {;}

   /**
    * Copy constructor
    * \param other Other instance to duplicate.
    */
   StreamString (const StreamString &other) : std::string (other), a_flags (other.a_flags) {;}

   /**
    * Constructor copia.
    * \param other Buffer to initialize content of the instance. It can be NULL.
    * \param flag Set how to manage the null value.
    */
   StreamString (const std::string& other, const Flag::_v flag = Flag::None) : std::string (other), a_flags (flag) {;}

   /**
    * Convert to upper case.
    */
   adt::StreamString& toUpper () noexcept;

   /**
    * Convert to lower case.
    */
   adt::StreamString& toLower () noexcept;

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const char vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const char* vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const int vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const unsigned int vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const bool vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const Integer64 vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const Unsigned64 vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const float vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const double vv) noexcept { std::string::clear (); return operator<< (vv); }

   /**
    * Initialize this instance with the value of \b vv converted to std::string.
    */
   StreamString& operator = (const std::string& vv) noexcept { std::string::operator= (vv); return *this; }

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const char vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const char* vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const int vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const unsigned int vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const bool vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const Integer64 vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const Unsigned64 vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const float vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const double vv) noexcept;

   /**
    * Append the value of \b vv converted to std::string into this instance.
    * \return This instance to be able to concatenate operations of apending.
    */
   StreamString& operator << (const std::string& vv) noexcept { std::string::append (vv); return *this; }

private:
   Flag::_v a_flags;
};

}
}

#endif
