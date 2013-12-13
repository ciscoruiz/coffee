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
   AddUpIsANumber () : m_valueUnderStudy (0) {;}

   void setValueUnderStudy (const int valueUnderStudy) { m_valueUnderStudy = valueUnderStudy; }

private:
   int m_valueUnderStudy;

   int getStartingPoint () const throw () { return m_valueUnderStudy; }

   int first (int value) const throw () { return (value > 0) ? value - 1: 0; }
   bool stop (int value) const throw () { return value <= 0; }
   int next (int value) const throw () { return value - 1; }

   bool reject (const Solution* solution, const int candidate) const throw () {
      return (addUp (solution) + candidate) > m_valueUnderStudy;
   }
   bool accept (const Solution* solution, const int candidate) const throw () {
      return (addUp (solution) + candidate) == m_valueUnderStudy;
   }

   static int addUp (const Solution* solution) {
      int result = 0;

      // Avoid root node value
      while (solution->getPredeccessor() != NULL) {
         result += solution->getValue();
         solution = solution->getPredeccessor();
      }

      return result;
   }
};

class PrintAddUp : public AddUpIsANumber::Solution::Predicate {
public:
   void apply (const AddUpIsANumber::Solution& solution, const int margin) const throw () {
      for (int ii = 0; ii < margin; ++ ii)
         std::cout << "   ";
      std::cout << solution.getValue() << std::endl;
   }
};

BOOST_AUTO_TEST_CASE (addup_is_ten)
{
   AddUpIsANumber solver;

   solver.setValueUnderStudy(10);

   const AddUpIsANumber::Solution* root = solver.apply ();

   BOOST_REQUIRE(root != NULL);

   PrintAddUp print;

   root->depthFirst (print);

   BOOST_REQUIRE_EQUAL (root->countSolutions (), 9);

   const AddUpIsANumber::Solution* step;

   step = root->getNextStep(2);

   BOOST_REQUIRE_EQUAL (step->getValue(), 7);

   step = step->getNextStep(1);

   BOOST_REQUIRE_EQUAL (step->getValue(), 2);

   step = step->getNextStep(0);

   BOOST_REQUIRE_EQUAL (step->getValue(), 1);

   BOOST_REQUIRE_THROW (root->getNextStep(10), wepa::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (addup_without_solution)
{
   AddUpIsANumber solver;

   solver.setValueUnderStudy(2);

   const AddUpIsANumber::Solution* root = solver.apply ();

   BOOST_REQUIRE(root == NULL);

   solver.setValueUnderStudy(0);

   root = solver.apply ();

   BOOST_REQUIRE(root == NULL);
}

// See: http://www.cs.uiuc.edu/~jeffe/teaching/algorithms/notes/02-backtracking.pdf
typedef std::pair <int, int> ColumnAndRow;

class NQueen : public Solver <ColumnAndRow> {
public:
   NQueen (const int size) : m_size (size) {;}

private:
   const int m_size;

   ColumnAndRow getStartingPoint () const throw () { return ColumnAndRow (0, 0); }
   ColumnAndRow first (ColumnAndRow pos) const throw () { return next (pos);}

   bool stop (ColumnAndRow pos) const throw () { return pos.second == m_size; }

   ColumnAndRow next (ColumnAndRow pos) const throw ();

   bool reject (const Solution* solution, const ColumnAndRow candidate) const throw ();
   bool accept (const Solution* solution, const ColumnAndRow candidate) const throw ();

   static bool sharedDiagonal (const ColumnAndRow& step, const ColumnAndRow& candidate) throw ();
};

ColumnAndRow NQueen::next (ColumnAndRow pos) const throw () {
   if (++ pos.first == m_size) {
      pos.first = 0;
      pos.second ++;
   }

   return pos;
}

bool NQueen::reject (const NQueen::Solution* solution, const ColumnAndRow candidate) const throw ()
{
   while (solution->getPredeccessor() != NULL) {
      const ColumnAndRow& step = solution->getValue();

      if (step.first == candidate.first)
         return true;

      if (step.second == candidate.second)
         return true;

      if (sharedDiagonal (step, candidate) == true)
         return true;

      solution = solution->getPredeccessor();
   }

   return false;
}

bool NQueen::accept (const Solution* solution, const ColumnAndRow candidate) const throw ()
{
   int numberOfQueens = 0;

   while (solution->getPredeccessor() != NULL) {
      solution = solution->getPredeccessor();
      numberOfQueens ++;
   }

   return (numberOfQueens + 1) == m_size;
}

//static
bool NQueen::sharedDiagonal (const ColumnAndRow& step, const ColumnAndRow& candidate) throw () {
   if ((step.first - step.second) == (candidate.first - candidate.second))
      return true;

   if ((step.second - step.first) == (candidate.second - candidate.first))
      return true;

   return false;
}

class PrintChessboard: public NQueen::Solution::Predicate {
public:
   void apply (const NQueen::Solution& solution, const int margin) const throw () {
      for (int ii = 0; ii < margin; ++ ii)
         std::cout << "   ";

      ColumnAndRow columnAndRow (solution.getValue());

      std::cout << "X=" << columnAndRow.first << " | Y=" << columnAndRow.second << std::endl;
   }
};

BOOST_AUTO_TEST_CASE (NQueen8x8)
{
   NQueen solver (4);

   const NQueen::Solution* root = solver.apply ();

   BOOST_REQUIRE(root != NULL);

   PrintChessboard print;

   root->depthFirst (print);
}

