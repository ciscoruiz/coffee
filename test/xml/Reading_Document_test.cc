#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <coffee/adt/DataBlock.hpp>
#include <coffee/adt/AsString.hpp>

#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;
using namespace coffee::xml;

struct XmlFixture {
   void setup(const char* buffer) {
      adt::DataBlock content(buffer, strlen(buffer) + 1);
      BOOST_REQUIRE(document.getHandler() == NULL);
      document.parse(content);
      BOOST_REQUIRE(document.getHandler() != NULL);
      root = document.getRoot();

      BOOST_REQUIRE(root);
   }

   xml::Document document;
   std::shared_ptr<xml::Node> root;

};

//BOOST_FIXTURE_TEST_CASE(persistence_storage_read, Fixture)


BOOST_FIXTURE_TEST_CASE(simpliest_memory, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <one_node><param>the text</param><param>the text</param></one_node>";

   setup(buffer);

   BOOST_REQUIRE_EQUAL(root->hasText(), false);

   BOOST_REQUIRE_EQUAL(root->children_size(), 2);

   for(xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      std::shared_ptr<xml::Node> child = xml::Node::get_child(ii);
      BOOST_REQUIRE_EQUAL(child->getName(), "param");
      BOOST_REQUIRE_EQUAL(child->children_size(), 0);
      BOOST_REQUIRE_EQUAL(child->attributes_size(), 0);
      BOOST_REQUIRE_EQUAL(child->hasText(), true);
      BOOST_REQUIRE_EQUAL(child->getText(), "the text");
   }
}

BOOST_FIXTURE_TEST_CASE(memory_document, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> \
   <peticion> \
      <param nombre=\"Cita Médica\">13:00 sala 23</param> \
      <param nombre=\"asunto\">Cita Médica</param>  \
      <param nombre=\"texto\">Cita médica mañana a las 13.30 en el centro de salud MiraSierra</param> \
   </peticion>";

   setup(buffer);

   BOOST_REQUIRE_EQUAL(root->children_size(), 3);

   for(xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      std::shared_ptr<xml::Node> child = xml::Node::get_child(ii);
      BOOST_REQUIRE_EQUAL(child->getName(), "param");
      BOOST_REQUIRE_EQUAL(child->children_size(), 0);
      BOOST_REQUIRE_EQUAL(child->attributes_size(), 1);
      BOOST_REQUIRE_EQUAL(child->hasText(), true);
   }
}

