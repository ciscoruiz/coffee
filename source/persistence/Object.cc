#include <coffee/persistence/Object.hpp>
#include <coffee/persistence/PrimaryKey.hpp>

#include <coffee/adt/AsHexString.hpp>

#include <coffee/persistence/Class.hpp>
#include <coffee/persistence/PrimaryKey.hpp>


using namespace coffee;

persistence::Object::Object(const Class& clazz, const std::shared_ptr<PrimaryKey>& primaryKey, const dbms::datatype::Set& members) :
   dbms::datatype::Set(members),
   m_class(clazz), 
   m_primaryKey(primaryKey)
{;}

std::string persistence::Object::getInternalId () const noexcept
{
   return adt::AsHexString::apply (coffee_ptrnumber_cast(this));
}

adt::StreamString persistence::Object::asString() const noexcept
{
   adt::StreamString result("persistence.Object { ClassName=");
   result << m_class.getName();
   result << " | " << m_primaryKey->asString();
   
   result << " | Members={";
   for(auto& member : *this) {
      if(!member.second) {
         result << "," << member.second->getName();
      }
   }
   return result << "} }";
}

