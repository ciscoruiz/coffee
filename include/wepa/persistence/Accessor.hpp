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
#ifndef __wepa_persistence_Accessor_hpp
#define __wepa_persistence_Accessor_hpp

#include <vector>

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/adt/NamedObject.hpp>

#include <wepa/dbms/DatabaseException.hpp>

#include <wepa/persistence/PrimaryKey.hpp>

namespace wepa {

namespace dbms {
   class Statement;

   namespace datatype {
      class Abstract;
   }
}

namespace persistence {

class Class;
class GuardClass;
class Object;

class Accessor : public adt::NamedObject {
   typedef std::vector <dbms::datatype::Abstract*> Values;

public:
   virtual ~Accessor () { m_statement = NULL; }

   void initialize (Class& _class, dbms::Statement* statement) throw (adt::RuntimeException);

   const PrimaryKey& getPrimaryKey () const throw (adt::RuntimeException);
   const int getIdent () const noexcept { return m_ident; }

   virtual void apply (GuardClass& _class, Object& object) throw (adt::RuntimeException, dbms::DatabaseException) = 0;

   Accessor (const Accessor&) = delete;

protected:
   Accessor (const char* name, const int ident) : adt::NamedObject (name), m_ident (ident), m_statement (NULL) {;}

   dbms::Statement& getStatement () throw (adt::RuntimeException);

   virtual bool isInputValue (const int columnNumber) const noexcept = 0;
   virtual bool isPrimaryKeyComponent (const int columnNumber) const noexcept = 0;
   virtual bool isOutputValue (const int columnNumber) const noexcept = 0;

private:
   // Store pointers to remove when terminate the AccessoIf
   Values m_inputValues;
   Values m_outputValues;
   PrimaryKey m_primaryKey;
   const int m_ident;
   dbms::Statement* m_statement;
};

} /* namespace persistence */
} /* namespace wepa */

#endif


