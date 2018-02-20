#ifndef __coffee_mock_MockDatabase_hpp
#define __coffee_mock_MockDatabase_hpp

#include <coffee/app/Application.hpp>
#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/ErrorCodeInterpreter.hpp>
#include <coffee/dbms/datatype/Abstract.hpp>

#include "MockInput.hpp"
#include "MockOutput.hpp"
#include "MockConnection.hpp"

namespace coffee {

namespace mock {

class MockDatabase : public dbms::Database {
public:
   enum ErrorCode { NotFound, Successful, Lock, LostConnection };

   explicit MockDatabase(app::Application& app);
   explicit MockDatabase(const char* name);

   void add(const mock::MockLowLevelRecord& record) noexcept {
      m_container [record.m_id] = record;
   }

   void update(const mock::MockLowLevelRecord& record) noexcept {
      m_container [record.m_id] = record;
   }

   void erase(const int id) noexcept {
      m_container.erase(id);
   }

   int container_size() const noexcept { return m_container.size(); }

private:
   mock::MockLowLevelContainer m_container;

   class MockErrorCodeInterpreter : public dbms::ErrorCodeInterpreter {
   public:
      bool notFound(const int errorCode) const noexcept { return errorCode == NotFound; }
      bool successful(const int errorCode) const noexcept { return errorCode == Successful; }
      bool locked(const int errorCode) const noexcept { return errorCode == Lock; }
      bool lostConnection(const int errorCode) const noexcept { return errorCode == LostConnection; }
   };

   std::shared_ptr<dbms::Connection> allocateConnection(const std::string& name, const char* user, const char* password)
      throw(adt::RuntimeException)
   {
      return std::make_shared<mock::MockConnection>(std::ref(*this), name, user, password);
   }

   std::shared_ptr<dbms::binder::Input> allocateInputBind(std::shared_ptr<dbms::datatype::Abstract> data) const
      throw(adt::RuntimeException)
   {
      if (std::string("give-me-null") == data->getName())
         return std::shared_ptr<MockInput>();

      return std::make_shared<MockInput>(data);
   }

   std::shared_ptr<dbms::binder::Output> allocateOutputBind(std::shared_ptr<dbms::datatype::Abstract> data) const
      throw(adt::RuntimeException)
   {
      if (std::string("give-me-null") == data->getName())
         return std::shared_ptr<MockOutput>();

      return std::make_shared<MockOutput>(data);
   }

   friend class MockConnection;

};
} /* namespace mock */
} /* namespace coffee */
#endif
