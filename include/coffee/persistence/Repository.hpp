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

#ifndef __coffee_persistence_Repository_hpp
#define __coffee_persistence_Repository_hpp

#include <unordered_map>
#include <memory>
#include <functional>

#include <coffee/basis/NamedObject.hpp>
#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace persistence {

class Storage;

class Repository : public basis::NamedObject {
public:
   explicit Repository(const char* name) : NamedObject(name) {;}
   explicit Repository(const std::string& name) : NamedObject(name) {;}

   std::shared_ptr<Storage> createStorage(const std::string& name, const int maxCacheSize) throw(basis::RuntimeException);
   std::shared_ptr<Storage>& findStorage(const std::string& name) throw(basis::RuntimeException);

   basis::StreamString asString() const noexcept;

   std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   Repository(const Repository&) = delete;

private:
   typedef std::unordered_map<std::string, std::shared_ptr<Storage> > Storages;
   typedef Storages::iterator storage_iterator;
   typedef Storages::const_iterator const_storage_iterator;

   Storages m_storages;

   storage_iterator storage_begin() noexcept { return m_storages.begin(); }
   storage_iterator storage_end() noexcept { return m_storages.end(); }
   static std::shared_ptr<Storage>& storage(storage_iterator ii) noexcept { return std::ref(ii->second); }

   const_storage_iterator storage_begin() const noexcept { return m_storages.begin(); }
   const_storage_iterator storage_end() const noexcept { return m_storages.end(); }
   static const std::shared_ptr<Storage>& storage(const_storage_iterator ii) noexcept { return std::ref(ii->second); }
};

} /* namespace persistence */
} /* namespace coffee */
#endif
