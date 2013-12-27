// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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
#include <wepa/balance/BalanceIf.hpp>
#include <wepa/balance/Resource.hpp>

using namespace wepa;

#include <wepa/adt/StreamString.hpp>
#include <wepa/adt/AsString.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/balance/SCCS.hpp>

using namespace wepa;

auto_enum_assign (balance::BalanceIf::Requires) = { "Key", "PositiveKey", NULL };

//static
const int balance::BalanceIf::NullKey = INT_MIN;

void balance::BalanceIf::initialize ()
   throw (adt::RuntimeException)
{
   LOG_THIS_METHOD();

   SCCS::activate();

   do_initialize ();

   for (resource_iterator ii = resource_begin(), maxii = resource_end(); ii != maxii; ++ ii) {
      try {
         do_initializer (resource (ii));
      }
      catch (adt::RuntimeException& ex) {
         LOG_ERROR (resource (ii)->getName () << " | " << ex.asString());
      }
   }

   if (m_resources.empty ())
      LOG_WARN (asString () << " does not have any resource");

   if (countAvailableResources() == 0)
      LOG_WARN (asString () << " does not have any available resource");
}

//virtual
void balance::BalanceIf::do_initializer (Resource* resource)
   throw (adt::RuntimeException)
{
   logger::TraceMethod tm (logger::Level::Local7, WEPA_FILE_LOCATION);
   resource->initialize();
   LOG_DEBUG (resource->asString ());
}

balance::Resource* balance::BalanceIf::apply (const int key)
   throw (adt::RuntimeException)
{
   logger::TraceMethod tm (logger::Level::Local7, WEPA_FILE_LOCATION);

   if (requiresKey () == true) {
      if (key == NullKey)
         WEPA_THROW_EXCEPTION(asString () << " requires a key-value");

      if (key < 0 && requiresPositiveKey() == true)
         WEPA_THROW_EXCEPTION(asString () << " requires a positive key-value");
   }

   Resource* result = NULL;

   if (true) { // Minimize critical section
      std::lock_guard <std::recursive_mutex> guard (m_mutex);

      result = do_apply (key);

      if (result == NULL)
         WEPA_THROW_EXCEPTION(asString () << " does not have available any resources");

      if (result->isAvailable() == false)
         WEPA_THROW_EXCEPTION(asString () << " can not choose an unavailable resource " << result->asString ());
   }

   if (requiresKey () == true) {
      LOG_DEBUG ("Key=" << key << " | " << asString () << " | Result=" << result->asString ());
   }
   else {
      LOG_DEBUG (asString () << " | Result=" << result->asString ());
   }

   return result;
}

bool balance::BalanceIf::add (Resource* resource)
   throw (adt::RuntimeException)
{
    logger::TraceMethod tm (logger::Level::Local7, WEPA_FILE_LOCATION);

   if (resource == NULL)
      WEPA_THROW_EXCEPTION(asString () << " can not add a NULL resource");

   bool result;

   if (true) { // Minimize critical section
      std::lock_guard <std::recursive_mutex> guard (m_mutex);

      if (do_contains (resource) == true)
         result = false;
      else {
         m_resources.push_back (resource);
         result = true;
      }
   }

   LOG_INFO (asString () << " | Add: " << resource->asString () << " | Result=" << adt::AsString::apply (result));

   return result;
}

bool balance::BalanceIf::contains (const balance::Resource* resource) const
   throw ()
{
   if (resource == NULL)
      return false;

   logger::TraceMethod tm (logger::Level::Local7, WEPA_FILE_LOCATION);

   std::lock_guard <std::recursive_mutex> guard (m_mutex);

   return do_contains (resource);
}

// virtual
bool balance::BalanceIf::do_contains (const balance::Resource* resource) const
   throw ()
{
   const_resource_iterator end = resource_end ();

   return (std::find (resource_begin (), end, resource) != end);
}


size_t balance::BalanceIf::countAvailableResources () const
   throw ()
{
   size_t result = 0;

   for (const_resource_iterator ii = resource_begin(), maxii = resource_end(); ii != maxii; ++ ii) {
      if (resource (ii)->isAvailable())
         result ++;
   }

   return result;
}

balance::BalanceIf::resource_iterator balance::BalanceIf::next (balance::BalanceIf::resource_iterator ii)
   throw ()
{
   ii ++;

   if (ii == this->resource_end ())
      ii = this->resource_begin ();

   return ii;
}

//virtual
adt::StreamString balance::BalanceIf::asString () const
   throw ()
{
   adt::StreamString result ("balance::BalanceIf {");

   result += adt::NamedObject::asString();

   const char* requires = Requires::enumName(m_requires);

   if (requires != NULL)
      result.append (" | Requires=").append (requires);

   result += " } | Available = ";
   result += adt::AsString::apply(countAvailableResources());
   result.append (" of ").append (adt::AsString::apply (size ()));
   return result.append ("}");
}

//virtual
xml::Node& balance::BalanceIf::asXML (xml::Node& parent) const
   throw ()
{
   xml::Node& result = parent.createChild (this->getName());

   const char* requires = Requires::enumName(m_requires);

   if (requires != NULL)
      result.createAttribute("Requires", requires);

   for (const_resource_iterator ii = resource_begin(), maxii = resource_end(); ii != maxii; ++ ii) {
      resource (ii)->asXML(result);
   }

   return std::ref (result);
}
