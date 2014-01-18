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
#include <boost/test/unit_test.hpp>

#include <wepa/adt/Second.hpp>

#include <wepa/persistence/Repository.hpp>
#include <wepa/persistence/Storage.hpp>
#include <wepa/persistence/Loader.hpp>
#include <wepa/persistence/Object.hpp>
#include <wepa/persistence/Class.hpp>
#include <wepa/persistence/GuardClass.hpp>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/String.hpp>
#include <wepa/dbms/datatype/Float.hpp>
#include <wepa/dbms/datatype/Date.hpp>

namespace wepa {

namespace persistence {

namespace test {

struct MyPhysicalRecord {
   int m_id;
   std::string m_name;
   int m_integer;
   float m_float;
   adt::Second m_time;
};

class MyObject : public Object {
public:
   MyObject (Class& _class) : Object (_class) {
      m_id = m_integer = 0;
      m_float = 0.0;
   }

   int getId () const noexcept { return m_id; }
   const std::string& getName() const noexcept { return m_name; }
   int getInteger () const noexcept { return m_integer; }
   float getFloat () const noexcept { return m_float; }
   const adt::Second& getTime () const noexcept { return std::ref (m_time); }

   void setId (const int value) noexcept { m_id = value; }
   void setName (const std::string& value) noexcept { m_name = value; }
   void setInteger (const int value) noexcept { m_integer = value; }
   void setFloat (const float value) noexcept { m_float = value; }
   void setTime (const adt::Second& value) noexcept { m_time = value; }

   adt::StreamString asString () const noexcept {
      adt::StreamString result ("MyObject { ");
      result << m_id;
      return result += " }";
   }

private:
   int m_id;
   std::string m_name;
   int m_integer;
   float m_float;
   adt::Second m_time;

   void releaseDependences () noexcept {;}
};

class MyClass : public Class  {
public:
   MyClass () : Class ("MyClass") {;}

private:
   dbms::datatype::Abstract* do_createMember (const int columnNumber) const noexcept;
   Object* createObject () noexcept { return new MyObject (*this); }
};

struct MyLoader : public Loader {
public:
   MyLoader () : Loader ("MyLoader", 0) {;}


private:
   void apply (GuardClass& _class, Object& object) throw (adt::RuntimeException, dbms::DatabaseException);
   bool hasToRefresh (GuardClass& _class, const Object& object) throw (adt::RuntimeException, dbms::DatabaseException) { return false; }
   bool isInputValue (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isPrimaryKeyComponent (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isOutputValue (const int columnNumber) const noexcept { return columnNumber >= 1; }
};

}
}
}

using namespace wepa;
using namespace wepa::dbms;

dbms::datatype::Abstract* persistence::test::MyClass::do_createMember (const int columnNumber) const noexcept
{
   dbms::datatype::Abstract* result = NULL;

   switch (columnNumber) {
   case 0: result = new datatype::Integer ("id"); break;
   case 1: result = new datatype::String ("name", 64, dbms::datatype::Constraint::CanBeNull); break;
   case 2: result = new datatype::Integer ("ii"); break;
   case 3: result = new datatype::Float ("ff", dbms::datatype::Constraint::CanBeNull); break;
   case 4: result = new datatype::Date ("dd", dbms::datatype::Constraint::CanBeNull); break;
   }

   return result;
}

void persistence::test::MyLoader::apply (GuardClass& _class, Object& object)
   throw (adt::RuntimeException, dbms::DatabaseException)
{

}

BOOST_AUTO_TEST_CASE (persistence_storage_readonly)
{
   persistence::test::MyClass _class;

   persistence::Repository repository ("persistence_storage_readonly");
   persistence::Storage* ii = repository.createStorage(0, "storage_readonly", persistence::Storage::AccessMode::ReadOnly);

   BOOST_REQUIRE_NE (ii, (void*) 0);

   persistence::test::MyLoader myLoader;

   persistence::GuardClass myGuard (_class);

   try{
      myLoader.initialize(_class, NULL);
      persistence::Object& object = ii->load(myGuard, myLoader);
   }
   catch (adt::Exception& ex) {
      BOOST_REQUIRE_EQUAL (std::string ("no error"), ex.what ());
      std::cout << ex.what() << std::endl;
   }
}
