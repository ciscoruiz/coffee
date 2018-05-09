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

#include <coffee/http/url/URLBuilder.hpp>
#include <coffee/http/url/URL.hpp>

using namespace coffee;

http::url::URLBuilder& http::url::URLBuilder::setCompoment(const http::url::ComponentName::_v component, const std::string& value)
   throw(basis::RuntimeException)
{
   if (m_components.find(component) != m_components.end()) {
      COFFEE_THROW_EXCEPTION(component << " already defined");
   }
   m_components[component] = value;
   return *this;
}

std::shared_ptr<http::url::URL> http::url::URLBuilder::build() const
   throw(basis::RuntimeException)
{
   auto end = m_components.end();

   if (m_components.find(ComponentName::Scheme) == end) {
      COFFEE_THROW_EXCEPTION("URLBuilder should contain the Scheme");
   }

   if (m_components.find(ComponentName::Host) == end) {
      COFFEE_THROW_EXCEPTION("URLBuilder should contain the Host");
   }

   std::shared_ptr<http::url::URL> result(new URL(*this));
   return result;
}
