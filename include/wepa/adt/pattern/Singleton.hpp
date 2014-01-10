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
#ifndef _wepa_adt_pattern_Singleton_h
#define _wepa_adt_pattern_Singleton_h

#include <mutex>

namespace wepa {

namespace adt {

namespace pattern {

template <class _T> class Singleton {
public:
   /**
      @return La instancia de la clase indicada en la creacin de la template.
   */
   static _T& getInstance () noexcept { return std::ref (*pointer ()); }

   /**
      @return La instancia de la clase indicada en la creacin de la template.
   */
   static const _T* getConstPointer () noexcept { return pointer (); }

private:
   /**
      @return El puntero de la clase indicada en la creacin de la template.
      \warning Este puntero nunca se debe liberar por parte del usuario.
   */
   static _T* pointer () noexcept {
      static std::mutex mutex;
      static _T* result (NULL);

      if (result == NULL) {
         std::lock_guard<std::mutex> lock (mutex);
         if (result == NULL)
            result = new _T ();
      }

      return result;
   }
};

}
}
}

#endif


