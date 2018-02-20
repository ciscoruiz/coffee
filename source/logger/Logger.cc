#include <coffee/logger/Logger.hpp>

#include <coffee/logger/Writer.hpp>
#include <coffee/logger/DefaultFormatter.hpp>
#include <coffee/logger/Level.hpp>
#include <coffee/logger/SCCS.hpp>

using namespace coffee;

#ifdef _DEBUG
   logger::Level::_v logger::Logger::m_level = Level::Debug;
#else
   logger::Level::_v logger::Logger::m_level = Level::Warning;
#endif

std::shared_ptr<logger::Formatter> logger::Logger::m_formatter;
logger::Logger::Writers logger::Logger::m_writers;

//static
void logger::Logger::initialize(std::shared_ptr<Writer> writer, std::shared_ptr<Formatter> formatter)
   throw(adt::RuntimeException)
{
   SCCS::activate();

   m_writers.clear();
   m_writers.push_back(writer);
   m_formatter = formatter;

   writer->initialize();
}

//static
void logger::Logger::initialize(std::shared_ptr<Writer> writer)
   throw(adt::RuntimeException)
{
   initialize(writer, std::make_shared<DefaultFormatter>());
}

//static
void logger::Logger::add(std::shared_ptr<Writer> writer)
   throw (adt::RuntimeException)
{
   if (!m_formatter) {
      COFFEE_THROW_EXCEPTION("You should initialize the Logger before add a new writer");
   }

   writer->initialize();
   m_writers.push_back(writer);
}

//static
void logger::Logger::write(const Level::_v level, const adt::StreamString& input, const char* function, const char* file, const unsigned lineno)
   noexcept
{
   if (!wantsToProcess(level))
      return;

   Formatter::Elements elements(level, input, function, file, lineno);
   const std::string string = m_formatter->apply(elements);

   for (auto writer : m_writers) {
      if (writer->wantsToProcess(level)) {
         writer->apply(level, string);
      }
   }
}

//static
bool logger::Logger::wantsToProcess(const Level::_v level)
   noexcept
{
   if(m_writers.empty() || !m_formatter)
      return false;

   bool result = false;
   for (auto writer : m_writers) {
      if (writer->wantsToProcess(level)) {
         result = true;
         break;
      }
   }

   if (result == false)
      return false;

   return isActive(level) ? true: result;
}



