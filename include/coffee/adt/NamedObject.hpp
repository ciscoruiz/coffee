#ifndef _coffee_adt_NamedObject_h
#define _coffee_adt_NamedObject_h

#include <coffee/adt/StreamString.hpp>

namespace coffee {

namespace adt {

class NamedObject {
public:
  virtual ~NamedObject () { ;}

  const std::string& getName () const noexcept { return m_name; }

  bool isEqual (const std::string& name) const noexcept { return m_name == name; }

  bool isEqual (const NamedObject& other) const noexcept { return isEqual (other.m_name); }

  bool operator == (const std::string& name) const noexcept { return isEqual (name); }

  bool operator == (const NamedObject& other) const noexcept { return isEqual (other.m_name); }

  operator StreamString () const noexcept { return asString (); }

  virtual StreamString asString () const noexcept { StreamString result ("adt.NamedObject { Name: "); return result << m_name << " }"; }

protected:
  explicit NamedObject (const std::string& name) : m_name (name) {;}

private:
   const std::string m_name;
};

}
}


#endif

