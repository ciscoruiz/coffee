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

#ifndef _coffee_app_Runnable_h
#define _coffee_app_Runnable_h

#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/AsString.hpp>
#include <coffee/adt/StreamString.hpp>

namespace coffee {

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
   virtual ~Runnable() { ;}

   // Accesores
   /**
      Devuelve el nombre l�gico de esta instancia.
      @return El nombre l�gico de esta instancia.
   */
   const std::string& getName() const noexcept { return m_name; }

   /**
    * Devuelve \em true si est� instancia est� parada o \em false en otro caso.
    */
   bool isStopped() const noexcept { return m_statusFlags == StatusFlags::Stopped; }

   /**
      Devuelve \em true si la instancia esta comenzando su ejecucion o \em false en otro caso.
      \return \em true si la instancia esta comenzando su ejecucion o \em false en otro caso.
      \since NemesisRD.core 1.2.30
   */
   bool isStarting() const noexcept { return(m_statusFlags & StatusFlags::Starting) != 0; }

   /**
      Devuelve \em true si la instancia esta en ejecucion o \em false en otro caso.
      \return \em true si la instancia esta en ejecucion o \em false en otro caso.
      \since NemesisRD.core 1.2.30
   */
   bool isRunning() const noexcept { return(m_statusFlags & StatusFlags::Running) != 0; }

   /**
      Devuelve el valor del indicador de parada.
      \return el valor del indicador de parada.
      \warning La implementacion particular del metodo run deberia comprobar este valor
      periodicamente.
   */
   bool isWaitingStop() const noexcept { return(m_statusFlags & StatusFlags::WaitingStop) != 0; }

   /**
    * Solicita la parada de esta instancia. Invocar� al m�todo virtual puro \em do_requestStop.
    */
   void requestStop() throw(adt::RuntimeException);

   /**
      Devuelve una cadena con la informacion relevante de este objeto.
      \return Una cadena con la informacion relevante de este objeto.
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
     Constructor.
     @param name Nombre l�gico de esta instancia.
   */
   explicit Runnable(const std::string& name) : m_name(name), m_statusFlags(StatusFlags::Stopped) {;}

   /**
     Constructor.
     @param name Nombre l�gico de esta clase.
   */
   explicit Runnable(const char* name) : m_name(name), m_statusFlags(StatusFlags::Stopped) {;}

   /**
    * Constructor vac�o.
    */
   Runnable() : m_name(""), m_statusFlags(StatusFlags::Stopped) {;}

   /**
    * Activa el flag de estado recibido como par�metro
    */
   void activate(const StatusFlags::_v statusFlag) noexcept { m_statusFlags |= statusFlag; }

   /**
    * Activa el flag de estado recibido como par�metro
    */
   void deactivate(const StatusFlags::_v statusFlag) noexcept { m_statusFlags &= ~statusFlag; }

   /**
    * Establece directamente el estado de esta instancia.
    * \internal.
    */
//   void setStatusFlags(const int status) noexcept { a_statusFlags = status; }

   /**
    * Establece los flags que indica que esta instancia est� arrancando.
    */
   void statusStarting() noexcept { m_statusFlags = StatusFlags::Starting; }

   /**
    * Establece los flags que indica que ha recibido una solicitud de parada y
    * est� a la espera de realizar las operaciones necesarias.
    */
   void statusWaitingStop() noexcept { m_statusFlags |= StatusFlags::WaitingStop;  }

   /**
    * Establece los flags que indica que esta instancia est� en ejecuci�n.
    */
   void statusRunning() noexcept {
      deactivate(StatusFlags::Starting);
      activate(StatusFlags::Running);
   }

   /**
    * Establece los flags que indica que esta instancia ha terminado su ejecuci�n.
    */
   void statusStopped() noexcept { m_statusFlags = StatusFlags::Stopped; }

   /**
    * Inicializa el estado de esta instancia.
    * \internal.
    */
   void clearStatusFlags() noexcept { m_statusFlags = StatusFlags::Stopped; }

   /**
    * M�todo virtual que pueden usar las clases heredadas para particular su proceso de solicitud de parada.
    */
   virtual void do_requestStop() throw(adt::RuntimeException) {;}

private:
   const std::string m_name;
   int m_statusFlags;

   std::string flagsAsString() const noexcept;
};

}
}


#endif

