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

#ifndef _coffee_dbms_datatype_Abstract_h
#define _coffee_dbms_datatype_Abstract_h

#include <functional>
#include <memory>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/RuntimeException.hpp>
#include <coffee/dbms/datatype/Constraint.hpp>

#include <coffee/dbms/InvalidDataException.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
   Clase base para las variables de entrada/salida asociadas a las sentencias SQL.

   @author  cisco.tierra@gmail.com.
*/
class Abstract  {
public:
   struct Datatype {
      enum _v {
         Integer, /**< Numeros enteros */
         String,  /**< Cadenas de caracteres */
         Float, /**< N�mero en coma flotante */
         ShortBlock,  /**< Tipos de dato RAW */
         LongBlock,  /**< Tipo de datos CLOB */
         Date, /** Tipo de fecha(dependiendo del gestor de base de datos puede contener tambien la hora) */
         TimeStamp /** Tipo para contener simult�neamente la fecha y la hora */
      };
   };

   virtual ~Abstract() {;}

   const char* getName() const noexcept { return m_name.c_str(); }

   int getMaxSize() const noexcept { return m_maxSize; }

   Datatype::_v getType() const noexcept { return m_type; }

   void* getBuffer() noexcept { return m_buffer; }
   const void* getBuffer() const noexcept { return m_buffer; }

   bool hasValue() const noexcept { return m_isNull == false; }

   bool canBeNull() const noexcept { return m_constraint == Constraint::CanBeNull; }

   void isNull() throw(adt::RuntimeException);

   void clear() noexcept;

   int compare(const std::shared_ptr<Abstract>& other) const throw(adt::RuntimeException);
   int compare(const Abstract& other) const throw(adt::RuntimeException);

   operator adt::StreamString() const noexcept { return asString(); }

   virtual adt::StreamString asString() const noexcept;

   virtual std::shared_ptr<Abstract> clone() const noexcept = 0;
   virtual size_t hash() const noexcept = 0;

protected:
   explicit Abstract(const char* name, const Datatype::_v type, const int maxSize, const Constraint::_v constraint) :
      m_name(name),
      m_type(type),
      m_maxSize(maxSize),
      m_constraint(constraint),
      m_isNull(constraint == Constraint::CanBeNull),
      m_buffer(NULL)
   {;}

   explicit Abstract(const std::string& name, const Datatype::_v type, const int maxSize, const Constraint::_v constraint) :
      m_name(name),
      m_type(type),
      m_maxSize(maxSize),
      m_constraint(constraint),
      m_isNull(constraint == Constraint::CanBeNull),
      m_buffer(NULL)
   {;}

   Abstract(const Abstract& other) :
      m_name(other.m_name),
      m_type(other.m_type),
      m_maxSize(other.m_maxSize),
      m_constraint(other.m_constraint),
      m_isNull(other.m_isNull),
      m_buffer(NULL)
   {;}

   void setBuffer(void* buffer) noexcept { m_buffer = buffer; }

   void isNotNull() noexcept { m_isNull = false; }

   void exceptionWhenIsNull() const throw(adt::RuntimeException);

private:
   const std::string m_name;
   const Datatype::_v m_type;
   const int m_maxSize;
   const bool m_constraint;
   void* m_buffer;
   bool m_isNull;

   virtual void do_clear() noexcept = 0;
   virtual int do_compare(const Abstract& other) const throw(adt::RuntimeException) = 0;
};

#define coffee_declare_datatype_downcast(inherit) \
   static const inherit& downcast(const datatype::Abstract& data,const char* function, const char* file, const int lineno) \
         throw(coffee::dbms::InvalidDataException) { \
      try { \
         return dynamic_cast <const inherit&>(data); \
      } \
      catch(std::bad_cast& ex) { \
      } \
      coffee::adt::StreamString str; \
      str << data.asString() << " | Invalid down cast"; \
      throw coffee::dbms::InvalidDataException(str, function, file, lineno); \
   } \
   static inherit& downcast(datatype::Abstract& data,char* function, char* file, int lineno) \
         throw(coffee::dbms::InvalidDataException) { \
      try { \
         return dynamic_cast <inherit&>(data); \
      } \
      catch(std::bad_cast& ex) { \
      } \
      coffee::adt::StreamString str; \
      str << data.asString() << " | Invalid down cast"; \
      throw coffee::dbms::InvalidDataException(str, function, file, lineno); \
   } \
   \
   static const std::shared_ptr<inherit> downcast(const std::shared_ptr<datatype::Abstract>& data,const char* function, const char* file, const int lineno) \
         throw(coffee::dbms::InvalidDataException) { \
      const std::shared_ptr<inherit> result = std::dynamic_pointer_cast<inherit>(data); \
      if(!result) { \
         coffee::adt::StreamString str; \
         str << data->asString() << " | Invalid down cast"; \
         throw coffee::dbms::InvalidDataException(str, function, file, lineno); \
      } \
      return result; \
   } \
   \
   static std::shared_ptr<inherit> downcast(std::shared_ptr<datatype::Abstract>& data,const char* function, const char* file, const int lineno) \
   throw(dbms::InvalidDataException) { \
      std::shared_ptr<inherit> result = std::dynamic_pointer_cast<inherit>(data); \
      if(!result) { \
         coffee::adt::StreamString str; \
         str << data->asString() << " | Invalid down cast"; \
         throw coffee::dbms::InvalidDataException(str, function, file, lineno); \
      } \
      return result; \
   }

#define coffee_datatype_downcast(inherit,data) inherit::downcast(data,COFFEE_FILE_LOCATION)

}
}
}

#endif

