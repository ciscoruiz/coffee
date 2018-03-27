// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#include <coffee/dbms/datatype/MultiString.hpp>

using namespace coffee;
using namespace coffee::dbms;

datatype::MultiString::MultiString(const char* name, const Constraint::_v constraint) :
   datatype::Abstract(name, Datatype::MultiString, 0, constraint)
{
}

datatype::MultiString::MultiString(const MultiString& other) :
   datatype::Abstract(other),
   values(other.values)
{
}

basis::StreamString datatype::MultiString::asString() const
   noexcept
{
   basis::StreamString result("datatype.MultiString { ");
   result << datatype::Abstract::asString();
   result << " | #Values=" << values.size();
   return result += " }";
}

size_t datatype::MultiString::hash() const
   noexcept
{
   size_t result = values.size();
   for (auto ii = begin_value(), maxii = end_value(); ii != maxii; ++ ii) {
      result ^= std::hash<std::string>{}(value(ii));
   }
   return result;
}

int datatype::MultiString::do_compare(const datatype::Abstract& other) const
   throw(basis::RuntimeException)
{
   const MultiString& _other = coffee_datatype_downcast(MultiString, other);

   const int diffSize = values.size() - _other.values.size();

   if (diffSize != 0)
      return diffSize;

   for (int ii = 0, maxii = values.size(); ii != maxii; ++ ii) {
      const int diff = coffee_strcmp(values[ii].c_str(), _other.values[ii].c_str());
      if (diff != 0)
         return diff;
   }

   return 0;
}
