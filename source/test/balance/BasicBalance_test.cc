// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#include <gtest/gtest.h>

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/ResourceContainer.hpp>
#include <coffee/balance/GuardResourceContainer.hpp>
#include <coffee/balance/StrategyRoundRobin.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Compiler.hpp>

#include "TestResource.hpp"
#include "ResourceListFixture.hpp"

using namespace coffee;
using namespace coffee::balance;

using namespace coffee::test::balance;

const int ResourceListFixture::MaxResources = 10;

TEST(BasicBalanceTest, avoid_empties)
{
   ResourceContainer resourceList("otherList");
   std::shared_ptr<Resource> emptyResource;
   ASSERT_THROW(resourceList.add(emptyResource), basis::RuntimeException);
}

TEST_F(ResourceListFixture, count_availables)
{
   GuardResourceContainer guard(resourceList);

   ASSERT_EQ(MaxResources, resourceList->size(guard));
   ASSERT_EQ(MaxResources, resourceList->countAvailableResources(guard));

   bool available = false;
   for(auto ii = resourceList->resource_begin(guard), maxii = resourceList->resource_end(guard); ii != maxii; ++ ii) {
      std::shared_ptr<TestResource> myResource = TestResource::cast(ResourceContainer::resource(ii));
      myResource->setAvailable(available);
      available = !available;
   }

   ASSERT_EQ(MaxResources / 2, resourceList->countAvailableResources(guard));
}

TEST(BasicBalanceTest,error_while_initialize )
{
   std::shared_ptr<coffee::balance::ResourceContainer> resourceList = std::make_shared<coffee::balance::ResourceContainer>("TestResources");

   resourceList->add(std::make_shared<TestResource>(0));
   resourceList->add(std::make_shared<UnusableResource>());
   resourceList->add(std::make_shared<TestResource>(1));

   resourceList->initialize();

   GuardResourceContainer guard(resourceList);

   ASSERT_EQ(2, resourceList->countAvailableResources(guard));
}

TEST(BasicBalanceTest,avoid_repeated_resource )
{
   std::shared_ptr<coffee::balance::ResourceContainer> resourceList = std::make_shared<coffee::balance::ResourceContainer>("TestResources");

   ASSERT_TRUE(resourceList->add(std::make_shared<TestResource>(0)));
   ASSERT_TRUE(resourceList->add(std::make_shared<TestResource>(1)));
   ASSERT_FALSE(resourceList->add(std::make_shared<TestResource>(0)));

   GuardResourceContainer guard(resourceList);

   ASSERT_EQ(2, resourceList->size(guard));
}

TEST(BasicBalanceTest,remove_resource )
{
   std::shared_ptr<coffee::balance::ResourceContainer> resourceList = std::make_shared<coffee::balance::ResourceContainer>("TestResources");

   auto resource = std::make_shared<TestResource>(1);

   ASSERT_TRUE(resourceList->add(std::make_shared<TestResource>(0)));
   ASSERT_TRUE(resourceList->add(resource));
   ASSERT_TRUE(resourceList->add(std::make_shared<TestResource>(2)));

   {
      GuardResourceContainer guard(resourceList);
      ASSERT_EQ(3, resourceList->size(guard));
   }

   ASSERT_TRUE(resourceList->remove(resource->getName()));

   {
      GuardResourceContainer guard(resourceList);
      ASSERT_EQ(2, resourceList->size(guard));
   }
}

TEST(BasicBalanceTest,remove_unknow_resource )
{
   std::shared_ptr<coffee::balance::ResourceContainer> resourceList = std::make_shared<coffee::balance::ResourceContainer>("TestResources");

   ASSERT_TRUE(resourceList->add(std::make_shared<TestResource>(0)));
   ASSERT_TRUE(resourceList->add(std::make_shared<TestResource>(2)));

   ASSERT_FALSE(resourceList->remove("not-exist"));

   {
      GuardResourceContainer guard(resourceList);
      ASSERT_EQ(2, resourceList->size(guard));
   }
}

TEST(BasicBalanceTest,initialize_empty_list )
{
   std::shared_ptr<coffee::balance::ResourceContainer> resourceList = std::make_shared<coffee::balance::ResourceContainer>("TestResources");
   ASSERT_NO_THROW(resourceList->initialize());
}

TEST(BasicBalanceTest,initialize_without_available_resources )
{
   std::shared_ptr<coffee::balance::ResourceContainer> resourceList = std::make_shared<coffee::balance::ResourceContainer>("TestResources");
   resourceList->add(std::make_shared<UnusableResource>());
   resourceList->add(std::make_shared<UnusableResource>());
   ASSERT_NO_THROW(resourceList->initialize());

   GuardResourceContainer guard(resourceList);
   ASSERT_EQ(0, resourceList->countAvailableResources(guard));
}

TEST_F(ResourceListFixture, as_string)
{
   if(true) {
      GuardResourceContainer guard(resourceList);
      std::shared_ptr<TestResource> myResource = TestResource::cast(resourceList->at(guard, 0));
      myResource->setAvailable(false);
   }

   ASSERT_EQ("balance.ResourceContainer { basis.NamedObject {Name=TestResources} | Available = 9 of 10 }", resourceList->asString());
}

TEST_F(ResourceListFixture, as_xml)
{
   const int hotIndex = ResourceListFixture::MaxResources / 2;

   if(true) {
      GuardResourceContainer guard(resourceList);
      std::shared_ptr<TestResource> myResource = TestResource::cast(resourceList->at(guard, hotIndex));
      myResource->setAvailable(false);
   }

   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("root");
   resourceList->asXML(root);
   xml::Compiler compiler;

   std::string str = compiler.apply(root);

   for(int ii = 0; ii < ResourceListFixture::MaxResources; ++ ii) {
      basis::StreamString ss("<balance.Resource IsAvailable=\"");
      ss << basis::AsString::apply(ii != hotIndex) << "\" Name=\"TestResource-";
      ss << basis::AsString::apply(ii, "%02d") << "\"/>";
      ASSERT_TRUE(str.find(ss) != std::string::npos);
   }
}

