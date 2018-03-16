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

#ifndef _coffee_adt_Average_h
#define _coffee_adt_Average_h

#include <coffee/adt/StreamString.hpp>

#include <coffee/adt/AsString.hpp>
namespace coffee {

namespace adt {

/**
 * Class for calculating average values for a set of values.
 * \param _T Type of values. It should have operator=,  operator for numeric comparation and numeric operation against integers.
 * \include test/adt/average_test.cc
 */
template <class _T> class Average {
public:
   /**
      Constructor.
      \param name Logical name. It will be use only in asString method.
      \param measure Measure name. It will be use only in asString method.
   */
   Average(const char* name, const char* measure ) : a_name(name), a_measure(measure), a_accumulator(0), a_n(0) {;}

   /**
      Constructor.
      \param name Logical name. It will be use only in asString method.
   */
   explicit Average(const char* name) : a_name(name), a_measure(""), a_accumulator(0), a_n(0) {;}

   /**
    * \return \b true if the instance has any valid value or \b false otherwise.
    */
   bool isEmpty() const noexcept  { return (a_n == 0); }

   /**
    * \return \b true if average value is zero \b false otherwise.
    * \warning it could zero due to there is no values or because all registered values are zero.
    */
   bool isZero() const noexcept { return a_n == 0 || a_accumulator == 0; }

   /**
    * \return Number of registered values.
   */
   int size() const noexcept { return a_n; }

   /**
    * \return Sum up of all registered values.
   */
   _T getAccumulator() const noexcept { return a_accumulator; }

   /**
    * \return the average value of the set of registered values.
    * \warning It will return zero if isEmpty return  \b true.
    */
   _T value() const noexcept { return(isEmpty() == true) ? _T(0):(a_accumulator / a_n);  }
   
   /**
    * Initialize all values for this instance.
   */
   void clear() noexcept {  a_accumulator = 0; a_n = 0; }

   /**
    * Set values for this instance.
    */
   void setValue(const _T& value, const unsigned int _n) noexcept {
      a_accumulator = value;
      a_n = _n;
   }

   /**
    * Operator conversion.
    * \return It will return the value method,
    */
   operator _T() const noexcept { return value(); }

   /**
    * Initialize the instance. It will store the first value of the set of values.
   */
   Average<_T>& operator =(const _T value)
      noexcept 
   {
      a_accumulator = value;
      a_n = 1;
      return *this;
   }

   /**
    * Copy constructor.
    */
   Average<_T>& operator =(const Average<_T>& other)
      noexcept 
   {
      a_name = other.a_name;
      a_measure = other.a_measure;
      a_accumulator = other.a_accumulator;
      a_n = other.a_n;
      return *this;
   }

   /**
    * Register a new value on the set of values.
    * \return This instance.
    */
   Average<_T>& operator +=(const _T& v)
      noexcept 
   {
      const _T backup(a_accumulator);
      if ((a_accumulator += v) < backup) {
         a_accumulator = v;
         a_n = 1;
      }
      else
         a_n ++;
      
      return *this;
   }

   /**
    * Deegister a value on the set of values.
    * \return This instance.
    */
   Average<_T>& operator -=(const _T v)
      noexcept 
   {
      if (a_accumulator > v && a_n > 1) {
         a_accumulator -= v;
         a_n --;
      }
      else {
         a_accumulator = 0;
         a_n =0;
      }
      return *this;
   }

   /**
    * \return Summarize information of this instance in a StreamString.
   */
   StreamString asString() const
      noexcept
   {
      StreamString result(a_name);
      result << " { Accumulator:" << adt::AsString::apply((int64_t) a_accumulator);

      if (!a_measure.empty())
         result << " " << a_measure;

      result << " | N=" <<  a_n;
      result << " | Value=" << adt::AsString::apply((int64_t) value());

      if (!a_measure.empty())
         result << " " << a_measure;
      
      return result += " }";
   }
   
private:
   std::string a_name;
   std::string a_measure;
   _T a_accumulator;
   unsigned int a_n; 
};

}
}

#endif



