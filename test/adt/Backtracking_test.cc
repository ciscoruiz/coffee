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
#include <boost/test/unit_test.hpp>

#include <wepa/adt/pattern/backtracking/Solver.hpp>

using namespace wepa::adt::pattern::backtracking;

class AddUpIsANumber : public Solver <int> {
public:
   AddUpIsANumber () : m_startingPoint (0) {;}

   void setStartingPoint (const int startingPoint) { m_startingPoint = startingPoint; }

   int getStartingPoint () const throw () { return m_startingPoint; }

   int first (const int value) const throw () { return (value > 0) ? value - 1: 0; }
   bool stop (const int value) const throw () { return value <= 0; }
   int next (const int value) const throw () { return value - 1; }

   bool reject (const Solution* solution, const int candidate) const throw ();
   bool accept (const Solution* solution, const int candidate) const throw ();

private:
   int m_startingPoint;

   static int addUp (const Solution* solution) {
      int result = 0;

      while (solution->getPredeccessor() != NULL) {
         result += solution->getValue();
         solution = solution->getPredeccessor();
      }

      return result;
   }
};

bool AddUpIsANumber::reject (const Solution* solution, const int candidate) const
   throw ()
{
   return (addUp (solution) + candidate) > m_startingPoint;
}

bool AddUpIsANumber::accept (const Solution* solution, const int candidate) const
   throw ()
{
   return (addUp (solution) + candidate) == m_startingPoint;
}

class Print {
public:
   void operator () (const AddUpIsANumber::Solution& solution, const int margin) {
      for (int ii = 0; ii < margin; ++ ii)
         std::cout << " ";
      std::cout << solution.getValue() << std::endl;
   }
};

BOOST_AUTO_TEST_CASE (sumup_is_ten)
{
   AddUpIsANumber solver;

   solver.setStartingPoint(10);

   const AddUpIsANumber::Solution* solution = solver.apply ();

   BOOST_REQUIRE(solution != NULL);

   Print print;

   AddUpIsANumber::Solution::depthFirst (solution, print);

}

