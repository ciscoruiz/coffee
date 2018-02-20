#ifndef _coffee_adt_pattern_observer_Event_hpp
#define _coffee_adt_pattern_observer_Event_hpp

namespace coffee {

namespace adt {

namespace pattern {

namespace observer {

class Subject;

class Event {
public:
   typedef int Id;
   typedef unsigned int BitMask;

   static const Id NullId = -1;

   Event () : m_id (NullId), m_bitMask (0) {;}
   Event (const Id id, const BitMask bitMask) : m_id (id), m_bitMask (bitMask) {;}
   Event (const Event& other) : m_id (other.m_id), m_bitMask (other.m_bitMask) {;}

   Id getId () const noexcept { return m_id; }
   BitMask getBitMask () const noexcept { return m_bitMask; }

   bool operator== (const Event& other) const noexcept { return m_id == other.m_id; }

   Event& operator= (const Event& other) {
      m_id = other.m_id;
      m_bitMask = other.m_bitMask;
      return *this;
   }

private:
   Id m_id;
   BitMask m_bitMask;

   friend class Subject;
};

inline Event operator+ (const Event& left, const Event& right)
   noexcept
{
   return Event (Event::NullId, left.getBitMask () | right.getBitMask ());
}

inline bool operator < (const Event& right, const Event& left)
   noexcept
{
   return right.getId () < left.getId ();
}

}
}
}
}

#endif /* EVENT_HPP_ */
