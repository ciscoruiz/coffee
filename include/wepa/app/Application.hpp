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

#ifndef _wepa_app_Application_hpp_
#define _wepa_app_Application_hpp_

#include <vector>

#include <wepa/adt/RuntimeException.hpp>

#include <wepa/app/Runnable.hpp>

namespace wepa {

namespace xml {
class Node;
}

namespace app {

class EngineIf;
struct functions;

/**
   Abstraccion de la aplicacion.

   Mantiene la informacion de todos los recursos usados (version, titulo, linea de comandos,
   threads, etc ...) por nuestras aplicaciones.

   Slo puede haber una nica instancia de la aplicacio, que seria accesible mediante el
   metodo nemesis::functions::getApp.

   @author cisco.tierra@gmail.com.
*/
class Application : public Runnable {
public:
   typedef std::vector <EngineIf*>::iterator engine_iterator;
   typedef std::vector <EngineIf*>::const_iterator const_engine_iterator;

   /**
      Constructor.

      @param shortName Nombre logico de esta instancia.
      @param title Titulo de la aplicacion que aparecera al arrancar.
      @param version version de este programa. Aconsejamos el forma X.YRZZ. Donde X es la
      version principal, Y la version secundaria y ZZ es el numero de entrega realizada.
      \param date Fecha de realizacion del enginee. Normalmente sera el contenido de la macro __DATE__.
      \param time Hora de realizacion del enginee. Normalmente sera el contenido de la macro __TIME__.
   */
   Application (const char* shortName, const char* title, const char* version, const char* date = NULL, const char* time = NULL);

   /**
      Destructor.
   */
   virtual ~Application () { a_engines.clear (); }

   // Accesores
   /**
      Devuelve el nombre corto indicado en el constructor.
      \return El nombre corto indicado en el constructor.
   */
   const char* getShortName () const throw () { return getName (); }

   /**
      Devuelve la version indicado en el contructor de esta aplicacion.
      \return La version indicado en el contructor de esta aplicacion.
   */
   const std::string& getVersion () const throw () { return a_version; }

   /**
      Devuelve el titulo indicado en el contructor de esta aplicacion.
      \return el titulo indicado en el contructor de esta aplicacion.
   */
   const std::string& getTitle () const throw () { return a_title; }

   /**
      Devuelve el pid asignado por el sistema operativo a la aplicacion que estamos ejecutando.
      @return El pid asignado por el sistema operativo a la aplicacion que estamos ejecutando.
   */
   pid_t getPid () const throw () { return a_pid; }

   /**
      Activa la salida por pantalla del mensaje referente a la licencia GPL 3.0.
      \warning Para que tenga efecto debe ser invocado antes de ejecutar el metodo Application::start.
   */
   void activateGeneralPublicLicense () throw () { a_enableGPL = true; }

   /**
      Devuelve la instancia del enginee que corresponde con el nombre recibido.
      \return La instancia del enginee que corresponde con el nombre recibido. Puede ser NULL si no
      hay ningun enginee asociado a la �ta aplicacion que corresponda con el nombre.
   */
   EngineIf* engine_find (const char* className) throw ();

   /**
      Inicializa los elementos propios de nuestra aplicacio, invoca al metodo #initialize
      e invoca al metodo #run.
   */
   void start () throw (adt::RuntimeException);

   /**
    * Este m�todo se invocar� cuando alguna capa superior a �sta detecte un problema tan grave
    * como para parar la aplicaci�n de forma s�bita.
    * \param className Nombre de la clase que genera el evento.
    *
    * \warning En el momento de invocar a este m�todo la aplicaci�n puede estar en un estado
    * no muy estable por lo que se deber�an minizar las operaciones a realizar para evitar
    * bloquear la aplicaci�n.
    */
   virtual void eventAbnormalTermination (const char* className) throw () { ; }

   /**
      Devuelve un iterador al primer enginee definido en la aplicacion.
      \return un iterador al primer enginee definido en la aplicacion.
   */
   engine_iterator engine_begin () throw () { return a_engines.begin (); }

   /**
      Devuelve un iterador al ultimo enginee definido en la aplicacion.
      \return un iterador al ultimo enginee definido en la aplicacion.
   */
   const_engine_iterator engine_end () const throw () { return a_engines.end (); }

   /**
      Devuelve el objeto sobre el que esta posicionado el iterator recibido como parametro.
      \param ii Iterator que deberia estar comprendido entre #begin y #end.
      \return El objeto sobre el que esta posicionado el iterator recibido como parametro.
   */
   static EngineIf* engine (engine_iterator ii) throw () { return *ii; }

   /**
      Devuelve un iterador al primer enginee definido en la aplicacion.
      \return un iterador al primer enginee definido en la aplicacion.
   */
   const_engine_iterator engine_begin () const throw () { return a_engines.begin (); }

   /**
      Devuelve un iterador al ultimo enginee definido en la aplicacion.
      \return un iterador al ultimo enginee definido en la aplicacion.
   */
   engine_iterator engine_end () throw () { return a_engines.end (); }

   /**
      Devuelve el objeto sobre el que esta posicionado el iterator recibido como parametro.
      \param ii Iterator que deberia estar comprendido entre #begin y #end.
      \return El objeto sobre el que esta posicionado el iterator recibido como parametro.
   */
   static const EngineIf* engine (const_engine_iterator ii) throw () { return *ii; }

   /**
      Vuelva un documento XML con el contexto de la aplicacion.
      \param file Ruta completa del fichero en el que grabar el contexto de la aplicacion.
   */
   void writeContext (const std::string& file) throw (adt::RuntimeException);

   /**
      metodo que puede ser reescrito en las clases heredadas de Application y que nos
      da la posibilidad de inicializar los elementos particulares de nuestra aplicacion.
      Por defecto no realiza ninguna operacion.
      \warning En el momento de invocar a esta funcin los enginees (\see EngineIf) pueden no
      estar disponibles.
   */
   virtual void initialize () throw (adt::RuntimeException) {;}

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      \param parent Nodo XML del que dependende la informacion.
      @return Una cadena con la informacion referente a esta instancia.
   */
   virtual xml::Node& asXML (xml::Node& parent) const throw ();

protected:
   /**
      metodo que debemos implementar para ejecutar nuestra aplicacion.
   */
   virtual void run () throw (adt::RuntimeException) = 0;

   virtual void do_requestStop () throw (adt::RuntimeException);

   /**
      Metodo manejador que podemos re-escribir para tratar la recepcion de la senhal USR1.
      Por defecto volcar� el contexto.
   */
   virtual void signalUSR1 () throw (adt::RuntimeException);

   /**
      Metodo manejador que podemos re-escribir para tratar la recepcion de la senhal USR1.
      Por defecto incrementar� el nivel de trazas actual. Si llega se alcanza el nivel Level::Debug
      volver� al nivel Level::Error.
   */
   virtual void signalUSR2 () throw (adt::RuntimeException);

private:
   std::string a_version;
   const std::string a_title;
   std::vector <EngineIf*> a_engines;
   pid_t a_pid;
   bool a_enableGPL;

   void attach (EngineIf*) throw (adt::RuntimeException);

   void startEngines () throw (adt::RuntimeException);
   void stopEngines () throw ();
   void sendSignalToChilds (const int signal) throw ();

   static void handlerUserSignal (int) throw ();
   static void handlerSignalTerminate (int) throw ();
   static void handlerChildTerminate (int sig) throw ();

   friend class EngineIf;  
   friend struct functions;
};

}
}

#endif
