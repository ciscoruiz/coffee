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

#include <gtest/gtest.h>

#include <coffee/basis/pattern/backtracking/Solver.hpp>

using namespace coffee::basis::pattern::backtracking;

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
   std::stringstream& ss;

public:
   explicit PrintAddUp(std::stringstream& _ss) : ss(_ss) {;}

   void apply(const AddUpIsANumber::Solution& solution, const int margin) const noexcept {
      if (margin > 0) {
         ss << "{" << margin << "," << solution.getValue() << "}";
      }
   }
};

TEST(BacktrakingAddupTest,addup_is_ten)
{
   AddUpIsANumber solver;

   solver.setValueUnderStudy(10);

   ASSERT_TRUE(solver.apply());

   std::stringstream ss;
   PrintAddUp print(ss);

   solver.depthFirst(print);

   ASSERT_EQ(6, solver.successors_size());
   ASSERT_EQ(9, solver.countSolutions());
   ASSERT_EQ("{1,9}{2,1}{1,8}{2,2}{1,7}{2,3}{2,2}{3,1}{1,6}{2,4}{2,3}{3,1}{1,5}{2,4}{3,1}{2,3}{3,2}{1,4}{2,3}{3,2}{4,1}", ss.str());

   AddUpIsANumber::shared_solution step;

   step = solver.getNextStep(2);

   // You can get 10 by summing up (7 + 3) or (7 + 2 + 1)
   ASSERT_EQ(7, step->getValue());
   ASSERT_EQ(2, step->countSolutions());
   ASSERT_EQ(3, step->getNextStep(0)->getValue());
   ASSERT_EQ(2, step->getNextStep(1)->getValue());

   ASSERT_EQ(1, step->getNextStep(1)->countSolutions());
   ASSERT_EQ(1, step->getNextStep(1)->getNextStep(0)->getValue());

   ASSERT_THROW(solver.getNextStep(10), coffee::basis::RuntimeException);
}

TEST(BacktrakingAddupTest,addup_out_of_range)
{
   AddUpIsANumber solver;

   solver.setValueUnderStudy(10);

   ASSERT_TRUE(solver.apply());
   ASSERT_EQ(6, solver.successors_size());
   ASSERT_EQ(9, solver.countSolutions());

   ASSERT_THROW(solver.getNextStep(10), coffee::basis::RuntimeException);
}

TEST(BacktrakingAddupTest,addup_without_solution)
{
   AddUpIsANumber solver;

   solver.setValueUnderStudy(2);

   ASSERT_FALSE(solver.apply());
   ASSERT_FALSE(solver.hasSuccessor());

   solver.setValueUnderStudy(0);

   ASSERT_FALSE(solver.apply());
}
