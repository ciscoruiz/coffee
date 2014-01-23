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
#include <wepa/persistence/Creator.hpp>
#include <wepa/persistence/Storage.hpp>
#include <wepa/persistence/GuardClass.hpp>

using namespace wepa;

auto_enum_assign(persistence::Storage::AccessMode) = { "ReadOnly", "ReadWrite", NULL };

persistence::Storage::Storage (const char* name, const AccessMode::_v accessMode) :
   adt::NamedObject (name),
   m_accessMode(accessMode),
   m_maxCacheSize (DefaultMaxCacheSize),
   m_cacheSize (0)
{
   m_hitCounter = m_faultCounter = 0;
}

persistence::Storage::~Storage ()
{
   for (entry_type ii : m_entries) {
      object (ii)->clear ();
      delete object(ii);
   }

   m_entries.clear ();
}

void persistence::Storage::setMaxCacheSize (const int maxCacheSize)
   throw (adt::RuntimeException)
{
   if (maxCacheSize > UpperLimitForMaxCacheSize || maxCacheSize < LowerLimitForMaxCacheSize) {
      WEPA_THROW_EXCEPTION("Storage='" << getName () << "' max cache size " << maxCacheSize << " out of range [" << LowerLimitForMaxCacheSize << "," << UpperLimitForMaxCacheSize << "]");
   }

   m_maxCacheSize = maxCacheSize;

   LOG_DEBUG ("Storage='" << getName () << "' | MaxCacheSize=" << maxCacheSize);
}

persistence::Object& persistence::Storage::load (dbms::Connection& connection, GuardClass& _class, persistence::Loader& loader)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   Object* result = NULL;

   const PrimaryKey& primaryKey (loader.getPrimaryKey());

   LOG_DEBUG (getName () << " | Loading=" << primaryKey);

   entry_iterator ii = m_entries.find (primaryKey);

   if (ii == m_entries.end ()) {
      m_faultCounter ++;
      result = createEntry (connection, _class, loader);
   }
   else {
      m_hitCounter ++;

      Entry& entry = Storage::entry(ii);
      try {
         result = instanciateAccessMode(m_accessMode).refresh(connection, _class, loader, entry);
         removeCachedEntry(entry);
         entry.m_useCounter ++;
      }
      catch (adt::Exception& ex) {
         logger::Logger::write(ex);
         delete entry.m_object;
         m_entries.erase(ii);
      }
   }

   if (result == NULL) {
      WEPA_THROW_EXCEPTION(loader << " | There is not any match for the loader");
   }

   LOG_DEBUG (loader << " | ObjectId=" << result->getInternalId() << " | Result=" << result->asString ());

   return std::ref (*result);
}

persistence::Object& persistence::Storage::create (dbms::Connection& connection, GuardClass& _class, Creator& creator)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   exceptionWhenReadOnly(creator);

   const PrimaryKey& primaryKey (creator.getPrimaryKey());

   LOG_DEBUG (getName () << " | Creating=" << primaryKey);

   entry_iterator ii = m_entries.find (primaryKey);

   if (ii != m_entries.end ()) {
      WEPA_THROW_EXCEPTION(getName () << " | " << primaryKey << " | PrimaryKey is already registered");
   }

   return std::ref (*createEntry(connection, _class, creator));
}

void persistence::Storage::save (dbms::Connection& connection, GuardClass& _class, Recorder& recorder)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   exceptionWhenReadOnly(recorder);

   Object &object = recorder.getObject();

   dbms::ResultCode resultCode = recorder.apply (connection, _class, object);

   if (resultCode.successful() == false)
      WEPA_THROW_NAME_DB_EXCEPTION(recorder.getName (), resultCode);

   LOG_DEBUG (recorder << " | ObjectId=" << object.getInternalId() << " | " << resultCode);
}

