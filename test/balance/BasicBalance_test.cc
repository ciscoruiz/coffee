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

#include <memory>

#include <boost/ptr_container/ptr_vector.hpp>

#include <boost/test/unit_test.hpp>

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/ResourceList.hpp>
#include <coffee/balance/GuardResourceList.hpp>
#include <coffee/balance/StrategyRoundRobin.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Compiler.hpp>

#include "TestResource.hpp"
#include "ResourceListFixture.hpp"

using namespace coffee;
using namespace coffee::balance;

using namespace coffee::test::balance;

const int ResourceListFixture::MaxResources = 10;

BOOST_AUTO_TEST_CASE( avoid_empties )
{
   ResourceList resourceList("otherList");
   std::shared_ptr<Resource> emptyResource;
   BOOST_REQUIRE_THROW (resourceList.add (emptyResource), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE( count_availables, ResourceListFixture)
{
   GuardResourceList guard(resourceList);

   BOOST_REQUIRE_EQUAL(resourceList->size(guard), MaxResources);
   BOOST_REQUIRE_EQUAL(resourceList->countAvailableResources(guard), MaxResources);

   bool available = false;
   for (auto ii = resourceList->resource_begin(guard), maxii = resourceList->resource_end(guard); ii != maxii; ++ ii) {
      std::shared_ptr<TestResource> myResource = TestResource::cast(ResourceList::resource(ii));
      myResource->setAvailable(available);
      available = !available;
   }

   BOOST_REQUIRE_EQUAL(resourceList->countAvailableResources(guard), MaxResources / 2);
}

BOOST_AUTO_TEST_CASE( error_while_initialize )
{
   std::shared_ptr<coffee::balance::ResourceList> resourceList = std::make_shared<coffee::balance::ResourceList>("TestResources");

   resourceList->add(std::make_shared<TestResource>(0));
   resourceList->add(std::make_shared<UnusableResource>());
   resourceList->add(std::make_shared<TestResource>(1));

   resourceList->initialize();

   GuardResourceList guard(resourceList);

   BOOST_REQUIRE_EQUAL(resourceList->countAvailableResources(guard), 2);
}

BOOST_AUTO_TEST_CASE( initialize_empty_list )
{
   std::shared_ptr<coffee::balance::ResourceList> resourceList = std::make_shared<coffee::balance::ResourceList>("TestResources");
   BOOST_REQUIRE_NO_THROW(resourceList->initialize());
}

BOOST_AUTO_TEST_CASE( initialize_without_available_resources )
{
   std::shared_ptr<coffee::balance::ResourceList> resourceList = std::make_shared<coffee::balance::ResourceList>("TestResources");
   resourceList->add(std::make_shared<UnusableResource>());
   resourceList->add(std::make_shared<UnusableResource>());
   BOOST_REQUIRE_NO_THROW(resourceList->initialize());

   GuardResourceList guard(resourceList);
   BOOST_REQUIRE_EQUAL(resourceList->countAvailableResources(guard), 0);
}

BOOST_FIXTURE_TEST_CASE (as_string, ResourceListFixture)
{
   if (true) {
      GuardResourceList guard(resourceList);
      std::shared_ptr<TestResource> myResource = TestResource::cast(resourceList->at(guard, 0));
      myResource->setAvailable(false);
   }

   BOOST_REQUIRE_EQUAL (resourceList->asString (), "balance.ResourceList { basis.NamedObject { Name: TestResources } | Available = 9 of 10 }");
}

BOOST_FIXTURE_TEST_CASE (as_xml, ResourceListFixture)
{
   const int hotIndex = ResourceListFixture::MaxResources / 2;

   if (true) {
      GuardResourceList guard(resourceList);
      std::shared_ptr<TestResource> myResource = TestResource::cast(resourceList->at(guard, hotIndex));
      myResource->setAvailable(false);
   }

   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("root");
   resourceList->asXML(root);
   xml::Compiler compiler;

   std::string str = compiler.apply(root);

   for (int ii = 0; ii < ResourceListFixture::MaxResources; ++ ii) {
      basis::StreamString ss("<balance.Resource IsAvailable=\"");
      ss << basis::AsString::apply(ii != hotIndex) << "\" Name=\"TestResource-";
      ss << basis::AsString::apply(ii, "%02d") << "\"/>";
      BOOST_REQUIRE(str.find(ss) != std::string::npos);
   }
}

