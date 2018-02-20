#include <memory>

#include "MockDatabase.hpp"

using namespace coffee;

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

mock::MockDatabase::MockDatabase(app::Application& app) :
   dbms::Database(app, "map", app.getTitle().c_str())
{
   std::shared_ptr<dbms::ErrorCodeInterpreter> eci = std::make_shared<MockErrorCodeInterpreter>();
   setErrorCodeInterpreter(eci);
}

mock::MockDatabase::MockDatabase (const char* name) :
   dbms::Database("map", name)
{
   logger::Logger::initialize(std::make_shared<logger::TtyWriter>());

   std::shared_ptr<dbms::ErrorCodeInterpreter> eci = std::make_shared<MockErrorCodeInterpreter>();
   setErrorCodeInterpreter(eci);
}

