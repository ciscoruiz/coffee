// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <boost/filesystem.hpp>

#include <boost/test/unit_test.hpp>

#include <coffee/basis/DataBlock.hpp>

#include <coffee/xml/DTD.hpp>
#include <coffee/xml/Document.hpp>


using namespace coffee;

BOOST_AUTO_TEST_CASE(dtd_does_not_exists)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   boost::filesystem::path dtdFile(coffeePath.native() + "does_not_exist.dtd");
   xml::DTD dtd;
   BOOST_REQUIRE_THROW(dtd.initialize(dtdFile), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dtd_incomplete_expression)
{
   const basis::DataBlock bad_dtd("<!ELEMENT Intersect (Second,Second)");
   xml::DTD dtd;
   BOOST_REQUIRE_THROW(dtd.initialize(bad_dtd), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dtd_uninit_dtd)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   xml::DTD dtd;

   boost::filesystem::path xmlFile(coffeePath.native() + "/source/test/xml/vertex.xml");
   xml::Document doc;
   BOOST_REQUIRE_THROW(doc.parse(xmlFile, dtd), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dtd_file_validate_file)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   boost::filesystem::path dtdFile(coffeePath.native() + "/source/test/xml/vertex.dtd");
   xml::DTD dtd;
   BOOST_REQUIRE_NO_THROW(dtd.initialize(dtdFile));

   boost::filesystem::path xmlFile(coffeePath.native() + "/source/test/xml/vertex.xml");
   xml::Document doc;
   BOOST_REQUIRE_NO_THROW(doc.parse(xmlFile, dtd));
}

BOOST_AUTO_TEST_CASE(dtd_memory_validate_file)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   const basis::DataBlock expression("<!ELEMENT Intersect (First,Second)><!ELEMENT First (Vertex+)><!ELEMENT Second (Vertex+)><!ELEMENT Vertex EMPTY><!ATTLIST Vertex X CDATA #REQUIRED><!ATTLIST Vertex Y CDATA #REQUIRED>");

   xml::DTD dtd;
   BOOST_REQUIRE_NO_THROW(dtd.initialize(expression));

   boost::filesystem::path xmlFile(coffeePath.native() + "/source/test/xml/vertex.xml");
   xml::Document doc;
   BOOST_REQUIRE_NO_THROW(doc.parse(xmlFile, dtd));
}

BOOST_AUTO_TEST_CASE(dtd_memory_validate_memory)
{
   const basis::DataBlock expression("<!ELEMENT Intersect (First,Second)><!ELEMENT First (Vertex+)><!ELEMENT Second (Vertex+)><!ELEMENT Vertex EMPTY><!ATTLIST Vertex X CDATA #REQUIRED><!ATTLIST Vertex Y CDATA #REQUIRED>");
   const basis::DataBlock xml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><Intersect><First><Vertex X= \"1\" Y=\"3\"/></First><Second><Vertex X= \"1\" Y=\"3\"/></Second></Intersect>");

   xml::DTD dtd;
   BOOST_REQUIRE_NO_THROW(dtd.initialize(expression));

   xml::Document doc;
   BOOST_REQUIRE_NO_THROW(doc.parse(xml, dtd));
}

BOOST_AUTO_TEST_CASE(dtd_novalidate_file)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   boost::filesystem::path dtdFile(coffeePath.native() + "/source/test/xml/vertex.dtd");
   xml::DTD dtd;
   BOOST_REQUIRE_NO_THROW(dtd.initialize(dtdFile));

   boost::filesystem::path xmlFile(coffeePath.native() + "/source/test/xml/bad_vertex.xml");
   xml::Document doc;
   BOOST_REQUIRE_THROW(doc.parse(xmlFile, dtd), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dtd_novalidate_memory)
{
   boost::filesystem::path coffeePath(boost::filesystem::current_path());

   boost::filesystem::path dtdFile(coffeePath.native() + "/source/test/xml/vertex.dtd");
   xml::DTD dtd;
   BOOST_REQUIRE_NO_THROW(dtd.initialize(dtdFile));

   const basis::DataBlock xml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><Intersect><First><Vertex X= \"1\" Y=\"3\"/></First></Intersect>");
   xml::Document doc;
   BOOST_REQUIRE_THROW(doc.parse(xml, dtd), basis::RuntimeException);
}


