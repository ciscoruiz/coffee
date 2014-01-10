// WEPA - Write Excellent Professional Applications
#include <wepa/logger/TraceMethod.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/dbms/Database.hpp>
#include <wepa/dbms/Statement.hpp>

#include <wepa/dbms/binder/Input.hpp>
#include <wepa/dbms/binder/Output.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

using namespace std;
using namespace wepa;
using namespace wepa::dbms;

// virtual
Statement::~Statement ()
{
   m_inputBinds.clear ();
   m_outputBinds.clear ();
}


binder::Input* Statement::createBinderInput (datatype::Abstract& data)
   throw (adt::RuntimeException)
{
   binder::Input* result = m_database.allocateInputBind (data);

   if (result == NULL)
      WEPA_THROW_EXCEPTION(data << " | Data returned a null binder");

   m_inputBinds.push_back (result);
   return result;
}

datatype::Abstract& Statement::getInputData (const int pos)
   throw (adt::RuntimeException)
{
   if (pos >= input_size ()) {
      WEPA_THROW_EXCEPTION(pos << " is out of range [0," << input_size() << "]");
   }

   return std::ref (m_inputBinds [pos].getData ());
}

const datatype::Abstract& Statement::getOutputData (const int pos) const
   throw (adt::RuntimeException)
{
   if (pos >= output_size ()) {
      WEPA_THROW_EXCEPTION(pos << " is out of range [0," << output_size() << "]");
   }

   return std::ref (m_outputBinds [pos].getData ());
}

binder::Output* Statement::createBinderOutput (datatype::Abstract& data)
   throw (adt::RuntimeException)
{
   binder::Output* result = m_database.allocateOutputBind (data);

   if (result == NULL)
      WEPA_THROW_EXCEPTION(data << " | Data returned a null binder");

   m_outputBinds.push_back (result);
   return result;
}

void Statement::prepare (Connection* connection)
   throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   do_prepare (connection);

   int pos = 0;
   for (binder::Input& ii : m_inputBinds) {
      ii.do_prepare(this, connection, pos ++);
   }

   pos = 0;
   for (binder::Output& oo : m_outputBinds) {
      oo.do_prepare(this, connection, pos ++);
   }
}

ResultCode Statement::execute (Connection* connection)
   throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   for (binder::Input& ii : m_inputBinds) {
      LOG_DEBUG (ii.asString());
      ii.do_encode ();
   }

   ResultCode result = do_execute (connection);

   LOG_DEBUG (result.asString ());

   return result;
}

bool Statement::fetch()
   throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   bool result;

   if ((result = do_fetch ()) == true) {
      for (binder::Output& oo : m_outputBinds) {
         oo.do_decode();
         LOG_DEBUG (oo.asString());
      }
   }

   return result;
}

adt::StreamString  Statement::asString () const
   noexcept
{
   adt::StreamString  result ("dbms::Statement { Name: ");
   result += m_name;
   result << " | N-input: " << input_size ();
   result << " | N-output: " << output_size ();
   result << " | " << m_measureTiming.asString ();
   result << " | Expression: " << m_expression;
   return result += " }";
}

xml::Node& dbms::Statement::asXML (xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild ("dbms.Statement");

   adt::Microsecond avgT = m_measureTiming;
   result.createAttribute ("Name", m_name);

   xml::Node& node = result.createChild ("Timing");
   node.createAttribute ("N", m_measureTiming.size ());
   node.createAttribute ("Accumulator", m_measureTiming.getAccumulator ());
   node.createAttribute ("Timing", m_measureTiming.value ().asString());

   result.createChild ("Expression").createText (m_expression);

   return result;
}
