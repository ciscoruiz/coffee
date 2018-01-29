// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//

#include <stdlib.h>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <coffee/adt/pattern/backtracking/Solver.hpp>

using namespace coffee::adt::pattern::backtracking;

class AddUpIsANumber : public Solver<int> {
public:
   AddUpIsANumber() : m_valueUnderStudy(0) {;}

   void setValueUnderStudy(const int valueUnderStudy) { m_valueUnderStudy = valueUnderStudy; }

private:
   int m_valueUnderStudy;

   int getStartingPoint() const noexcept { return m_valueUnderStudy; }

   int first(const int depth, int value) const noexcept { return(value > 0) ? value - 1: 0; }
   bool stop(const int depth, int value) const noexcept { return value <= 0; }
   int next(const int depth, int value) const noexcept { return value - 1; }

   bool reject(const shared_solution& solution, const int candidate) const noexcept {
      return(addUp(solution) + candidate) > m_valueUnderStudy;
   }
   bool accept(const shared_solution& solution, const int candidate) const noexcept {
      return(addUp(solution) + candidate) == m_valueUnderStudy;
   }

   static int addUp(const shared_solution& solution) {
      int result = 0;

      for (shared_solution ii = solution; ii; ii = ii->getPredeccessor()) {
         result += ii->getValue();
      }

      return result;
   }
};

class PrintAddUp : public AddUpIsANumber::Solution::Predicate {
public:
   void apply(const AddUpIsANumber::Solution& solution, const int margin) const noexcept {
      for(int ii = 0; ii < margin; ++ ii)
         std::cout << "   ";
      std::cout << solution.getValue() << std::endl;
   }
};

BOOST_AUTO_TEST_CASE(addup_is_ten)
{
   AddUpIsANumber solver;

   solver.setValueUnderStudy(10);

   BOOST_REQUIRE_EQUAL(solver.apply(), true);

   PrintAddUp print;

   solver.depthFirst(print);

   BOOST_REQUIRE_EQUAL(solver.successors_size(), 6);
   BOOST_REQUIRE_EQUAL(solver.countSolutions(), 9);

   AddUpIsANumber::shared_solution step;

   step = solver.getNextStep(2);

   BOOST_REQUIRE_EQUAL(step->getValue(), 7);

   step = step->getNextStep(1);

   BOOST_REQUIRE_EQUAL(step->getValue(), 2);

   step = step->getNextStep(0);

   BOOST_REQUIRE_EQUAL(step->getValue(), 1);

   BOOST_REQUIRE_THROW(solver.getNextStep(10), coffee::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(addup_without_solution)
{
   AddUpIsANumber solver;

   solver.setValueUnderStudy(2);

   BOOST_REQUIRE_EQUAL(solver.apply(), false);
   BOOST_REQUIRE_EQUAL(solver.hasSuccessor(), false);

   solver.setValueUnderStudy(0);

   BOOST_REQUIRE_EQUAL(solver.apply(), false);
}

// See: http://www.cs.uiuc.edu/~jeffe/teaching/algorithms/notes/02-backtracking.pdf
typedef std::pair <int, int> ColumnAndRow;

class NQueen : public Solver<ColumnAndRow> {
public:
   NQueen(const int size) : m_size(size) {;}

private:
   const int m_size;

   ColumnAndRow getStartingPoint() const noexcept { return ColumnAndRow(0, 0); }

   ColumnAndRow first(const int depth, ColumnAndRow pos) const noexcept;
   bool stop(const int depth, ColumnAndRow pos) const noexcept;
   ColumnAndRow next(const int depth, ColumnAndRow pos) const noexcept;

   bool reject(const shared_solution& solution, const ColumnAndRow candidate) const noexcept;
   bool accept(const shared_solution& solution, const ColumnAndRow candidate) const noexcept;

   static bool diagonalCollision(const ColumnAndRow& step, const ColumnAndRow& candidate) noexcept;
};

ColumnAndRow NQueen::first(const int depth, ColumnAndRow pos) const noexcept
{
   if(depth == 0)
      return ColumnAndRow(0, 0);

   return next(depth, pos);
}

ColumnAndRow NQueen::next(const int depth, ColumnAndRow pos) const noexcept {
   ++ pos.first;

   if(depth != 0 && pos.first == m_size) {
      pos.first = 0;
      pos.second ++;
   }

   return pos;
}

bool NQueen::stop(const int depth, ColumnAndRow pos) const noexcept
{
   return(depth == 0) ?(pos.first == m_size):(pos.second == m_size);
}

bool NQueen::reject(const NQueen::shared_solution& solution, const ColumnAndRow candidate) const noexcept
{
   for (shared_solution ii = solution; ii; ii = ii->getPredeccessor()) {
      const ColumnAndRow& step = ii->getValue();

      if(step.first == candidate.first)
         return true;

      if(step.second == candidate.second)
         return true;

      if(diagonalCollision(step, candidate) == true)
         return true;
   }

   return false;
}

bool NQueen::accept(const NQueen::shared_solution& solution, const ColumnAndRow candidate) const noexcept
{
   int numberOfQueens = 0;

   for (shared_solution ii = solution; ii; ii = ii->getPredeccessor()) {
      numberOfQueens ++;
   }

   return(numberOfQueens + 1) == m_size;
}

//static
bool NQueen::diagonalCollision(const ColumnAndRow& step, const ColumnAndRow& candidate) noexcept {
   if(step.first == candidate.second && step.second == candidate.first)
      return true;

   if((step.first - step.second) ==(candidate.first - candidate.second))
      return true;

   if((step.second - step.first) ==(candidate.second - candidate.first))
      return true;

   // m = y2 - y1 / x2 - x1, If m is 1 then points are in the same diagonal.
   return abs(step.second - candidate.second) * 100 / abs(step.first - candidate.first) == 100;
}

class PrintChessboard: public NQueen::Solution::Predicate {
public:
   void apply(const NQueen::Solution& solution, const int depth) const noexcept {
      if(depth == 0)
         return;

      for(int ii = 0; ii < depth; ++ ii)
         std::cout << "   ";

      ColumnAndRow columnAndRow(solution.getValue());

      std::cout << "X=" << columnAndRow.first << " | Y=" << columnAndRow.second << std::endl;
   }
};

BOOST_AUTO_TEST_CASE(NQueen4x4)
{
   NQueen solver(4);

   BOOST_REQUIRE_EQUAL(solver.apply(), true);

   PrintChessboard print;

   solver.depthFirst(print);

   BOOST_REQUIRE_EQUAL(solver.successors_size(), 2);
   BOOST_REQUIRE_EQUAL(solver.countSolutions(), 2);
}

