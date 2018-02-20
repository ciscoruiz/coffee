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

#ifndef __coffee_balance_GuardResourceList_hpp
#define __coffee_balance_GuardResourceList_hpp

#include <memory>
#include <mutex>

namespace coffee {

namespace balance {

class ResourceList;

class GuardResourceList {
   typedef std::lock_guard<std::mutex> lock_guard;

public:
   explicit GuardResourceList(std::shared_ptr<ResourceList>& resourceList);
   explicit GuardResourceList(std::mutex& mutex) : m_lock(new lock_guard(mutex)) {}
   ~GuardResourceList() { m_lock.reset(); }

private:
   std::unique_ptr<lock_guard> m_lock;

   // Need it for Fake-Guard
   explicit GuardResourceList(const ResourceList& resourceList) {}

   friend class ResourceList;
};

}
}

#endif
