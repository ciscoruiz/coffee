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

#ifndef __coffee_persistence_Accessor_hpp
#define __coffee_persistence_Accessor_hpp

#include <vector>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/NamedObject.hpp>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/ResultCode.hpp>

#include <coffee/persistence/PrimaryKey.hpp>

namespace coffee {

namespace basis {
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

class Accessor : public basis::NamedObject {
public:
   typedef std::shared_ptr<dbms::Connection> TheConnection;
   typedef std::shared_ptr<dbms::Statement> TheStatement;
   typedef std::shared_ptr<Class> TheClass;
   typedef std::shared_ptr<Object> TheObject;
   typedef std::shared_ptr<PrimaryKey> ThePrimaryKey;
   
   TheStatement& getStatement() noexcept { return m_statement; }
   const ThePrimaryKey& getPrimaryKey() const throw(basis::RuntimeException) { return m_primaryKey; }
   
   bool hasAutoCommit() const noexcept { return m_autoCommit; }
   bool setAutoCommit(const bool value) { bool result(m_autoCommit); m_autoCommit = value; return result; }
   bool enableAutoCommit() noexcept { return setAutoCommit(true); }
   void disableAutoCommit() noexcept { setAutoCommit(false); }

   Accessor(const Accessor&) = delete;

protected:
   Accessor(const char* name, TheStatement& statement, const ThePrimaryKey& primaryKey) :
      basis::NamedObject(name),
      m_statement(statement),
      m_primaryKey(primaryKey),
      m_autoCommit(false)
   {;}

   Accessor(const char* name, const ThePrimaryKey& primaryKey) :
      basis::NamedObject(name),
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


