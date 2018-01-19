// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2018 Francisco Ruiz Rayo
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
#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/adt/AsHexString.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

#include <wepa/persistence/Object.hpp>
#include <wepa/persistence/Loader.hpp>
#include <wepa/persistence/Recorder.hpp>
#include <wepa/persistence/Eraser.hpp>
#include <wepa/persistence/Storage.hpp>
#include <wepa/persistence/Class.hpp>

#include <wepa/dbms/GuardConnection.hpp>
#include <wepa/dbms/GuardStatement.hpp>

using namespace wepa;
using namespace wepa::persistence;

Storage::Storage(const std::string& name, const int maxCacheSize) :
   adt::NamedObject(name),
   m_cache(maxCacheSize)
{
   m_hitCounter = m_faultCounter = 0;
}

Storage::~Storage()
{
}

Accessor::TheObject Storage::load(Accessor::TheConnection& connection, Loader& loader)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   Accessor::TheObject result;

   const Accessor::ThePrimaryKey& primaryKey(loader.getPrimaryKey());

   LOG_DEBUG(asString() << " | Loading=" << primaryKey->asString());

   Cache::pair_iterator vv = m_cache.find(primaryKey);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStatement(guardConnection, loader.getStatement());

   if(vv == m_cache.end()) {
      m_faultCounter ++;

      result = loader.getClass()->createObject(primaryKey);

      dbms::ResultCode resultCode = loader.apply(guardStatement, result);

      if(resultCode.successful() == false)
         WEPA_THROW_NAME_DB_EXCEPTION(loader.getName(), resultCode);

      m_cache.set(primaryKey, result);
   }
   else {
      m_hitCounter ++;

      result = m_cache.value(vv);

      if (loader.hasToRefresh(guardStatement, result)) {
         dbms::ResultCode resultCode = loader.apply(guardStatement, result);

         if(resultCode.successful() == false)
            WEPA_THROW_NAME_DB_EXCEPTION(loader.getName(), resultCode);
      }
   }

   if(!result) {
      WEPA_THROW_EXCEPTION(loader.asString() << " | There is not any match for the loader");
   }

   LOG_DEBUG(loader << " | ObjectId=" << result->getInternalId() << " | Result=" << result->asString());

   return result;
}

void Storage::save(Accessor::TheConnection& connection, Recorder& recorder)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();
   dbms::GuardConnection guardConnection(connection);
   save(guardConnection, recorder);
}

void Storage::save(dbms::GuardConnection& guardConnection, Recorder& recorder)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   dbms::GuardStatement guardStatement(guardConnection, recorder.getStatement());

   Accessor::TheObject object = recorder.getObject();

   dbms::ResultCode resultCode = recorder.apply(guardStatement);

   if(resultCode.successful() == false)
      WEPA_THROW_NAME_DB_EXCEPTION(recorder.getName(), resultCode);

   if (recorder.hasAutoCommit()) {
      guardConnection.commit();
   }

   LOG_DEBUG(recorder << " | ObjectId=" << object->getInternalId() << " | " << resultCode);
}

void Storage::erase(Accessor::TheConnection& connection, Eraser& eraser)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();
   dbms::GuardConnection guardConnection(connection);
   erase(guardConnection, eraser);
}

void Storage::erase(dbms::GuardConnection& guardConnection, Eraser& eraser)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   const Accessor::ThePrimaryKey& primaryKey(eraser.getPrimaryKey());

   LOG_DEBUG(asString() << " | Erasing =" << primaryKey->asString());
   
   if(!m_cache.erase(primaryKey)) {
      LOG_DEBUG(primaryKey->asString() << " is not loaded in storage '" << getName() << "'");
      m_faultCounter ++;
   }
   else {      
      m_hitCounter ++;
   }

   dbms::GuardStatement guardStatement(guardConnection, eraser.getStatement());

   dbms::ResultCode resultCode = eraser.apply(guardStatement);

   if(resultCode.successful() == false)
      WEPA_THROW_NAME_DB_EXCEPTION(eraser.getName(), resultCode);

   if (eraser.hasAutoCommit()) {
      guardConnection.commit();
   }

   LOG_DEBUG(eraser << " | " << primaryKey->asString() << " | " << resultCode);
}

adt::StreamString Storage::asString() const
   noexcept
{
   adt::StreamString result(adt::StreamString::Flag::ShowNull);

   result = "persistence.Storage {";
   result << adt::NamedObject::asString();
   result << " | Hit=" << m_hitCounter;
   result << " | Fault=" << m_faultCounter;
   result << " | " << m_cache;
   return result += " }";
}

xml::Node& Storage::asXML(xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild("persistence.Storage");

   result.createAttribute("Name", adt::NamedObject::getName());
   xml::Node& access = result.createChild("Access");
   access.createAttribute("Hit", m_hitCounter);
   access.createAttribute("Fault", m_faultCounter);

   xml::Node& cache = result.createChild("Cache(LRU)");
   access.createAttribute("Size", adt::AsString::apply((int) m_cache.size()));

   return std::ref(result);
}

