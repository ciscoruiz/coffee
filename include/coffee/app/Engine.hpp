// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef _nemesis_app_EngineIf_h
#define _nemesis_app_EngineIf_h

#include <vector>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/StreamString.hpp>

#include <coffee/app/Runnable.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace app {

class Application;

/**
   Clase de la que heredan los enginees de aplicacion.

   Solo deberia haber una instancia de cada uno de los enginees, pero no podemos
   declararlos como heredados de nemesis::Singleton porque debemos dar la posiblidad de
   que el programador re-escriba su comportamiento mediante herencia.

   Todos los enginees se arrancan y paran automaticamente desde la aplicacion.

   El siguiente ejemplo muestra como obtener la instancia de un enginee asociado a nuestra aplicacion:

   \code
      Clase* objeto = nemesis::app::functions::engine <Clase>(FILE_LOCATION);

      ..... uso del objeto ....
   \endcode

   Si el enginee \em 'Clase' no hubiera sido registrado(instanciado) en nuestra aplicacion el metodo
   template nemesis::engine lanzara una excepcion.

   \author cisco.tierra@gmail.com.
*/
class Engine : public Runnable  {
public:
   /**
      Destructor.
   */
   virtual ~Engine() {;}

   /**
    * Devuelve la aplicaci�n asociado a este enginee.
    * \return la aplicaci�n asociado a este enginee.
    * \since NemesisRD.app 2.0.0
    */
   Application& getApplication() noexcept { return a_app; }

   /**
      Devuelve el nombre de esta clase indicado en el constructor.
      \return El nombre de la clase indicado en el constructor.
   */
   const char* getClassName() const noexcept { return getName(); }

   operator adt::StreamString() const noexcept { return asString(); }

   /**
      Devuelve una cadena con la informacion mas relevante de esta instancia.
      \return Una cadena con la informacion mas relevante de esta instancia.
   */
   virtual adt::StreamString asString() const noexcept;

   /**
      Devuelve un documento XML con la informacion mas relevante de esta instancia.
      \param parent Nodo XML del que colgar la informacion referente a esta instancia.
      \return Un documento XML con la informacion mas relevante de esta instancia.
   */
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

protected:
   /**
      Contructor.
      \param app Aplicaci�n a la que pertence este enginee.
      @param className Nombre lgico asociado a esta clase.
   */
   Engine(Application& app, const char* className) :
      Runnable(className),
      a_app(app)
   {
   }

   /**
      Indica que el nombre de un enginee que debe ser initializa antes que este.
      \param engineName Nombre de enginee requerido por esta instancia.
      \warning Solo tendra efecto antes de inicializar el enginee.
   */
   void addPredecessor(const char* engineName) noexcept;

   /**
      metodo que debemos implementar si la clase heredada necesita algn tipo de inicializacin.
      Este metodo se invocara automaticamente desde nemesis::Application::start.
   */
   void initialize() throw(adt::RuntimeException);

   /**
      metodo que debemos implementar en la clase heredada para implementar el proceso de parada de
      esta clase.

      Este metodo debe implementar un metodo de parada controlada. Se invocara automaticamente
      desde el ncleo de NemesisRD.
   */
   void stop() throw(adt::RuntimeException) { statusStopped(); do_stop(); }

private:
   typedef std::vector <std::string>::iterator iterator;

   Application& a_app;
   std::vector <std::string> a_predecessors;

   Engine(const Engine& other);
   iterator begin() noexcept { return a_predecessors.begin(); }
   iterator end() noexcept { return a_predecessors.end(); }
   const std::string& data(iterator ii) noexcept { return *ii; }

   /**
      metodo que debemos implementar si la clase heredada necesita algn tipo de inicializacin.
      Este metodo se invocara automaticamente desde nemesis::Application::start.
   */
   virtual void do_initialize() throw(adt::RuntimeException) = 0;

   /**
      Metodo que debemos implementar en la clase heredada para tratar el proceso de parada de
      esta clase.
      Este metodo debe implementar un metodo de parada controlada. Se invocara automaticamente
      desde el ncleo de NemesisRD.

      La mayor�a de las veces ser� la re-implementaci�n del m�todo nemesis::Runnable::do_requestStop
      el que realice las acciones necesarias para parar un enginee.
   */
   virtual void do_stop() throw(adt::RuntimeException) = 0;

   friend class Application;
};


}
}

#endif
