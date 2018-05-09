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

#ifndef __coffee_persistence_Storage_hpp
#define __coffee_persistence_Storage_hpp

#include <memory>

#include <coffee/basis/NamedObject.hpp>

#include <coffee/basis/pattern/lru/Cache.hpp>

#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/Accessor.hpp>

#include <coffee/dbms/DatabaseException.hpp>

namespace coffee {
   namespace xml {
      class Node;
   }
   namespace dbms {
      class Connection;
      class GuardConnection;
   }

namespace persistence {

class Repository;
class Object;
class Loader;
class Recorder;
class Eraser;
class Creator;

class Storage : public basis::NamedObject {
public:

private:
   typedef basis::pattern::lru::Cache<
         Accessor::ThePrimaryKey,
         Accessor::TheObject,
         persistence::PrimaryKey::HashSharedPointer,
         persistence::PrimaryKey::EqualSharedPointer> Cache;

   static const int UpperLimitForMaxCacheSize = 4 * 1024;
   static const int LowerLimitForMaxCacheSize = 16;

public:

   static const int DefaultMaxCacheSize = 128;

   Storage(const std::string& name, const int maxCacheSize);
   ~Storage();

   unsigned int getHitCounter() const noexcept { return m_hitCounter; }
   unsigned int getFaultCounter() const noexcept { return m_faultCounter; }

   Accessor::TheObject load(Accessor::TheConnection& connection, Loader& loader)
      throw(basis::RuntimeException, dbms::DatabaseException);
   void save(Accessor::TheConnection& connection, Recorder& recorder)
      throw(basis::RuntimeException, dbms::DatabaseException);
   void erase(Accessor::TheConnection& connection, Eraser& eraser)
      throw(basis::RuntimeException, dbms::DatabaseException);

   void save(dbms::GuardConnection& connection, Recorder& recorder)
      throw(basis::RuntimeException, dbms::DatabaseException);
   void erase(dbms::GuardConnection& connection, Eraser& eraser)
      throw(basis::RuntimeException, dbms::DatabaseException);

   operator basis::StreamString() const noexcept { return asString(); }

   basis::StreamString asString() const noexcept;

   std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const throw(basis::RuntimeException);

   Storage() = delete;
   Storage(const Storage&) = delete;
   
private:
   Cache m_cache;
   mutable unsigned int m_hitCounter;
   mutable unsigned int m_faultCounter;
};

} /* namespace persistence */
} /* namespace coffee */
#endif
