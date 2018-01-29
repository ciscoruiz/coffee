// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <boost/test/unit_test.hpp>

#include <thread>
#include <iostream>

#include <coffee/adt/Second.hpp>

#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/Logger.hpp>

#include <coffee/persistence/Repository.hpp>
#include <coffee/persistence/Storage.hpp>
#include <coffee/persistence/Loader.hpp>
#include <coffee/persistence/Recorder.hpp>
#include <coffee/persistence/Eraser.hpp>
#include <coffee/persistence/Object.hpp>
#include <coffee/persistence/Class.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>
#include <coffee/persistence/ClassBuilder.hpp>

#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/GuardConnection.hpp>
#include <coffee/dbms/GuardStatement.hpp>
#include <coffee/dbms/Statement.hpp>

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Date.hpp>

#include "../dbms/MockDatabase.hpp"
#include "../dbms/MockLowLevelRecord.hpp"
#include "../dbms/MockConnection.hpp"

#include "PersistenceMocks.hpp"

using namespace coffee;
using namespace coffee::mock;

struct Fixture {
   test_persistence::MyDatabase database;
   persistence::Repository repository;

   std::shared_ptr<dbms::Connection> connection;
   std::shared_ptr<dbms::Statement> readerStatement;
   std::shared_ptr<dbms::Statement> writerStatement;
   std::shared_ptr<dbms::Statement> eraserStatement;

   std::shared_ptr<persistence::Storage> storage;
   std::shared_ptr<persistence::Class> customerClass;
   std::shared_ptr<persistence::PrimaryKey> primaryKeyForFind;

   Fixture() : database ("dbName"), repository("repoName") {
      connection = database.createConnection("default", "user", "password");
      readerStatement = database.createStatement("read_only", "read");
      writerStatement = database.createStatement("writer", "write");
      eraserStatement = database.createStatement("eraser", "delete");

      BOOST_REQUIRE_NO_THROW(database.externalInitialize());
      BOOST_REQUIRE_EQUAL(database.isRunning(), true);

      storage = repository.createStorage("default", persistence::Storage::DefaultMaxCacheSize);

      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id"));
      std::shared_ptr<persistence::PrimaryKey> primaryKeyCustomer = pkBuilder.build();
      primaryKeyForFind = pkBuilder.build();

      persistence::ClassBuilder classBuilder("customer");
      classBuilder.set(primaryKeyCustomer).add(std::make_shared<dbms::datatype::String>("name", 64));
      customerClass = classBuilder.build();
   }
};

BOOST_FIXTURE_TEST_CASE(persistence_storage_read, Fixture)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   if(true) {
      primaryKeyForFind->setInteger("id", 6);
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 6);
      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 6");

      test_persistence::CustomerObjectWrapper customer2(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), customer2.getId());
      BOOST_REQUIRE_EQUAL(customer.getName(), customer2.getName());
   }

   BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
   BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 1);

   if(true) {
      primaryKeyForFind->setInteger("id", 7);
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 7);
      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 7");

      test_persistence::CustomerObjectWrapper customer2(storage->load(connection, myLoader));
      BOOST_REQUIRE_EQUAL(customer.getId(), customer2.getId());
   }

   BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 2);
   BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 2);
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_reload, Fixture)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 6);
      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 6");

      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);
   }

   mock::MockLowLevelRecord newSix;
   newSix.m_id = 6;
   newSix.m_name = "only odd id's will be reloaded";
   newSix.m_float = 0.123;
   newSix.m_integer = 6 * 6;
   database.update(newSix);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 6);
      BOOST_REQUIRE_EQUAL(customer.getName(), "only odd id's will be reloaded");

      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 1);
   }
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_reload_erased, Fixture)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 6);
      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 6");

      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);
   }

   database.erase(6);

   {
      BOOST_REQUIRE_THROW(storage->load(connection, myLoader), dbms::DatabaseException);
      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 1);
   }
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_noreload_erased, Fixture)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 7);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 7);
      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 7");

      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);
   }

   database.erase(7);

   {
      BOOST_REQUIRE_NO_THROW(storage->load(connection, myLoader));
      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 1);
   }
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_noreload, Fixture)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 7);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 7);
      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 7");

      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);
   }

   mock::MockLowLevelRecord newSix;
   newSix.m_id = 7;
   newSix.m_name = "even id will not be reload";
   newSix.m_float = 0.123;
   newSix.m_integer = 6 * 6;
   database.update(newSix);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 7);
      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 7");

      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 1);
   }
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_read_not_found, Fixture)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6000);
   BOOST_REQUIRE_THROW(storage->load(connection, myLoader), dbms::DatabaseException);

   BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
   BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_write, Fixture)
{
   BOOST_REQUIRE_EQUAL(database.container_size(), test_persistence::MyDatabase::PreloadRegisterCounter);

   if (true) {
      primaryKeyForFind->setInteger("id", 5555);
      auto object = customerClass->createObject(primaryKeyForFind);

      object->setString("name", "5555 name");
      test_persistence::MockCustomerRecorder recorder(writerStatement, object);

      recorder.enableAutoCommit();

      storage->save(connection, recorder);
   }

   BOOST_REQUIRE_EQUAL(database.container_size(), test_persistence::MyDatabase::PreloadRegisterCounter + 1);

   BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 0);
   BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);

   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   BOOST_REQUIRE_NO_THROW(storage->load(connection, myLoader));

   BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
   BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_erase_preloaded, Fixture)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      BOOST_REQUIRE_EQUAL(customer.getId(), 6);
      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 6");

      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);
   }

   BOOST_REQUIRE_EQUAL(database.container_size(), test_persistence::MyDatabase::PreloadRegisterCounter);

   if (true) {
      primaryKeyForFind->setInteger("id", 6);
      test_persistence::MockCustomerEraser eraser(eraserStatement, primaryKeyForFind);

      eraser.enableAutoCommit();

      storage->erase(connection, eraser);

      BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 1);
   }

   BOOST_REQUIRE_EQUAL(database.container_size(), test_persistence::MyDatabase::PreloadRegisterCounter - 1);

   BOOST_REQUIRE_THROW(storage->load(connection, myLoader), dbms::DatabaseException);

   BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 2);
   BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 1);
}


