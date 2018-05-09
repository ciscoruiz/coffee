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
#ifndef TEST_DBMS_PRINTCHRONO_HPP_
#define TEST_DBMS_PRINTCHRONO_HPP_

#include <chrono>
#include <coffee/basis/AsString.hpp>

// See https://stackoverflow.com/questions/17572583/boost-check-fails-to-compile-operator-for-custom-types
namespace boost {
namespace test_tools {
   template<> struct print_log_value<std::chrono::seconds> {
      void operator()( std::ostream& os, std::chrono::seconds const& ts)
      {
          os << coffee::basis::AsString::apply(ts);
      }
   };
}}

#endif /* TEST_DBMS_PRINTCHRONO_HPP_ */
