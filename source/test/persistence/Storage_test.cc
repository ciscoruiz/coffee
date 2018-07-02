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

#include <gtest/gtest.h>

#include <thread>
#include <iostream>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

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

#include <MockDatabase.hpp>
#include <MockLowLevelRecord.hpp>
#include <MockConnection.hpp>

#include "PersistenceMocks.hpp"
#include "MockDatabaseFixture.hpp"

using namespace coffee;
using namespace coffee::mock;

struct StorageTestFixture : public MockDatabaseFixture<test_persistence::MyDatabase> {
   persistence::Repository repository;

   std::shared_ptr<dbms::Statement> readerStatement;
   std::shared_ptr<dbms::Statement> writerStatement;
   std::shared_ptr<dbms::Statement> eraserStatement;

   std::shared_ptr<persistence::Storage> storage;
   std::shared_ptr<persistence::Class> customerClass;
   std::shared_ptr<persistence::PrimaryKey> primaryKeyForFind;

   StorageTestFixture() : MockDatabaseFixture<test_persistence::MyDatabase>("Storage-test"), repository("repoName") {}

   void SetUp() {
      MockDatabaseFixture<test_persistence::MyDatabase>::SetUp();

      readerStatement = database->createStatement("read_only", "read");
      writerStatement = database->createStatement("writer", "write");
      eraserStatement = database->createStatement("eraser", "delete");

      ASSERT_TRUE(database->isRunning());

      storage = repository.createStorage("default", persistence::Storage::DefaultMaxCacheSize);

      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id"));
      std::shared_ptr<persistence::PrimaryKey> primaryKeyCustomer = pkBuilder.build();
      primaryKeyForFind = pkBuilder.build();

      persistence::ClassBuilder classBuilder("customer");
      classBuilder.set(primaryKeyCustomer).add(std::make_shared<dbms::datatype::String>("name", 64));
      customerClass = classBuilder.build();

      mockDatabase = std::dynamic_pointer_cast<test_persistence::MyDatabase>(database);
   }

   std::shared_ptr<test_persistence::MyDatabase> mockDatabase;

};

TEST_F(StorageTestFixture, read)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   if(true) {
      primaryKeyForFind->setInteger("id", 6);
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(6, customer.getId());
      ASSERT_EQ("the name 6", customer.getName());

      test_persistence::CustomerObjectWrapper customer2(storage->load(connection, myLoader));

      ASSERT_EQ(customer2.getId(), customer.getId());
      ASSERT_EQ(customer2.getName(), customer.getName());
   }

   ASSERT_EQ(1, storage->getFaultCounter());
   ASSERT_EQ(1, storage->getHitCounter());

   if(true) {
      primaryKeyForFind->setInteger("id", 7);
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(7, customer.getId());
      ASSERT_EQ("the name 7", customer.getName());

      test_persistence::CustomerObjectWrapper customer2(storage->load(connection, myLoader));
      ASSERT_EQ(customer2.getId(), customer.getId());
   }

   ASSERT_EQ(2, storage->getFaultCounter());
   ASSERT_EQ(2, storage->getHitCounter());
}

TEST_F(StorageTestFixture, reload)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(6, customer.getId());
      ASSERT_EQ("the name 6", customer.getName());

      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(0, storage->getHitCounter());
   }

   mock::MockLowLevelRecord newSix;
   newSix.m_id = 6;
   newSix.m_name = "only odd id's will be reloaded";
   newSix.m_float = 0.123;
   newSix.m_integer = 6 * 6;
   database->update(newSix);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(6, customer.getId());
      ASSERT_EQ("only odd id's will be reloaded", customer.getName());

      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(1, storage->getHitCounter());
   }
}

TEST_F(StorageTestFixture, reload_erased)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(6, customer.getId());
      ASSERT_EQ("the name 6", customer.getName());

      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(0, storage->getHitCounter());
   }

   database->erase(6);

   {
      ASSERT_THROW(storage->load(connection, myLoader), dbms::DatabaseException);
      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(1, storage->getHitCounter());
   }
}

