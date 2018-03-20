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

#include <memory>

#include <coffee/app/Application.hpp>

#include <coffee/dbms.sqlite/SqliteDatabase.hpp>
#include <coffee/dbms.sqlite/SqliteConnection.hpp>
#include <coffee/dbms.sqlite/SqliteStatement.hpp>
#include <coffee/dbms.sqlite/SqliteInputBinder.hpp>
#include <coffee/dbms.sqlite/SqliteOutputBinder.hpp>
#include <coffee/dbms.sqlite/SqliteErrorCodeInterpreter.hpp>

#include <coffee/dbms.sqlite/SCCS.hpp>

using namespace coffee;
using namespace coffee::dbms;

//static
std::shared_ptr<sqlite::SqliteDatabase> sqlite::SqliteDatabase::instantiate(app::Application& application, const boost::filesystem::path& dbFile)
   throw(adt::RuntimeException)
{
   std::shared_ptr<SqliteDatabase> result(new SqliteDatabase(application, dbFile));
   application.attach(result);
   return result;
}

sqlite::SqliteDatabase::SqliteDatabase(app::Application& application, const boost::filesystem::path& dbFile) :
   Database(application, "SQLite3", dbFile.c_str())
{
   sqlite::SCCS::activate();

   setErrorCodeInterpreter(std::make_shared<sqlite::SqliteErrorCodeInterpreter>());
}

sqlite::SqliteDatabase::~SqliteDatabase()
{
}

std::shared_ptr<Connection> sqlite::SqliteDatabase::allocateConnection(const std::string& name, const ConnectionParameters& parameters)
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteConnection>(*this, name, parameters);
}

std::shared_ptr<Statement> sqlite::SqliteDatabase::allocateStatement(const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteStatement>(*this, name, expression, actionOnError);
}

std::shared_ptr<binder::Input> sqlite::SqliteDatabase::allocateInputBind(std::shared_ptr<datatype::Abstract> data) const
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteInputBinder>(data);
}

std::shared_ptr<binder::Output> sqlite::SqliteDatabase::allocateOutputBind(std::shared_ptr<datatype::Abstract> data) const
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteOutputBinder>(data);
}

