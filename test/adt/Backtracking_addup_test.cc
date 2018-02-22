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

   // You can get 10 by summing up (7 + 3) or (7 + 2 + 1)
   BOOST_REQUIRE_EQUAL(step->getValue(), 7);
   BOOST_REQUIRE_EQUAL(step->countSolutions(), 2);
   BOOST_REQUIRE_EQUAL(step->getNextStep(0)->getValue(), 3);
   BOOST_REQUIRE_EQUAL(step->getNextStep(1)->getValue(), 2);

   BOOST_REQUIRE_EQUAL(step->getNextStep(1)->countSolutions(), 1);
   BOOST_REQUIRE_EQUAL(step->getNextStep(1)->getNextStep(0)->getValue(), 1);

   BOOST_REQUIRE_THROW(solver.getNextStep(10), coffee::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(addup_out_of_range)
{
   AddUpIsANumber solver;

   solver.setValueUnderStudy(10);

   BOOST_REQUIRE_EQUAL(solver.apply(), true);

   PrintAddUp print;

   solver.depthFirst(print);

   BOOST_REQUIRE_EQUAL(solver.successors_size(), 6);
   BOOST_REQUIRE_EQUAL(solver.countSolutions(), 9);

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
