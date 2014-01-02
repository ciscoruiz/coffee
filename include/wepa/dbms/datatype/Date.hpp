// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_type_Date_h
#define _wepa_dbms_type_Date_h

#include <time.h>

#include <wepa/config/defines.hpp>

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/adt/Second.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Tipo de datos que permite trabajar con el tipo de dato 'Date' de un gestor de base de
   datos generico.

   Dependiendo el gestor de base de datos usado el tipo \em date puede contener informacion que incluya
   la hora del d�a, en Oracle (tm) la incluye, mientras que en mysql, por ejemplo, no la incluye.

   Internamente trabaja con una estructura de tipo 'tm' que habitualmente tendr� los campos:
   \code
   struct tm {
     int tm_sec;                   // Seconds.     [0-60] (1 leap second)
     int tm_min;                   // Minutes.     [0-59]
     int tm_hour;                  // Hours.       [0-23]
     int tm_mday;                  // Day.         [1-31]
     int tm_mon;                   // Month.       [0-11]
     int tm_year;                  // Year - 1900.
     int tm_wday;                  // Day of week. [0-6]
     int tm_yday;                  // Days in year.[0-365]
     int tm_isdst;                 // DST.         [-1/0/1]
   };
   \endcode

   \author cisco.tierra@gmail.com.
   \since NemesisRD.dbms 1.6.1
*/
class Date : public datatype::Abstract {
public:
   /**
    * Espacio maximo reservado para representar lo datos de una fecha sobre una cadena.
    */
   static const int MaxDateSize = 48;

   /**
      Constructor.
      \param name Nombre l�gico de este campo
      \param isNulleable Indica si el dato puede tomar valores nulos.
      \param format Formato usado para interpretar los datos de esta fecha, en los metodos Date::getCStringValue y
      Date::setValue (const char*) y Date::setValue (const std::string&). Sigue la especificacion:

      \code
       %a     Replaced by the localeâs abbreviated weekday name. [ tm_wday]

       %A     Replaced by the localeâs full weekday name. [ tm_wday]

       %b     Replaced by the localeâs abbreviated month name. [ tm_mon]

       %B     Replaced by the localeâs full month name. [ tm_mon]

       %c     Replaced  by  the  localeâs  appropriate date and time representation.  (See the Base Definitions volume of
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

       %p     Replaced by the localeâs equivalent of either a.m. or p.m. [ tm_hour]

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

       %x     Replaced   by   the  localeâs  appropriate  date  representation.  (See  the  Base  Definitions  volume  of
              IEEE Std 1003.1-2001, <time.h>.)

       %X     Replaced  by  the  localeâs  appropriate  time  representation.  (See  the  Base  Definitions   volume   of
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
   */
   explicit Date (const char* name, const bool isNulleable = false, const char* format = NULL) ;

   /**
       Constructor.
       \param name Nombre l�gico de este campo
       \param isNulleable Indica si el dato puede tomar valores nulos.
       \param format Formato usado para interpretar los datos de esta fecha, en los metodos Date::getCStringValue y
       Date::setValue (const char*) y Date::setValue (const std::string&). Sigue la especificacion:

       \code
        %a     Replaced by the localeâs abbreviated weekday name. [ tm_wday]

        %A     Replaced by the localeâs full weekday name. [ tm_wday]

        %b     Replaced by the localeâs abbreviated month name. [ tm_mon]

        %B     Replaced by the localeâs full month name. [ tm_mon]

        %c     Replaced  by  the  localeâs  appropriate date and time representation.  (See the Base Definitions volume of
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

        %p     Replaced by the localeâs equivalent of either a.m. or p.m. [ tm_hour]

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

        %x     Replaced   by   the  localeâs  appropriate  date  representation.  (See  the  Base  Definitions  volume  of
               IEEE Std 1003.1-2001, <time.h>.)

        %X     Replaced  by  the  localeâs  appropriate  time  representation.  (See  the  Base  Definitions   volume   of
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

       \since NemesisRD.dbms 2.10.06.3
   */
   explicit Date (const std::string& name, const bool isNulleable = false, const char* format = NULL) ;

