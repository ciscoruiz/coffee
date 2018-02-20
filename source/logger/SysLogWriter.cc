#include <syslog.h>

#include <coffee/logger/SysLogWriter.hpp>

using namespace coffee;

logger::SysLogWriter::SysLogWriter (const std::string& ident, const OptionBuilder& builder) :
   logger::Writer ("SysLogWriter"),
   m_ident(ident),
   m_options(builder.build())
{
}

logger::SysLogWriter::~SysLogWriter ()
{
   closelog();
}

void logger::SysLogWriter::do_initialize ()
   throw (adt::RuntimeException)
{
   openlog(m_ident.c_str(), LOG_USER, m_options);
}

void logger::SysLogWriter::apply (const Level::_v level, const std::string& line)
   noexcept
{
   syslog(level, "%s", line.c_str());
}

logger::SysLogWriter::OptionBuilder& logger::SysLogWriter::OptionBuilder::showPid() noexcept
{
	flags += LOG_PID;
	return *this;
}

logger::SysLogWriter::OptionBuilder& logger::SysLogWriter::OptionBuilder::consoleWhenError() noexcept
{
	flags += LOG_CONS;
	return *this;
}


logger::SysLogWriter::OptionBuilder& logger::SysLogWriter::OptionBuilder::delayOpen() noexcept
{
	flags += LOG_ODELAY;
	return *this;
}

logger::SysLogWriter::OptionBuilder& logger::SysLogWriter::OptionBuilder::noDelayOpen() noexcept
{
	flags += LOG_NDELAY;
	return *this;
}

logger::SysLogWriter::OptionBuilder& logger::SysLogWriter::OptionBuilder::logToStdErr() noexcept
{
   flags += LOG_PERROR;
   return *this;
}
