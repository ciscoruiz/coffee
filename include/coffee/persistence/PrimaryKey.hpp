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

#ifndef __coffee_persistence_PrimaryKey_hpp
#define __coffee_persistence_PrimaryKey_hpp

#include <vector>
#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/StreamString.hpp>

#include <coffee/dbms/datatype/Set.hpp>

namespace coffee {

namespace dbms {
   namespace datatype {
      class Abstract;
   }
}

namespace persistence {

class PrimaryKeyBuilder;

class PrimaryKey : public dbms::datatype::Set {
public:
   struct EqualSharedPointer {
      bool operator()(const std::shared_ptr<PrimaryKey>& lhs, const std::shared_ptr<PrimaryKey>& rhs) const {
         return lhs->compare(rhs) == 0;
      }
   };
   struct HashSharedPointer {
      size_t operator()(const std::shared_ptr<PrimaryKey>& primaryKey) const { return primaryKey->hash(); }
   };

   explicit PrimaryKey(const PrimaryKeyBuilder& builder);
   PrimaryKey (const PrimaryKey& other) : dbms::datatype::Set(other) {;}

   PrimaryKey& operator= (const PrimaryKey& other) throw (adt::Exception) { Set::operator=(other); return *this; }
   bool operator== (const PrimaryKey& other) const throw (adt::RuntimeException) { return compare(other) == 0; }
   bool operator< (const PrimaryKey& other) const throw (adt::RuntimeException){ return compare(other) < 0; }
   bool matches(const PrimaryKey& other) const noexcept;

   size_t hash() const noexcept;

   operator adt::StreamString () const noexcept { return asString (); }
   adt::StreamString asString () const noexcept;
};

} /* namespace persistence */
} /* namespace coffee */

#endif
