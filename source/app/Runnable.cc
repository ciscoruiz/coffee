#include <coffee/app/Runnable.hpp>
#include <coffee/logger/Logger.hpp>

#include <coffee/config/defines.hpp>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/AsString.hpp>
#include <coffee/adt/AsHexString.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

using namespace std;
using namespace coffee;

void app::Runnable::requestStop ()
   throw (adt::RuntimeException)
{
   if (isWaitingStop () == true)
      return;

   do_requestStop ();

   m_statusFlags |= StatusFlags::WaitingStop;
}

//virtual
adt::StreamString app::Runnable::asString () const
   noexcept
{
   adt::StreamString result ("coffee::app::Runnable { Reference: ");

   result << adt::AsHexString::apply (coffee_ptrnumber_cast (this));

   if (!m_name.empty()) {
      result << " | Name: " << m_name;
   }

   result << " | Status:" << flagsAsString ();

   return result += " }";
}

// virtual
std::shared_ptr<xml::Node> app::Runnable::asXML (std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("Runnable");

   result->createAttribute ("Reference", adt::AsHexString::apply (coffee_ptrnumber_cast (this)));

   if (!m_name.empty())
      result->createAttribute ("Name", m_name);

   result->createAttribute ("Status", flagsAsString());

   return result;
}

std::string app::Runnable::flagsAsString () const
   noexcept
{
   string result;

   if (m_statusFlags == StatusFlags::Stopped) {
      result += " Stopped";
   }
   else {
      if (isWaitingStop () == true)
         result += " WaitingStop";

      if (isRunning() == true)
         result += " Running";

      if (isStarting() == true)
         result += " Starting";
   }

   return result;
}
