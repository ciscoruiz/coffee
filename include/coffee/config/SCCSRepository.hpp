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


#ifndef _coffee_config_SCCSREPOSITORY_HPP_
#define _coffee_config_SCCSREPOSITORY_HPP_

#include <set>

#include <coffee/adt/pattern/Singleton.hpp>

namespace coffee {
namespace config {

class SCCSRepository : public adt::pattern::Singleton <SCCSRepository>{
public:
   typedef const char* ModuleName;
   typedef std::set <ModuleName> Entries;
   typedef Entries::const_iterator const_entry_iterator;

   ~SCCSRepository() {
      m_entries.clear ();
   }

   void registerModule (ModuleName moduleName) noexcept {
      m_entries.insert (moduleName);
   }

   const_entry_iterator entry_begin () const noexcept { return m_entries.begin (); }
   const_entry_iterator entry_end () const noexcept { return m_entries.end (); }

   static ModuleName module_name (const_entry_iterator ii) { return *ii; }

private:
   Entries m_entries;

   SCCSRepository() {;}

   friend class adt::pattern::Singleton <SCCSRepository>;
};

} /* namespace config */
} /* namespace coffee */
#endif /* SCCSREPOSITORY_HPP_ */
