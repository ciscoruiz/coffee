// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef _wepa_adt_DataBlock_h
#define _wepa_adt_DataBlock_h

#include <string>

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/config/defines.hpp>

namespace wepa {

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
   DataBlock (const char* buffer, const size_t size) throw () : std::string (buffer, size) {;}

   /**
     Copy Constructor.
     @param other Datablock to copy.
   */
   DataBlock (const DataBlock& other) throw () : std::string (other) {;}

   /**
      Destructor.
   */
   virtual ~DataBlock () {;}

   /**
    * @brief append the received character
    * @param character Character to append to the buffer
    * @return The reference of itself.
    */
   DataBlock& append (const char character) throw () { std::string::append (1, character); return *this;}

   char at (const size_t index) const throw (RuntimeException);

   char& at (const size_t index) throw (RuntimeException);

   char operator[] (const size_type index) const throw (RuntimeException) { return at (index); }

   char& operator[] (const size_type index) throw (RuntimeException) { return at (index); }

//
// You can execute "g++ -E -dM -std=c++0x -x c++ /dev/null" to see list of macros 
// See: http://stackoverflow.com/questions/2958398/gnu-c-how-to-check-when-std-c0x-is-in-effect
   #ifdef __GXX_EXPERIMENTAL_CXX0X__
      DataBlock& operator=(const DataBlock&) = default;
   #endif
};

}
}

#endif