void persistence::Storage::erase (dbms::Connection& connection, GuardClass& _class, Eraser& eraser)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   exceptionWhenReadOnly(eraser);

   Object& object = eraser.getObject();

   const PrimaryKey& primaryKey = object.getPrimaryKey();

   LOG_DEBUG (getName () << " | Erasing =" << primaryKey);

   entry_iterator ii = m_entries.find (primaryKey);

   bool objectHasBeenFound;

   if (ii == m_entries.end ()) {
      LOG_WARN(primaryKey << " is not loaded in storage '" << getName () << "'");
      objectHasBeenFound = false;
   }
   else {
      Entry& entry = Storage::entry(ii);

      if (entry.hasMultipleReferences () == true) {
         WEPA_THROW_EXCEPTION(eraser.getName () << " can not erase primary key " << primaryKey << " due to multiple references");
      }

      objectHasBeenFound = true;
   }

   const std::string internalId = object.getInternalId();

   dbms::ResultCode resultCode = eraser.apply (connection, _class, object);

   if (resultCode.successful() == false)
      WEPA_THROW_NAME_DB_EXCEPTION(eraser.getName (), resultCode);

   if (objectHasBeenFound) {
      destroyEntry(ii);
   }

   LOG_DEBUG (eraser << " | ObjectId=" << internalId << " | " << resultCode);
}

bool persistence::Storage::release (GuardClass& _class, Object& object) noexcept
{
   bool result = false;

   try {
      const PrimaryKey& primaryKey = object.getPrimaryKey();

      LOG_DEBUG (getName () << " | Releasing=" << primaryKey);
      entry_iterator ii = m_entries.find (primaryKey);

      if (ii != m_entries.end ()) {
         Entry& entry = Storage::entry(ii);

         if (entry.m_useCounter == 0) {
            LOG_WARN ("Storage='" << getName () << "' | ObjectId=" << entry.m_object->getInternalId() << " has already been released");
         }
         else {
            LOG_DEBUG ("Storage='" << getName () << "' | ObjectId=" << entry.m_object->getInternalId() << " | Uses=" << entry.m_useCounter);

            if (-- entry.m_useCounter == 0) {
               setCachedEntry (entry);
               result = true;
            }
         }
      }
   }
   catch (adt::RuntimeException& ex) {
      LOG_ERROR (ex.asString ());
   }

   LOG_DEBUG (getName () << " | Result=" << result);

   return result;
}

persistence::Object* persistence::Storage::createEntry (dbms::Connection& connection, GuardClass& _class, Accessor& accessor)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   Entry entry;

   const PrimaryKey& primaryKey = accessor.getPrimaryKey();

   LOG_DEBUG ("Storage='" << getName () << "' | " << primaryKey);

   try {
      if (m_cacheSize < m_maxCacheSize) {
         entry.m_object = _class.createObject();
      }
      else {
         entry.m_object = reuseCachedObject ();
      }

      dbms::ResultCode resultCode = accessor.apply (connection, _class, std::ref (*entry.m_object));

      if (resultCode.successful() == false)
         WEPA_THROW_NAME_DB_EXCEPTION(accessor.getName(), resultCode);

      entry.m_useCounter = 1;
      std::pair <entry_iterator, bool> rr = m_entries.insert (std::make_pair(primaryKey, entry));
      entry.m_object->setPrimaryKey(Storage::primaryKey (rr.first));
   }
   catch (adt::Exception& ex) {
      delete entry.m_object;
      throw;
   }

   LOG_DEBUG ("Result (ObjectId)=" << entry.m_object->getInternalId());

   return entry.m_object;
}

void persistence::Storage::destroyEntry (entry_iterator ii)
   noexcept
{
   Entry& entry = Storage::entry(ii);

   LOG_DEBUG ("Storage='" << getName () << "' | ObjectId=" << entry.m_object->getInternalId());

   entry.m_object->clear();

   removeCachedEntry(entry);

   delete entry.m_object;

   m_entries.erase (ii);
}

void persistence::Storage::setCachedEntry (Entry& entry)
   noexcept
{
   // Remember we must not call to m_cache.size () because it has O(n) efficiency
   m_cacheSize ++;

   entry.m_useCounter = 0;
   entry.m_cached.first = true;
   entry.m_cached.second = m_cache.insert (m_cache.end (), entry.m_object);

   LOG_DEBUG ("Storage='" << getName () << "' | ObjectId=" << entry.m_object->getInternalId());
}

void persistence::Storage::removeCachedEntry (Entry& entry)
   noexcept
{
   if (entry.m_cached.first == true) {
      m_cache.erase(entry.m_cached.second);
      m_cacheSize --;
      entry.m_cached.first = false;

      LOG_DEBUG ("Storage='" << getName () << "' | ObjectId=" << entry.m_object->getInternalId());
   }
}

