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

#ifndef _coffee_dbms_datatype_Date_h
#define _coffee_dbms_datatype_Date_h

#include <time.h>
#include <chrono>
#include <functional>

#include <coffee/config/defines.hpp>

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
   Tipo de datos que permite trabajar con el tipo de dato 'Date' de un gestor de base de
   datos generico.
*/
class Date : public datatype::Abstract {
public:
   /**
    * Espacio maximo reservado para representar lo datos de una fecha sobre una cadena.
    */
   static const int MaxDateSize = 48;
   static const char* DefaultFormat;

   explicit Date(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull) ;

   explicit Date(const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull) : Date(name.c_str(), constraint) {;}

   Date(const Date& other);

   ~Date() { ; }

   const std::chrono::seconds& getValue() const throw(adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   struct tm* getLocalTime() const throw(adt::RuntimeException);

   /**
       \code
        %a     Replaced by the localeâs abbreviated weekday name. [ tm_wday]

        %A     Replaced by the localeâs full weekday name. [ tm_wday]

        %b     Replaced by the localeâs abbreviated month name. [ tm_mon]

        %B     Replaced by the localeâs full month name. [ tm_mon]

        %c     Replaced  by  the  localeâs  appropriate date and time representation. (See the Base Definitions volume of
               IEEE Std 1003.1-2001, <time.h>.)

        %C     Replaced by the year divided by 100 and truncated to an integer, as a decimal number [00,99]. [ tm_year]

        %d     Replaced by the day of the month as a decimal number [01,31]. [ tm_mday]

        %D     Equivalent to %m / %d / %y . [ tm_mon, tm_mday, tm_year]

        %e     Replaced by the day of the month as a decimal number [1,31]; a single digit  is  preceded  by  a  space.  [
               tm_mday]

        %F     Equivalent to %Y - %m - %d(the ISO 8601:2000 standard date format). [ tm_year, tm_mon, tm_mday]

        %g     Replaced  by  the  last 2 digits of the week-based year(see below) as a decimal number [00,99]. [ tm_year,
               tm_wday, tm_yday]

        %G     Replaced by the week-based year(see below) as a decimal number(for example, 1977).  [  tm_year,  tm_wday,
               tm_yday]

        %h     Equivalent to %b . [ tm_mon]

        %H     Replaced by the hour(24-hour clock) as a decimal number [00,23].  [ tm_hour]

        %I     Replaced by the hour(12-hour clock) as a decimal number [01,12].  [ tm_hour]

        %j     Replaced by the day of the year as a decimal number [001,366]. [ tm_yday]

        %m     Replaced by the month as a decimal number [01,12]. [ tm_mon]

        %M     Replaced by the minute as a decimal number [00,59]. [ tm_min]

        %n     Replaced by a <newline>.

        %p     Replaced by the localeâs equivalent of either a.m. or p.m. [ tm_hour]

        %r     Replaced  by the time in a.m. and p.m. notation;    in the POSIX locale this shall be equivalent to %I : %M
               : %S %p .  [ tm_hour, tm_min, tm_sec]

        %R     Replaced by the time in 24-hour notation( %H : %M ).  [ tm_hour, tm_min]

        %S     Replaced by the second as a decimal number [00,60]. [ tm_sec]

        %t     Replaced by a <tab>.

        %T     Replaced by the time( %H : %M : %S ). [ tm_hour, tm_min, tm_sec]

        %u     Replaced by the weekday as a decimal number [1,7], with 1 representing Monday. [ tm_wday]

        %U     Replaced by the week number of the year as a decimal number [00,53].  The first Sunday of  January  is  the
               first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]

        %V     Replaced  by the week number of the year(Monday as the first day of the week) as a decimal number [01,53].
               If the week containing 1 January has four or more days in the new year, then it is considered week 1.  Oth-
               erwise,  it  is  the  last week of the previous year, and the next week is week 1. Both January 4th and the
               first Thursday of January are always in week 1. [ tm_year, tm_wday, tm_yday]

        %w     Replaced by the weekday as a decimal number [0,6], with 0 representing Sunday. [ tm_wday]

        %W     Replaced by the week number of the year as a decimal number [00,53].  The first Monday of  January  is  the
               first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]

        %x     Replaced   by   the  localeâs  appropriate  date  representation. (See  the  Base  Definitions  volume  of
               IEEE Std 1003.1-2001, <time.h>.)

        %X     Replaced  by  the  localeâs  appropriate  time  representation. (See  the  Base  Definitions   volume   of
               IEEE Std 1003.1-2001, <time.h>.)

        %y     Replaced by the last two digits of the year as a decimal number [00,99].  [ tm_year]

        %Y     Replaced by the year as a decimal number(for example, 1997). [ tm_year]

        %z     Replaced  by  the offset from UTC in the ISO 8601:2000 standard format( +hhmm or -hhmm ), or by no charac-
               ters if no timezone is determinable. For example, "-0430" means 4 hours 30  minutes  behind  UTC (west  of
               Greenwich).    If tm_isdst is zero, the standard time offset is used. If tm_isdst is greater than zero, the
               daylight savings time offset is used. If tm_isdst is negative, no characters are returned.  [ tm_isdst]

        %Z     Replaced by the timezone name or abbreviation, or  by  no  bytes  if  no  timezone  information  exists.  [
               tm_isdst]

        %%     Replaced by % .
       \endcode
   */
   void setValue(const char* str, const char* format) throw(adt::RuntimeException);

   void setValue(const std::string& str, const char* format) throw(adt::RuntimeException) { setValue(str.c_str(), format); }

   void setValue(const std::chrono::seconds& second) throw(adt::RuntimeException);

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<Date>(*this); }

   operator adt::StreamString() const noexcept { return asString(); }

   virtual adt::StreamString asString() const noexcept;

   Date& operator=(const Date&) = delete;
   bool operator==(const Date& other) const noexcept { return m_value == other.m_value; }

   size_t hash() const noexcept { return m_value.count(); }

   coffee_declare_datatype_downcast(Date);

protected:
   std::chrono::seconds m_value;
   char m_buffer[MaxDateSize + 1];

   explicit Date(const char* name, const Datatype::_v type, const Constraint::_v constraint);

   void do_clear() noexcept { m_value = std::chrono::seconds::zero(); }

private:
   int do_compare(const Abstract& other) const throw(adt::RuntimeException);
};

}
}
}

#endif

