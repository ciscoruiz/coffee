#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/PrimaryKey.hpp>

using namespace coffee;

persistence::ClassBuilder& persistence::ClassBuilder::set(std::shared_ptr<PrimaryKey>& primaryKey)
throw (adt::RuntimeException)
{
   if (m_primaryKey) {
      COFFEE_THROW_EXCEPTION(m_className << " primary key already added");
   }

   m_primaryKey = primaryKey;
   return *this;
}

std::shared_ptr<persistence::Class> persistence::ClassBuilder::build() const
      throw (adt::RuntimeException)
{
   if (!m_primaryKey) {
      COFFEE_THROW_EXCEPTION(m_className << " does not define a valid primary key");
   }

   if (empty()) {
      COFFEE_THROW_EXCEPTION(m_className << " does not define any member");
   }

   for (const_data_iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
      if (m_primaryKey->constains(name(ii)) == true) {
         COFFEE_THROW_EXCEPTION(m_className << " member " << name(ii) << " already used as a member in the primary key");
      }
   }

   return std::make_shared<persistence::Class>(*this);
}
