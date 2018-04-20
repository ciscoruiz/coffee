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
#include <coffee/http/url/URLParser.hpp>
#include <coffee/http/url/URL.hpp>

using namespace coffee;

http::url::URL::URL(const URLBuilder& builder) :
   m_components(builder.m_components),
   m_keyValues(builder.m_keyValues)
{
}

http::url::URL::URL(const URLParser& builder) :
   m_components(builder.m_components),
   m_keyValues(builder.m_keyValues)
{
}

const std::string& http::url::URL::getComponent(const ComponentName::_v component) const
   throw(basis::RuntimeException)
{
   auto ii = m_components.find(component);

   if (ii == m_components.end()) {
      COFFEE_THROW_EXCEPTION("URL does not contain component " << ComponentName::asString(component));
   }

   return ii->second;
}

std::string http::url::URL::encode() const
   throw(basis::RuntimeException)
{
   basis::StreamString ss;

   ss << getComponent(ComponentName::Scheme) << "://";

   bool hasUserInformation = false;

   if (hasComponent(ComponentName::User)) {
      ss << getComponent(ComponentName::User);
      hasUserInformation = true;
   }

   if (hasComponent(ComponentName::Password)) {
      ss << ":" << getComponent(ComponentName::Password);
      hasUserInformation = true;
   }

   if (hasUserInformation) {
      ss << "@";
   }

   ss << getComponent(ComponentName::Host);

   if (hasComponent(ComponentName::Port)) {
      ss << ":" << getComponent(ComponentName::Port);
   }

   if (hasComponent(ComponentName::Path)) {
      ss << getComponent(ComponentName::Path);
   }

   if (!m_keyValues.empty()) {
      bool isFirst = true;
      for (auto& pair : m_keyValues) {
         if (isFirst) {
            ss << "?";
            isFirst = false;
         }
         else {
            ss << "&";
         }
         ss << pair.first;

         if (!pair.second.empty()) {
            ss << "=" << pair.second;
         }
      }
   }

   if (hasComponent(ComponentName::Fragment)) {
      ss << getComponent(ComponentName::Fragment);
   }

   return ss;
}

