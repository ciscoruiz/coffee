#ifndef _coffee_test_balance_TestResource_hpp
#define _coffee_test_balance_TestResource_hpp

#include <coffee/balance/Resource.hpp>
#include <coffee/adt/AsString.hpp>

#include <coffee/balance/ResourceList.hpp>

namespace coffee {

namespace test {

namespace balance {

class TestResource : public coffee::balance::Resource {
public:
   explicit TestResource (const int key) :
      Resource (adt::StreamString ("TestResource-").append (adt::AsString::apply (key, "%02d"))),
      m_key (key),
      m_available (false) {;}

   void setAvailable (const bool available) noexcept { m_available = available; }
   int getKey () const noexcept { return m_key; }

   static std::shared_ptr<TestResource> cast(std::shared_ptr<coffee::balance::Resource>& resource) { return std::dynamic_pointer_cast<TestResource>(resource); }
   static std::shared_ptr<TestResource> cast(const std::shared_ptr<coffee::balance::Resource>& resource) { return std::dynamic_pointer_cast<TestResource>(resource); }
   static std::shared_ptr<TestResource> cast_copy(std::shared_ptr<coffee::balance::Resource> resource) { return std::dynamic_pointer_cast<TestResource>(resource); }

private:
   const int m_key;
   bool m_available;

   bool isAvailable () const noexcept { return m_available; }
   void initialize () throw (adt::RuntimeException) { m_available = true; }
};

inline std::shared_ptr<coffee::balance::ResourceList> setup(const int maxResource, const int firstId = 0)
{
   std::shared_ptr<coffee::balance::ResourceList> result = std::make_shared<coffee::balance::ResourceList>("TestResources");

   logger::Logger::initialize(std::make_shared<logger::TtyWriter>());

   int id = firstId;
   for (int ii = 0; ii < maxResource; ++ ii) {
      result->add(std::make_shared<TestResource>(id ++));
   }

   result->initialize();

   return result;
}

}
}
}

#endif
