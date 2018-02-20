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


#ifndef _coffee_adt_pattern_backtracking_Solver_hpp_
#define _coffee_adt_pattern_backtracking_Solver_hpp_

#include <coffee/adt/pattern/backtracking/TreeNode.hpp>

namespace coffee {

namespace adt {

namespace pattern {

namespace backtracking {

/**
 * This is the solver using Backtracking techniques.
 *
 * \param _T Data type for scope for the problem's solutions
 *
 * \see http://en.wikipedia.org/wiki/Backtracking
 * \see http://www.cs.uiuc.edu/~jeffe/teaching/algorithms/notes/02-backtracking.pdf
 */
template <typename _T> class Solver : public TreeNode<_T> {
public:
   typedef TreeNode <_T> Solution;
   typedef typename std::shared_ptr<Solution> shared_solution;

   /**
    * Constructor.
    */
   Solver() {;}

   /**
    * Destructor.
    */
   virtual ~Solver() { ; }

   /**
    *
    * @param problem
    * @return Solutions for the received problem. It will erased when the Solver goes out of scope.
    */
   bool apply() noexcept {
      this->clear();


      for(_T ii = first(0, getStartingPoint()); stop(0, ii) == false; ii = next(0, ii)) {
         shared_solution solution = std::make_shared<Solution>(ii);

         if(exploreSolutions(1, solution) == true)
            this->add(solution);
      }

      return this->hasSuccessor();
   }

   shared_solution getSolution(const int index) const throw(RuntimeException) { return this->getNextStep(index); }

protected:
   virtual _T getStartingPoint() const noexcept = 0;

   virtual _T first(const int depth, _T value) const noexcept = 0;
   virtual bool stop(const int depth, _T value) const noexcept = 0;
   virtual _T next(const int depth, _T value) const noexcept = 0;

   virtual bool reject(const shared_solution& solution, const _T candidate) const noexcept = 0;
   virtual bool accept(const shared_solution& solution, const _T candidate) const noexcept = 0;

private:
   bool exploreSolutions(const int depth, shared_solution& onTestSolution) throw () {
      bool hasSolution = false;

      for(_T ii = first(depth, onTestSolution->getValue()); stop(depth, ii) == false; ii = next(depth, ii)) {
         if(backtracking(depth, onTestSolution, ii) == true)
            hasSolution = true;
      }

      return hasSolution;
   }

   bool backtracking(const int depth, shared_solution& onTestSolution, const _T candidate) noexcept {
      if(reject(onTestSolution, candidate) == true) return false;

      // onTestSolution + candidate become to a real Solution
      if(accept(onTestSolution, candidate) == true) {
         onTestSolution->add(std::make_shared<Solution>(candidate, onTestSolution));
         return true;
      }

      shared_solution partialSolution = std::make_shared<Solution>(candidate, onTestSolution);

      if(exploreSolutions(depth + 1, partialSolution) == false) {
         return false;
      }

      onTestSolution->add(partialSolution);

      return true;
   }

};

}
}
}
}

#endif


