// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef __wepa_mock_MockDatabase_hpp
#define __wepa_mock_MockDatabase_hpp

#include <wepa/app/Application.hpp>

#include <wepa/dbms/Database.hpp>

#include <mock/MockInput.hpp>
#include <mock/MockOutput.hpp>
#include <mock/MockConnection.hpp>

namespace wepa {

namespace mock {

class MockDatabase : public dbms::Database {
public:
   enum ErrorCode { NotFound, Successful, Lock, LostConnection };

   MockDatabase (app::Application& app) : dbms::Database (app, "map", app.getTitle ().c_str ()) {;}

   void add (const mock::MockLowLevelRecord& record) noexcept {
      m_container [record.m_id] = record;
   }

   int container_size () const noexcept { return m_container.size (); }

private:
   mock::MockLowLevelContainer m_container;

   bool notFound (const int errorCode) const noexcept { return errorCode == NotFound; }
   bool successful (const int errorCode) const noexcept { return errorCode == Successful; }
   bool locked (const int errorCode) const noexcept { return errorCode == Lock; }
   bool lostConnection (const int errorCode) const noexcept { return errorCode == LostConnection; }

   dbms::Connection* allocateConnection (const std::string& name, const char* user, const char* password)
      throw (adt::RuntimeException)
   {
      return new mock::MockConnection (std::ref (*this), name, user, password);
   }

   dbms::binder::Input* allocateInputBind (dbms::datatype::Abstract& data) throw (adt::RuntimeException) {
      return new MockInput (data);
   }

   dbms::binder::Output* allocateOutputBind (dbms::datatype::Abstract& data) throw (adt::RuntimeException) {
      return new MockOutput (data);
   }

   friend class MockConnection;

};
} /* namespace mock */
} /* namespace wepa */
#endif
