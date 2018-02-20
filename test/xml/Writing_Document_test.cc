#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <coffee/adt/DataBlock.hpp>
#include <coffee/adt/Second.hpp>

#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;
using namespace coffee::xml;

BOOST_AUTO_TEST_CASE(create_nodes)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   std::shared_ptr<xml::Node> level00 = root->createChild("level0-0");
   level00->createChild("level1-0");
   level00->createChild("level1-1");
   level00->createChild("level1-2");
   level00->createChild("level1-3")->createChild("2-0");
   BOOST_REQUIRE_EQUAL(level00->children_size(), 4);

   root->createChild("level0-1");
   root->createChild("level0-2");

   BOOST_REQUIRE_EQUAL(root->children_size(), 3);

   BOOST_REQUIRE_EQUAL(root->lookupChild("level0-0")->lookupChild("level1-0")->children_size(), 0);
   BOOST_REQUIRE_EQUAL(root->lookupChild("level0-0")->lookupChild("level1-3")->children_size(), 1);
}

BOOST_AUTO_TEST_CASE(create_text)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   std::shared_ptr<xml::Node> level00 = root->createChild("level0-0");
   level00->createText("0-0");
   level00->createChild("level1-3")->createText("2-0");
   BOOST_REQUIRE_EQUAL(level00->children_size(), 1);

   BOOST_REQUIRE_EQUAL(level00->getText(), "0-0");
   BOOST_REQUIRE_EQUAL(level00->lookupChild("level1-3")->getText(), "2-0");
}

BOOST_AUTO_TEST_CASE(create_attribute)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   root->createAttribute("attr_char", "This is the char");
   root->createAttribute("attr_int", 1024);
   root->createAttribute("attr_second", adt::Second(59));

   BOOST_REQUIRE_EQUAL(root->attributes_size(), 3);

   BOOST_REQUIRE_EQUAL(root->lookupAttribute("attr_char")->getValue(), "This is the char");
   BOOST_REQUIRE_EQUAL(root->lookupAttribute("attr_int")->getValue(), "1024");
   BOOST_REQUIRE_EQUAL(root->lookupAttribute("attr_second")->getValue(), "59");
}

BOOST_AUTO_TEST_CASE(change_attribute)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   root->createAttribute("attr_char", "This is the char");

   BOOST_REQUIRE_EQUAL(root->attributes_size(), 1);

   BOOST_REQUIRE_EQUAL(root->lookupAttribute("attr_char")->getValue(), "This is the char");

   {
      std::string innerScope = "zzz";
      root->searchAttribute("attr_char")->setValue(innerScope);
   }

   int ii;

   BOOST_REQUIRE_EQUAL(root->lookupAttribute("attr_char")->getValue(), "zzz");
}

BOOST_AUTO_TEST_CASE(compile_text)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the_root");

   std::shared_ptr<xml::Node> level00 = root->createChild("level0-0");
   level00->createText("aaaa-bbbb");

   xml::Compiler compiler;

   std::string str = compiler.apply(root);

   BOOST_REQUIRE_EQUAL(str, "<the_root><level0-0>aaaa-bbbb</level0-0></the_root>");
}

BOOST_AUTO_TEST_CASE(compile_tree)
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

   BOOST_REQUIRE_EQUAL(str, "<the_root><level0 MyAttr=\"It works\"><level1 ZZ=\"zz\"/><level2/><level3><LevelA first=\"111\" second=\"222\"/></level3></level0></the_root>");
}

BOOST_AUTO_TEST_CASE(compile_iso)
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

   BOOST_REQUIRE_EQUAL(str, "<the_root><level00 Name=\"J&#xF6;rg1\" Other=\"B&#xFC;&#xF6;rg&#xE4;\"/><level01>Jörg2</level01></the_root>");
}

BOOST_AUTO_TEST_CASE(compare_attribute)
{
   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("the root");

   auto attr00 = root->createAttribute("attr00", "This is the char");
   auto attr01 = root->createAttribute("attr01", 1024);

   BOOST_REQUIRE(*attr00 < *attr01);
}
