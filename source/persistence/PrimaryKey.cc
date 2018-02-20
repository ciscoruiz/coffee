#include <memory>

#include <coffee/dbms/datatype/Abstract.hpp>

#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>

using namespace coffee;

persistence::PrimaryKey::PrimaryKey(const PrimaryKeyBuilder& builder) :
   dbms::datatype::Set(builder)
{
}

bool persistence::PrimaryKey::matches(const PrimaryKey& other) const
   noexcept
{
   if (size() != other.size())
      return false;

   for (const_data_iterator ii = begin(), maxii = end(); ii != maxii; ii ++) {
      const_data_iterator oo = other.search(PrimaryKey::name(ii));

      if (oo == other.end())
         return false;

      if (PrimaryKey::data(oo)->getType() != PrimaryKey::data(ii)->getType())
         return false;
   }

   return true;
}

size_t persistence::PrimaryKey::hash() const noexcept
{
   size_t result = size();

   for(PrimaryKey::const_data_iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
      result ^= PrimaryKey::data(ii)->hash() << 1;
   }

   return result;
}

adt::StreamString persistence::PrimaryKey::asString() const noexcept
{
   adt::StreamString result("persistence.PrimaryKey { ");

   for (const_data_iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
      result += data(ii)->asString();
      result += "  ";
   }

   return result += "}";
}
