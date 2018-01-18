// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
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

#include <wepa/adt/Second.hpp>

#include <wepa/logger/TraceMethod.hpp>
#include <wepa/logger/Logger.hpp>

#include <wepa/persistence/Repository.hpp>
#include <wepa/persistence/Storage.hpp>
#include <wepa/persistence/Loader.hpp>
#include <wepa/persistence/Recorder.hpp>
#include <wepa/persistence/Eraser.hpp>
#include <wepa/persistence/Creator.hpp>
#include <wepa/persistence/Object.hpp>
#include <wepa/persistence/Class.hpp>
#include <wepa/persistence/PrimaryKeyBuilder.hpp>
#include <wepa/persistence/ClassBuilder.hpp>

#include <wepa/dbms/Database.hpp>
#include <wepa/dbms/GuardConnection.hpp>
#include <wepa/dbms/GuardStatement.hpp>
#include <wepa/dbms/Statement.hpp>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/String.hpp>
#include <wepa/dbms/datatype/Float.hpp>
#include <wepa/dbms/datatype/Date.hpp>

#include "../dbms/MockDatabase.hpp"
#include "../dbms/MockLowLevelRecord.hpp"
#include "../dbms/MockConnection.hpp"

#include "PersistenceMocks.hpp"

using namespace wepa;
using namespace wepa::mock;

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
      auto object = customerClass->createObject(primaryKeyForFind);
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


