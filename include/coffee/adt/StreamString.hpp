#ifndef _coffee_adt_StreamString_H
#define _coffee_adt_StreamString_H

#include <string>

#include <stdio.h>

#include <coffee/config/defines.hpp>

namespace coffee  {

namespace adt {

/**
 *  
 *
 */
class StreamString : public std::string {
public:
   /**
    * Flags to tuning behaviour 
    */
   struct Flag { enum _v { None, ShowNull }; };

   /**
    * Constructor.
    * \param flag Set how to manage the null value.
    */
   explicit StreamString (const Flag::_v flag = Flag::None) : a_flags (flag) {;}

   /**
    * Constructor.
    * \param str Buffer to initialize content of the instance. It can be NULL.
    * \param flag Set how to manage the null value.
    */
   StreamString (const char* str, const Flag::_v flag = Flag::None) :
      std::string ((str == NULL) ? ((flag & Flag::ShowNull) ? "<null>": ""): str),
      a_flags (flag)
   {;}

   /**
    * Copy constructor
    * \param other Other instance to duplicate.
    */
   StreamString (const StreamString &other) : std::string (other), a_flags (other.a_flags) {;}

   /**
    * Constructor copia.
    * \param other Buffer to initialize content of the instance. It can be NULL.
    * \param flag Set how to manage the null value.
    */
   StreamString (const std::string& other, const Flag::_v flag = Flag::None) : std::string (other), a_flags (flag) {;}

   /**
    * Convert to upper case.
    */
   adt::StreamString& toUpper () noexcept;

   /**
    * Convert to lower case.
    */
   adt::StreamString& toLower () noexcept;

   StreamString& operator = (const char vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const char* vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const int vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const unsigned int vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const bool vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const Integer64 vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const Unsigned64 vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const float vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const double vv) noexcept { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const std::string& vv) noexcept { std::string::operator= (vv); return *this; }

   StreamString& operator << (const char vv) noexcept;
   StreamString& operator << (const char* vv) noexcept;
   StreamString& operator << (const int vv) noexcept;
   StreamString& operator << (const unsigned int vv) noexcept;
   StreamString& operator << (const bool vv) noexcept;
   StreamString& operator << (const Integer64 vv) noexcept;
   StreamString& operator << (const Unsigned64 vv) noexcept;
   StreamString& operator << (const float vv) noexcept;
   StreamString& operator << (const double vv) noexcept;
   StreamString& operator << (const std::string& vv) noexcept { std::string::append (vv); return *this; }

private:
   Flag::_v a_flags;
};

}
}

#endif
