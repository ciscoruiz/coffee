// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
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

   if (m_name != NULL) {
      result << " | Name: " << *m_name;
   }

   result << " | Status:" << flagsAsString ();

   return result += " }";
}

// virtual
xml::Node& app::Runnable::asXML (xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild("Runnable");

   result.createAttribute ("Reference", adt::AsHexString::apply (coffee_ptrnumber_cast (this)));

   if (m_name != NULL)
      result.createAttribute ("Name", *m_name);

   result.createAttribute ("Status", flagsAsString());

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
