// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <wepa/adt/DataBlock.hpp>
#include <wepa/adt/AsString.hpp>

#include <wepa/xml/Document.hpp>
#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>
#include <wepa/xml/Compiler.hpp>

using namespace wepa;
using namespace wepa::xml;

BOOST_AUTO_TEST_CASE (simpliest_memory)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <one_node><param>the text</param><param>the text</param></one_node>";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* root = doc.getRoot();

   BOOST_REQUIRE (root != NULL);

   BOOST_REQUIRE_EQUAL(root->hasText (), false);

   BOOST_REQUIRE_EQUAL(root->children_size(), 2);

   for (xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      xml::Node& child = xml::Node::get_child(ii);
      BOOST_REQUIRE_EQUAL (child.getName (), "param");
      BOOST_REQUIRE_EQUAL (child.children_size(), 0);
      BOOST_REQUIRE_EQUAL (child.attributes_size(), 0);
      BOOST_REQUIRE_EQUAL (child.hasText(), true);
      BOOST_REQUIRE_EQUAL (child.getText (), "the text");
   }
}

BOOST_AUTO_TEST_CASE (memory_document)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> \
   <peticion> \
      <param nombre=\"Cita Médica\">13:00 sala 23</param> \
      <param nombre=\"asunto\">Cita Médica</param>  \
      <param nombre=\"texto\">Cita médica mañana a las 13.30 en el centro de salud MiraSierra</param> \
   </peticion>";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* root = doc.getRoot();

   BOOST_REQUIRE (root != NULL);

   BOOST_REQUIRE_EQUAL(root->children_size(), 3);

   for (xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      xml::Node& child = xml::Node::get_child(ii);
      BOOST_REQUIRE_EQUAL (child.getName (), "param");
      BOOST_REQUIRE_EQUAL (child.children_size(), 0);
      BOOST_REQUIRE_EQUAL (child.attributes_size(), 1);
      BOOST_REQUIRE_EQUAL (child.hasText(), true);
   }
}

