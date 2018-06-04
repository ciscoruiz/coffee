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


#include <gtest/gtest.h>

#include <map>
#include <functional>
#include <mutex>
#include <thread>

#include <coffee/basis/RuntimeException.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/persistence/Repository.hpp>
#include <coffee/persistence/Storage.hpp>
#include <coffee/persistence/Class.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;

TEST(RepositoryTest, repeat)
{
   persistence::Repository repository("persistence_define_structure");

   auto ii = repository.createStorage("the 0", 128);

   std::shared_ptr<persistence::Storage>& find = repository.findStorage("the 0");

   ASSERT_EQ(find.get(), ii.get());

   ASSERT_THROW(repository.createStorage("the 0", 16), basis::RuntimeException);
}

TEST(RepositoryTest, asString)
{
   persistence::Repository repository("persistence_define_structure");

   repository.createStorage("the 0", 16);
   repository.createStorage("the 1", persistence::Storage::DefaultMaxCacheSize);

   basis::StreamString zz = repository.asString();

   ASSERT_EQ("persistence.Repository { basis.NamedObject {Name=persistence_define_structure} | N-Size=2 }", zz);

   xml::Node myNode("root");

   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("root");

   auto info = repository.asXML(root);

   ASSERT_EQ(2, info->children_size());
}

TEST(RepositoryTest, notfound)
{
   persistence::Repository repository("persistence_repository_notfound");

   ASSERT_THROW(repository.findStorage("does-not-exist"), basis::RuntimeException);
}