BOOST_FIXTURE_TEST_CASE(persistence_storage_erase_noloaded, Fixture)
{
   BOOST_REQUIRE_EQUAL(database.container_size(), test_persistence::MyDatabase::PreloadRegisterCounter);

   primaryKeyForFind->setInteger("id", 6);
   test_persistence::MockCustomerEraser eraser(eraserStatement, primaryKeyForFind);

   eraser.enableAutoCommit();

   storage->erase(connection, eraser);

   BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 1);
   BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);

   BOOST_REQUIRE_EQUAL(database.container_size(), test_persistence::MyDatabase::PreloadRegisterCounter - 1);

   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   BOOST_REQUIRE_THROW(storage->load(connection, myLoader), dbms::DatabaseException);

   BOOST_REQUIRE_EQUAL(storage->getFaultCounter(), 2);
   BOOST_REQUIRE_EQUAL(storage->getHitCounter(), 0);
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_save_commit_pending, Fixture)
{
   const int maxCommitPending = 64;
   const int minId = test_persistence::MyDatabase::PreloadRegisterCounter * 2;
   const int commitCounter = 3;

   {
      dbms::GuardConnection guardConnection(connection);
      guardConnection.setMaxCommitPending(maxCommitPending);

      for (int ii = minId; ii < (minId + (maxCommitPending * commitCounter)); ++ ii) {
         primaryKeyForFind->setInteger("id", ii);
         auto object = customerClass->createObject(primaryKeyForFind);

         adt::StreamString streamString;
         streamString << ii << " - name";
         object->setString("name", streamString);
         test_persistence::MockCustomerRecorder recorder(writerStatement, object);

         storage->save(guardConnection, recorder);
      }
   }

   BOOST_REQUIRE_EQUAL(database.container_size(), test_persistence::MyDatabase::PreloadRegisterCounter + (maxCommitPending * commitCounter));

   auto mockConnection = std::dynamic_pointer_cast<mock::MockConnection>(connection);

   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), commitCounter);
}

BOOST_FIXTURE_TEST_CASE(persistence_storage_erase_commit_pending, Fixture)
{
   const int maxCommitPending = 16;
   const int commitCounter = 3;
   const int maxId = 10 + maxCommitPending * commitCounter;

   {
      dbms::GuardConnection guardConnection(connection);
      guardConnection.setMaxCommitPending(maxCommitPending);

      for (int ii = 10; ii < maxId; ++ ii) {
         primaryKeyForFind->setInteger("id", ii);
         test_persistence::MockCustomerEraser eraser(eraserStatement, primaryKeyForFind);
         storage->erase(guardConnection, eraser);
      }
   }

   BOOST_REQUIRE_EQUAL(database.container_size(), test_persistence::MyDatabase::PreloadRegisterCounter + 10 - maxId);

   auto mockConnection = std::dynamic_pointer_cast<mock::MockConnection>(connection);

   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), commitCounter);
}

