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
