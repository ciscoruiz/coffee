// COFFEE - COmpany eFFEEctive Platform
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
   result << " | " << m_measureTiming.asString();
   result << " | Expression: " << m_expression;
   return result += " }";
}

std::shared_ptr<xml::Node> dbms::Statement::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("dbms.Statement");

   adt::Microsecond avgT = m_measureTiming;
   result->createAttribute("Name", m_name);

   std::shared_ptr<xml::Node> node = result->createChild("Timing");
   node->createAttribute("N", m_measureTiming.size());
   node->createAttribute("Accumulator", m_measureTiming.getAccumulator());
   node->createAttribute("Timing", m_measureTiming.value().asString());

   result->createChild("Expression")->createText(m_expression);

   return result;
}
