
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
#ifndef __wepa_adt_pattern_lru_Cache_hpp
#define __wepa_adt_pattern_lru_Cache_hpp

#include <unordered_map>
#include <list>
#include <utility>
#include <functional>

#include <wepa/adt/StreamString.hpp>

namespace wepa {
namespace adt {
namespace pattern {
namespace lru {

template <class _Key, class _Value> class Cache {
public:
   typedef std::list<std::pair<_Key,_Value> > Pairs;
   typedef typename Pairs::iterator pair_iterator;
   typedef std::unordered_map<_Key, pair_iterator> QuickAccess;
   typedef typename QuickAccess::iterator qa_iterator;

   Cache(const int maxSize) : m_maxSize(maxSize){;}
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
         refValue.operator=(value);
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
      StreamString result ("wepa.adt.pattern.lru.Cache { MaxSize=");
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
      
      const std::pair<_Key,_Value>& backup = qa_pairKV(ii);
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
