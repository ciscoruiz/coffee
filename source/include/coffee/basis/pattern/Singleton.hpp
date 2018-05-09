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

#ifndef _coffee_basis_pattern_Singleton_h
#define _coffee_basis_pattern_Singleton_h

#include <mutex>

namespace coffee {

namespace basis {

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


