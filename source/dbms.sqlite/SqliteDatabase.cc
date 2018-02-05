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
#include <memory>

#include <coffee/dbms.sqlite/SqliteDatabase.hpp>
#include <coffee/dbms.sqlite/SqliteConnection.hpp>
#include <coffee/dbms.sqlite/SqliteStatement.hpp>
#include <coffee/dbms.sqlite/SqliteInputBinder.hpp>
#include <coffee/dbms.sqlite/SqliteOutputBinder.hpp>
#include <coffee/dbms.sqlite/SqliteErrorCodeInterpreter.hpp>

using namespace coffee;
using namespace coffee::dbms;

sqlite::SqliteDatabase::SqliteDatabase(const boost::filesystem::path& dbFile) :
   Database("SQLite3", dbFile.c_str())
{
   setErrorCodeInterpreter(std::make_shared<sqlite::SqliteErrorCodeInterpreter>());
}

sqlite::SqliteDatabase::~SqliteDatabase()
{
}

std::shared_ptr<Connection> sqlite::SqliteDatabase::allocateConnection(const std::string& name, const char* user, const char* password)
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteConnection>(*this, name, user, password);
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

