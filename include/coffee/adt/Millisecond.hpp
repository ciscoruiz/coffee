#ifndef _coffee_adt_Millisecond_h
#define _coffee_adt_Millisecond_h

#include <time.h>

#include <string>

#include <coffee/config/defines.hpp>
#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace adt {

class Second;
class Microsecond;

class Millisecond {
public:
   typedef Integer64 type_t;

   /**
    * Constructor
    */
   Millisecond() : m_value (0) {;}

   /**
    * Constructor.
    * \param value Valor inicial de esta instancia.
    */
   explicit Millisecond(const type_t value) : m_value (value) {;}

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Millisecond (const Millisecond& other) : m_value (other.m_value) {;}

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Millisecond (const Second& other);

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Millisecond (const Microsecond& other);

   operator type_t () const noexcept { return m_value; }

   Millisecond& operator= (const type_t other) noexcept { m_value = other; return *this; }

   Millisecond& operator= (const Millisecond& other) noexcept { m_value = other.m_value; return *this; }

   Millisecond& operator= (const Second& other) noexcept;

   Millisecond& operator= (const Microsecond& other) noexcept;

   bool operator== (const Millisecond& other) const noexcept { return m_value == other.m_value; }

   bool operator== (const Second& other) const noexcept;

   bool operator== (const Microsecond& other) const noexcept;

   bool operator!= (const Millisecond& other) const noexcept { return m_value != other.m_value; }

   bool operator!= (const Second& other) const noexcept;

   bool operator!= (const Microsecond& other) const noexcept;

   bool operator> (const Millisecond& other) const noexcept { return m_value > other.m_value; }

   bool operator> (const Second& other) const noexcept;

   bool operator> (const Microsecond& other) const noexcept;

   bool operator< (const Millisecond& other) const noexcept { return m_value < other.m_value; }

   bool operator< (const Second& other) const noexcept;

   bool operator< (const Microsecond& other) const noexcept;

   bool operator>= (const Millisecond& other) const noexcept { return m_value >= other.m_value; }

   bool operator>= (const Second& other) const noexcept { return (operator==(other) == true) ? true: operator>(other); }

   bool operator>= (const Microsecond& other) const noexcept { return (operator==(other) == true) ? true: operator>(other); }

   bool operator<= (const Millisecond& other) const noexcept { return m_value <= other.m_value; }

   bool operator<= (const Second& other) const noexcept { return (operator==(other) == true) ? true: operator<(other); }

   bool operator<= (const Microsecond& other) const noexcept { return (operator==(other) == true) ? true: operator<(other); }

   Millisecond& operator+= (const Millisecond& other) noexcept { m_value += other.m_value; return *this; }

   Millisecond& operator-= (const Millisecond& other) noexcept { (m_value > other.m_value) ? (m_value -= other.m_value): (m_value = 0); return *this; }

   type_t getValue () const noexcept { return m_value; }

   static Millisecond getTime () noexcept;

   std::string asString () const noexcept;

   static Millisecond fromString (const std::string& value) throw (RuntimeException);

private:
   type_t m_value;

   friend class Second;
   friend class Microsecond;

   friend class Millisecond operator / (const Millisecond& left, const Millisecond& right) noexcept;
   friend class Millisecond operator + (const Millisecond& left, const Millisecond& right) noexcept;
   friend class Millisecond operator - (const Millisecond& left, const Millisecond& right) noexcept;
   friend class Millisecond operator / (const Millisecond& left, const int right) noexcept;
   friend class Millisecond operator / (const Millisecond& left, const unsigned int right) noexcept;
   friend class Millisecond operator * (const Millisecond& left, const int right) noexcept;
};

inline Millisecond operator / (const Millisecond& left, const Millisecond& right)
   noexcept
{
   return Millisecond (left.m_value / right.m_value);
}

inline Millisecond operator + (const Millisecond& left, const Millisecond& right)
   noexcept
{
   return Millisecond (left.m_value + right.m_value);
}

inline Millisecond operator - (const Millisecond& left, const Millisecond& right)
   noexcept
{
   return Millisecond (left.m_value - right.m_value);
}

inline Millisecond operator / (const Millisecond& left, const int right)
   noexcept
{
   return Millisecond (left.m_value / right);
}

inline Millisecond operator / (const Millisecond& left, const unsigned int right)
   noexcept
{
   return Millisecond (left.m_value / right);
}

inline Millisecond operator * (const Millisecond& left, const int right)
   noexcept
{
   return Millisecond (left.m_value * right);
}

}
}

#endif