   /**
      Constructor copia.
      \param other Instancia de la que copiar.
   */
   Date (const Date& other);

   /**
      Destructor.
   */
   virtual ~Date ();

   /**
      Devuelve el contenido de esta fecha.
      \return El contenido de esta fecha.
      \warning Si el metodo type::Abstract::isNull devolvio \em true el contenido de la estructura no esta definido.
   */
   const tm& getValue () const throw () { return m_value; }

   /**
      Devuelve el contenido de esta fecha.
      \return El contenido de esta fecha.
      \warning Si el metodo type::Abstract::isNull devolvio \em true el contenido de la estructura no esta definido.
   */
   tm& getValue () throw () { return m_value; }

   /**
    * Interpreta el contenido de la fecha y lo transfiere al buffer.
    * \return El buffer que contiene esta fecha interpretada con el formato indicado en el contructor.
    *  \warning El resultado sera NULL en caso de no poder interpretar correctamente la fecha.
    */
   virtual const char* getCStringValue () const throw ();

   /**
    * Interpreta el contenido de esta fecha como el numero de segundos transcurridos desde el 1 de Enero de 1970.
    * Si el contenido de la columna sociada es nulo este metodo devolvera 0. Si la conversion a segundos no puede
    * ser realizada devolvera -1.
    * \return Interpreta el contenido de esta fecha como el numero de segundos transcurridos desde el 1 de Enero de 1970.
    * Si el contenido de la columna sociada es nulo este metodo devolvera 0. Si la conversion a
    * segundos no puede ser realizada devolvera -1.
    */
   adt::Second getSecondValue () const throw () { return adt::Second ((Abstract::isNull () == true) ? 0: mktime (&const_cast <Date*> (this)->m_value)); }

   /**
    * Devuelve el formato indicado en el constructor de la clase.
    * \return El formato indicado en el constructor de la clase.
    */
   const char* getFormat () const throw () { return m_format; }

   /**
    * Devuelve el a�o contenido por esta fecha.
    * \return El a�o contenido por esta fecha.
    */
   int getYear () const throw () { return m_value.tm_year + 1900; }

   /**
    * Devuelve el mes contenido por esta fecha.
    * \return El mes contenido por esta fecha.
    */
   int getMonth () const throw () { return m_value.tm_mon + 1; }

   /**
    * Devuelve el dia del mes contenido por esta fecha.
    * \return El dia del mes contenido por esta fecha.
    */
   int getDay () const throw () { return m_value.tm_mday; }

   /**
    * Devuelve la hora del dia contenida en la fecha.
    * \return La hora del dia contenida en la fecha.
    * \warning Verifique que el tipo 'Date' de su RDBMS es capaz de contener horas, minutos y segundos.
    */
   int getHour () const throw () { return m_value.tm_hour; }

   /**
    * Devuelve el minuto de la hora contenida en la fecha.
    * \return El minuto de la hora contenida en la fecha.
    * \warning Verifique que el tipo 'Date' de su RDBMS es capaz de contener horas, minutos y segundos.
    */
   int getMinute () const throw () { return m_value.tm_min; }

   /**
    * Devuelve el segundo de la hora contenida en la fecha.
    * \return El segundo de la hora contenida en la fecha.
    * \warning Verifique que el tipo 'Date' de su RDBMS es capaz de contener horas, minutos y segundos.
    */
   int getSecond () const throw () { return m_value.tm_sec; }

   /**
    * Establece el a�o de esta fecha
    * \param year A�o de la fecha. Debe ser mayor de 1900.
    */
   void setYear (const int year) throw (adt::RuntimeException) { set ("Year", m_value.tm_year, year - 1900, 0, -1); }

   /**
    * Establece mes de esta fecha.
    * \param month Mes de la fecha. Debe estar comprendido entre 1 y 12.
    */
   void setMonth (const int month) throw (adt::RuntimeException) { set ("Month", m_value.tm_mon, month - 1, 0, 11); }

   /**
    * Establece el dia del mes de esta fecha.
    * \param day Dia del mes. Debe estar comprendido entre 1 y 31.
    */
   void setDay (const int day) throw (adt::RuntimeException) { set ("Day", m_value.tm_mday, day, 1, 31); }

