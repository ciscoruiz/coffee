// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
#ifndef _wepa_test_balance_TestResource_hpp
#define _wepa_test_balance_TestResource_hpp

#include <wepa/balance/Resource.hpp>
#include <wepa/adt/AsString.hpp>

#include <wepa/balance/ResourceList.hpp>

namespace wepa {

namespace test {

namespace balance {

class TestResource : public wepa::balance::Resource {
public:
   TestResource (const int key) :
      Resource (adt::StreamString ("TestResource-").append (adt::AsString::apply (key, "%02d"))),
      m_key (key),
      m_available (false) {;}

   void setAvailable (const bool available) noexcept { m_available = available; }
   int getKey () const noexcept { return m_key; }

   static std::shared_ptr<TestResource> cast(std::shared_ptr<wepa::balance::Resource>& resource) { return std::dynamic_pointer_cast<TestResource>(resource); }
   static std::shared_ptr<TestResource> cast(const std::shared_ptr<wepa::balance::Resource>& resource) { return std::dynamic_pointer_cast<TestResource>(resource); }
   static std::shared_ptr<TestResource> cast_copy(std::shared_ptr<wepa::balance::Resource> resource) { return std::dynamic_pointer_cast<TestResource>(resource); }

private:
   const int m_key;
   bool m_available;

   bool isAvailable () const noexcept { return m_available; }
   void initialize () throw (adt::RuntimeException) { m_available = true; }
};

inline std::shared_ptr<wepa::balance::ResourceList> setup(const int maxResource, const int firstId = 0)
{
   std::shared_ptr<wepa::balance::ResourceList> result = std::make_shared<wepa::balance::ResourceList>("TestResources");

   logger::Logger::initialize(new logger::TtyWriter);

   int id = firstId;
   for (int ii = 0; ii < maxResource; ++ ii) {
      result->add(std::make_shared<TestResource>(id ++));
   }

   result->initialize();

   return result;
}

}
}
}

#endif
