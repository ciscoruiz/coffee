
#ifndef _coffee_adt_AsHexString_h
#define _coffee_adt_AAsHexString_h

#include <string>

#include <coffee/config/defines.hpp>

namespace coffee {

namespace adt {

/**
 * @brief The AsString class. This class convert different data types into std::string.
 */
class AsHexString {
public:
   /**
      @return A string with the number.
   */
   static std::string apply (const int number) noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const unsigned int number) noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const Integer64 number) noexcept;

   /**
      @return A string with the number.
   */
   static std::string apply (const Unsigned64 number) noexcept;

private:
   AsHexString ();
   AsHexString (const AsHexString&);
};

}
}

#endif