//
//BOOST_AUTO_TEST_CASE(persistence_storage_write)
//{
//   test_persistence::MyDatabase database("persistence_storage_write");
//
//   BOOST_REQUIRE_NO_THROW(database.externalInitialize());
//
//   mock::MockConnection* conn0 = static_cast <mock::MockConnection*>(database.createConnection("0", "0", "0"));
//   dbms::Statement* stReader = database.createStatement("read_only", "read");
//   dbms::Statement* stWriter = database.createStatement("writer", "write");
//
//   BOOST_REQUIRE_EQUAL(database.isRunning(), true);
//
//   persistence::Repository repository("persistence_storage_write");
//   persistence::Storage* wr_storage = repository.createStorage(0, "storage_write", persistence::Storage::AccessMode::ReadWrite);
//
//   test_persistence::MockCustomerClass _class;
//
//   BOOST_REQUIRE_NE(wr_storage,(void*) 0);
//
//   test_persistence::MockCustomerLoader myLoader;
//   test_persistence::MockCustomerRecorder myRecorder;
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class);
//
//      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 2));
//      test_persistence::CustomerObjectWrapper& customer = static_cast <test_persistence::CustomerObjectWrapper&>(wr_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(customer.getId(), 2);
//      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 2");
//
//      BOOST_REQUIRE_NO_THROW(myRecorder.initialize(guardCustomer, stWriter));
//
//      customer.setName("updated name 2");
//
//      BOOST_REQUIRE_THROW(myRecorder.getObject(), adt::RuntimeException);
//      BOOST_REQUIRE_NO_THROW(myRecorder.setObject(customer));
//
//      BOOST_REQUIRE_NO_THROW(wr_storage->save(*conn0, guardCustomer, myRecorder));
//
//      BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
//      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
//   }
//
//   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
//   BOOST_REQUIRE_EQUAL(database.container_size(), 10);
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class);
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 2));
//      test_persistence::CustomerObjectWrapper& customer = static_cast <test_persistence::CustomerObjectWrapper&>(wr_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(customer.getId(), 2);
//      BOOST_REQUIRE_EQUAL(customer.getName(), "updated name 2");
//   }
//
//   BOOST_REQUIRE_NO_THROW(database.externalStop());
//}
//
//BOOST_AUTO_TEST_CASE(persistence_storage_erase)
//{
//   test_persistence::MyDatabase database("dbms_with_app");
//
//   BOOST_REQUIRE_NO_THROW(database.externalInitialize());
//
//   BOOST_REQUIRE_EQUAL(database.isRunning(), true);
//
//   mock::MockConnection* conn0 = static_cast <mock::MockConnection*>(database.createConnection("0", "0", "0"));
//   dbms::Statement* stReader = database.createStatement("read_only", "read");
//   dbms::Statement* stEraser = database.createStatement("eraser", "delete");
//
//   persistence::Repository repository("persistence_storage_erase");
//   persistence::Storage* wr_storage = repository.createStorage(0, "storage_erase", persistence::Storage::AccessMode::ReadWrite);
//
//   test_persistence::MockCustomerClass _class;
//
//   BOOST_REQUIRE_NE(wr_storage,(void*) 0);
//
//   test_persistence::MockCustomerLoader myLoader;
//   test_persistence::MockCustomerEraser myEraser;
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class);
//
//      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 2));
//      test_persistence::CustomerObjectWrapper& customer = static_cast <test_persistence::CustomerObjectWrapper&>(wr_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(customer.getId(), 2);
//      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 2");
//
//      BOOST_REQUIRE_NO_THROW(myEraser.initialize(guardCustomer, stEraser));
//
//      BOOST_REQUIRE_THROW(myEraser.getObject(), adt::RuntimeException);
//
//      BOOST_REQUIRE_NO_THROW(customer.getPrimaryKey());
//
//      myEraser.setObject(customer);
//
//      BOOST_REQUIRE_NO_THROW(myEraser.getObject());
//      BOOST_REQUIRE_NO_THROW(myEraser.getObject().getPrimaryKey());
//
//      try {
//         wr_storage->erase(*conn0, guardCustomer, myEraser);
//      }
//      catch(adt::RuntimeException& ex) {
//         std::cout << ex.asString() << std::endl;
//         BOOST_REQUIRE_EQUAL(std::string("none"), ex.what());
//      }
//
//      BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
//      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
//   }
//
//   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
//   BOOST_REQUIRE_EQUAL(database.container_size(), 9);
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class);
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 4));
//      test_persistence::CustomerObjectWrapper& customer = static_cast <test_persistence::CustomerObjectWrapper&>(wr_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(customer.getId(), 4);
//      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 4");
//
//      BOOST_REQUIRE_NO_THROW(wr_storage->load(*conn0, guardCustomer, myLoader));
//
//      try {
//         myEraser.setObject(customer);
//         wr_storage->erase(*conn0, guardCustomer, myEraser);
//         BOOST_REQUIRE_EQUAL(true, false);
//      }
//      catch(adt::RuntimeException& ex) {
//         std::cout << ex.asString() << std::endl;
//         std::string error = ex.what();
//         BOOST_REQUIRE_NE(error.find("due to multiple references"), std::string::npos);
//      }
//   }
//
//   BOOST_REQUIRE_NO_THROW(database.externalStop());
//}
//
//BOOST_AUTO_TEST_CASE(persistence_storage_create)
//{
//   test_persistence::MyDatabase database("persistence_storage_create");
//
//   BOOST_REQUIRE_NO_THROW(database.externalInitialize());
//
//   BOOST_REQUIRE_EQUAL(database.isRunning(), true);
//
//   mock::MockConnection* conn0 = static_cast <mock::MockConnection*>(database.createConnection("0", "0", "0"));
//
//   BOOST_REQUIRE_NE(conn0,(void*) 0);
//
//   dbms::Statement* stReader = database.createStatement("read_only", "read");
//
//   BOOST_REQUIRE_NE(stReader,(void*) 0);
//
//   dbms::Statement* stCreator = database.createStatement("creator", "none");
//
//   BOOST_REQUIRE_NE(stCreator,(void*) 0);
//
//   dbms::Statement* stWriter = database.createStatement("writer", "write");
//
//   persistence::Repository repository("persistence_storage_create");
//   persistence::Storage* wr_storage = repository.createStorage(0, "storage_create", persistence::Storage::AccessMode::ReadWrite);
//
//   test_persistence::MockCustomerClass _class;
//
//   BOOST_REQUIRE_NE(wr_storage,(void*) 0);
//
//   test_persistence::MockCustomerRecorder myRecorder;
//   test_persistence::MockCustomerCreator myCreator;
//   test_persistence::MockCustomerLoader myLoader;
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class);
//
//      BOOST_CHECK_NO_THROW(myCreator.initialize(guardCustomer, stCreator));
//
//      myCreator.setId(guardCustomer, 1000);
//      myCreator.setName(guardCustomer, "the name 1000");
//      test_persistence::CustomerObjectWrapper& customer = static_cast <test_persistence::CustomerObjectWrapper&>(wr_storage->create(*conn0, guardCustomer, myCreator));
//
//      LOG_DEBUG("Customer has been created");
//
//      BOOST_REQUIRE_NO_THROW(myRecorder.initialize(guardCustomer, stWriter));
//      myRecorder.setObject(customer);
//
//      BOOST_REQUIRE_NO_THROW(wr_storage->save(*conn0, guardCustomer, myRecorder));
//
//      BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
//      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
//   }
//
//   BOOST_REQUIRE_EQUAL(database.container_size(), 11);
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class);
//
//      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 1000));
//      test_persistence::CustomerObjectWrapper& customer = static_cast <test_persistence::CustomerObjectWrapper&>(wr_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(customer.getId(), 1000);
//      BOOST_REQUIRE_EQUAL(customer.getName(), "the name 1000");
//   }
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class);
//
//      // Accessor already initialized
//      BOOST_CHECK_THROW(myCreator.initialize(guardCustomer, stCreator), adt::RuntimeException);
//
//      myCreator.setId(guardCustomer, 1000);
//      myCreator.setName(guardCustomer, "the name 1000");
//
//      try {
//         wr_storage->create(*conn0, guardCustomer, myCreator);
//         BOOST_REQUIRE_EQUAL(false, true);
//      }
//      catch(adt::RuntimeException& ex) {
//         std::cout << ex.asString() << std::endl;
//         BOOST_REQUIRE_NE(ex.asString().find("PrimaryKey is already registered"), std::string::npos);
//      }
//   }
//
//   BOOST_REQUIRE_NO_THROW(database.externalStop());
//}
//
//BOOST_AUTO_TEST_CASE(persistence_storage_cache)
//{
//   test_persistence::MyDatabase database("persistence_storage_cache");
//
//   BOOST_REQUIRE_NO_THROW(database.externalInitialize());
//
//   BOOST_REQUIRE_EQUAL(database.isRunning(), true);
//
//   mock::MockConnection* conn0 = static_cast <mock::MockConnection*>(database.createConnection("0", "0", "0"));
//
//   persistence::Repository repository("persistence_storage_cache");
//   persistence::Storage* wr_storage = repository.createStorage(0, "storage_cache", persistence::Storage::AccessMode::ReadWrite);
//
//   BOOST_REQUIRE_THROW(wr_storage->setMaxCacheSize(10000), adt::RuntimeException);
//   BOOST_REQUIRE_THROW(wr_storage->setMaxCacheSize(0), adt::RuntimeException);
//
//   BOOST_REQUIRE_NO_THROW(wr_storage->setMaxCacheSize(16));
//
//   test_persistence::MockCustomerClass _class;
//
//   BOOST_REQUIRE_NE(wr_storage,(void*) 0);
//
//   test_persistence::MockCustomerLoader myLoader;
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class, "Register 100 new records");
//      test_persistence::MockCustomerCreator myCreator;
//      test_persistence::MockCustomerRecorder myRecorder;
//
//      BOOST_CHECK_NO_THROW(myCreator.initialize(guardCustomer, database.createStatement("creator", "none")));
//      BOOST_REQUIRE_NO_THROW(myRecorder.initialize(guardCustomer, database.createStatement("writer", "write")));
//
//      adt::StreamString name;
//
//      int initSize = database.container_size();
//
//      BOOST_REQUIRE_EQUAL(wr_storage->getSize(), 0);
//
//      for(int ii = 100; ii < 200; ++ ii) {
//         myCreator.setId(guardCustomer, ii);
//         myCreator.setName(guardCustomer, adt::StreamString("the name ") << ii);
//
//         persistence::Object& object = wr_storage->create(*conn0, guardCustomer, myCreator);
//
//         BOOST_REQUIRE_NO_THROW(myRecorder.setObject(object));
//
//         BOOST_REQUIRE_NO_THROW(wr_storage->save(*conn0, guardCustomer, myRecorder));
//
//         BOOST_REQUIRE_EQUAL(initSize +(ii - 99), database.container_size());
//
//         BOOST_REQUIRE_EQUAL(wr_storage->release(guardCustomer, object), true);
//
//         if((ii - 100) > 16)
//            BOOST_REQUIRE_EQUAL(wr_storage->getCacheSize(), 16);
//      }
//
//      BOOST_REQUIRE_EQUAL(wr_storage->getSize(), 16);
//   }
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class, "Read object stored in cache");
//
//      dbms::Statement* stReader = database.createStatement("read_only", "read");
//
//      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));
//
//      for(int ii = 200 - 16; ii < 200; ++ ii) {
//         BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, ii));
//
//         test_persistence::CustomerObjectWrapper& customer = static_cast <test_persistence::CustomerObjectWrapper&>(wr_storage->load(*conn0, guardCustomer, myLoader));
//
//         BOOST_REQUIRE_EQUAL(customer.getId(), ii);
//         BOOST_REQUIRE_EQUAL(customer.getName(), adt::StreamString("the name ") << ii);
//
//         BOOST_REQUIRE_EQUAL(wr_storage->getCacheSize(), 15);
//
//         BOOST_REQUIRE_EQUAL(wr_storage->release(guardCustomer, customer), true);
//      }
//
//      BOOST_REQUIRE_EQUAL(wr_storage->getHitCounter(), 16);
//      BOOST_REQUIRE_EQUAL(wr_storage->getCacheSize(), 16);
//   }
//}
//
//BOOST_AUTO_TEST_CASE(persistence_storage_using_auto)
//{
//   test_persistence::MyDatabase database("persistence_storage_using_auto");
//
//   BOOST_REQUIRE_NO_THROW(database.externalInitialize());
//
//   BOOST_REQUIRE_EQUAL(database.isRunning(), true);
//
//   dbms::Connection* conn0 = database.createConnection("0", "0", "0");
//   dbms::Statement* stReader = database.createStatement("read_only", "read");
//
//   persistence::Repository repository("persistence_storage_using_auto");
//   persistence::Storage* ro_storage = repository.createStorage(0, "storage_using_auto", persistence::Storage::AccessMode::ReadOnly);
//
//   test_persistence::MockCustomerClass _class;
//
//   BOOST_REQUIRE_NE(ro_storage,(void*) 0);
//
//   test_persistence::MockCustomerLoader myLoader;
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class, "First load 6");
//
//      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 6));
//      persistence::AutoObject<test_persistence::CustomerObjectWrapper> customer(ro_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(customer->getId(), 6);
//      BOOST_REQUIRE_EQUAL(customer->getName(), "the name 6");
//
//      persistence::AutoObject<test_persistence::CustomerObjectWrapper> customer2(ro_storage->load(*conn0, guardCustomer, myLoader));
//   }
//
//   BOOST_REQUIRE_EQUAL(ro_storage->getFaultCounter(), 1);
//   BOOST_REQUIRE_EQUAL(ro_storage->getHitCounter(), 1);
//   BOOST_REQUIRE_EQUAL(ro_storage->getCacheSize(), 1);
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class, "Second load 7");
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 7));
//      persistence::AutoObject<test_persistence::CustomerObjectWrapper> customer(ro_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(customer->getId(), 7);
//      BOOST_REQUIRE_EQUAL(customer->getName(), "the name 7");
//
//      BOOST_REQUIRE_EQUAL(ro_storage->getFaultCounter(), 2);
//      BOOST_REQUIRE_EQUAL(ro_storage->getCacheSize(), 1);
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 7));
//      persistence::AutoObject<test_persistence::CustomerObjectWrapper> customer2(ro_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(ro_storage->getCacheSize(), 1);
//
//      BOOST_REQUIRE_EQUAL(customer2->getId(), 7);
//      BOOST_REQUIRE_EQUAL(customer2->getName(), "the name 7");
//
//      BOOST_REQUIRE_EQUAL(ro_storage->getFaultCounter(), 2);
//      BOOST_REQUIRE_EQUAL(ro_storage->getHitCounter(), 2);
//
//      BOOST_REQUIRE_EQUAL(&customer.get(), &customer2.get());
//   }
//
//   BOOST_REQUIRE_EQUAL(ro_storage->getFaultCounter(), 2);
//   BOOST_REQUIRE_EQUAL(ro_storage->getHitCounter(), 2);
//   BOOST_REQUIRE_EQUAL(ro_storage->getCacheSize(), 2);
//
//   if(true) {
//      persistence::GuardClass guardCustomer(_class, "Reload records");
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 6));
//      persistence::AutoObject<test_persistence::CustomerObjectWrapper> customer(ro_storage->load(*conn0, guardCustomer, myLoader));
//
//      BOOST_REQUIRE_EQUAL(customer->getId(), 6);
//      BOOST_REQUIRE_EQUAL(ro_storage->getCacheSize(), 1);
//      BOOST_REQUIRE_EQUAL(ro_storage->getFaultCounter(), 2);
//      BOOST_REQUIRE_EQUAL(ro_storage->getHitCounter(), 3);
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 6));
//      customer = ro_storage->load(*conn0, guardCustomer, myLoader);
//
//      BOOST_REQUIRE_EQUAL(customer->getId(), 6);
//      BOOST_REQUIRE_EQUAL(ro_storage->getCacheSize(), 1);
//      BOOST_REQUIRE_EQUAL(ro_storage->getFaultCounter(), 2);
//      BOOST_REQUIRE_EQUAL(ro_storage->getHitCounter(), 4);
//
//      BOOST_REQUIRE_NO_THROW(myLoader.setId(guardCustomer, 7));
//      customer = ro_storage->load(*conn0, guardCustomer, myLoader);
//
//      BOOST_REQUIRE_EQUAL(customer->getId(), 7);
//      BOOST_REQUIRE_EQUAL(ro_storage->getCacheSize(), 1);
//      BOOST_REQUIRE_EQUAL(ro_storage->getFaultCounter(), 2);
//      BOOST_REQUIRE_EQUAL(ro_storage->getHitCounter(), 5);
//   }
//
//   BOOST_REQUIRE_EQUAL(ro_storage->getCacheSize(), 2);
//
//   BOOST_REQUIRE_NO_THROW(database.externalStop());
//}
