
#ifndef _coffee_adt_ASSTRING_H
#define _coffee_adt_ASSTRING_H

#include <string>

#include <coffee/config/defines.hpp>

namespace coffee {

namespace adt {

class DataBlock;

/**
 * @brief The AsString class. This class convert different data types into std::string.
 */
class AsString {
public:
   /**
      @return A string with the number.
   */
   static std::string apply (const int number, const char* format = "%d") noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const unsigned int number) noexcept;

#ifndef __coffee64__
   /**
      @return A string with the number.
   */
   static std::string apply (const long number) noexcept;
#endif

   /**
      @return A string with the number.
   */
   static std::string apply (const Integer64 number) noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const Unsigned64 number) noexcept;

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
   static std::string apply (const DataBlock& dataBlock, const int characterByLine = 16) noexcept;
};

}
}

#endif // ASSTRING_H
