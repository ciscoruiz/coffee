#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include <coffee/adt/AsString.hpp>
#include <coffee/adt/Millisecond.hpp>
#include <coffee/adt/Second.hpp>
#include <coffee/adt/Microsecond.hpp>

using namespace std;
using namespace coffee;

#define implement_operator(op) \
   bool adt::Microsecond::operator op (const adt::Millisecond& other) const \
      noexcept\
   {\
      return (m_value / 1000) op (((type_t) other.m_value));\
   }\
   bool adt::Microsecond::operator op (const adt::Second& other) const\
      noexcept\
   {\
      return (m_value / 1000000) op ((type_t) other.m_value);\
   }

adt::Microsecond::Microsecond (const adt::Millisecond& other) : m_value (other.m_value) { m_value *= 1000; }

adt::Microsecond::Microsecond (const adt::Second& other) : m_value (other.m_value) { m_value *= 1000000; }

adt::Microsecond& adt::Microsecond::operator= (const adt::Millisecond& other)
   noexcept
{
   m_value = other.m_value;
   m_value *= 1000;
   return *this;
}

adt::Microsecond& adt::Microsecond::operator= (const adt::Second& other)
   noexcept
{
   m_value = other.m_value;
   m_value *= 1000000;
   return *this;
}

implement_operator (==)
implement_operator (!=)
implement_operator (>)
implement_operator (<)


//static
adt::Microsecond adt::Microsecond::getTime ()
   noexcept
{
   struct timeval tv;
   gettimeofday (&tv, NULL);
   Microsecond result (Second (tv.tv_sec));
   result.m_value += tv.tv_usec;
   return result;
}

string adt::Microsecond::asString () const
   noexcept
{
   string result (AsString::apply (m_value));
   return result += " us";
}

//static
adt::Microsecond adt::Microsecond::fromString (const std::string& value)
   throw (adt::RuntimeException)
{
   if (value.find (" us") == string::npos)
      COFFEE_THROW_EXCEPTION ("String: " << value << " is not a valid microsecond expression");

   return Microsecond (atoll (value.c_str ()));
}
