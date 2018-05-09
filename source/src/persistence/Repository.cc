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

#include <coffee/persistence/Repository.hpp>
#include <coffee/persistence/Storage.hpp>

#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

using namespace coffee;

std::shared_ptr<persistence::Storage> persistence::Repository::createStorage(const std::string& ident, const int maxCacheSize)
   throw(basis::RuntimeException)
{
   storage_iterator ii = m_storages.find(ident);

   if(ii != m_storages.end()) {
      COFFEE_THROW_EXCEPTION("Ident=" << ident << " has already been used");
   }

   std::shared_ptr<Storage> result = std::make_shared<Storage>(ident, maxCacheSize);
   LOG_DEBUG(result->asString());
   m_storages[ident] = result;

   return result;
}

std::shared_ptr<persistence::Storage>& persistence::Repository::findStorage(const std::string& ident)
   throw(basis::RuntimeException)
{

   storage_iterator ii = m_storages.find(ident);

   if(ii == m_storages.end()) {
      COFFEE_THROW_EXCEPTION("ID=" << ident << " has not been defined yet");
   }

   return std::ref(Repository::storage(ii));
}

basis::StreamString persistence::Repository::asString() const
   noexcept
{
   basis::StreamString result("persistence.Repository { ");
   result << basis::NamedObject::asString();
   result << " | N-Size=" << m_storages.size();
   return result += " }";
}

std::shared_ptr<xml::Node> persistence::Repository::asXML(std::shared_ptr<xml::Node>& parent) const
   throw(basis::RuntimeException)
{
   std::shared_ptr<xml::Node> result = parent->createChild("persistence.Repository");

   result->createAttribute("Name", getName());

   for(const_storage_iterator ii = storage_begin(), maxii = storage_end(); ii != maxii; ++ ii)  {
      storage(ii)->asXML(result);
   }

   return result;
}
