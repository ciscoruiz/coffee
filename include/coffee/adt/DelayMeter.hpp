#ifndef __coffee_adt_DelayMeter_hpp
#define __coffee_adt_DelayMeter_hpp

namespace coffee {

namespace adt {

template <class _TimeUnit> class DelayMeter {
public:
   DelayMeter () { m_timeStamp = _TimeUnit::getTime (); }
   DelayMeter (const DelayMeter& other) { m_timeStamp = other.m_timeStamp; }

   _TimeUnit getValue () const noexcept {
      return _TimeUnit::getTime () - m_timeStamp;
   }

   operator _TimeUnit () const noexcept { return getValue (); }

   void reset () noexcept {
      m_timeStamp = _TimeUnit::getTime ();
   }

   DelayMeter <_TimeUnit>& operator= (const DelayMeter& other) noexcept { m_timeStamp = other.m_timeStamp; return *this; }
   bool operator < (const _TimeUnit& other) const noexcept { return getValue () < other; }
   bool operator > (const _TimeUnit& other) const noexcept { return getValue () > other; }
   bool operator == (const _TimeUnit& other) const noexcept { return getValue () == other; }

private:
   _TimeUnit m_timeStamp;
};

}
}

#endif
