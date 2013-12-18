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

#ifndef _wepa_config_SCCSREPOSITORY_HPP_
#define _wepa_config_SCCSREPOSITORY_HPP_

#include <set>

#include <wepa/adt/pattern/Singleton.hpp>

namespace wepa {
namespace config {

class SCCSRepository : public adt::pattern::Singleton <SCCSRepository>{
public:
   typedef const char* ModuleName;
   typedef std::set <ModuleName> Entries;
   typedef Entries::const_iterator const_entry_iterator;

   ~SCCSRepository() {
      m_entries.clear ();
   }

   void registerModule (ModuleName moduleName) throw () {
      m_entries.insert (moduleName);
   }

   const_entry_iterator entry_begin () const throw () { return m_entries.begin (); }
   const_entry_iterator entry_end () const throw () { return m_entries.end (); }

   static ModuleName module_name (const_entry_iterator ii) { return *ii; }

private:
   Entries m_entries;

   SCCSRepository() {;}

   friend class adt::pattern::Singleton <SCCSRepository>;
};

} /* namespace config */
} /* namespace wepa */
#endif /* SCCSREPOSITORY_HPP_ */
