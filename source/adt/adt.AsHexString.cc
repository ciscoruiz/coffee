#include <ctype.h>
#include <stdio.h>

#include <coffee/config/defines.hpp>

#include <coffee/adt/AsHexString.hpp>
#include <coffee/adt/DataBlock.hpp>

using namespace std;
using namespace coffee;

string adt::AsHexString::apply (const int number)
   noexcept
{
   char aux [16];
   sprintf (aux, "0x%x", number);
   return string (aux);
}

string adt::AsHexString::apply (const unsigned int number)
   noexcept
{
   char aux [16];
   sprintf (aux, "0x%x", number);
   return string (aux);
}

string adt::AsHexString::apply (const Integer64 number)
   noexcept
{
   char aux [24];
#ifdef __coffee64__
   sprintf (aux, "0x%lx", number);
#else
   sprintf (aux, "0x%llx", number);
#endif

   return string (aux);
}

string adt::AsHexString::apply (const Unsigned64 number)
   noexcept
{
   char aux [32];
#ifdef __coffee64__
   sprintf (aux, "0x%lx", number);
#else
   sprintf (aux, "0x%llx", number);
#endif
   return string (aux);
}

