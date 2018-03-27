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

#ifndef _coffee_dbms_datatype_MultiString_h
#define _coffee_dbms_datatype_MultiString_h

#include <vector>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
 * Defines a variables with multiple values.
 *
 */
class MultiString : public datatype::Abstract {
public:
   typedef std::vector<std::string> Values;
   typedef Values::const_iterator const_value_iterator;

   explicit MultiString(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull);
   explicit MultiString(const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull) : MultiString(name.c_str(), constraint) {;}
   MultiString(const MultiString& other);

   const_value_iterator begin_value() const noexcept { return values.begin(); }
   const_value_iterator end_value() const noexcept { return values.end(); }
   static const std::string& value(const_value_iterator ii) noexcept { return std::ref(*ii); }
   size_t size() const noexcept { return values.size(); }

   void addValue(const std::string& value) noexcept { values.push_back(value); this->isNotNull(); }

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<MultiString>(*this); }

   bool operator==(const MultiString& other) const noexcept { return values == other.values; }

   operator basis::StreamString() const noexcept { return asString(); }

   basis::StreamString asString() const noexcept;

   size_t hash() const noexcept;

   coffee_declare_datatype_downcast(MultiString);

private:
   std::vector<std::string> values;

   void do_clear() noexcept { values.clear(); }
   int do_compare(const Abstract& other) const throw(basis::RuntimeException);
};

}
}
}

#endif