   /**
    * Establece la hora de esta fecha.
    * \param hour Hora del dia. Debe estar comprendida entre 0 y 23.
    * \warning Verifique que el tipo 'Date' de su RDBMS es capaz de contener horas, minutos y segundos.
    */
   void setHour (const int hour) throw (adt::RuntimeException) { set ("Hour", m_value.tm_hour, hour, 0, 23); }

   /**
    * Establece el minuto de esta fecha.
    * \param minute Minuto de la hora del dia. Debe estar comprendida entre 0 y 59.
    * \warning Verifique que el tipo 'Date' de su RDBMS es capaz de contener horas, minutos y segundos.
    */
   void setMinute (const int minute) throw (adt::RuntimeException) { set ("Minute", m_value.tm_min, minute, 0, 59); }

   /**
    * Establece el segundo de esta fecha.
    * \param second Segungo de la hora del dia. Debe estar comprendida entre 0 y 60.
    * \warning Verifique que el tipo 'Date' de su RDBMS es capaz de contener horas, minutos y segundos.
    */
   void setSecond (const int second) throw (adt::RuntimeException) { set ("Second", m_value.tm_sec, second, 0, 60); }

   /**
      Interpreta la cadena recibida segun el formato indicado en el constructor y la asigna a esta instancia, pero requiere que al
      invocar al constructor de esta fecha se indique el formato usado para traducir.
      \param str Cadena de la que copiar.
   */
   void setValue (const char* str) throw (adt::RuntimeException);

   /**
      Interpreta la cadena recibida segun el formato indicado en el constructor y la asigna a esta instancia, pero requiere que al
      invocar al constructor de esta fecha se indique el formato usado para traducir.
      \param str Cadena de la que copiar.
   */
   void setValue (const std::string& str) throw (adt::RuntimeException) { setValue (str.c_str ()); }

   /**
    * Establece esta fecha con los segundos transcurridos desde el 1/1/1970.
    * \param second Numeros de segundos transcurridos desde el 1 de Enero de 1970.
    * \see wepa::functions::second
    */
   void setValue (const adt::Second& second) throw (adt::RuntimeException);

   /**
      Operador de copia.
      \param date Fecha de la que copiar.
      \return La instancia de esta fecha.
      \warning Solo copia el contenido de la fecha recibida, no cambia el formato de interpretacion de la fecha origen.
   */
   Date& operator = (const Date& date) throw (adt::RuntimeException);

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      \return Una cadena con la informacion referente a esta instancia.
   */
   virtual adt::StreamString asString () const throw ();

   /**
    * Devuelve el nombre l�gico de esta clase
    * \return el nombre l�gico de esta clase
    * \since NemesisRD.dbms 2.10.16.04
    */
   static const char* className () throw () { return "dbms::type::Date"; }

protected:
   char* m_format;
   tm m_value;
   char m_buffer  [MaxDateSize + 1];

   /**
    * Constructor invocado desde el constructor de TimeStamp.
      \param name Nombre l�gico de este campo
      \param type Sera type::Abstract::Type::TimeStamp.
      \param isNulleable Indica si el dato puede tomar valores nulos.
      \param format Formato usado para representar los datos de esta fecha.
    */
   explicit Date (const char* name, const Datatype::_v type, const bool isNulleable, const char* format);

   /**
    * Constructor invocado desde el constructor de TimeStamp.
      \param name Nombre l�gico de este campo
      \param type Sera type::Abstract::Type::TimeStamp.
      \param isNulleable Indica si el dato puede tomar valores nulos.
      \param format Formato usado para representar los datos de esta fecha.

      \since NemesisRD.dbms 2.10.06.3
    */
   explicit Date (const std::string& name, const Datatype::_v type, const bool isNulleable, const char* format);

private:
   void set (const char* what, int& variable, const int value, const int min, const int max) throw (adt::RuntimeException);
   void do_clear () throw () { wepa_memset (&m_value, 0, sizeof (m_value)); }
};

}
}
}

#endif