BOOST_AUTO_TEST_CASE (find_child_byname)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* root = doc.getRoot();

   BOOST_REQUIRE (root != NULL);

   BOOST_REQUIRE_EQUAL(root->hasText (), false);

   BOOST_REQUIRE_EQUAL(root->children_size(), 2);

   const Node& childOne = root->lookupChild ("child_one");
   BOOST_REQUIRE_EQUAL (&childOne, root->searchChild("child_one"));
   BOOST_REQUIRE_EQUAL(childOne.hasText(), true);
   BOOST_REQUIRE_EQUAL(childOne.getText(), "text one");

   const Node& childTwo = root->lookupChild ("child_two");
   BOOST_REQUIRE_EQUAL (&childTwo, root->searchChild("child_two"));
   BOOST_REQUIRE_EQUAL(childTwo.hasText(), true);
   BOOST_REQUIRE_EQUAL(childTwo.getText(), "text two");

   BOOST_REQUIRE (root->searchChild("not_found") == NULL);

   BOOST_REQUIRE_THROW(root->lookupChild("not_found"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (const_find_child_byname)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* _root = doc.getRoot();

   const Node* root = _root;

   BOOST_REQUIRE (root != NULL);

   BOOST_REQUIRE_EQUAL(root->hasText (), false);

   BOOST_REQUIRE_EQUAL(root->children_size(), 2);

   const Node& childOne = root->lookupChild ("child_one");
   BOOST_REQUIRE_EQUAL (&childOne, root->searchChild("child_one"));
   BOOST_REQUIRE_EQUAL(childOne.hasText(), true);
   BOOST_REQUIRE_EQUAL(childOne.getText(), "text one");

   const Node& childTwo = root->lookupChild ("child_two");
   BOOST_REQUIRE_EQUAL (&childTwo, root->searchChild("child_two"));
   BOOST_REQUIRE_EQUAL(childTwo.hasText(), true);
   BOOST_REQUIRE_EQUAL(childTwo.getText(), "text two");

   BOOST_REQUIRE (root->searchChild("not_found") == NULL);

   BOOST_REQUIRE_THROW(root->lookupChild("not_found"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (locate_child_byname)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* root = doc.getRoot();

   BOOST_REQUIRE (root != NULL);

   BOOST_REQUIRE_EQUAL(root->hasText (), false);

   BOOST_REQUIRE_EQUAL(root->children_size(), 2);

   const Node& childOne = root->childAt (0);
   BOOST_REQUIRE_EQUAL (&childOne, root->searchChild("child_one"));
   BOOST_REQUIRE_EQUAL(childOne.hasText(), true);
   BOOST_REQUIRE_EQUAL(childOne.getText(), "text one");

   const Node& childTwo = root->childAt (1);
   BOOST_REQUIRE_EQUAL (&childTwo, root->searchChild("child_two"));
   BOOST_REQUIRE_EQUAL(childTwo.hasText(), true);
   BOOST_REQUIRE_EQUAL(childTwo.getText(), "text two");

   BOOST_REQUIRE_THROW(root->childAt (-1), adt::RuntimeException);
   BOOST_REQUIRE_THROW(root->childAt (10), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (const_iterate_attr)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node attr_aaa='text one' attr_bbb='text two' attr_ccc='text three'/>";
   const char* names [] = { "attr_aaa", "attr_bbb", "attr_ccc" };
   const char* texts [] = { "text one", "text two", "text three" };

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* _root = doc.getRoot();

   const Node* root = _root;

   BOOST_REQUIRE (root != NULL);

   BOOST_REQUIRE_EQUAL(root->hasText (), false);

   BOOST_REQUIRE_EQUAL(root->children_size(), 0);
   BOOST_REQUIRE_EQUAL(root->attributes_size(), 3);

   int index = 0;
   for (xml::Node::attribute_iterator ii = root->attribute_begin(), maxii = root->attribute_end(); ii != maxii; ++ ii) {
      xml::Attribute& attr = xml::Node::get_attribute(ii);

      BOOST_REQUIRE_EQUAL (attr.getName (), names [index]);
      BOOST_REQUIRE_EQUAL (attr.getValue (), texts [index]);
      ++ index;
   }
}

BOOST_AUTO_TEST_CASE (locate_attr_by_name)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node attr_aaa='text one' attr_bbb='text two' attr_ccc='text three'/>";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* root = doc.getRoot();

   xml::Attribute* null = NULL;

   xml::Attribute& attr = root->lookupAttribute("attr_aaa");
   BOOST_REQUIRE_EQUAL (attr.getName(), "attr_aaa");
   BOOST_REQUIRE_EQUAL (attr.getValue(), "text one");

   BOOST_REQUIRE_EQUAL (root->searchAttribute("attr_aaa"), &attr);

   xml::Attribute& attr2 = root->lookupAttribute("attr_ccc");
   BOOST_REQUIRE_EQUAL (attr2.getName(), "attr_ccc");
   BOOST_REQUIRE_EQUAL (attr2.getValue(), "text three");

   BOOST_REQUIRE_THROW(root->lookupAttribute("not_found"), adt::RuntimeException);
   BOOST_REQUIRE_EQUAL(root->searchAttribute("not_found"), null);
}

BOOST_AUTO_TEST_CASE (encoding)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> \
   <peticion> \
      <param1 nombre=\"Cita Médica\">13:00 sala 23</param1> \
      <param2 nombre=\"asunto\">Cita Médica</param2>  \
      <param3 nombre=\"texto\">Cita médica mañana a las 13.30 en el centro de salud MiraSierra</param3> \
   </peticion>";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* root = doc.getRoot();

   const xml::Node& param1 = root->lookupChild ("param1");
   BOOST_REQUIRE_EQUAL (param1.lookupAttribute("nombre").getValue (), "Cita Médica");

   const xml::Node& param2 = root->lookupChild ("param2");
   BOOST_REQUIRE_EQUAL (param2.getText (), "Cita Médica");
}

BOOST_AUTO_TEST_CASE (file_document)
{
   boost::filesystem::path wepaPath (boost::filesystem::current_path());

   boost::filesystem::path xmlPath (wepaPath.native() + "/test/xml/example00.xml");

   boost::filesystem::path file (xmlPath);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   try {
      doc.parse (file);
   }
   catch (adt::RuntimeException& ex) {
      std::cout << ex.asString() << std::endl << std::endl;
   }

   BOOST_REQUIRE (doc.getHandler() != NULL);

   Node* root = doc.getRoot();

   BOOST_REQUIRE (root != NULL);

   BOOST_REQUIRE_EQUAL(root->children_size(), 3);

   for (xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      xml::Node& child = xml::Node::get_child(ii);
      BOOST_REQUIRE_EQUAL (child.getName (), "param");
      BOOST_REQUIRE_EQUAL (child.children_size(), 0);
      BOOST_REQUIRE_EQUAL (child.attributes_size(), 1);
      BOOST_REQUIRE_EQUAL (child.hasText(), true);
   }
}

BOOST_AUTO_TEST_CASE (compile_document)
{
   const char* buffer = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<root_node attr_aaa=\"text one\" attr_bbb=\"text two\" attr_ccc=\"text three\"/>\n";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   Compiler compiler;

   std::string str = compiler.apply (doc);

   BOOST_REQUIRE_EQUAL (str, buffer);
}

BOOST_AUTO_TEST_CASE (compile_document_iso)
{
   const char* buffer = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<root_node>Jörg</root_node>\n";

   adt::DataBlock content (buffer, strlen (buffer) + 1);

   Document doc;

   BOOST_REQUIRE (doc.getHandler() == NULL);

   doc.parse (content);

   Compiler compiler;

   std::string str = compiler.apply (doc);

   BOOST_REQUIRE_EQUAL (str, buffer);
}
