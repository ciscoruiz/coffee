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
#ifndef __wepa_persistence_Object_hpp
#define __wepa_persistence_Object_hpp

#include <functional>

#include <wepa/adt/StreamString.hpp>
#include <wepa/adt/RuntimeException.hpp>

#include <wepa/dbms/DatabaseException.hpp>
#include <wepa/dbms/InvalidDataException.hpp>

#include <wepa/persistence/Class.hpp>

namespace wepa {
   
namespace adt {
   class Date;
   class DataBlock;
   class Second;
}
namespace persistence {

class PrimaryKey;
class Class;


//// See http://stackoverflow.com/questions/1319876/weird-c-templating-issues
//template <typename _T> class AutoObject;

class Object {
public:
   Object(const Class& _class, const std::shared_ptr<PrimaryKey>& primaryKey, const Class::Members& members);

   virtual ~Object() { 
       m_primaryKey.reset();
       m_members.clear();
   }

   const Class& getClass() const noexcept { return std::ref(m_class); }
   const std::shared_ptr<PrimaryKey>& getPrimaryKey() const throw(adt::RuntimeException) { return m_primaryKey; }
   std::shared_ptr<dbms::datatype::Abstract>& getMember(const int columnNumber) throw(adt::RuntimeException);
   const std::shared_ptr<dbms::datatype::Abstract>& getMember(const int columnNumber) const throw(adt::RuntimeException);

   std::string getInternalId() const noexcept;
   
   int getInteger(const int columnNumber) const throw(dbms::InvalidDataException);
   std::string getString(const int columnNumber) const throw(adt::RuntimeException, dbms::InvalidDataException);
   float getFloat(const int columnNumber) const throw(dbms::InvalidDataException);
   const adt::DataBlock& getDataBlock(const int columnNumber) const throw(dbms::InvalidDataException);
   const adt::Second& getDate(const int columnNumber) const throw(dbms::InvalidDataException);

   void setInteger(const int columnNumber, const int value) throw(dbms::InvalidDataException);
   void setString(const int columnNumber, const std::string& value) throw(adt::RuntimeException, dbms::InvalidDataException);
   void setFloat(const int columnNumber, const float value) throw(dbms::InvalidDataException);
   void setDataBlock(const int columnNumber, const adt::DataBlock& value) throw(dbms::InvalidDataException);
   void setDate(const int columnNumber, const adt::Second& date) throw(adt::RuntimeException, dbms::InvalidDataException);
   
   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;

private:
   const Class& m_class;
   std::shared_ptr<PrimaryKey> m_primaryKey;
   Class::Members m_members;
};

} /* namespace persistence */
} /* namespace wepa */
#endif
