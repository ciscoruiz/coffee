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
#include <wepa/balance/ByRange.hpp>
#include <wepa/balance/Resource.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

using namespace wepa;

balance::ByRange::ByRange() :
   balance::BalanceIf("balance::ByRange", Requires::Key)
{
}

void balance::ByRange::initialize ()
   throw (adt::RuntimeException)
{
   for (auto& range : m_ranges) {
      try {
         range.second->initialize ();
      }
      catch (adt::RuntimeException& ex) {
         LOG_ERROR (range.second->asString () << ex.asString());
      }
   }
}

void balance::ByRange::addRange (const int bottom, const int top, BalanceIf* balanceIf)
   throw (adt::RuntimeException)
{
   if (balanceIf == NULL)
      WEPA_THROW_EXCEPTION(asString () << " can not associate a null BalanceIf");

   if (balanceIf == this)
      WEPA_THROW_EXCEPTION(asString () << " can not generate a recursive association");

   if (balanceIf->size() == 0)
      WEPA_THROW_EXCEPTION(asString () << " can not associate an empty BalanceIf " << balanceIf->asString());

   if (bottom > top)
      WEPA_THROW_EXCEPTION(asString () << " | invalid range (bottom > top)");

   BalanceIf* aux;

   if ((aux = find_range (bottom)) != NULL)
      WEPA_THROW_EXCEPTION(aux->asString () << " has overlapping with " << balanceIf->asString());

   if ((aux = find_range (top)) != NULL)
      WEPA_THROW_EXCEPTION(aux->asString () << " has overlapping with " << balanceIf->asString());

   Range range (top, bottom);
   m_ranges [range] = balanceIf;

   LOG_DEBUG ("Range (" <<  bottom << "," << top << "): " << balanceIf->asString () << " has been created");

   for (BalanceIf::resource_iterator ii = balanceIf->resource_begin(), maxii = balanceIf->resource_end(); ii != maxii; ++ ii)
      BalanceIf::add (BalanceIf::resource (ii));
}

balance::BalanceIf* balance::ByRange::find_range (const int key)
throw ()
{
   for (auto& range : m_ranges) {
      if (range.first.first >= key && range.first.second <= key)
         return range.second;
   }

   return NULL;
}

balance::Resource* balance::ByRange::do_apply (const int key)
   throw (adt::RuntimeException)
{
   logger::TraceMethod tm (logger::Level::Local7, WEPA_FILE_LOCATION);

   balance::Resource* result = NULL;

   BalanceIf* balanceIf = find_range (key);

   if (balanceIf == NULL)
      WEPA_THROW_EXCEPTION("Key=" << key << " does not have a defined range");

   result = balanceIf->apply(key);

   if (result != NULL)
      LOG_LOCAL7("Result=" << result);

   return result;
}

