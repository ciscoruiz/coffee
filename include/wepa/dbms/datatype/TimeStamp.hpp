// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_TimeStamp_h
#define _wepa_dbms_datatype_TimeStamp_h

#include <wepa/dbms/datatype/Date.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Tipo de datos que permite trabajar con el tipo de dato 'TimeStamp' de un gestor de base de
   datos generico.

   El tipo de dato TimeStamp contiene la informaci�n suficiente para representar una fecha
   incluyendo la hora del d�a.

   \author cisco.tierra@gmail.com.
   \since NemesisRD.dbms 1.6.1
*/
class TimeStamp : public Date {
public:
   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param isNulleable Indica si el dato puede tomar valores nulos.
      \param format Formato usado para interpretar los datos de esta fecha, en los metodos Date::getCStringValue y
      Date::setValue (const char*) y Date::setValue (const std::string&). Sigue la especificacion:

      \code
       %a     Replaced by the localeâs abbreviated weekday name. [ tm_wday]

       %A     Replaced by the localeâs full weekday name. [ tm_wday]

       %b     Replaced by the localeâs abbreviated month name. [ tm_mon]

       %B     Replaced by the localeâs full month name. [ tm_mon]

       %c     Replaced  by  the  locale's  appropriate date and time representation.  (See the Base Definitions volume of
              IEEE Std 1003.1-2001, <time.h>.)

       %C     Replaced by the year divided by 100 and truncated to an integer, as a decimal number [00,99]. [ tm_year]

       %d     Replaced by the day of the month as a decimal number [01,31]. [ tm_mday]

       %D     Equivalent to %m / %d / %y . [ tm_mon, tm_mday, tm_year]

       %e     Replaced by the day of the month as a decimal number [1,31]; a single digit  is  preceded  by  a  space.  [
              tm_mday]

       %F     Equivalent to %Y - %m - %d (the ISO 8601:2000 standard date format). [ tm_year, tm_mon, tm_mday]

       %g     Replaced  by  the  last 2 digits of the week-based year (see below) as a decimal number [00,99]. [ tm_year,
              tm_wday, tm_yday]

       %G     Replaced by the week-based year (see below) as a decimal number (for example, 1977).  [  tm_year,  tm_wday,
              tm_yday]

       %h     Equivalent to %b . [ tm_mon]

       %H     Replaced by the hour (24-hour clock) as a decimal number [00,23].  [ tm_hour]

       %I     Replaced by the hour (12-hour clock) as a decimal number [01,12].  [ tm_hour]

       %j     Replaced by the day of the year as a decimal number [001,366]. [ tm_yday]

       %m     Replaced by the month as a decimal number [01,12]. [ tm_mon]

       %M     Replaced by the minute as a decimal number [00,59]. [ tm_min]

       %n     Replaced by a <newline>.

       %p     Replaced by the locale's equivalent of either a.m. or p.m. [ tm_hour]

       %r     Replaced  by the time in a.m. and p.m. notation;    in the POSIX locale this shall be equivalent to %I : %M
              : %S %p .  [ tm_hour, tm_min, tm_sec]

       %R     Replaced by the time in 24-hour notation ( %H : %M ).  [ tm_hour, tm_min]

       %S     Replaced by the second as a decimal number [00,60]. [ tm_sec]

       %t     Replaced by a <tab>.

       %T     Replaced by the time ( %H : %M : %S ). [ tm_hour, tm_min, tm_sec]

       %u     Replaced by the weekday as a decimal number [1,7], with 1 representing Monday. [ tm_wday]

       %U     Replaced by the week number of the year as a decimal number [00,53].  The first Sunday of  January  is  the
              first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]

       %V     Replaced  by the week number of the year (Monday as the first day of the week) as a decimal number [01,53].
              If the week containing 1 January has four or more days in the new year, then it is considered week 1.  Oth-
              erwise,  it  is  the  last week of the previous year, and the next week is week 1. Both January 4th and the
              first Thursday of January are always in week 1. [ tm_year, tm_wday, tm_yday]

       %w     Replaced by the weekday as a decimal number [0,6], with 0 representing Sunday. [ tm_wday]

       %W     Replaced by the week number of the year as a decimal number [00,53].  The first Monday of  January  is  the
              first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]

       %x     Replaced   by   the  locale's  appropriate  date  representation.  (See  the  Base  Definitions  volume  of
              IEEE Std 1003.1-2001, <time.h>.)

       %X     Replaced  by  the  locale's  appropriate  time  representation.  (See  the  Base  Definitions   volume   of
              IEEE Std 1003.1-2001, <time.h>.)

       %y     Replaced by the last two digits of the year as a decimal number [00,99].  [ tm_year]

       %Y     Replaced by the year as a decimal number (for example, 1997). [ tm_year]

       %z     Replaced  by  the offset from UTC in the ISO 8601:2000 standard format ( +hhmm or -hhmm ), or by no charac-
              ters if no timezone is determinable. For example, "-0430" means 4 hours 30  minutes  behind  UTC  (west  of
              Greenwich).    If tm_isdst is zero, the standard time offset is used. If tm_isdst is greater than zero, the
              daylight savings time offset is used. If tm_isdst is negative, no characters are returned.  [ tm_isdst]

       %Z     Replaced by the timezone name or abbreviation, or  by  no  bytes  if  no  timezone  information  exists.  [
              tm_isdst]

       %%     Replaced by % .
      \endcode

      Para obtener m�s informacion sobre la espeficacion de formato \em man \em strftime (p.e.).

      Para poder obtener la parte fraccionaria en la salida del metodo #getCStringValue hay que indicar el literal \em %%d. Por ejemplo:

      \code
      TimeStamp oneTime (false, "%T.%%d")
      \endcode
    */
   explicit TimeStamp (const char* name, const bool isNulleable = false, const char* format=NULL)  :
      Date (name, datatype::Abstract::Datatype::TimeStamp, isNulleable, format),
      m_fractionalSecond (0)
      {;}

   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param isNulleable Indica si el dato puede tomar valores nulos.
      \param format Formato usado para interpretar los datos de esta fecha, en los metodos Date::getCStringValue y
      Date::setValue (const char*) y Date::setValue (const std::string&). Sigue la especificacion:

      \code
       %a     Replaced by the localeâs abbreviated weekday name. [ tm_wday]

       %A     Replaced by the localeâs full weekday name. [ tm_wday]

       %b     Replaced by the localeâs abbreviated month name. [ tm_mon]

       %B     Replaced by the localeâs full month name. [ tm_mon]

       %c     Replaced  by  the  locale's  appropriate date and time representation.  (See the Base Definitions volume of
              IEEE Std 1003.1-2001, <time.h>.)

       %C     Replaced by the year divided by 100 and truncated to an integer, as a decimal number [00,99]. [ tm_year]

       %d     Replaced by the day of the month as a decimal number [01,31]. [ tm_mday]

       %D     Equivalent to %m / %d / %y . [ tm_mon, tm_mday, tm_year]

       %e     Replaced by the day of the month as a decimal number [1,31]; a single digit  is  preceded  by  a  space.  [
              tm_mday]

       %F     Equivalent to %Y - %m - %d (the ISO 8601:2000 standard date format). [ tm_year, tm_mon, tm_mday]

       %g     Replaced  by  the  last 2 digits of the week-based year (see below) as a decimal number [00,99]. [ tm_year,
              tm_wday, tm_yday]

       %G     Replaced by the week-based year (see below) as a decimal number (for example, 1977).  [  tm_year,  tm_wday,
              tm_yday]

       %h     Equivalent to %b . [ tm_mon]

       %H     Replaced by the hour (24-hour clock) as a decimal number [00,23].  [ tm_hour]

       %I     Replaced by the hour (12-hour clock) as a decimal number [01,12].  [ tm_hour]

       %j     Replaced by the day of the year as a decimal number [001,366]. [ tm_yday]

       %m     Replaced by the month as a decimal number [01,12]. [ tm_mon]

       %M     Replaced by the minute as a decimal number [00,59]. [ tm_min]

       %n     Replaced by a <newline>.

       %p     Replaced by the locale's equivalent of either a.m. or p.m. [ tm_hour]

       %r     Replaced  by the time in a.m. and p.m. notation;    in the POSIX locale this shall be equivalent to %I : %M
              : %S %p .  [ tm_hour, tm_min, tm_sec]

       %R     Replaced by the time in 24-hour notation ( %H : %M ).  [ tm_hour, tm_min]

       %S     Replaced by the second as a decimal number [00,60]. [ tm_sec]

       %t     Replaced by a <tab>.

       %T     Replaced by the time ( %H : %M : %S ). [ tm_hour, tm_min, tm_sec]

       %u     Replaced by the weekday as a decimal number [1,7], with 1 representing Monday. [ tm_wday]

       %U     Replaced by the week number of the year as a decimal number [00,53].  The first Sunday of  January  is  the
              first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]

       %V     Replaced  by the week number of the year (Monday as the first day of the week) as a decimal number [01,53].
              If the week containing 1 January has four or more days in the new year, then it is considered week 1.  Oth-
              erwise,  it  is  the  last week of the previous year, and the next week is week 1. Both January 4th and the
              first Thursday of January are always in week 1. [ tm_year, tm_wday, tm_yday]

       %w     Replaced by the weekday as a decimal number [0,6], with 0 representing Sunday. [ tm_wday]

       %W     Replaced by the week number of the year as a decimal number [00,53].  The first Monday of  January  is  the
              first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]

       %x     Replaced   by   the  locale's  appropriate  date  representation.  (See  the  Base  Definitions  volume  of
              IEEE Std 1003.1-2001, <time.h>.)

       %X     Replaced  by  the  locale's  appropriate  time  representation.  (See  the  Base  Definitions   volume   of
              IEEE Std 1003.1-2001, <time.h>.)

       %y     Replaced by the last two digits of the year as a decimal number [00,99].  [ tm_year]

       %Y     Replaced by the year as a decimal number (for example, 1997). [ tm_year]

       %z     Replaced  by  the offset from UTC in the ISO 8601:2000 standard format ( +hhmm or -hhmm ), or by no charac-
              ters if no timezone is determinable. For example, "-0430" means 4 hours 30  minutes  behind  UTC  (west  of
              Greenwich).    If tm_isdst is zero, the standard time offset is used. If tm_isdst is greater than zero, the
              daylight savings time offset is used. If tm_isdst is negative, no characters are returned.  [ tm_isdst]

       %Z     Replaced by the timezone name or abbreviation, or  by  no  bytes  if  no  timezone  information  exists.  [
              tm_isdst]

       %%     Replaced by % .
      \endcode

      Para obtener m�s informacion sobre la espeficacion de formato \em man \em strftime (p.e.).

      Para poder obtener la parte fraccionaria en la salida del metodo #getCStringValue hay que indicar el literal \em %%d. Por ejemplo:

      \code
      TimeStamp oneTime (false, "%T.%%d")
      \endcode
   */
   explicit TimeStamp (const std::string& name, const bool isNulleable = false, const char* format=NULL)  :
      Date (name, datatype::Abstract::Datatype::TimeStamp, isNulleable, format),
      m_fractionalSecond (0)
      {;}

   /**
    * Devuelve la parte fraccionaria de los segundos asociados a este objeto.
    * \return La parte fraccionaria de los segundos asociados a este objeto.
    */
   int getFractionalSecond () const throw () { return m_fractionalSecond; }

   /**
    * Establece la parte fraccionaria de los segundos de este objeto.
    * \param fsec Parte fraccionaria de los segundos.
    */
   void setFractionalSecond (const int fsec) throw () { m_fractionalSecond = fsec; }

   /**
    * Interpreta el contenido de la fecha y lo transfiere al buffer.
    * \return El buffer que contiene esta fecha interpretada con el formato indicado en el contructor.
    *  \warning El resultado sera NULL en caso de no poder interpretar correctamente la fecha.
    */
   virtual const char* getCStringValue () const throw ();

   wepa_declare_datatype_downcast(TimeStamp)

private:
   char m_anotherBuffer [MaxDateSize + 1];
   int m_fractionalSecond;

   void do_clear () throw () { m_fractionalSecond = 0; m_anotherBuffer [0] = 0; }
};

}
}
}

#endif