BOOST_FIXTURE_TEST_CASE(find_child_byname, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   setup(buffer);

   BOOST_REQUIRE_EQUAL(root->hasText(), false);
   BOOST_REQUIRE_EQUAL(root->children_size(), 2);

   BOOST_REQUIRE_NO_THROW(root->lookupChild("child_one"));
   BOOST_REQUIRE_NO_THROW(root->lookupChild("child_two"));

   std::shared_ptr<Node> childOne = root->searchChild("child_one");
   BOOST_REQUIRE(childOne);
   BOOST_REQUIRE_EQUAL(childOne->hasText(), true);
   BOOST_REQUIRE_EQUAL(childOne->getText(), "text one");

   std::shared_ptr<Node> childTwo = root->searchChild("child_two");
   BOOST_REQUIRE(childTwo);
   BOOST_REQUIRE_EQUAL(childTwo->hasText(), true);
   BOOST_REQUIRE_EQUAL(childTwo->getText(), "text two");

   std::shared_ptr<Node> found = root->searchChild("not_found");
   BOOST_REQUIRE(!found);

   BOOST_REQUIRE_THROW(root->lookupChild("not_found"), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(const_find_child_byname, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   setup(buffer);

   BOOST_REQUIRE_EQUAL(root->hasText(), false);
   BOOST_REQUIRE_EQUAL(root->children_size(), 2);
   BOOST_REQUIRE_NO_THROW(root->lookupChild("child_one"));
   BOOST_REQUIRE_NO_THROW(root->lookupChild("child_two"));

   std::shared_ptr<Node> childOne = root->searchChild("child_one");
   BOOST_REQUIRE(childOne);
   BOOST_REQUIRE_EQUAL(childOne->hasText(), true);
   BOOST_REQUIRE_EQUAL(childOne->getText(), "text one");

   std::shared_ptr<Node> childTwo = root->searchChild("child_two");
   BOOST_REQUIRE(childTwo);
   BOOST_REQUIRE_EQUAL(childTwo->hasText(), true);
   BOOST_REQUIRE_EQUAL(childTwo->getText(), "text two");

   std::shared_ptr<Node> found = root->searchChild("not_found");
   BOOST_REQUIRE(!found);

   BOOST_REQUIRE_THROW(root->lookupChild("not_found"), adt::RuntimeException);

   const Node& constRoot = *root;
   BOOST_REQUIRE_NO_THROW(constRoot.lookupChild("child_one"));
   BOOST_REQUIRE(constRoot.searchChild("child_two"));
   BOOST_REQUIRE_THROW(constRoot.lookupChild("not_found"), adt::RuntimeException);
   BOOST_REQUIRE(!constRoot.searchChild("not_found"));
   BOOST_REQUIRE_NO_THROW(constRoot.childAt(0));
}

BOOST_FIXTURE_TEST_CASE(locate_child_by_pos, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node><child_one>text one</child_one><child_two>text two</child_two></root_node>";

   setup(buffer);

   BOOST_REQUIRE_EQUAL(root->hasText(), false);
   BOOST_REQUIRE_EQUAL(root->children_size(), 2);

   std::shared_ptr<Node> child = root->childAt(0);
   BOOST_REQUIRE_EQUAL(child->hasText(), true);
   BOOST_REQUIRE_EQUAL(child->getText(), "text one");

   child  = root->childAt(1);
   BOOST_REQUIRE_EQUAL(child->hasText(), true);
   BOOST_REQUIRE_EQUAL(child->getText(), "text two");

   BOOST_REQUIRE_THROW(root->childAt(-1), adt::RuntimeException);
   BOOST_REQUIRE_THROW(root->childAt(10), adt::RuntimeException);

   const Node& constRoot = *root;
   BOOST_REQUIRE_THROW(constRoot.childAt(-1), adt::RuntimeException);
   BOOST_REQUIRE_THROW(constRoot.childAt(10), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(const_iterate_attr, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node attr_aaa='text one' attr_bbb='text two' attr_ccc='text three'/>";
   const char* names [] = { "attr_aaa", "attr_bbb", "attr_ccc" };
   const char* texts [] = { "text one", "text two", "text three" };

   setup(buffer);

   BOOST_REQUIRE_EQUAL(root->hasText(), false);
   BOOST_REQUIRE_EQUAL(root->children_size(), 0);
   BOOST_REQUIRE_EQUAL(root->attributes_size(), 3);

   int index = 0;
   for(xml::Node::const_attribute_iterator ii = root->attribute_begin(), maxii = root->attribute_end(); ii != maxii; ++ ii) {
      const std::shared_ptr<xml::Attribute>& attr = xml::Node::get_attribute(ii);

      BOOST_REQUIRE_EQUAL(attr->getName(), names[index]);
      BOOST_REQUIRE_EQUAL(attr->getValue(), texts[index]);
      ++ index;
   }
}

BOOST_FIXTURE_TEST_CASE(locate_attr_by_name, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node attr_aaa='text one' attr_bbb='text two' attr_ccc='text three'/>";
   const char* names [] = { "attr_aaa", "attr_bbb", "attr_ccc" };
   const char* texts [] = { "text one", "text two", "text three" };

   setup(buffer);

   for (int ii = 0; ii < 3; ++ ii) {
      std::shared_ptr<xml::Attribute> attr = root->lookupAttribute(names[ii]);
      BOOST_REQUIRE_EQUAL(attr->getValue(), texts[ii]);
   }

   BOOST_REQUIRE_THROW(root->lookupAttribute("not_found"), adt::RuntimeException);
   BOOST_REQUIRE_THROW(root->getText(), adt::RuntimeException);

   std::shared_ptr<xml::Attribute> found = root->searchAttribute("not_found");
   BOOST_REQUIRE(!found);

   const Node& constRoot = *root;
   BOOST_REQUIRE_THROW(constRoot.lookupAttribute("not_found"), adt::RuntimeException);
   BOOST_REQUIRE(!constRoot.searchAttribute("not_found"));
}

BOOST_FIXTURE_TEST_CASE(locate_attr_duplicate, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> <root_node attr_aaa='text one' attr_bbb='text two' attr_ccc='text three'/>";
   const char* names [] = { "attr_aaa", "attr_bbb", "attr_ccc" };
   const char* texts [] = { "text one", "text two", "text three" };

   setup(buffer);

   BOOST_REQUIRE_THROW(root->createAttribute("attr_aaa", 22), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(encoding, XmlFixture)
{
   const char* buffer = "<?xml version='1.0' encoding='ISO-8859-1'?> \
   <peticion> \
      <param1 nombre=\"Cita Médica\">13:00 sala 23</param1> \
      <param2 nombre=\"asunto\">Cita Médica</param2>  \
      <param3 nombre=\"texto\">Cita médica mañana a las 13.30 en el centro de salud MiraSierra</param3> \
   </peticion>";

   setup(buffer);

   std::shared_ptr<xml::Node> node = root->lookupChild("param1");
   BOOST_REQUIRE_EQUAL(node->lookupAttribute("nombre")->getValue(), "Cita Médica");

   node  = root->lookupChild("param2");
   BOOST_REQUIRE_EQUAL(node->getText(), "Cita Médica");
}

BOOST_AUTO_TEST_CASE(file_document)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   boost::filesystem::path xmlPath(coffeePath.native() + "/test/xml/example00.xml");

   boost::filesystem::path file(xmlPath);

   Document doc;

   BOOST_REQUIRE(doc.getHandler() == NULL);

   try {
      doc.parse(file);
   }
   catch(adt::RuntimeException& ex) {
      std::cout << ex.asString() << std::endl << std::endl;
   }

   BOOST_REQUIRE(doc.getHandler() != NULL);

   std::shared_ptr<Node> root = doc.getRoot();

   BOOST_REQUIRE(root);

   BOOST_REQUIRE_EQUAL(root->children_size(), 3);

   for(xml::Node::child_iterator ii = root->child_begin(), maxii = root->child_end(); ii != maxii; ++ ii) {
      std::shared_ptr<xml::Node> child = xml::Node::get_child(ii);
      BOOST_REQUIRE_EQUAL(child->getName(), "param");
      BOOST_REQUIRE_EQUAL(child->children_size(), 0);
      BOOST_REQUIRE_EQUAL(child->attributes_size(), 1);
      BOOST_REQUIRE_EQUAL(child->hasText(), true);
   }
}

BOOST_AUTO_TEST_CASE(file_notexisting_document)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   boost::filesystem::path xmlPath(coffeePath.native() + "does_not_exist.xml");

   boost::filesystem::path file(xmlPath);

   Document doc;

   BOOST_REQUIRE(doc.getHandler() == NULL);

   BOOST_REQUIRE_THROW(doc.parse(file), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(compile_document)
{
   const char* buffer = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<root_node attr_aaa=\"text one\" attr_bbb=\"text two\" attr_ccc=\"text three\"/>\n";

   adt::DataBlock content(buffer, strlen(buffer) + 1);

   Document doc;

   BOOST_REQUIRE(doc.getHandler() == NULL);

   doc.parse(content);

   Compiler compiler;

   std::string str = compiler.apply(doc);

   BOOST_REQUIRE_EQUAL(str, buffer);
}

BOOST_AUTO_TEST_CASE(compile_document_iso)
{
   const char* buffer = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<root_node>Jörg</root_node>\n";

   Document doc;

   BOOST_REQUIRE(doc.getHandler() == NULL);

   doc.parse(buffer, coffee_strlen(buffer));

   Compiler compiler;

   std::string str = compiler.apply(doc);

   BOOST_REQUIRE_EQUAL(str, buffer);
}

