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

#include <iostream>

#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include <coffee/basis/DataBlock.hpp>
#include <coffee/basis/AsString.hpp>

#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;
using namespace coffee::xml;

struct XmlReadingOnMemoryTest : public ::testing::Test {
   void setup(const char* buffer) {
      basis::DataBlock content(buffer, strlen(buffer) + 1);
      ASSERT_TRUE(document.getHandler() == NULL);
      document.parse(content);
      ASSERT_TRUE(document.getHandler() != NULL);
      root = document.getRoot();

      ASSERT_TRUE(root != nullptr);
   }

   xml::Document document;
   std::shared_ptr<xml::Node> root;
};

TEST_F(XmlReadingOnMemoryTest, simpliest_memory)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <one_node><param>the text</param><param>the text</param></one_node>";

   setup(buffer);

   ASSERT_FALSE(root->hasText());

   ASSERT_EQ(2, root->children_size());

   for(xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      std::shared_ptr<xml::Node> child = xml::Node::get_child(ii);
      ASSERT_EQ("param", child->getName());
      ASSERT_EQ(0, child->children_size());
      ASSERT_EQ(0, child->attributes_size());
      ASSERT_EQ(true, child->hasText());
      ASSERT_EQ("the text", child->getText());
   }
}

TEST_F(XmlReadingOnMemoryTest, memory_document)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> \
   <peticion> \
      <param nombre=\"Cita Médica\">13:00 sala 23</param> \
      <param nombre=\"asunto\">Cita Médica</param>  \
      <param nombre=\"texto\">Cita médica mañana a las 13.30 en el centro de salud MiraSierra</param> \
   </peticion>";

   setup(buffer);

   ASSERT_EQ(3, root->children_size());

   for(xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      std::shared_ptr<xml::Node> child = xml::Node::get_child(ii);
      ASSERT_EQ("param", child->getName());
      ASSERT_EQ(0, child->children_size());
      ASSERT_EQ(1, child->attributes_size());
      ASSERT_EQ(true, child->hasText());
   }
}

TEST_F(XmlReadingOnMemoryTest, find_child_byname)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   setup(buffer);

   ASSERT_EQ(false, root->hasText());
   ASSERT_EQ(2, root->children_size());

   ASSERT_NO_THROW(root->lookupChild("child_one"));
   ASSERT_NO_THROW(root->lookupChild("child_two"));

   std::shared_ptr<Node> childOne = root->searchChild("child_one");
   ASSERT_TRUE(childOne != nullptr);
   ASSERT_EQ(true, childOne->hasText());
   ASSERT_EQ("text one", childOne->getText());

   std::shared_ptr<Node> childTwo = root->searchChild("child_two");
   ASSERT_FALSE(!childTwo);
   ASSERT_EQ(true, childTwo->hasText());
   ASSERT_EQ("text two", childTwo->getText());

   std::shared_ptr<Node> found = root->searchChild("not_found");
   ASSERT_TRUE(!found);

   ASSERT_THROW(root->lookupChild("not_found"), basis::RuntimeException);
}

TEST_F(XmlReadingOnMemoryTest, const_find_child_byname)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   setup(buffer);

   ASSERT_EQ(false, root->hasText());
   ASSERT_EQ(2, root->children_size());
   ASSERT_NO_THROW(root->lookupChild("child_one"));
   ASSERT_NO_THROW(root->lookupChild("child_two"));

   std::shared_ptr<Node> childOne = root->searchChild("child_one");
   ASSERT_TRUE(childOne != nullptr);
   ASSERT_EQ(true, childOne->hasText());
   ASSERT_EQ("text one", childOne->getText());

   std::shared_ptr<Node> childTwo = root->searchChild("child_two");
   ASSERT_TRUE(childTwo != nullptr);
   ASSERT_EQ(true, childTwo->hasText());
   ASSERT_EQ("text two", childTwo->getText());

   std::shared_ptr<Node> found = root->searchChild("not_found");
   ASSERT_TRUE(!found);

   ASSERT_THROW(root->lookupChild("not_found"), basis::RuntimeException);

   const Node& constRoot = *root;
   ASSERT_NO_THROW(constRoot.lookupChild("child_one"));
   ASSERT_FALSE(!constRoot.searchChild("child_two"));
   ASSERT_THROW(constRoot.lookupChild("not_found"), basis::RuntimeException);
   ASSERT_TRUE(!constRoot.searchChild("not_found"));
   ASSERT_NO_THROW(constRoot.childAt(0));
}

TEST_F(XmlReadingOnMemoryTest, locate_child_by_pos)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   setup(buffer);

   ASSERT_EQ(false, root->hasText());
   ASSERT_EQ(2, root->children_size());

   std::shared_ptr<Node> child = root->childAt(0);
   ASSERT_EQ(true, child->hasText());
   ASSERT_EQ("text one", child->getText());

   child  = root->childAt(1);
   ASSERT_EQ(true, child->hasText());
   ASSERT_EQ("text two", child->getText());

   ASSERT_THROW(root->childAt(-1), basis::RuntimeException);
   ASSERT_THROW(root->childAt(10), basis::RuntimeException);

   const Node& constRoot = *root;
   ASSERT_THROW(constRoot.childAt(-1), basis::RuntimeException);
   ASSERT_THROW(constRoot.childAt(10), basis::RuntimeException);
}