TEST_F(StorageTestFixture, noreload_erased)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 7);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(7, customer.getId());
      ASSERT_EQ("the name 7", customer.getName());

      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(0, storage->getHitCounter());
   }

   database->erase(7);

   {
      ASSERT_NO_THROW(storage->load(connection, myLoader));
      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(1, storage->getHitCounter());
   }
}

TEST_F(StorageTestFixture, noreload)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 7);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(7, customer.getId());
      ASSERT_EQ("the name 7", customer.getName());

      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(0, storage->getHitCounter());
   }

   mock::MockLowLevelRecord newSix;
   newSix.m_id = 7;
   newSix.m_name = "even id will not be reload";
   newSix.m_float = 0.123;
   newSix.m_integer = 6 * 6;
   database->update(newSix);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(7, customer.getId());
      ASSERT_EQ("the name 7", customer.getName());

      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(1, storage->getHitCounter());
   }
}

TEST_F(StorageTestFixture, read_not_found)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6000);
   ASSERT_THROW(storage->load(connection, myLoader), dbms::DatabaseException);

   ASSERT_EQ(1, storage->getFaultCounter());
   ASSERT_EQ(0, storage->getHitCounter());
}

TEST_F(StorageTestFixture, write)
{
   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter, mockDatabase->container_size());

   if (true) {
      primaryKeyForFind->setInteger("id", 5555);
      auto object = customerClass->createObject(primaryKeyForFind);

      object->setString("name", "5555 name");
      test_persistence::MockCustomerRecorder recorder(writerStatement, object);

      recorder.enableAutoCommit();

      storage->save(connection, recorder);
   }

   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter + 1, mockDatabase->container_size());

   ASSERT_EQ(0, storage->getFaultCounter());
   ASSERT_EQ(0, storage->getHitCounter());

   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   ASSERT_NO_THROW(storage->load(connection, myLoader));

   ASSERT_EQ(1, storage->getFaultCounter());
   ASSERT_EQ(0, storage->getHitCounter());
}

TEST_F(StorageTestFixture, write_fail)
{
   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter, mockDatabase->container_size());

   if (true) {
      primaryKeyForFind->setInteger("id", test_persistence::IdToThrowDbException);
      auto object = customerClass->createObject(primaryKeyForFind);

      object->setString("name", "5555 name");
      test_persistence::MockCustomerRecorder recorder(writerStatement, object);

      ASSERT_THROW(storage->save(connection, recorder), dbms::DatabaseException);
   }

   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter, mockDatabase->container_size());
}

TEST_F(StorageTestFixture, erase_preloaded)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6);

   {
      test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

      ASSERT_EQ(6, customer.getId());
      ASSERT_EQ("the name 6", customer.getName());

      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(0, storage->getHitCounter());
   }

   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter, mockDatabase->container_size());

   if (true) {
      primaryKeyForFind->setInteger("id", 6);
      test_persistence::MockCustomerEraser eraser(eraserStatement, primaryKeyForFind);

      eraser.enableAutoCommit();

      storage->erase(connection, eraser);

      ASSERT_EQ(1, storage->getFaultCounter());
      ASSERT_EQ(1, storage->getHitCounter());
   }

   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter - 1, mockDatabase->container_size());

   ASSERT_THROW(storage->load(connection, myLoader), dbms::DatabaseException);

   ASSERT_EQ(2, storage->getFaultCounter());
   ASSERT_EQ(1, storage->getHitCounter());
}


TEST_F(StorageTestFixture, erase_noloaded)
{
   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter, mockDatabase->container_size());

   primaryKeyForFind->setInteger("id", 6);
   test_persistence::MockCustomerEraser eraser(eraserStatement, primaryKeyForFind);

   eraser.enableAutoCommit();

   storage->erase(connection, eraser);

   ASSERT_EQ(1, storage->getFaultCounter());
   ASSERT_EQ(0, storage->getHitCounter());

   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter - 1, mockDatabase->container_size());

   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   ASSERT_THROW(storage->load(connection, myLoader), dbms::DatabaseException);

   ASSERT_EQ(2, storage->getFaultCounter());
   ASSERT_EQ(0, storage->getHitCounter());
}

