// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef _coffee_adt_Second_h
#define _coffee_adt_Second_h

#include <string>

#include <coffee/config/defines.hpp>
#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace adt {

class Millisecond;
class Microsecond;

/**
 * This class models the time unit Second
 */
class Second {
public:
   typedef long type_t;

   /**
      Reserved space to show a time on a c-buffer.

      @see #asDateTime
   */
   static const int DateTimeSizeString = 64;

   /**
    * Constructor
    */
   Second() : m_value (0) {;}

   /**
    * Constructor.
    * \param value Valor inicial de esta instancia.
    */
   explicit Second(const type_t value) : m_value (value) {;}

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Second (const Second& other) : m_value (other.m_value) {;}

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Second (const Millisecond& other);

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Second (const Microsecond& other);

   operator type_t  () const noexcept { return m_value; }

   Second& operator= (const type_t other) noexcept { m_value = other; return *this; }

   Second& operator= (const Second& other) noexcept { m_value = other.m_value; return *this; }

   Second& operator= (const Millisecond& other) noexcept;

   Second& operator= (const Microsecond& other) noexcept;

   bool operator== (const Second& other) const noexcept { return m_value == other.m_value; }

   bool operator== (const Millisecond& other) const noexcept;

   bool operator== (const Microsecond& other) const noexcept;

   bool operator!= (const Second& other) const noexcept { return m_value != other.m_value; }

   bool operator!= (const Millisecond& other) const noexcept;

   bool operator!= (const Microsecond& other) const noexcept;

   bool operator> (const Second& other) const noexcept { return m_value > other.m_value; }

   bool operator> (const Millisecond& other) const noexcept;

   bool operator> (const Microsecond& other) const noexcept;

   bool operator< (const Second& other) const noexcept { return m_value < other.m_value; }

   bool operator< (const Millisecond& other) const noexcept;

   bool operator< (const Microsecond& other) const noexcept;

   bool operator>= (const Second& other) const noexcept { return m_value >= other.m_value; }

   bool operator>= (const Millisecond& other) const noexcept { return (operator==(other) == true) ? true: operator>(other); }

   bool operator>= (const Microsecond& other) const noexcept { return (operator==(other) == true) ? true: operator>(other); }

   bool operator<= (const Second& other) const noexcept { return m_value <= other.m_value; }

   bool operator<= (const Millisecond& other) const noexcept { return (operator==(other) == true) ? true: operator<(other); }

   bool operator<= (const Microsecond& other) const noexcept { return (operator==(other) == true) ? true: operator<(other); }

   /**
    * Devuelve el valor asociado a esta instancia.
    * \return el valor asociado a esta instancia.
    */
   type_t getValue () const noexcept { return m_value; }

   /**
      Devuelve una cadena con la hora en formato 'dd/mm/yyyy hh:mm:ss'.
      @param format Indicador de formato.
      @return Un literal con la hora en el formato 'dd/mm/yyyy hh:mm:ss'.

      \see man strftime
   */
   std::string asDateTime (const char* format = "%d/%0m/%Y %T") const throw (RuntimeException);

   /**
    * Devuelve una cadena con el valor de esta instancia y las unidades "sec".
    * \return una cadena con el valor de esta instancia y las unidades "sec".
    */
   std::string asString () const noexcept;

   /**
    * Devuelve la hora actual de sistema expresada en segundos transcurridos desde el 1 de Enero de 1970
    * \return la hora actual de sistema expresada en segundos transcurridos desde el 1 de Enero de 1970
    */
   static Second getTime () noexcept;

   /**
    * Devuelve la hora actual de sistema expresada en segundos transcurridos desde el 1 de Enero de 1970 aplicando
    * las correciones correspondientes a la hora local.
    * \return la hora actual de sistema expresada en segundos transcurridos desde el 1 de Enero de 1970
    */
   static Second getLocalTime () noexcept;

   static Second fromString (const std::string& value) throw (RuntimeException);

   static Second fromString(const std::string& text, const char* format) throw(RuntimeException);

private:
   type_t m_value;

   friend class Millisecond;
   friend class Microsecond;

   friend class Second operator + (const Second& left, const Second& right) noexcept;
   friend class Second operator + (const Second& left, const int right) noexcept;
   friend class Second operator - (const Second& left, const Second& right) noexcept;
   friend class Second operator / (const Second& left, const Second& right) noexcept;
   friend class Second operator / (const Second& left, const int right) noexcept;
   friend class Second operator / (const Second& left, const unsigned int right) noexcept;
};

inline Second operator + (const Second& left, const Second& right)
   noexcept
{
   return Second (left.m_value + right.m_value);
}

inline Second operator + (const Second& left, const int right)
   noexcept
{
   return Second (left.m_value + right);
}

inline Second operator - (const Second& left, const Second& right)
   noexcept
{
   return Second (left.m_value - right.m_value);
}

inline Second operator / (const Second& left, const Second& right)
   noexcept
{
   return Second (left.m_value / right.m_value);
}

inline Second operator / (const Second& left, const int right)
   noexcept
{
   return Second (left.m_value / right);
}

inline Second operator / (const Second& left, const unsigned int right)
   noexcept
{
   return Second (left.m_value / right);
}

}
}

#endif 
