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

#ifndef __coffee_persistence_PrimaryKeyBuilder_hpp
#define __coffee_persistence_PrimaryKeyBuilder_hpp

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/datatype/Set.hpp>

#include <coffee/persistence/PrimaryKey.hpp>

namespace coffee {
    
namespace persistence {

/**
 * @todo write docs
 */
class PrimaryKeyBuilder : public dbms::datatype::Set {
public:
   /**
    * Default constructor
    */
   PrimaryKeyBuilder() {;}

   PrimaryKeyBuilder& add(std::shared_ptr<dbms::datatype::Abstract> data) throw (adt::RuntimeException) {
      insert(data);
      return *this;
   }
    
   std::shared_ptr<persistence::PrimaryKey> build() const throw (adt::RuntimeException);
    
private:
    friend class PrimaryKey;
};

}
}

#endif // PRIMARYKEYBUILDER_H
