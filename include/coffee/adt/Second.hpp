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
