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

#include <coffee/logger/TraceMethod.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/dbms/Statement.hpp>
#include <coffee/dbms/binder/Input.hpp>
#include <coffee/dbms/binder/Output.hpp>
#include <coffee/dbms/datatype/Abstract.hpp>
#include <coffee/dbms/Database.hpp>

using namespace std;
using namespace coffee;
using namespace coffee::dbms;

// virtual
Statement::~Statement()
{
   m_inputBinds.clear();
   m_outputBinds.clear();
}

void Statement::registerElapsedTime(const std::chrono::microseconds& elapsedTime)
   noexcept
{
   m_elapsedTime += elapsedTime.count();
}

void Statement::createBinderInput(std::shared_ptr<datatype::Abstract> data)
   throw(adt::RuntimeException)
{
   std::shared_ptr<binder::Input> result = m_database.allocateInputBind(data);

   if(!result)
      COFFEE_THROW_EXCEPTION(data->asString() << " | Data returned a null binder");

   m_inputBinds.push_back(result);
}

std::shared_ptr<datatype::Abstract>& Statement::getInputData(const GuardStatement&, const int pos)
   throw(adt::RuntimeException)
{
   if(pos >= input_size()) {  
      COFFEE_THROW_EXCEPTION(pos << " is out of range [0," << input_size() << ")");
   }

   return m_inputBinds[pos]->getData();   
}

const std::shared_ptr<datatype::Abstract>& Statement::getInputData(const int pos) const
   throw(adt::RuntimeException)
{
   if(pos >= input_size()) {
      COFFEE_THROW_EXCEPTION(pos << " is out of range [0," << input_size() << ")");
   }

   return m_inputBinds[pos]->getData();
}

std::shared_ptr<datatype::Abstract>& Statement::getOutputData(const GuardStatement&, const int pos)
   throw(adt::RuntimeException)
{
   return getOutputData(pos);
}

std::shared_ptr<datatype::Abstract>& Statement::getOutputData(const int pos)
   throw(adt::RuntimeException)
{
   if(pos >= output_size()) {
      COFFEE_THROW_EXCEPTION(pos << " is out of range [0," << output_size() << ")");
   }

   return m_outputBinds[pos]->getData();
}

void Statement::createBinderOutput(std::shared_ptr<datatype::Abstract> data)
   throw(adt::RuntimeException)
{
   std::shared_ptr<binder::Output> result = m_database.allocateOutputBind(data);

   if(!result)
      COFFEE_THROW_EXCEPTION(data->asString() << " | Data returned a null binder");

   m_outputBinds.push_back(result);
}

void Statement::prepare(Connection& connection)
   throw(adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   do_prepare(connection);

   int pos = 0;
   for(auto& ii : m_inputBinds) {
      ii->do_prepare(*this, pos ++);
   }

   pos = 0;
   for(auto& oo : m_outputBinds) {
      oo->do_prepare(*this, pos ++);
   }
}

ResultCode Statement::execute(Connection& connection)
   throw(adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   int pos = 0;
   for(auto& ii : m_inputBinds) {
      LOG_DEBUG(ii->asString());
      ii->do_encode(*this, pos ++);
   }

   ResultCode result = do_execute(connection);

   LOG_DEBUG(getName() << " | " << result.asString());

   return result;
}

bool Statement::fetch()
   throw(adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   bool result;

   if((result = do_fetch()) == true) {
      int pos = 0;
      for(auto& oo : m_outputBinds) {
         oo->do_decode(*this, pos ++);
         LOG_DEBUG(oo->asString());
      }
   }

   return result;
}

adt::StreamString Statement::asString() const
   noexcept
{
   adt::StreamString  result("dbms::Statement { Name: ");
   result += m_name;
   result << " | N-input: " << input_size();
   result << " | N-output: " << output_size();
   result << " | " << m_elapsedTime.asString();
   result << " | Expression: " << m_expression;
   return result += " }";
}

std::shared_ptr<xml::Node> dbms::Statement::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("dbms.Statement");

   result->createAttribute("Name", m_name);

   std::shared_ptr<xml::Node> node = result->createChild("Timing");
   node->createAttribute("N", m_elapsedTime.size());
   node->createAttribute("Accumulator", (int64_t) m_elapsedTime.getAccumulator());
   node->createAttribute("ElapsedTime", adt::AsString::apply((uint64_t) m_elapsedTime.value()));

   result->createChild("Expression")->createText(m_expression);

   return result;
}
