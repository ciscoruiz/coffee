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

#ifndef _coffee_dbms_datatype_Set_h
#define _coffee_dbms_datatype_Set_h

#include <memory>
#include <unordered_map>
#include <vector>
#include <chrono>

#include <coffee/basis/RuntimeException.hpp>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/InvalidDataException.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

namespace basis {
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
   typedef std::vector<data_iterator> CreationOrderAccess;

   Set() {;}
   Set(const Set& other);

   virtual ~Set() {
      m_datas.clear();
   }

   void clear() { m_datas.clear(); }

   void insert(std::shared_ptr<Abstract> data) throw (basis::RuntimeException);
   std::shared_ptr<Abstract>& find(const std::string& name) throw (basis::RuntimeException);
   const std::shared_ptr<Abstract>& find(const std::string& name) const throw (basis::RuntimeException);
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
   const_data_iterator search(const std::string& name) const noexcept { return m_datas.find(name); }

   Set& operator=(const Set& other) throw(basis::RuntimeException);

   int compare(const Set& other) const throw (basis::RuntimeException);
   int compare(const std::shared_ptr<Set>& other) const throw (basis::RuntimeException) { return compare(*other.get()); }

   bool operator==(const Set& other) const noexcept;
   bool operator< (const Set& other) const throw (basis::RuntimeException){ return compare(other) < 0; }
   bool operator!=(const Set& other) const noexcept { return !operator==(other); }

   int getInteger(const std::string& columnName) const throw(basis::RuntimeException, dbms::InvalidDataException);
   std::string getString(const std::string& columnName) const throw(basis::RuntimeException, dbms::InvalidDataException);
   float getFloat(const std::string& columnName) const throw(basis::RuntimeException, dbms::InvalidDataException);
   const basis::DataBlock& getDataBlock(const std::string& columnName) const throw(basis::RuntimeException, dbms::InvalidDataException);
   const std::chrono::seconds& getDate(const std::string& columnName) const throw(basis::RuntimeException, dbms::InvalidDataException);

   void setInteger(const std::string& columnName, const int value) throw(basis::RuntimeException,dbms::InvalidDataException);
   void setString(const std::string& columnName, const std::string& value) throw(basis::RuntimeException, dbms::InvalidDataException);
   void setFloat(const std::string& columnName, const float value) throw(basis::RuntimeException,dbms::InvalidDataException);
   void setDataBlock(const std::string& columnName, const basis::DataBlock& value) throw(basis::RuntimeException,dbms::InvalidDataException);
   void setDate(const std::string& columnName, const std::chrono::seconds& date) throw(basis::RuntimeException, dbms::InvalidDataException);

private:
   Datas m_datas;
   CreationOrderAccess m_creationOrder;
};

}
}
}




#endif
