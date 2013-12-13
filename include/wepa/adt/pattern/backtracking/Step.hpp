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

#ifndef _wepa_adt_pattern_backtracking_Step_hpp_
#define _wepa_adt_pattern_backtracking_Step_hpp_

#include <boost/ptr_container/ptr_vector.hpp>

namespace wepa {

namespace adt {

namespace pattern {

namespace backtracking {

template <typename _T> class Step {
   typedef boost::ptr_vector <Step <_T> > Successors;

public:
   typedef typename Successors::iterator successor_iterator;
   typedef typename Successors::const_iterator const_successor_iterator;

   Step (const _T& value) : m_predeccesor (NULL), m_value (value) {;}
   Step (const _T& value, const Step* predeccesor) : m_predeccesor (predeccesor), m_value (value) {;}
   ~Step () { m_successors.clear (); }

   void add (Step <_T>* step) throw () { m_successors.push_back (step); }

   const _T& getValue () const throw () { return m_value; }

   successor_iterator successor_begin () throw () { return m_successors.begin (); }
   successor_iterator successor_end () throw () { return m_successors.end (); }
   static Step<_T>& get_successor (successor_iterator ii) throw (){ return *ii; }

   const_successor_iterator successor_begin () const throw () { return m_successors.begin (); }
   const_successor_iterator successor_end () const throw () { return m_successors.end (); }
   static const Step<_T>& get_successor (const_successor_iterator ii) throw (){ return *ii; }

   const Step<_T>* getPredeccessor () const throw () { return m_predeccesor; }
   
   template <typename _Predicate> static void depthFirst (const Step <_T>* solution, _Predicate& predicate, const int margin = 0) {
      predicate (*solution, margin);

      for (const_successor_iterator ii = solution->successor_begin (), maxii = solution->successor_end (); ii != maxii; ++ ii) {
         depthFirst (&get_successor (ii), predicate, margin + 1);
      }
   }

private:
   const Step<_T>* m_predeccesor;
   _T m_value;
   Successors m_successors;
};


}
}
}
}

#endif