TEST_F(XmlReadingOnMemoryTest, const_iterate_attr)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node attr_aaa='text one' attr_bbb='text two' attr_ccc='text three'/>";
   const char* names [] = { "attr_aaa", "attr_bbb", "attr_ccc" };
   const char* texts [] = { "text one", "text two", "text three" };

   setup(buffer);

   ASSERT_EQ(false, root->hasText());
   ASSERT_EQ(0, root->children_size());
   ASSERT_EQ(3, root->attributes_size());

   int index = 0;
   for(xml::Node::const_attribute_iterator ii = root->attribute_begin(), maxii = root->attribute_end(); ii != maxii; ++ ii) {
      const std::shared_ptr<xml::Attribute>& attr = xml::Node::get_attribute(ii);

      ASSERT_EQ(names[index], attr->getName());
      ASSERT_EQ(texts[index], attr->getValue());
      ++ index;
   }
}

TEST_F(XmlReadingOnMemoryTest, locate_attr_by_name)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node attr_aaa='text one' attr_bbb='text two' attr_ccc='text three'/>";
   const char* names [] = { "attr_aaa", "attr_bbb", "attr_ccc" };
   const char* texts [] = { "text one", "text two", "text three" };

   setup(buffer);

   for (int ii = 0; ii < 3; ++ ii) {
      std::shared_ptr<xml::Attribute> attr = root->lookupAttribute(names[ii]);
      ASSERT_EQ(texts[ii], attr->getValue());
   }

   ASSERT_THROW(root->lookupAttribute("not_found"), basis::RuntimeException);
   ASSERT_THROW(root->getText(), basis::RuntimeException);

   std::shared_ptr<xml::Attribute> found = root->searchAttribute("not_found");
   ASSERT_TRUE(!found);

   const Node& constRoot = *root;
   ASSERT_THROW(constRoot.lookupAttribute("not_found"), basis::RuntimeException);
   ASSERT_TRUE(!constRoot.searchAttribute("not_found"));
}

TEST_F(XmlReadingOnMemoryTest, locate_attr_duplicate)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node attr_aaa='text one' attr_bbb='text two' attr_ccc='text three'/>";
   const char* names [] = { "attr_aaa", "attr_bbb", "attr_ccc" };
   const char* texts [] = { "text one", "text two", "text three" };

   setup(buffer);

   ASSERT_THROW(root->createAttribute("attr_aaa", 22), basis::RuntimeException);
}

TEST_F(XmlReadingOnMemoryTest, encoding)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> \
   <peticion> \
      <param1 nombre=\"Cita Médica\">13:00 sala 23</param1> \
      <param2 nombre=\"asunto\">Cita Médica</param2>  \
      <param3 nombre=\"texto\">Cita médica mañana a las 13.30 en el centro de salud MiraSierra</param3> \
   </peticion>";

   setup(buffer);

   std::shared_ptr<xml::Node> node = root->lookupChild("param1");
   ASSERT_EQ("Cita Médica", node->lookupAttribute("nombre")->getValue());

   node  = root->lookupChild("param2");
   ASSERT_EQ("Cita Médica", node->getText());
}

TEST(XmlReadingDocumentTest, file_document)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   boost::filesystem::path xmlPath(coffeePath.native() + "/source/test/xml/example00.xml");

   boost::filesystem::path file(xmlPath);

   Document doc;

   ASSERT_TRUE(doc.getHandler() == NULL);

   try {
      doc.parse(file);
   }
   catch(basis::RuntimeException& ex) {
      std::cout << ex.asString() << std::endl << std::endl;
   }

   ASSERT_TRUE(doc.getHandler() != NULL);

   std::shared_ptr<Node> root = doc.getRoot();

   ASSERT_TRUE(root != nullptr);

   ASSERT_EQ(3, root->children_size());

   for(xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      std::shared_ptr<xml::Node> child = xml::Node::get_child(ii);
      ASSERT_EQ("param", child->getName());
      ASSERT_EQ(0, child->children_size());
      ASSERT_EQ(1, child->attributes_size());
      ASSERT_EQ(true, child->hasText());
   }
}

TEST(XmlReadingDocumentTest, file_notexisting_document)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   boost::filesystem::path xmlPath(coffeePath.native() + "does_not_exist.xml");

   boost::filesystem::path file(xmlPath);

   Document doc;

   ASSERT_TRUE(doc.getHandler() == NULL);

   ASSERT_THROW(doc.parse(file), basis::RuntimeException);
}

TEST(XmlReadingDocumentTest, file_bad_document)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());
   boost::filesystem::path xmlPath(coffeePath.native() + "/source/test/xml/bad.xml");

   Document doc;

   ASSERT_THROW(doc.parse(xmlPath), basis::RuntimeException);
}

TEST(XmlReadingDocumentTest, memory_bad_document)
{
   const basis::DataBlock content("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<root_node attr_aaa=\"text one\" attr_bbb=");

   Document doc;

   ASSERT_THROW(doc.parse(content), basis::RuntimeException);
}

TEST(XmlReadingDocumentTest, compile_document)
{
   const basis::DataBlock content("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<root_node attr_aaa=\"text one\" attr_bbb=\"text two\" attr_ccc=\"text three\"/>\n");
   Document doc;

   ASSERT_TRUE(doc.getHandler() == NULL);

   Compiler compiler;

   std::string str = compiler.apply(doc.parse(content));

   ASSERT_EQ(content.data(), str);
}

TEST(XmlReadingDocumentTest, compile_document_iso)
{
   const basis::DataBlock content("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<root_node>Jörg</root_node>\n");

   Document doc;

   ASSERT_TRUE(doc.getHandler() == NULL);

   Compiler compiler;

   std::string str = compiler.apply(doc.parse(content));

   ASSERT_EQ(content.data(), str);
}

