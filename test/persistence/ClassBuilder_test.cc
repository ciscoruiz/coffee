#include <boost/test/unit_test.hpp>

#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>
#include <coffee/persistence/PrimaryKey.hpp>

#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;

BOOST_AUTO_TEST_CASE(persistence_classbuilder_twice_pks)
{
   std::shared_ptr<persistence::PrimaryKey> primaryKey;

   {
      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id"));
      primaryKey = pkBuilder.build();
   }

   persistence::ClassBuilder classBuilder("customer");
   BOOST_REQUIRE_NO_THROW(classBuilder.set(primaryKey));

   std::shared_ptr<persistence::PrimaryKey> badPrimaryKey;
   {
      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::Integer>("some-other-stuff"));
      badPrimaryKey = pkBuilder.build();
   }

   BOOST_REQUIRE_THROW(classBuilder.set(badPrimaryKey), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(persistence_classbuilder_without_pk)
{
   persistence::ClassBuilder classBuilder("customer");

   classBuilder.add(std::make_shared<dbms::datatype::Integer>("id")).add(std::make_shared<dbms::datatype::Integer>("age"));
   BOOST_REQUIRE_THROW(classBuilder.build(), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(persistence_classbuilder_without_fields)
{
   persistence::ClassBuilder classBuilder("customer");

   persistence::PrimaryKeyBuilder pkBuilder;
   pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id"));
   auto primaryKey = pkBuilder.build();

   BOOST_REQUIRE_NO_THROW(classBuilder.set(primaryKey));
   BOOST_REQUIRE_THROW(classBuilder.build(), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(persistence_classbuilder_dual_member)
{
   static const char* NameInPkAndMember = "id";
   persistence::ClassBuilder classBuilder("customer");

   persistence::PrimaryKeyBuilder pkBuilder;
   pkBuilder.add(std::make_shared<dbms::datatype::Integer>(NameInPkAndMember));
   auto primaryKey = pkBuilder.build();

   BOOST_REQUIRE_NO_THROW(classBuilder.set(primaryKey));
   classBuilder.add(std::make_shared<dbms::datatype::Integer>("size")).add(std::make_shared<dbms::datatype::Integer>(NameInPkAndMember));

   BOOST_REQUIRE_THROW(classBuilder.build(), adt::RuntimeException);
}
