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

#ifndef _coffee_test_balance_TestResource_hpp
#define _coffee_test_balance_TestResource_hpp

#include <coffee/balance/Resource.hpp>
#include <coffee/basis/AsString.hpp>

#include <coffee/balance/ResourceContainer.hpp>

namespace coffee {

namespace test {

namespace balance {

class TestResource : public coffee::balance::Resource {
public:
   explicit TestResource (const int key) :
      Resource (basis::StreamString ("TestResource-").append (basis::AsString::apply (key, "%02d"))),
      m_key (key),
      m_available (false) {;}

   void setAvailable (const bool available) noexcept { m_available = available; }
   int getKey () const noexcept { return m_key; }

   static std::shared_ptr<TestResource> cast(std::shared_ptr<coffee::balance::Resource>& resource) { return std::dynamic_pointer_cast<TestResource>(resource); }
   static std::shared_ptr<TestResource> cast(const std::shared_ptr<coffee::balance::Resource>& resource) { return std::dynamic_pointer_cast<TestResource>(resource); }
   static std::shared_ptr<TestResource> cast_copy(std::shared_ptr<coffee::balance::Resource> resource) { return std::dynamic_pointer_cast<TestResource>(resource); }

private:
   const int m_key;
   bool m_available;

   bool isAvailable () const noexcept { return m_available; }
   virtual void initialize () throw (basis::RuntimeException) { m_available = true; }
};

class UnusableResource  : public TestResource {
public:
   UnusableResource() :  TestResource(555){;}

private:
   void initialize () throw (basis::RuntimeException) { setAvailable(false); COFFEE_THROW_EXCEPTION("Can not initialize resource"); }
};

}
}
}

#endif
