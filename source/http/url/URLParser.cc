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

#include <coffee/http/url/URLParser.hpp>
#include <coffee/http/url/URL.hpp>
#include <coffee/http/url/ComponentName.hpp>

#include <coffee/http/protocol/defines.hpp>

using namespace coffee;

// Analize searching for  scheme:[//[user[:password]@]host[:port]][/path][?query][#fragment]
//
//               hierarchical part
//    ┌───────────────────┴─────────────────────┐
//                authority               path
//      ┌───────────────┴───────────────┐┌───┴────┐
//abc://username:password@example.com:123/path/data?key=value#fragid1
//└┬┘   └───────┬───────┘ └────┬────┘ └┬┘           └───┬───┘ └──┬──┘
//scheme  user information     host     port            query   fragment
//
//   urn:example:mammal:monotreme:echidna
//└┬┘ └──────────────┬───────────────┘
//scheme              path
//
std::shared_ptr<http::url::URL> http::url::URLParser::build()
   throw(basis::RuntimeException)
{
   auto separation = protocol::separate(m_url, ':');
   const std::string& scheme = separation.first;
   addMandatoryComponent(ComponentName::Scheme, scheme);

   const std::string& withoutScheme = separation.second;

   auto endAuthority = readAuthority(withoutScheme);
   auto endPath = readPath(withoutScheme, endAuthority);
   auto endQuery = readQuery(withoutScheme, endPath);
   readFragment(withoutScheme, endQuery);

   std::shared_ptr<http::url::URL> result(new URL(*this));
   return result;
}

std::string::size_type http::url::URLParser::readAuthority(const std::string& withoutScheme)
   throw(basis::RuntimeException)
{
   auto start = withoutScheme.find("//");

   if (start != std::string::npos) {
      start += 2;
   }
   else {
      start = 0;
   }

   auto token = extractToken(withoutScheme, start, "/?#");

   if (token.first.find('@', start) == std::string::npos) {
      auto separation = protocol::separate(token.first, ':');
      addMandatoryComponent(ComponentName::Host, separation.first);
      addOptionalComponent(ComponentName::Port, verifyPortIsNumeric(separation.second));
   }
   else {
      auto separation = protocol::separate(token.first, '@');
      const std::string& userInformation = separation.first;
      const std::string& endPoint = separation.second;

      {
         auto separation = protocol::separate(userInformation, ':');
         addOptionalComponent(ComponentName::User, separation.first);
         addOptionalComponent(ComponentName::Password, separation.second);
      }
      {
         auto separation = protocol::separate(endPoint, ':');
         addMandatoryComponent(ComponentName::Host, separation.first);
         addOptionalComponent(ComponentName::Port, verifyPortIsNumeric(separation.second));
      }
   }

   return token.second;
}

std::string::size_type http::url::URLParser::readPath(const std::string& withoutScheme, const std::string::size_type endPrevious)
   noexcept
{
   if (endPrevious == withoutScheme.size()) {
      return endPrevious;
   }

   auto token = extractToken(withoutScheme, endPrevious, "?#");
   addOptionalComponent(ComponentName::Path, token.first);
   return token.second;
}

std::string::size_type http::url::URLParser::readQuery(const std::string& withoutScheme, const std::string::size_type endPrevious)
   throw(basis::RuntimeException)
{
   if (endPrevious == withoutScheme.size()) {
      return endPrevious;
   }

   if (withoutScheme[endPrevious] != '?') {
      return endPrevious;
   }

   auto token = extractToken(withoutScheme, endPrevious + 1, "#");

   std::vector<std::string> pairs = protocol::split(token.first, '&');

   for (auto& pair : pairs) {
      auto keyValue = protocol::separate(pair, '=');

      if (keyValue.first.empty()) {
         COFFEE_THROW_EXCEPTION(token.first << " contains an empty key");
      }

      m_keyValues.push_back(keyValue);
   }

   return token.second;
}

void http::url::URLParser::readFragment(const std::string& withoutScheme, const std::string::size_type endPrevious)
   noexcept
{
   if (endPrevious == withoutScheme.size()) {
      return;
   }

   addOptionalComponent(ComponentName::Fragment, withoutScheme.substr(endPrevious));
}

void http::url::URLParser::addMandatoryComponent(const http::url::ComponentName::_v componentName, const std::string& value)
   throw(basis::RuntimeException)
{
   if (value.empty()) {
      COFFEE_THROW_EXCEPTION(m_url << " does not contain any " << http::url::ComponentName::asString(componentName));
   }
   m_components[componentName] = value;
}

void http::url::URLParser::addOptionalComponent(const http::url::ComponentName::_v componentName, const std::string& value)
   noexcept
{
   if (!value.empty())
      m_components[componentName] = value;
}

//static
http::url::URLParser::Token http::url::URLParser::extractToken(const std::string& withoutScheme, const std::string::size_type initpos, const char* finalDelims)
   noexcept
{
   std::string::size_type end = 0;

    do {
      end = withoutScheme.find(*finalDelims ++, initpos);
      if (end != std::string::npos)
         break;
   } while (*finalDelims);

   Token result;

   if (end == std::string::npos) {
      result.first = withoutScheme.substr(initpos);
      result.second = withoutScheme.size();
   }
   else {
      result.first.assign(withoutScheme.begin() + initpos, withoutScheme.begin() + end);
      result.second = end;
   }

   return result;
}

//static
const std::string& http::url::URLParser::verifyPortIsNumeric(const std::string& value)
   throw(basis::RuntimeException)
{
   if (value.empty())
      return value;

   auto ii = std::find_if_not(value.begin(), value.end(), [](unsigned char cc) { return isdigit(cc); });
   if (ii != value.end()) {
      COFFEE_THROW_EXCEPTION("Port " << value << " should be a numeric value");
   }

   return value;
}