persistence::Object* persistence::Storage::reuseCachedObject ()
   throw (adt::RuntimeException)
{
   persistence::Object* result = NULL;

   // Removes from cache
   result = Storage::object (m_cache.begin ());
   m_cache.pop_front();
   m_cacheSize --;

   // Removes from main object container
   entry_iterator ii = m_entries.find(result->getPrimaryKey());

   assert (ii != m_entries.end ());

   m_entries.erase (ii);

   result->clear();
   result->clearPrimaryKey();

   LOG_DEBUG ("Storage='" << getName () << "' | ObjectId=" << result->getInternalId());

   return result;
}

adt::StreamString persistence::Storage::asString () const
   noexcept
{
   adt::StreamString result (adt::StreamString::Flag::ShowNull);

   result = "persistence.Storage {";
   result << adt::NamedObject::asString();
   result << " | Mode=" << AccessMode::enumName (m_accessMode);
   result << " | Hit=" << m_hitCounter;
   result << " | Fault=" << m_faultCounter;
   result << " | N-size=" << m_entries.size ();
   result << " | CacheSize=" << m_cacheSize << "/" << m_maxCacheSize;
   return result += " }";
}

xml::Node& persistence::Storage::asXML (xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild("persistence.Storage");

   result.createAttribute("Name", getName ());
   result.createAttribute ("Mode", AccessMode::enumName(m_accessMode));
   result.createAttribute ("N-size", m_entries.size ());

   xml::Node& access = result.createChild ("Access");
   access.createAttribute ("Hit", m_hitCounter);
   access.createAttribute ("Fault", m_faultCounter);

   xml::Node& cache = result.createChild ("Cache (LRU)");
   access.createAttribute ("Size", m_cacheSize);
   access.createAttribute ("MaxSize", m_maxCacheSize);

   return std::ref (result);
}

void persistence::Storage::exceptionWhenReadOnly (const Accessor& accessor) const
   throw (adt::RuntimeException)
{
   if (instanciateAccessMode(m_accessMode).canWrite() == false) {
      WEPA_THROW_EXCEPTION("Accessor '" << accessor.getName () << "' can not write on a read-only storage '" << getName () << "'");
   }
}

//static
const persistence::Storage::AccessMode& persistence::Storage::instanciateAccessMode (const AccessMode::_v accessMode)
   throw (adt::RuntimeException)
{
   static AccessModeReadOnly readOnly;
   static AccessModeReadWrite readWrite;

   static const AccessMode* st_accessMode [2] = { &readOnly, &readWrite };

   if (accessMode == AccessMode::None)
      WEPA_THROW_EXCEPTION("There is not define any AccessMode");

   return std::ref (*st_accessMode [accessMode]);
}

persistence::Object* persistence::Storage::AccessMode::reload (dbms::Connection& connection, GuardClass& _class, Loader& loader, Entry& entry) const
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   Object& object = std::ref (*entry.m_object);

   bool hasToRefresh = loader.hasToRefresh(connection, _class, object);

   LOG_DEBUG (object << " | HasToRefresh=" << hasToRefresh);

   if (hasToRefresh == true) {
      LOG_DEBUG ("Release dependences for " << entry.m_object->asString ());
      object.clear();

      LOG_DEBUG ("Reloading data for " << entry.m_object->asString ());
      dbms::ResultCode resultCode = loader.apply (connection, _class, object);

      if (resultCode.successful() == false)
         WEPA_THROW_NAME_DB_EXCEPTION(loader.getName(), resultCode);
   }

   LOG_DEBUG ("Result=" << entry.m_object->asString ());

   return entry.m_object;
}


persistence::Object* persistence::Storage::AccessModeReadOnly::refresh(dbms::Connection&, GuardClass& _class, persistence::Loader& loader, Storage::Entry& entry) const
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_DEBUG("Loader=" << loader.getName () << " | ObjectId=" << entry.m_object->getInternalId());
   return entry.m_object;
}

persistence::Object* persistence::Storage::AccessModeReadWrite::refresh(dbms::Connection& connection, GuardClass& _class, persistence::Loader& loader, Storage::Entry& entry) const
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   persistence::Object* result = this->reload (connection,_class, loader, entry);
   LOG_DEBUG("Loader=" << loader.getName () << " | ObjectId=" << result->getInternalId());
   return result;
}

