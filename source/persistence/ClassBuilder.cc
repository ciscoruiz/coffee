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

#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/PrimaryKey.hpp>

using namespace coffee;

persistence::ClassBuilder& persistence::ClassBuilder::set(std::shared_ptr<PrimaryKey>& primaryKey)
throw (basis::RuntimeException)
{
   if (m_primaryKey) {
      COFFEE_THROW_EXCEPTION(m_className << " primary key already added");
   }

   m_primaryKey = primaryKey;
   return *this;
}

std::shared_ptr<persistence::Class> persistence::ClassBuilder::build() const
      throw (basis::RuntimeException)
{
   if (!m_primaryKey) {
      COFFEE_THROW_EXCEPTION(m_className << " does not define a valid primary key");
   }

   if (empty()) {
      COFFEE_THROW_EXCEPTION(m_className << " does not define any member");
   }

   for (const_data_iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
      if (m_primaryKey->constains(name(ii)) == true) {
         COFFEE_THROW_EXCEPTION(m_className << " member " << name(ii) << " already used as a member in the primary key");
      }
   }

   return std::make_shared<persistence::Class>(*this);
}
