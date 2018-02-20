
#include <boost/test/unit_test.hpp>

#include <map>
#include <functional>
#include <mutex>
#include <thread>

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/persistence/Repository.hpp>
#include <coffee/persistence/Storage.hpp>
#include <coffee/persistence/Class.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;

BOOST_AUTO_TEST_CASE (persistence_repository_repeat)
{
   persistence::Repository repository ("persistence_define_structure");

   auto ii = repository.createStorage("the 0", 128);

   std::shared_ptr<persistence::Storage>& find = repository.findStorage("the 0");

   BOOST_REQUIRE_EQUAL (ii.get(), find.get());

   BOOST_REQUIRE_THROW(repository.createStorage("the 0", 16), adt::RuntimeException);

}

BOOST_AUTO_TEST_CASE (persistence_repository_as)
{
   persistence::Repository repository ("persistence_define_structure");

   repository.createStorage("the 0", 16);
   repository.createStorage("the 1", persistence::Storage::DefaultMaxCacheSize);

   adt::StreamString zz = repository.asString ();

   BOOST_REQUIRE_EQUAL (zz, "persistence.Repository { adt.NamedObject { Name: persistence_define_structure } | N-Size=2 }");

   xml::Node myNode ("root");

   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("root");

   auto info = repository.asXML(root);

   BOOST_REQUIRE_EQUAL (info->children_size(), 2);
}
