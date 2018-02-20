#ifndef _coffee_adt_DataBlock_h
#define _coffee_adt_DataBlock_h

#include <string>
#include <string.h>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/config/defines.hpp>

namespace coffee {

namespace adt {

/**
   Class to facilitate access to data block of memory.
   @author frr@tid.es cisco.tierra@gmail.com.
*/
class DataBlock : std::string {
public:
   using std::string::append;
   using std::string::assign;
   using std::string::begin;
   using std::string::clear;
   using std::string::data;
   using std::string::empty;
   using std::string::end;
   using std::string::erase;
   using std::string::insert;
   using std::string::operator +=;
   using std::string::operator =;
   using std::string::size;
   using std::string::reserve;

   /**
    * Empty constructor.
    */
   DataBlock () {;}

   /**
     Constructor.

     @param buffer Address to the memory buffer.
     @param size Copy the first size bytes from the buffer pointed by \em buffer
   */
   DataBlock (const char* buffer, const size_t size) noexcept : std::string (buffer, size) {;}

   /**
     Copy Constructor.
     @param other Datablock to copy.
   */
   DataBlock (const DataBlock& other) noexcept : std::string (other.data(), other.size()) {;}

   /**
      Destructor.
   */
   virtual ~DataBlock () {;}

   /**
    * @brief append the received character
    * @param character Character to append to the buffer
    * @return The reference of itself.
    */
   DataBlock& append (const char character) noexcept { std::string::append (1, character); return *this;}

   char at (const size_t index) const throw (RuntimeException);

   char& at (const size_t index) throw (RuntimeException);

   char operator[] (const size_type index) const throw (RuntimeException) { return at (index); }

   char& operator[] (const size_type index) throw (RuntimeException) { return at (index); }

   bool operator==(const DataBlock& other) const noexcept {
      return size() == other.size() ? (memcmp(data(), other.data(), size()) == 0): false;
   }

   DataBlock& operator=(const DataBlock& other) noexcept {
      this->assign(other);
      return *this;
   }
};

}
}

#endif

