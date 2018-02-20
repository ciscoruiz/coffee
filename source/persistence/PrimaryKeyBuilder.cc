#include <coffee/persistence/PrimaryKeyBuilder.hpp>
#include <coffee/persistence/Class.hpp>

using namespace coffee;

std::shared_ptr<persistence::PrimaryKey> persistence::PrimaryKeyBuilder::build() const
      throw (adt::RuntimeException)
{
   if (empty()) {
      COFFEE_THROW_EXCEPTION("PrimaryKeyBuilder does not define any component");
   }

   return std::make_shared<persistence::PrimaryKey>(*this);
}
