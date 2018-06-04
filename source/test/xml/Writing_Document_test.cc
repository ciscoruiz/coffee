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

#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include <coffee/basis/DataBlock.hpp>

#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;
using namespace coffee::xml;

TEST(XmlWrittingDocumentTest,create_nodes)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   std::shared_ptr<xml::Node> level00 = root->createChild("level0-0");
   level00->createChild("level1-0");
   level00->createChild("level1-1");
   level00->createChild("level1-2");
   level00->createChild("level1-3")->createChild("2-0");
   ASSERT_EQ(4, level00->children_size());

   root->createChild("level0-1");
   root->createChild("level0-2");

   ASSERT_EQ(3, root->children_size());

   ASSERT_EQ(0, root->lookupChild("level0-0")->lookupChild("level1-0")->children_size());
   ASSERT_EQ(1, root->lookupChild("level0-0")->lookupChild("level1-3")->children_size());
}

TEST(XmlWrittingDocumentTest,create_text)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   std::shared_ptr<xml::Node> level00 = root->createChild("level0-0");
   level00->createText("0-0");
   level00->createChild("level1-3")->createText("2-0");
   ASSERT_EQ(1, level00->children_size());

   ASSERT_EQ("0-0", level00->getText());
   ASSERT_EQ("2-0", level00->lookupChild("level1-3")->getText());
}

TEST(XmlWrittingDocumentTest,create_attribute)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   root->createAttribute("attr_char", "This is the char");
   root->createAttribute("attr_int", 1024);
   root->createAttribute("attr_second", std::chrono::seconds(59));

   ASSERT_EQ(3, root->attributes_size());

   ASSERT_EQ("This is the char", root->lookupAttribute("attr_char")->getValue());
   ASSERT_EQ("1024", root->lookupAttribute("attr_int")->getValue());
   ASSERT_EQ("59 sec", root->lookupAttribute("attr_second")->getValue());
}

TEST(XmlWrittingDocumentTest,change_attribute)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   root->createAttribute("attr_char", "This is the char");

   ASSERT_EQ(1, root->attributes_size());

   ASSERT_EQ("This is the char", root->lookupAttribute("attr_char")->getValue());

   {
      std::string innerScope = "zzz";
      root->searchAttribute("attr_char")->setValue(innerScope);
   }

   int ii;

   ASSERT_EQ("zzz", root->lookupAttribute("attr_char")->getValue());
}

TEST(XmlWrittingDocumentTest,compile_text)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the_root");

   std::shared_ptr<xml::Node> level00 = root->createChild("level0-0");
   level00->createText("aaaa-bbbb");

   xml::Compiler compiler;

   std::string str = compiler.apply(root);

   ASSERT_EQ("<the_root><level0-0>aaaa-bbbb</level0-0></the_root>", str);
}

TEST(XmlWrittingDocumentTest,compile_tree)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the_root");

   std::shared_ptr<xml::Node> level00 = root->createChild("level0");

   level00->createAttribute("MyAttr", "It works");
   std::shared_ptr<xml::Node> level1 = level00->createChild("level1");
   level1->createAttribute("ZZ", "zz");

   level00->createChild("level2");
   std::shared_ptr<xml::Node> last = level00->createChild("level3")->createChild("LevelA");

   last->createAttribute("first", "111");
   last->createAttribute("second", 222);

   xml::Compiler compiler;
   std::string str = compiler.apply(root);

   ASSERT_EQ("<the_root><level0 MyAttr=\"It works\"><level1 ZZ=\"zz\"/><level2/><level3><LevelA first=\"111\" second=\"222\"/></level3></level0></the_root>", str);
}

TEST(XmlWrittingDocumentTest,compile_iso)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the_root");

   std::shared_ptr<xml::Node> level00 = root->createChild("level00");
   level00->createAttribute("Name", "Jörg1");
   level00->createAttribute("Other", "Büörgä");

   std::shared_ptr<xml::Node> level01 = root->createChild("level01");
   level01->createText("Jörg2");

   xml::Compiler compiler;
   compiler.setEncoding("ISO-8859-1");
   std::string str = compiler.apply(root);

   ASSERT_EQ("<the_root><level00 Name=\"J&#xF6;rg1\" Other=\"B&#xFC;&#xF6;rg&#xE4;\"/><level01>Jörg2</level01></the_root>", str);
}

TEST(XmlWrittingDocumentTest,compare_attribute)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   auto attr00 = root->createAttribute("attr00", "This is the char");
   auto attr01 = root->createAttribute("attr01", 1024);

   ASSERT_TRUE(*attr00 < *attr01);
}
