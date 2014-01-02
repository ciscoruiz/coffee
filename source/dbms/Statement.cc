// WEPA - Write Excellent Professional Applications
#include <wepa/logger/TraceMethod.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/dbms/Database.hpp>
#include <wepa/dbms/Statement.hpp>

#include <wepa/dbms/binder/Input.hpp>
#include <wepa/dbms/binder/Output.hpp>

using namespace std;
using namespace wepa;
using namespace wepa::dbms;

// virtual
Statement::~Statement ()
{
   for (input_iterator ii = input_begin (), maxii = input_end (); ii != maxii; ii ++)
      delete inputBind (ii);

   for (output_iterator ii = output_begin (), maxii = output_end (); ii != maxii; ii ++)
      delete outputBind (ii);

   m_inputBinds.clear ();
   m_outputBinds.clear ();
}

adt::StreamString  Statement::asString () const
   throw ()
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
   throw ()
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

void Statement::bindInput (datatype::Abstract& data)
   throw ()
{
   m_inputBinds.push_back (m_database.allocateInputBind (data));
}

const binder::Output* Statement::bindOutput (datatype::Abstract& data)
   throw ()
{
   binder::Output* result = m_database.allocateOutputBind (data);
   m_outputBinds.push_back (result);
   return result;
}

datatype::Abstract& Statement::input (input_iterator ii)
   throw ()
{
   return (*ii)->getData ();
}

datatype::Abstract& Statement::output (output_iterator ii)
   throw ()
{
   return (*ii)->getData ();
}

