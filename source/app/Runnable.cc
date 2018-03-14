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

bool app::Runnable::stop ()
   throw (adt::RuntimeException)
{
   if (isStopped())
      return false;

   try {
      do_stop ();
      m_statusFlags = StatusFlags::Stopped;
   }
   catch (adt::RuntimeException& ex) {
      m_statusFlags = StatusFlags::StoppedWithError;
      throw;
   }

   return true;
}

//virtual
adt::StreamString app::Runnable::asString () const
   noexcept
{
   adt::StreamString result ("coffee::app::Runnable { Reference: ");

   result << adt::AsHexString::apply (coffee_ptrnumber_cast (this));

   const std::string& name = getName();

   if (!name.empty()) {
      result << " | Name: " << name;
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

   const std::string& name = getName();

   if (!name.empty())
      result->createAttribute ("Name", name);

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
      if (isRunning() == true)
         result += " Running";

      if (isStarting() == true)
         result += " Starting";

      if (isStoppedWithError() == true)
         result += " Stopped with error";
   }

   return result;
}
