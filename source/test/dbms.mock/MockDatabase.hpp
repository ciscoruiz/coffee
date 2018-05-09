// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

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

   std::shared_ptr<dbms::Connection> allocateConnection(const std::string& name, const dbms::ConnectionParameters& parameters)
      throw(basis::RuntimeException, std::bad_cast)
   {
      return std::make_shared<mock::MockConnection>(std::ref(*this), name, parameters);
   }

   std::shared_ptr<dbms::binder::Input> allocateInputBind(std::shared_ptr<dbms::datatype::Abstract> data) const
      throw(basis::RuntimeException)
   {
      if (std::string("give-me-null") == data->getName())
         return std::shared_ptr<MockInput>();

      return std::make_shared<MockInput>(data);
   }

   std::shared_ptr<dbms::binder::Output> allocateOutputBind(std::shared_ptr<dbms::datatype::Abstract> data) const
      throw(basis::RuntimeException)
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
