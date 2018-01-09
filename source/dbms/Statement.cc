// WEPA - Write Excellent Professional Applications
#include <wepa/logger/TraceMethod.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/dbms/Statement.hpp>

#include <wepa/dbms/binder/Input.hpp>
#include <wepa/dbms/binder/Output.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>
#include <wepa/dbms/Database.hpp>

using namespace std;
using namespace wepa;
using namespace wepa::dbms;

// virtual
Statement::~Statement()
{
   m_inputBinds.clear();
   m_outputBinds.clear();
}

void Statement::createBinderInput(std::shared_ptr<datatype::Abstract>& data)
   throw(adt::RuntimeException)
{
   std::shared_ptr<binder::Input> result = m_database.allocateInputBind(data);

   if(!result)
      WEPA_THROW_EXCEPTION(data->asString() << " | Data returned a null binder");

   m_inputBinds.push_back(result);
}

std::shared_ptr<datatype::Abstract>& Statement::getInputData(const GuardStatement&, const int pos)
   throw(adt::RuntimeException)
{
   if(pos >= input_size()) {  
      WEPA_THROW_EXCEPTION(pos << " is out of range [0," << input_size() << ")");
   }

   return m_inputBinds[pos]->getData();   
}

const std::shared_ptr<datatype::Abstract>& Statement::getOutputData(const GuardStatement&, const int pos) const
   throw(adt::RuntimeException)
{
   if(pos >= output_size()) {
      WEPA_THROW_EXCEPTION(pos << " is out of range [0," << output_size() << ")");
   }

   return m_outputBinds[pos]->getData();
}

std::shared_ptr<datatype::Abstract>& Statement::getOutputData(const GuardStatement&, const int pos)
   throw(adt::RuntimeException)
{
   if(pos >= output_size()) {
      WEPA_THROW_EXCEPTION(pos << " is out of range [0," << output_size() << ")");
   }

   return m_outputBinds[pos]->getData();
}

void Statement::createBinderOutput(std::shared_ptr<datatype::Abstract>& data)
   throw(adt::RuntimeException)
{
   std::shared_ptr<binder::Output> result = m_database.allocateOutputBind(data);

   if(!result)
      WEPA_THROW_EXCEPTION(data->asString() << " | Data returned a null binder");

   m_outputBinds.push_back(result);
}

void Statement::prepare()
   throw(adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   do_prepare();

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

   for(auto& ii : m_inputBinds) {
      LOG_DEBUG(ii->asString());
      ii->do_encode();
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
      for(auto& oo : m_outputBinds) {
         oo->do_decode();
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

xml::Node& dbms::Statement::asXML(xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild("dbms.Statement");

   adt::Microsecond avgT = m_measureTiming;
   result.createAttribute("Name", m_name);

   xml::Node& node = result.createChild("Timing");
   node.createAttribute("N", m_measureTiming.size());
   node.createAttribute("Accumulator", m_measureTiming.getAccumulator());
   node.createAttribute("Timing", m_measureTiming.value().asString());

   result.createChild("Expression").createText(m_expression);

   return result;
}
