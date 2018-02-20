#include "MockApplication.hpp"

#include <iostream>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/TtyWriter.hpp>

using namespace coffee;

mock::MockApplication::MockApplication (const char* title) : app::Application ("MockApp", title, "0.0")
{
   logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
}

void mock::MockApplication::operator ()()
{
   try {
      start ();
   }
   catch (adt::Exception& ex) {
      logger::Logger::write(ex);
   }
}

void mock::MockApplication::run ()
   throw (adt::RuntimeException)
{
   LOG_THIS_METHOD();

   LOG_DEBUG (asString() << "Waiting for enabled termination");
   m_termination.lock();
   LOG_DEBUG (asString() << " will terminate now");

   // It will be block until something frees the mutex by calling enableTermination
}
