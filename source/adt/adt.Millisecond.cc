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

#include <sys/time.h>
#include <stdlib.h>

#include <coffee/adt/AsString.hpp>
#include <coffee/adt/Millisecond.hpp>
#include <coffee/adt/Second.hpp>
#include <coffee/adt/Microsecond.hpp>

using namespace std;
using namespace coffee;

#define implement_operator(op) \
   bool adt::Millisecond::operator op (const adt::Second& other) const \
      noexcept\
   {\
      return (m_value / 1000) op (other.m_value);\
   }\
   bool adt::Millisecond::operator op (const Microsecond& other) const\
      noexcept\
   {\
      return m_value op (other.m_value / 1000);\
   }

adt::Millisecond::Millisecond (const adt::Second& other) : m_value (other.m_value) { m_value *= 1000;}

adt::Millisecond::Millisecond (const adt::Microsecond& other) : m_value (other.m_value / 1000) {;}

adt::Millisecond& adt::Millisecond::operator= (const adt::Second& other)
   noexcept
{
   m_value = other.m_value;
   m_value *= 1000;
   return *this;
}

adt::Millisecond& adt::Millisecond::operator= (const Microsecond& other)
   noexcept
{
   m_value = (other.m_value / 1000);
   return *this;
}

implement_operator (==)
implement_operator (!=)
implement_operator (>)
implement_operator (<)

//static
adt::Millisecond adt::Millisecond::getTime ()
   noexcept
{
   struct timeval tv;
   gettimeofday (&tv, NULL);

   adt::Millisecond result (adt::Second (tv.tv_sec));
   adt::Millisecond aux (Microsecond (tv.tv_usec));
   result.m_value += aux.m_value;
   return result;
}

string adt::Millisecond::asString () const
   noexcept
{
   string result (AsString::apply (m_value));
   return result += " ms";
}

//static
adt::Millisecond adt::Millisecond::fromString (const std::string& value)
   throw (RuntimeException)
{
   if (value.find (" ms") == string::npos)
      COFFEE_THROW_EXCEPTION ("String: " << value << " is not a valid microsecond expression");

   return adt::Millisecond (atoll (value.c_str ()));
}

