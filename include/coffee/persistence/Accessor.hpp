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
#ifndef __coffee_persistence_Accessor_hpp
#define __coffee_persistence_Accessor_hpp

#include <vector>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/NamedObject.hpp>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/ResultCode.hpp>

#include <coffee/persistence/PrimaryKey.hpp>

namespace coffee {

namespace adt {
   class DataBlock;
   class Second;
}

namespace dbms {
   class Statement;
   class Connection;
   class GuardConnection;
   class GuardStatement;

   namespace datatype {
      class Abstract;
   }
}

namespace persistence {

class Class;
class Object;

class Accessor : public adt::NamedObject {
public:
   typedef std::shared_ptr<dbms::Connection> TheConnection;
   typedef std::shared_ptr<dbms::Statement> TheStatement;
   typedef std::shared_ptr<Class> TheClass;
   typedef std::shared_ptr<Object> TheObject;
   typedef std::shared_ptr<PrimaryKey> ThePrimaryKey;
   
   TheStatement& getStatement() noexcept { return m_statement; }
   const ThePrimaryKey& getPrimaryKey() const throw(adt::RuntimeException) { return m_primaryKey; }
   
   bool hasAutoCommit() const noexcept { return m_autoCommit; }
   bool setAutoCommit(const bool value) { bool result(m_autoCommit); m_autoCommit = value; return result; }
   bool enableAutoCommit() noexcept { return setAutoCommit(true); }
   void disableAutoCommit() noexcept { setAutoCommit(false); }

   Accessor(const Accessor&) = delete;

protected:
   Accessor(const char* name, TheStatement& statement, const ThePrimaryKey& primaryKey) :
      adt::NamedObject(name),
      m_statement(statement),
      m_primaryKey(primaryKey),
      m_autoCommit(false)
   {;}

   Accessor(const char* name, const ThePrimaryKey& primaryKey) :
      adt::NamedObject(name),
      m_statement(nullptr),
      m_primaryKey(primaryKey),
      m_autoCommit(false)
   {;}
      
private:
   TheStatement m_statement;
   const ThePrimaryKey m_primaryKey;
   bool m_autoCommit;
};

} /* namespace persistence */
} /* namespace coffee */

#endif


