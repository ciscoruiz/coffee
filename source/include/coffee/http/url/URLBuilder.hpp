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


#ifndef _coffee_http_url_URLBuilder_hpp_
#define _coffee_http_url_URLBuilder_hpp_

#include <coffee/http/url/defines.hpp>
#include <coffee/basis/RuntimeException.hpp>
#include <boost/shared_ptr.hpp>

namespace coffee {
namespace http {
namespace url {

class URL;

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
class URLBuilder {
public:
   URLBuilder() {;}
   URLBuilder& setCompoment(const ComponentName::_v component, const std::string& value) throw(basis::RuntimeException);
   URLBuilder& addKeyValue(const std::string& key, const std::string& value) noexcept {
      m_keyValues.push_back(KeyValue(key,value));
      return *this;
   }

   std::shared_ptr<URL> build() const throw(basis::RuntimeException);

private:
   std::map<ComponentName::_v, std::string> m_components;
   Query m_keyValues;

   friend class URL;
};

}
}
}

#endif /* _coffee_http_url_URLBuilder_hpp_ */
