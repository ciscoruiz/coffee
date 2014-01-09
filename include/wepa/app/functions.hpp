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
#ifndef _wepa_app_functions_hpp_
#define _wepa_app_functions_hpp_

#include <wepa/adt/RuntimeException.hpp>

#include <wepa/app/Application.hpp>

namespace wepa {

namespace app {

/**
   functions - Meodos pertenecientes al paquete app
   @author cisco.tierra@gmail.com.
*/
struct functions {
   /**
    * Devuelve la referencia de la instancia de nuestra aplicacion
    * @return la referencia de la instancia de nuestra aplicacion.
    */
   template <typename T> static T& getApplication () throw (adt::RuntimeException) {
      if (st_application == NULL)
         WEPA_THROW_EXCEPTION("There is not any registered app::Application");

      T* result = dynamic_cast <T*> (st_application);

      if (result == NULL)
         WEPA_THROW_EXCEPTION("Invalid convertion for the registered app::Application");

      return *result;
   }

   /**
    * Devuelve la referencia de la instancia de nuestra aplicacion
    * @return la referencia de la instancia de nuestra aplicacion.
    */
   static Application& getApp () throw (adt::RuntimeException);

   /**
      Patron para obtener facilmente la instancia de un determinado enginee.
      Estos dos parametros suelen ser sustituidos por la macro C <b>FILE_LOCATION</b>.

      \param fromFile Fichero desde el que se invoca a este metodo
      \param fromLine Numero de linea desde el que se invoca a este metodo.

      \return La instancia del enginee de la clase recibida como parametro.
      @author frr@tid.es cisco.tierra@gmail.com.
      \warning La clase T de implementar un metodo de la forma:
      \code
          static const char* getClassName () throw ();
      \endcode
      \see AbstractEngine
   */
   template <typename T> static T* engine (const char* fromFile, const int fromLine)
      throw (adt::RuntimeException)
   {
      return engine<T> (T::getClassName (), fromFile, fromLine);
   }

   /**
      Patron para obtener facilmente la instancia de un determinado enginee.
      Estos dos parametros suelen ser sustituidos por la macro C <b>FILE_LOCATION</b>.
      \param className Nombre del enginee buscado.
      \param fromFile Fichero desde el que se invoca a este metodo
      \param fromLine Numero de linea desde el que se invoca a este metodo.

      \return La instancia del enginee de la clase recibida como parametro.
      @author frr@tid.es cisco.tierra@gmail.com.
      \see AbstractEngine
   */
   template <typename T> static T* engine (const char* className, const char* fromFile, const int fromLine)
      throw (adt::RuntimeException)
   {
      T* result = static_cast <T*> (getApp ().engine_find (className));

      if (result == NULL) {
         WEPA_THROW_EXCEPTION(className << " | AbstractEngine was not registered");
      }

      return result;
   }

private:
   static Application* st_application;

   static void setApplication (Application* application) throw () { st_application = application; }

   friend class Application;
};

}
}

#endif


