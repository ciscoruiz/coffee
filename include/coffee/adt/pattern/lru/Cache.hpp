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

//
#ifndef __coffee_adt_pattern_lru_Cache_hpp
#define __coffee_adt_pattern_lru_Cache_hpp

#include <unordered_map>
#include <list>
#include <utility>
#include <functional>

#include <coffee/adt/StreamString.hpp>

namespace coffee {
namespace adt {
namespace pattern {
namespace lru {

template <class _Key, class _Value, class _Hash = std::hash<_Key>, class _Pred=std::equal_to<_Key> > class Cache {
public:
   typedef std::list<std::pair<_Key,_Value> > Pairs;
   typedef typename Pairs::iterator pair_iterator;
   typedef std::unordered_map<_Key, pair_iterator, _Hash, _Pred> QuickAccess;
   typedef typename QuickAccess::iterator qa_iterator;

   explicit Cache(const int maxSize) : m_maxSize(maxSize){;}
   ~Cache() {
      m_values.clear();
      m_quickAccess.clear();
   }

   pair_iterator begin() noexcept { return m_values.begin(); }
   pair_iterator end() noexcept { return m_values.end(); }
   static _Key& key(pair_iterator ii) noexcept { return kkvv(ii).first; }
   static _Value& value(pair_iterator ii) noexcept { return kkvv(ii).second; }
   
   size_t size() const noexcept { return m_quickAccess.size(); }

   pair_iterator find(const _Key& key) noexcept {
      auto ii = m_quickAccess.find(key);
      
      if (ii == m_quickAccess.end()) {
         return end();
      }
      
      return updateAccess(ii);
   }
   
   void set(const _Key& key, const _Value& value) {
      auto ii = m_quickAccess.find(key);
      
      if (ii == m_quickAccess.end()) {
         if (m_quickAccess.size() == m_maxSize) {
            const std::pair<_Key,_Value>& oldest = m_values.front();
            m_quickAccess.erase(oldest.first);
            m_values.pop_front();
         }
         m_values.push_back(std::make_pair(key,value));
         m_quickAccess[key] = last_value();
      }
      else {
         _Value& refValue = Cache<_Key,_Value>::value(ii->second);
         refValue = value;
         updateAccess(ii);         
      }   
   }

   bool erase(const _Key& key) {
      auto ii = m_quickAccess.find(key);
      
      if (ii != m_quickAccess.end()) {
         m_values.erase(ii->second);
         m_quickAccess.erase(ii);
         return true;
      }      
      
      return false;
   }
   
   operator StreamString () const noexcept { return asString (); }

   StreamString asString () const noexcept {
      StreamString result ("coffee.adt.pattern.lru.Cache { MaxSize=");
      return result << m_maxSize << " | Size=" << m_quickAccess.size () << " }";
   }

private:   
   const int m_maxSize;
   Pairs m_values;
   QuickAccess m_quickAccess;
   
   static std::pair<_Key,_Value>& kkvv(pair_iterator ii) noexcept { return std::ref<std::pair<_Key,_Value> >(*ii); }
   static std::pair<_Key,_Value>& qa_pairKV(qa_iterator ii) noexcept { return kkvv(ii->second); }

   pair_iterator updateAccess(qa_iterator ii) {
      if (ii->second == last_value())
         return ii->second;
      
      std::pair<_Key,_Value> backup = qa_pairKV(ii);
      m_values.erase(ii->second);
      m_values.push_back(backup);
      return ii->second = last_value();
   }
   
   pair_iterator last_value() noexcept {
      auto result = end();
      return -- result;
   }
};

}
}   
}
}

#endif
