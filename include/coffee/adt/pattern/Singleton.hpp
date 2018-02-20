#ifndef _coffee_adt_pattern_Singleton_h
#define _coffee_adt_pattern_Singleton_h

#include <mutex>

namespace coffee {

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


