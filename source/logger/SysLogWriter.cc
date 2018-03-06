// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

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

void logger::SysLogWriter::initialize ()
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
