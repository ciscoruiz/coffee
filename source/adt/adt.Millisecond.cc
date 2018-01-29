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

timeval* adt::Millisecond::getTimeVal (timeval& tv) const
   noexcept
{
   if (m_value < 0)
      return NULL;

   tv.tv_sec = m_value / 1000;
   tv.tv_usec = (m_value % 1000) * 1000;

   return &tv;
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

