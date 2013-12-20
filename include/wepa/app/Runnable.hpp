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
#ifndef _wepa_app_Runnable_h
#define _wepa_app_Runnable_h

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/adt/AsString.hpp>
#include <wepa/adt/StreamString.hpp>

namespace wepa {

namespace xml {
class Node;
}

namespace app {

/**
   Clase generica para implementar clases que deben realizar una unica accion concreta durante un periodo
   de tiempo indeterminado y que deben atender una solicitud de parada en cualquier momento.

   @author cisco.tierra@gmail.com.
*/
class Runnable {
   struct StatusFlags { enum _v { Stopped = 0, Starting = 1, Running = 2, WaitingStop = 4 }; };

public:
   /**
    * Destructor.
    */
   virtual ~Runnable () { delete m_name;}

   // Accesores
   /**
      Devuelve el nombre l�gico de esta instancia.
      @return El nombre l�gico de esta instancia.
   */
   const char* getName() const throw () { return (m_name == NULL) ? NULL: m_name->c_str (); }

   /**
    * Devuelve \em true si est� instancia est� parada o \em false en otro caso.
    */
   bool isStopped () const throw () { return m_statusFlags == StatusFlags::Stopped; }

   /**
      Devuelve \em true si la instancia esta comenzando su ejecucion o \em false en otro caso.
      \return \em true si la instancia esta comenzando su ejecucion o \em false en otro caso.
      \since NemesisRD.core 1.2.30
   */
   bool isStarting () const throw () { return (m_statusFlags & StatusFlags::Starting) != 0; }

   /**
      Devuelve \em true si la instancia esta en ejecucion o \em false en otro caso.
      \return \em true si la instancia esta en ejecucion o \em false en otro caso.
      \since NemesisRD.core 1.2.30
   */
   bool isRunning () const throw () { return (m_statusFlags & StatusFlags::Running) != 0; }

   /**
      Devuelve el valor del indicador de parada.
      \return el valor del indicador de parada.
      \warning La implementacion particular del metodo run deberia comprobar este valor
      periodicamente.
   */
   bool isWaitingStop () const throw () { return (m_statusFlags & StatusFlags::WaitingStop) != 0; }

   /**
    * Solicita la parada de esta instancia. Invocar� al m�todo virtual puro \em do_requestStop.
    */
   void requestStop () throw (adt::RuntimeException);

   /**
      Devuelve una cadena con la informacion relevante de este objeto.
      \return Una cadena con la informacion relevante de este objeto.
   */
   virtual adt::StreamString asString () const throw ();

   /**
      Devuelve un documento XML con la informacion mas relevante de esta instancia.
      \param parent Nodo XML del que colgar la informacion referente a esta instancia.
      \return Un documento XML con la informacion mas relevante de esta instancia.
   */
   virtual xml::Node& asXML (xml::Node& parent) const throw ();

protected:
   /**
     Constructor.
     @param name Nombre l�gico de esta instancia.
   */
   explicit Runnable (const std::string& name) : m_name (new std::string (name)), m_statusFlags (StatusFlags::Stopped) {;}

   /**
     Constructor.
     @param name Nombre l�gico de esta clase.
   */
   explicit Runnable (const char* name) : m_name (new std::string (name)), m_statusFlags (StatusFlags::Stopped) {;}

   /**
    * Constructor vac�o.
    */
   Runnable () : m_name (NULL), m_statusFlags (StatusFlags::Stopped) {;}

   /**
    * Activa el flag de estado recibido como par�metro
    */
   void activate (const StatusFlags::_v statusFlag) throw () { m_statusFlags |= statusFlag; }

   /**
    * Activa el flag de estado recibido como par�metro
    */
   void deactivate (const StatusFlags::_v statusFlag) throw () { m_statusFlags &= ~statusFlag; }

   /**
    * Establece directamente el estado de esta instancia.
    * \internal.
    */
//   void setStatusFlags (const int status) throw () { a_statusFlags = status; }

   /**
    * Establece los flags que indica que esta instancia est� arrancando.
    */
   void statusStarting () throw () { m_statusFlags = StatusFlags::Starting; }

   /**
    * Establece los flags que indica que ha recibido una solicitud de parada y
    * est� a la espera de realizar las operaciones necesarias.
    */
   void statusWaitingStop () throw () { m_statusFlags |= StatusFlags::WaitingStop;  }

   /**
    * Establece los flags que indica que esta instancia est� en ejecuci�n.
    */
   void statusRunning () throw () {
      deactivate (StatusFlags::Starting);
      activate (StatusFlags::Running);
   }

   /**
    * Establece los flags que indica que esta instancia ha terminado su ejecuci�n.
    */
   void statusStopped () throw () { m_statusFlags = StatusFlags::Stopped; }

   /**
    * Inicializa el estado de esta instancia.
    * \internal.
    */
   void clearStatusFlags () throw () { m_statusFlags = StatusFlags::Stopped; }

   /**
    * M�todo virtual que pueden usar las clases heredadas para particular su proceso de solicitud de parada.
    */
   virtual void do_requestStop () throw (adt::RuntimeException) {;}

private:
   const std::string* m_name;
   int m_statusFlags;

   std::string flagsAsString () const throw ();
};

}
}


#endif

