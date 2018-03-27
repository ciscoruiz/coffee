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


#ifndef _coffee_basis_ASSTRING_H
#define _coffee_basis_ASSTRING_H

#include <string>

#include <coffee/config/defines.hpp>


#include <chrono>

namespace coffee {

namespace basis {

class DataBlock;

/**
 * @brief The AsString class. This class convert different data types into std::string.
 */
class AsString {
public:
   static const int DefaultCharactersByLine;

   /**
      @return A string with the number.
   */
   static std::string apply (const int number, const char* format = "%d") noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const unsigned int number) noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const int64_t number) noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const uint64_t number) noexcept;

   /**
      @return A string with the number.
   */
   static const char* apply (const bool _bool) noexcept { return (_bool == true) ? "true": "false"; }

   /**
      @return A string with the number.
   */
   static std::string apply (const double v, const char* format="%e") noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const float v, const char* format="%f") noexcept;

   /**
    * \return A string with a brief description of the data block.
    */
   static std::string apply (const DataBlock& dataBlock, const int characterByLine = DefaultCharactersByLine) noexcept;

   /**
    * \return A string with the seconds
    */
   static std::string apply(const std::chrono::seconds& value) noexcept;

   /**
    * \return A string with the seconds
    */
   static std::string apply(const std::chrono::seconds& value, const char* format) noexcept;

   /**
    * \return A string with the milliseconds
    */
   static std::string apply(const std::chrono::milliseconds& value) noexcept;

   /**
    * \return A string with the microseconds
    */
   static std::string apply(const std::chrono::microseconds& value) noexcept;
};

}
}

#endif // ASSTRING_H
