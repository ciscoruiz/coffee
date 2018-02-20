#include <boost/test/unit_test.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/Writer.hpp>
#include <coffee/logger/DefaultFormatter.hpp>

#include <iostream>

using namespace coffee;

class TtyWriter : public logger::Writer {
public:
   TtyWriter () : logger::Writer ("TtyWriter"), m_counter (0) {;}

   unsigned getCounter () const throw () { return m_counter; }

private:
   unsigned m_counter;

   void apply (const logger::Level::_v level, const std::string& line) throw () {
      std::cout << line << std::endl;
      ++ m_counter;
   }
   void do_initialize () throw (adt::RuntimeException) {;}
};


BOOST_AUTO_TEST_CASE( DefaultFormatter_test )
{
   auto writer = std::make_shared<TtyWriter>();
   logger::Logger::initialize(writer);

   logger::Logger::setLevel (logger::Level::Local2);

   adt::StreamString msg;
   for (int ii = logger::Level::Emergency; ii <= logger::Level::Local7; ++ ii) {
      msg.clear ();
      logger::Logger::write((logger::Level::_v)ii, msg << " numeric value=" << ii, COFFEE_FILE_LOCATION);
   }

   BOOST_REQUIRE_EQUAL(writer->getCounter (), logger::Level::Local2 + 1);
}