TEST_F(StorageTestFixture, delete_fail)
{
   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter, mockDatabase->container_size());

   primaryKeyForFind->setInteger("id", test_persistence::IdToThrowDbException);
   test_persistence::MockCustomerEraser eraser(eraserStatement, primaryKeyForFind);
   ASSERT_THROW(storage->erase(connection, eraser), dbms::DatabaseException);
}

TEST_F(StorageTestFixture, save_commit_pending)
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

         basis::StreamString streamString;
         streamString << ii << " - name";
         object->setString("name", streamString);
         test_persistence::MockCustomerRecorder recorder(writerStatement, object);

         storage->save(guardConnection, recorder);
      }
   }

   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter + (maxCommitPending * commitCounter), mockDatabase->container_size());

   auto mockConnection = std::dynamic_pointer_cast<mock::MockConnection>(connection);

   ASSERT_EQ(commitCounter, mockConnection->getCommitCounter());
}

TEST_F(StorageTestFixture, erase_commit_pending)
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

   ASSERT_EQ(test_persistence::MyDatabase::PreloadRegisterCounter + 10 - maxId, mockDatabase->container_size());

   auto mockConnection = std::dynamic_pointer_cast<mock::MockConnection>(connection);

   ASSERT_EQ(commitCounter, mockConnection->getCommitCounter());
}

TEST_F(StorageTestFixture, type_mismatch_primarykey)
{
   {
      test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);
      primaryKeyForFind->setInteger("id", 6);
      ASSERT_NO_THROW(storage->load(connection, myLoader));
   }
   {
      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::String>("id", 64));
      auto unmatchesPrimaryKey = pkBuilder.build();
      unmatchesPrimaryKey->setString("id", "unused");

      test_persistence::MockCustomerLoader myLoader(readerStatement, unmatchesPrimaryKey, customerClass);
      ASSERT_THROW(storage->load(connection, myLoader), basis::RuntimeException);
   }
}

TEST_F(StorageTestFixture, size_mismatch_primarykey)
{
   {
      test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);
      primaryKeyForFind->setInteger("id", 6);
      ASSERT_NO_THROW(storage->load(connection, myLoader));
   }
   {
      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::String>("id", 64));
      pkBuilder.add(std::make_shared<dbms::datatype::String>("other", 64));
      auto unmatchesPrimaryKey = pkBuilder.build();
      unmatchesPrimaryKey->setString("id", "unused");
      unmatchesPrimaryKey->setString("other", "unused");

      test_persistence::MockCustomerLoader myLoader(readerStatement, unmatchesPrimaryKey, customerClass);
      ASSERT_THROW(storage->load(connection, myLoader), basis::RuntimeException);
   }
}

TEST(StorageTest, empty_pkbuilder)
{
   persistence::PrimaryKeyBuilder builder;
   ASSERT_THROW(builder.build(), basis::RuntimeException);
}

TEST_F(StorageTestFixture, showclass)
{
   auto root = std::make_shared<xml::Node>("root");
   auto xmlNode = customerClass->asXML(root);

   ASSERT_EQ("persistence.Class", xmlNode->getName());
   ASSERT_EQ("customer", xmlNode->lookupAttribute("Name")->getValue());
}

TEST_F(StorageTestFixture, asstring)
{
   test_persistence::MockCustomerLoader myLoader(readerStatement, primaryKeyForFind, customerClass);

   primaryKeyForFind->setInteger("id", 6);
   test_persistence::CustomerObjectWrapper customer(storage->load(connection, myLoader));

   std::string text = storage->asString();

   ASSERT_TRUE(text.find("pattern.lru.Cache { MaxSize=128 | Size=1 }") != std::string::npos);
}



