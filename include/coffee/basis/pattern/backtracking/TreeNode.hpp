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


#ifndef _coffee_basis_pattern_backtracking_Step_hpp_
#define _coffee_basis_pattern_backtracking_Step_hpp_

#include <memory>
#include <vector>

#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace basis {

namespace pattern {

namespace backtracking {

template <typename _T> class TreeNode {
   typedef typename std::shared_ptr<TreeNode<_T> > shared_tree_node;
   typedef std::vector<shared_tree_node> Successors;

public:
   typedef typename Successors::iterator successor_iterator;
   typedef typename Successors::const_iterator const_successor_iterator;

   /**
    * Class to operate on every treeNode of the found solution
    */
   class Predicate {
   public:
      /**
       * Destructor
       */
      virtual ~Predicate() {;}

      /**
       * Apply the predicate on the received treeNode.
       * @param treeNode One solution's treeNode
       * @param depth Depth of this treeNode.
       */
      virtual void apply(const TreeNode<_T>& treeNode, const int depth) const noexcept = 0;
   };

   /**
    * Constructor
    * \param value Solution value associated to this node.
    */
   explicit TreeNode(const _T& value) : m_value(value){;}

   /**
    * Constructor
    * \param value Solution value associated to this node.
    * \param predeccesor Previous steps associated to this node.
    */
   TreeNode(const _T& value, const shared_tree_node& predeccesor) :  m_value(value), m_predeccesor(predeccesor) {;}

   /**
    * Destructor.
    */
   virtual ~TreeNode() { m_successors.clear(); }

   /**
    * Append the received node as parameter as a possible next step in the solution.
    * \param treeNode Next step in the solution
    */
   void add(const shared_tree_node& treeNode) noexcept { m_successors.push_back(treeNode); }

   /**
    * \return the value associated to the node.
    */
   const _T& getValue() const noexcept { return m_value; }

   /**
    * \return iterator the first solution/successor.
    */
   successor_iterator successor_begin() noexcept { return m_successors.begin(); }

   /**
    * \return iterator to the last solution/successor.
    */
   successor_iterator successor_end() noexcept { return m_successors.end(); }

   /**
    * \return the successor address by the iterator.
    * \warning the value ii must be contained in [#successor_begin, #successor_end)
    */
   static shared_tree_node& get_successor(successor_iterator ii) noexcept { return std::ref(*ii); }

   /**
    * \return iterator the first solution/successor.
    */
   const_successor_iterator successor_begin() const noexcept { return m_successors.begin(); }

   /**
    * \return iterator to the last solution/successor.
    */
   const_successor_iterator successor_end() const noexcept { return m_successors.end(); }

   /**
    * \return the successor address by the iterator.
    * \warning the value ii must be contained in [#successor_begin, #successor_end)
    */
   static const shared_tree_node& get_successor(const_successor_iterator ii) noexcept { return std::ref(*ii); }

   /**
    * \return the number of successors.
    */
   size_t successors_size() const noexcept { return m_successors.size(); }

   /**
    * \return \b true if this node has some successor and \b false in other case.
    */
   bool hasSuccessor() const noexcept { return m_successors.empty() == false; }

   const shared_tree_node getPredeccessor() const noexcept { return m_predeccesor; }
   
   /**
    * Runs over tree with depth-first algorithm and apply the \em _Predicate on every node.
    * \param predicate Operation to apply on every Node.
    * \param depth Depth of this node
    */
   virtual void depthFirst(const Predicate& predicate, const int depth = 0) const noexcept {
      predicate.apply(*this, depth);

      for(const_successor_iterator ii = successor_begin(), maxii = successor_end(); ii != maxii; ++ ii) {
         get_successor(ii)->depthFirst(predicate, depth + 1);
      }
   }

   /**
    * @return The number of found solutions(which matches with the number of leaf nodes on the tree)
    */
   size_t countSolutions() const noexcept {
      int result = 0;

      if(hasSuccessor() == true) {
         for(const_successor_iterator ii = successor_begin(), maxii = successor_end(); ii != maxii; ++ ii) {
            result += get_successor(ii)->countSolutions();
         }
      }
      else
         result = 1;

      return result;
   }

   const shared_tree_node getNextStep(const int index) const
      throw(RuntimeException)
   {
      if(m_successors.size() <= index) {
         COFFEE_THROW_EXCEPTION("Index=" << index << " is out of range(" << m_successors.size() << ")");
      }

      return get_successor(m_successors.begin() + index);
   }

protected:
   TreeNode() : m_predeccesor(NULL) {;}

   void clear() noexcept { m_successors.clear(); }

private:
   _T m_value;
   shared_tree_node  m_predeccesor;
   Successors m_successors;
};


}
}
}
}

#endif

