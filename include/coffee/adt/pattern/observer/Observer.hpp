#ifndef _coffee_adt_pattern_observer_Observer_h
#define _coffee_adt_pattern_observer_Observer_h

#include <string>
#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/NamedObject.hpp>

namespace coffee {

namespace adt {

namespace pattern {

namespace observer {

class Event;
class Subject;

class Observer : public NamedObject {
public:
   bool isSubscribed() const noexcept { return m_isSubscribed; }

protected:
   explicit Observer(const std::string& name) : NamedObject(name), m_isSubscribed(false) {;}
   virtual ~Observer() {;}

   virtual void update(const Subject& subject, const Event&) noexcept = 0 ;

private:
   bool m_isSubscribed;

   void isSubscribed(const bool value) noexcept { m_isSubscribed = value; }

   friend class Subject;
};

}
}
}
}


#endif

