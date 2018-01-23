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
#ifndef _coffee_dbms_datatype_Set_h
#define _coffee_dbms_datatype_Set_h

#include <memory>
#include <unordered_map>

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/InvalidDataException.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

namespace adt {
   class Date;
   class DataBlock;
   class Second;
}

namespace dbms {

namespace datatype {

class Set {
public:
   typedef std::unordered_map<std::string, std::shared_ptr<Abstract> > Datas;
   typedef Datas::iterator data_iterator;
   typedef Datas::const_iterator const_data_iterator;

   Set() {;}
   Set(const Set& other) : m_datas(other.m_datas) {;}

   virtual ~Set() {
      m_datas.clear();
   }

   void clear() { m_datas.clear(); }

   void insert(std::shared_ptr<Abstract> data) throw (adt::RuntimeException);
   std::shared_ptr<Abstract>& find(const std::string& name) throw (adt::RuntimeException);
   const std::shared_ptr<Abstract>& find(const std::string& name) const throw (adt::RuntimeException);
   bool constains(const std::string& name) const noexcept { return m_datas.find(name) != end(); }

   size_t size() const noexcept { return m_datas.size(); }
   bool empty() const noexcept { return m_datas.empty(); }

   data_iterator begin() noexcept { return m_datas.begin(); }
   data_iterator end() noexcept { return m_datas.end(); }
   static const std::string& name(data_iterator ii) { return ii->first; }
   static std::shared_ptr<Abstract>& data(data_iterator ii) { return ii->second; }

   const_data_iterator begin() const noexcept { return m_datas.begin(); }
   const_data_iterator end() const noexcept { return m_datas.end(); }
   static const std::string& name(const_data_iterator ii) { return ii->first; }
   static const std::shared_ptr<Abstract>& data(const_data_iterator ii) { return ii->second; }

   Set& operator=(const Set& other) noexcept;

   int compare(const Set& other) const throw (adt::RuntimeException);
   int compare(const std::shared_ptr<Set>& other) const throw (adt::RuntimeException) { return compare(*other.get()); }

   bool operator==(const Set& other) const noexcept;
   bool operator< (const Set& other) const throw (adt::RuntimeException){ return compare(other) < 0; }

   int getInteger(const std::string& columnName) const throw(dbms::InvalidDataException);
   std::string getString(const std::string& columnName) const throw(adt::RuntimeException, dbms::InvalidDataException);
   float getFloat(const std::string& columnName) const throw(dbms::InvalidDataException);
   const adt::DataBlock& getDataBlock(const std::string& columnName) const throw(dbms::InvalidDataException);
   const adt::Second& getDate(const std::string& columnName) const throw(dbms::InvalidDataException);

   void setInteger(const std::string& columnName, const int value) throw(dbms::InvalidDataException);
   void setString(const std::string& columnName, const std::string& value) throw(adt::RuntimeException, dbms::InvalidDataException);
   void setFloat(const std::string& columnName, const float value) throw(dbms::InvalidDataException);
   void setDataBlock(const std::string& columnName, const adt::DataBlock& value) throw(dbms::InvalidDataException);
   void setDate(const std::string& columnName, const adt::Second& date) throw(adt::RuntimeException, dbms::InvalidDataException);

private:
   Datas m_datas;
};

}
}
}




#endif
