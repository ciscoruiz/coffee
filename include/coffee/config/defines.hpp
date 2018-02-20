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

#ifndef _coffee_defines_h
#define _coffee_defines_h

#include <sys/types.h>

#ifdef COFFEE_FILE_LOCATION
#undef COFFEE_FILE_LOCATION
#endif
#define COFFEE_FILE_LOCATION __PRETTY_FUNCTION__,__FILE__,__LINE__

#define COFFEE_VERSION 0x120214

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

