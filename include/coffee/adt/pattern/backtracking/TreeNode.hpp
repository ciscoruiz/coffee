
#ifndef _coffee_adt_pattern_backtracking_Step_hpp_
#define _coffee_adt_pattern_backtracking_Step_hpp_

#include <memory>
#include <vector>

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace adt {

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
       * @param treeNode
       * @param depth
       */
      virtual ~Predicate() {;}

      /**
       * Apply the predicate on the received treeNode.
       * @param treeNode One solution's treeNode
       * @param depth Depth of this treeNode.
       */
      virtual void apply(const TreeNode<_T>& treeNode, const int depth) const noexcept = 0;
   };

   explicit TreeNode(const _T& value) : m_value(value){;}
   TreeNode(const _T& value, const shared_tree_node& predeccesor) :  m_value(value), m_predeccesor(predeccesor) {;}
   virtual ~TreeNode() { m_successors.clear(); }

   void add(const shared_tree_node& treeNode) noexcept { m_successors.push_back(treeNode); }

   const _T& getValue() const noexcept { return m_value; }

   successor_iterator successor_begin() noexcept { return m_successors.begin(); }
   successor_iterator successor_end() noexcept { return m_successors.end(); }
   static shared_tree_node& get_successor(successor_iterator ii) noexcept { return std::ref(*ii); }

   const_successor_iterator successor_begin() const noexcept { return m_successors.begin(); }
   const_successor_iterator successor_end() const noexcept { return m_successors.end(); }
   static const shared_tree_node& get_successor(const_successor_iterator ii) noexcept { return std::ref(*ii); }

   size_t successors_size() const noexcept { return m_successors.size(); }

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

