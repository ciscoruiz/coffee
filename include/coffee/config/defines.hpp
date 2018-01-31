// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
#ifndef _coffee_defines_h
#define _coffee_defines_h

#include <sys/types.h>

#ifdef COFFEE_FILE_LOCATION
#undef COFFEE_FILE_LOCATION
#endif
#define COFFEE_FILE_LOCATION __PRETTY_FUNCTION__,__FILE__,__LINE__

#define COFFEE_VERSION 0x011202

#if defined(__linux__) && !defined (_NO_BUILTIN)
#define coffee_memcpy(a,b,c) __builtin_memcpy((a),(b),(c))
#define coffee_memset(a,b,c) __builtin_memset((a),(b),(c))
#define coffee_memmove(a,b,c) __builtin_memmove((a),(b),(c))
#define coffee_strcpy(a,b) __builtin_strcpy((a),(b))
#define coffee_strncpy(a,b,c) __builtin_strncpy((a),(b),(c))
#define coffee_strcmp(a,b) __builtin_strcmp((a),(b))
#define coffee_strncmp(a,b,n) __builtin_strncmp((a),(b),(n))
#define coffee_strcat(a,b) __builtin_strcat((a),(b))
#define coffee_strlen(a) __builtin_strlen((a))
#define coffee_strstr(a,b) __builtin_strstr((a),(b))
#define coffee_strchr(a,b) __builtin_strchr((a),(b))
#else
#define coffee_memcpy(a,b,c) memcpy((a),(b),(c))
#define coffee_memset(a,b,c) memset((a),(b),(c))
#define coffee_memmove(a,b,c) memmove((a),(b),(c))
#define coffee_strcpy(a,b) strcpy((a),(b))
#define coffee_strncpy(a,b,c) strncpy((a),(b),(c))
#define coffee_strcmp(a,b) strcmp((a),(b))
#define coffee_strncmp(a,b,n) strncmp((a),(b),(n))
#define coffee_strcat(a,b) strcat((a),(b))
#define coffee_strlen(a) strlen((a))
#define coffee_strstr(a,b) strstr((a),(b))
#define coffee_strchr(a,b) strchr((a),(b))
#endif

#define coffee_signal_shield(r,a) { register int cx (0); do { if ((r = (a)) < 0) cx ++; } while (r < 0 && errno == EINTR && cx < 5); }

namespace coffee {
   #if !defined(__x86_64__) && !defined(__x86_64)
      #undef  __coffee64__
      typedef int64_t Integer64;
      typedef u_int64_t Unsigned64;

      /**
       * Defines required data type to conversion from pointer to integer 
       */
      typedef int ptrnumber;
   #else
      #define __coffee64__
      typedef long Integer64;
      typedef unsigned long Unsigned64;

      /**
       * Defines required data type to conversion from pointer to integer 
       */
      typedef long ptrnumber;
   #endif
}

/**
 * Make conversion from pointer to integer, it will work both 64 bits and 32 bits architecture.
 */
#define coffee_ptrnumber_cast(pointer) (coffee::ptrnumber)((void*)(pointer))

#endif

