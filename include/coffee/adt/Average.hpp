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

#ifndef _coffee_adt_Average_h
#define _coffee_adt_Average_h

#include <coffee/adt/StreamString.hpp>

namespace coffee {

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
   Average(const char* name, const char* measure ) : a_name(name), a_measure(measure), a_accumulator(0), a_n(0) {;}

   /**
      Constructor.
      \param name Nombre logico de esta instancia.
      \param measure Unidad de medida. Solo se usa a efecto de salida de datos.
   */
   Average(const char* name) : a_name(name), a_measure(""), a_accumulator(0), a_n(0) {;}

   /**
      Devuelve el indicador de validez de esta media.
      \return \em true Si la media no tiene ningun valor o \em false en caso contrario.      
   */
   bool isEmpty() const noexcept  { return(a_n == 0); }

   /**
      Devuelve \em true si el valor de la media es cero, bien por no tener valores o 
      bien porque todos ellos eran cero.
      \return \em true el valor de la media es cero o \em false en otro caso.
   */
   bool isZero() const noexcept { return a_n == 0 || a_accumulator == 0; }

   /**
      Devuelve el numero de elementos de contiene la media.
      \return el numero de elementos de contiene la media.
   */
   int size() const noexcept { return a_n; }

   /**
      Devuelve el valor acumulado.
      \return el valor acumulado.
   */
   _T getAccumulator() const noexcept { return a_accumulator; }

   /**
      Devuelve la media de la sucesion de valores numericos asociados a esta.
      \return La media de la sucesion de valores numericos asociados a esta.      
      \warning Antes de invocar a este operador hay que verificar que #isEmpty devuelve \em false.
   */
   _T value() const noexcept { return(isEmpty() == true) ? _T(0):(a_accumulator / a_n);  }
   
   /**
      Inicializa el valor de la media.      
   */
   void clear() noexcept {  a_accumulator = 0; a_n = 0; }

   /**
    * Establece manualmente el valor de la estad�stica.
    * \param value Valor que tomar� el acumulador de este instancia.
    * \param _n valor que tomar� el conteador de esta instancia.
    */
   void setValue(const _T& value, const unsigned int _n) noexcept {
      a_accumulator = value;
      a_n = _n;
   }

   /**
      Devuelve la media de la sucesion de valores numericos asociados a esta.
      \return La media de la sucesion de valores numericos asociados a esta.      
      \warning Antes de invocar a este operador hay que verificar que #isEmpty devuelve \em false.
   */
   operator _T() const noexcept { return value(); }

   /**
      Inicializa el valor de esta media.
      \param value Valor con el que iniciar la media.
      \return La referencia a esta instancia.
   */
   Average<_T>& operator =(const _T value)
      noexcept 
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
   Average<_T>& operator =(const Average<_T>& other)
      noexcept 
   {
      a_name = other.a_name;
      a_measure = other.a_measure;
      a_accumulator = other.a_accumulator;
      a_n = other.a_n;
      return *this;
   }

   /**
      Incrementa la media con el valor recibido.
      \param v Valor con el que incrementar la media.
      \return La referencia a esta instancia.
   */
   Average<_T>& operator +=(const _T& v)
      noexcept 
   {
      const _T backup(a_accumulator);
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
   Average<_T>& operator -=(const _T v)
      noexcept 
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
   StreamString asString() const
      noexcept
   {
      StreamString result(a_name);
      result << " { Accumulator:" << a_accumulator;

      if (!a_measure.empty())
         result << " " << a_measure;

      result << " | N=" <<  a_n;
      result << " | Value=" << value();

      if (!a_measure.empty())
         result << " " << a_measure;
      
      return result += " }";
   }
   
private:
   std::string a_name;
   std::string a_measure;
   _T a_accumulator;
   unsigned int a_n; 
};

}
}

#endif



