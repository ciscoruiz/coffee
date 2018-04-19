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


#ifndef _coffee_http_url_URL_hpp_
#define _coffee_http_url_URL_hpp_

#include <coffee/http/url/ComponentName.hpp>
#include <coffee/http/url/defines.hpp>
#include <coffee/basis/RuntimeException.hpp>

namespace coffee {
namespace http {
namespace url {

class URLBuilder;
class URLParser;

/**
 * \see https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Syntax
 *
 *
 *                     hierarchical part
         ┌───────────────────┴─────────────────────┐
                    authority               path
        ┌───────────────┴───────────────┐┌───┴────┐
  abc://username:password@example.com:123/path/data?key=value#fragid1
  └┬┘   └───────┬───────┘ └────┬────┘ └┬┘           └───┬───┘ └──┬──┘
scheme  user information     host     port            query   fragment

  urn:example:mammal:monotreme:echidna
  └┬┘ └──────────────┬───────────────┘
scheme              path
 */
class URL {
public:
   bool hasCompoment(const ComponentName::_v component) const noexcept {
      auto ii = m_components.find(component);
      return ii != m_components.end();
   }

   const std::string& getCompoment(const ComponentName::_v component) const throw(basis::RuntimeException);

   keyvalue_iterator begin() const noexcept { return m_keyValues.begin(); }
   keyvalue_iterator end() const noexcept { return m_keyValues.end(); }
   const KeyValue& keyvalue(const keyvalue_iterator ii) noexcept { return *ii; }

private:
   const std::map<ComponentName::_v, std::string> m_components;
   const KeyValues m_keyValues;

   explicit URL(const URLBuilder& builder);
   explicit URL(const URLParser& parser);

   friend class URLBuilder;
   friend class URLParser;
};

}
}
}

#endif /* _coffee_http_url_URL_hpp_ */
