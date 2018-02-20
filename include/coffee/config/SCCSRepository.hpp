
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
