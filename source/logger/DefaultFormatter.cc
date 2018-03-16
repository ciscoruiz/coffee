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
#include <chrono>

#include <coffee/logger/DefaultFormatter.hpp>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/AsString.hpp>
#include <coffee/adt/AsHexString.hpp>

using namespace coffee;
using namespace coffee::logger;

std::string DefaultFormatter::apply (const Level::_v level, const adt::StreamString& comment, const char* methodName, const char* file, const unsigned lineno)
   noexcept
{
   // See https://stackoverflow.com/questions/9089842/c-chrono-system-time-in-milliseconds-time-operations
   auto second = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());

   adt::StreamString output;

   output << "[" << adt::AsString::apply(second, "%d/%0m/%Y %T") << "] ";
   output << "[thr=" << adt::AsHexString::apply((int64_t) pthread_self()) << "] ";
   output << Level::enumName(level) << " | ";
   output << methodName << " [" << file << "(" << lineno << ")]: ";
   output << comment;

   return output;
}
