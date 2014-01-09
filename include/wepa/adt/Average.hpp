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
#ifndef _wepa_adt_Average_h
#define _wepa_adt_Average_h

#include <wepa/adt/StreamString.hpp>

namespace wepa {

namespace adt {

/**
   Template para calcular la media de una sucesion de valores numericos.

   Internamente trabaja con Microsegundos porque los m�todos que los proporcionan son
   m�s eficientes.
      
   @author cisco.tierra@gmail.com.
*/
template <class _T> class Average {
public:
   /**
      Constructor.
      \param name Nombre logico de esta instancia. 
      \param measure Unidad de medida. Solo se usa a efecto de salida de datos.
   */
   Average (const char* name, const char* measure = NULL) : a_name (name), a_accumulator (0), a_n (0), a_measure (measure) {;}

   /**
      Devuelve el indicador de validez de esta media.
      \return \em true Si la media no tiene ningun valor o \em false en caso contrario.      
   */
   bool isEmpty () const throw ()  { return (a_n == 0); }

   /**
      Devuelve \em true si el valor de la media es cero, bien por no tener valores o 
      bien porque todos ellos eran cero.
      \return \em true el valor de la media es cero o \em false en otro caso.
   */
   bool isZero () const throw () { return a_n == 0 || a_accumulator == 0; }

   /**
      Devuelve el numero de elementos de contiene la media.
      \return el numero de elementos de contiene la media.
   */
   int size () const throw () { return a_n; }

   /**
      Devuelve el valor acumulado.
      \return el valor acumulado.
   */
   _T getAccumulator () const throw () { return a_accumulator; }

   /**
      Devuelve la media de la sucesion de valores numericos asociados a esta.
      \return La media de la sucesion de valores numericos asociados a esta.      
      \warning Antes de invocar a este operador hay que verificar que #isEmpty devuelve \em false.
   */
   _T value () const throw () { return (isEmpty () == true) ? _T(0): (a_accumulator / a_n);  }
   
   /**
      Inicializa el valor de la media.      
   */
   void clear () throw () {  a_accumulator = 0; a_n = 0; }

   /**
    * Establece manualmente el valor de la estad�stica.
    * \param value Valor que tomar� el acumulador de este instancia.
    * \param _n valor que tomar� el conteador de esta instancia.
    */
   void setValue (const _T& value, const unsigned int _n) throw () {
      a_accumulator = value;
      a_n = _n;
   }

   /**
      Devuelve la media de la sucesion de valores numericos asociados a esta.
      \return La media de la sucesion de valores numericos asociados a esta.      
      \warning Antes de invocar a este operador hay que verificar que #isEmpty devuelve \em false.
   */
   operator _T () const throw () { return value (); }

   /**
      Inicializa el valor de esta media.
      \param value Valor con el que iniciar la media.
      \return La referencia a esta instancia.
   */
   Average<_T>& operator = (const _T value)
      throw () 
   {
      a_accumulator = value;
      a_n = 1;
      return *this;
   }

   /**
      Contructor copia.
      \param other Objeto del que obtener los valores.
      \return La referencia a esta instancia.
   */
   Average<_T>& operator = (const Average<_T>& other)
      throw () 
   {
      a_accumulator = other.a_accumulator;
      a_n = other.a_n;
      return *this;
   }

   /**
      Incrementa la media con el valor recibido.
      \param v Valor con el que incrementar la media.
      \return La referencia a esta instancia.
   */
   Average<_T>& operator += (const _T& v)
      throw () 
   {
      const _T backup (a_accumulator);
      if ((a_accumulator += v) < backup) {
         a_accumulator = v;
         a_n = 1;
      }
      else
         a_n ++;
      
      return *this;
   }

   /**
      Decrementa la media con el valor recibido.
      \param v Valor con el que incrementar la media.
      \return La referencia a esta instancia.
   */
   Average<_T>& operator -= (const _T v)
      throw () 
   {
      if (a_accumulator > v && a_n > 1) {
         a_accumulator -= v;
         a_n --;
      }
      else {
         a_accumulator = 0;
         a_n =0;
      }
      return *this;
   }

   /**
      Devuelve una cadena con la informacion referente a esta clase.      
      \return Una cadena con la informacion referente a esta clase.
   */
   StreamString asString () const
      throw ()
   {
      StreamString result (a_name);
      result << " { Accumulator:" << a_accumulator;

      if (a_measure != NULL)
         result << " " << a_measure;

      result << " | N=" <<  a_n;

      result << " | Value=" << value ();

      if (a_measure != NULL)
         result << " " << a_measure;
      
      return result += " }";
   }
   
private:
   const char* a_name;
   const char* a_measure;
   _T a_accumulator;
   unsigned int a_n; 
};

}
}

#endif



