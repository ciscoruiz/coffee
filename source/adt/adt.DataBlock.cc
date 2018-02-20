
#include <coffee/adt/DataBlock.hpp>

using namespace std;
using namespace coffee;

char adt::DataBlock::at (const size_t index) const
   throw (adt::RuntimeException)
{
   if (index >= size ())  {
      COFFEE_THROW_EXCEPTION ("Index " << index << " out of range [0," << size () << ")");
   }

   return std::string::at (index);
}

char& adt::DataBlock::at (const size_t index) throw (adt::RuntimeException)
{
   if (index >= size ()) {
      COFFEE_THROW_EXCEPTION ("Index " << index << " out of range [0," << size () << ")");
   }

   return std::string::at (index);
}
