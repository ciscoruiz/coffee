#ifndef _coffee_adt_Microsecond_h
#define _coffee_adt_Microsecond_h

#include <string>

#include <coffee/config/defines.hpp>

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace adt {

class Millisecond;
class Second;

class Microsecond {
public:
   typedef Integer64 type_t;

   Microsecond() : m_value (0) {;}

   explicit Microsecond(const type_t value) : m_value (value) {;}

   Microsecond (const Microsecond& other) : m_value (other.m_value) {;}

   Microsecond (const Millisecond& other);

   Microsecond (const Second& other);

   operator type_t () const noexcept { return m_value; }

   Microsecond& operator= (const type_t other) noexcept { m_value = other; return *this; }

   Microsecond& operator= (const Microsecond& other) noexcept { m_value = other.m_value; return *this; }

   Microsecond& operator= (const Millisecond& other) noexcept;

   Microsecond& operator= (const Second& other) noexcept;

   bool operator== (const Microsecond& other) const noexcept { return m_value == other.m_value; }

   bool operator== (const Millisecond& other) const noexcept;

   bool operator== (const Second& other) const noexcept;

   bool operator!= (const Microsecond& other) const noexcept { return m_value != other.m_value; }

   bool operator!= (const Millisecond& other) const noexcept;

   bool operator!= (const Second& other) const noexcept;

   bool operator> (const Microsecond& other) const noexcept { return m_value > other.m_value; }

   bool operator> (const Millisecond& other) const noexcept;

   bool operator> (const Second& other) const noexcept;

   bool operator< (const Microsecond& other) const noexcept { return m_value < other.m_value; }

   bool operator< (const Millisecond& other) const noexcept;

   bool operator< (const Second& other) const noexcept;

   bool operator>= (const Microsecond& other) const noexcept { return m_value >= other.m_value; }

   bool operator>= (const Millisecond& other) const noexcept { return (operator==(other) == true) ? true: operator>(other); }

   bool operator>= (const Second& other) const noexcept { return (operator==(other) == true) ? true: operator>(other); }

   bool operator<= (const Microsecond& other) const noexcept { return m_value <= other.m_value; }

   bool operator<= (const Millisecond& other) const noexcept { return (operator==(other) == true) ? true: operator<(other); }

   bool operator<= (const Second& other) const noexcept { return (operator==(other) == true) ? true: operator<(other); }

   Microsecond& operator+= (const Microsecond& other) noexcept { m_value += other.m_value; return *this; }

   type_t getValue () const noexcept { return m_value; }

   static Microsecond getTime () noexcept;
   std::string asString () const noexcept;

   static Microsecond fromString (const std::string& value) throw (RuntimeException);

private:
   type_t m_value;

   friend class Millisecond;
   friend class Second;

   friend class Microsecond operator - (const Microsecond& left, const Microsecond& right) noexcept;
   friend class Microsecond operator + (const Microsecond& left, const Microsecond& right) noexcept;
   friend class Microsecond operator / (const Microsecond& left, const Microsecond& right) noexcept;
   friend class Microsecond operator / (const Microsecond& left, const int right) noexcept;
   friend class Microsecond operator / (const Microsecond& left, const unsigned int right) noexcept;
};

inline Microsecond operator - (const Microsecond& left, const Microsecond& right)
   noexcept
{
   return Microsecond (left.m_value - right.m_value);
}

inline Microsecond operator + (const Microsecond& left, const Microsecond& right)
   noexcept
{
   return Microsecond (left.m_value + right.m_value);
}

inline Microsecond operator / (const Microsecond& left, const Microsecond& right)
   noexcept
{
   return Microsecond (left.m_value / right.m_value);
}

inline Microsecond operator / (const Microsecond& left, const int right)
   noexcept
{
   return Microsecond (left.m_value / right);
}

inline Microsecond operator / (const Microsecond& left, const unsigned int right)
   noexcept
{
   return Microsecond (left.m_value / right);
}

}
}

#endif 
