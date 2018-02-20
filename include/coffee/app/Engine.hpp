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

#ifndef _coffee_app_Engine_h
#define _coffee_app_Engine_h

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
   declararlos como heredados de coffee::Singleton porque debemos dar la posiblidad de
   que el programador re-escriba su comportamiento mediante herencia.

   Todos los enginees se arrancan y paran automaticamente desde la aplicacion.

   El siguiente ejemplo muestra como obtener la instancia de un enginee asociado a nuestra aplicacion:

   \code
      Clase* objeto = coffee::app::functions::engine <Clase>(FILE_LOCATION);

      ..... uso del objeto ....
   \endcode

   Si el enginee \em 'Clase' no hubiera sido registrado(instanciado) en nuestra aplicacion el metodo
   template coffee::engine lanzara una excepcion.

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
   const std::string& getClassName() const noexcept { return getName(); }

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
      Este metodo se invocara automaticamente desde coffee::Application::start.
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
      Este metodo se invocara automaticamente desde coffee::Application::start.
   */
   virtual void do_initialize() throw(adt::RuntimeException) = 0;

   /**
      Metodo que debemos implementar en la clase heredada para tratar el proceso de parada de
      esta clase.
      Este metodo debe implementar un metodo de parada controlada. Se invocara automaticamente
      desde el ncleo de NemesisRD.

      La mayor�a de las veces ser� la re-implementaci�n del m�todo coffee::Runnable::do_requestStop
      el que realice las acciones necesarias para parar un enginee.
   */
   virtual void do_stop() throw(adt::RuntimeException) = 0;

   friend class Application;
};


}
}

#endif
