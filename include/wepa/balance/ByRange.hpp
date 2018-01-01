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
#ifndef __wepa_balance_ByRange_hpp
#define __wepa_balance_ByRange_hpp

#include <map>

#include "Balance.hpp"

namespace wepa {
namespace balance {

class ByRange : public Balance {
   typedef std::pair <int, int> Range;
   typedef std::map <Range, Balance*> Ranges;
   typedef Ranges::iterator range_iterator;

public:
   /**
    * Constructor
    */
   ByRange ();

   /**
    * Destructor
    */
   ~ByRange () { m_ranges.clear (); }

   void initialize () throw (adt::RuntimeException);

   /**
    * \warning Once you call this method you can not append more resources to this \em balanceIf
    * @param bottom Minimal value for this range
    * @param top Maximal value for this range
    * @param balanceIf Load balancing algorithm used under this range.
    */
   void addRange (const int bottom, const int top, Balance* balanceIf) throw (adt::RuntimeException);

private:
   Ranges m_ranges;

   Balance* find_range (const int key) noexcept;

   Resource* do_apply (const int key) throw (adt::RuntimeException);
};

} /* namespace balance */
} /* namespace wepa */
#endif
