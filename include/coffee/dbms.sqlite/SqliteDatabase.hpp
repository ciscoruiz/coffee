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

#ifndef _coffee_dbms_sqlite_SqliteDatabase_hpp_
#define _coffee_dbms_sqlite_SqliteDatabase_hpp_

#include <boost/filesystem.hpp>

#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/ErrorCodeInterpreter.hpp>

namespace coffee {

namespace dbms  {

namespace sqlite {

class SqliteDatabase : public Database {
public:
   static std::shared_ptr<SqliteDatabase> instantiate(app::Application& application, const boost::filesystem::path& dbFile)
      throw(adt::RuntimeException);
   ~SqliteDatabase();

private:
   class SqliteErrorCodeInterpreter : public dbms::ErrorCodeInterpreter {
   public:
      SqliteErrorCodeInterpreter() {;}
   private:
      bool notFound(const int errorCode) const throw(adt::RuntimeException);
      bool successful(const int errorCode) const throw(adt::RuntimeException);
      bool locked(const int errorCode) const throw(adt::RuntimeException);
      bool lostConnection(const int errorCode) const throw(adt::RuntimeException);
   };

   explicit SqliteDatabase(app::Application& application, const boost::filesystem::path& dbFile);

   std::shared_ptr<Connection> allocateConnection(const std::string& name, const ConnectionParameters& parameters)
      throw(adt::RuntimeException, std::bad_cast);

   std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const StatementParameters& parameters)
      throw(adt::RuntimeException, std::bad_cast);

   std::shared_ptr<binder::Input> allocateInputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(adt::RuntimeException);
   std::shared_ptr<binder::Output> allocateOutputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(adt::RuntimeException);

   void do_stop() throw(adt::RuntimeException) {;}
};

}
}
}

#endif
