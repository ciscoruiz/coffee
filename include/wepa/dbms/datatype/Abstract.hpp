// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_type_Abstract_h
#define _wepa_dbms_type_Abstract_h

#include <functional>

#include <wepa/adt/StreamString.hpp>
#include <wepa/adt/RuntimeException.hpp>
#include <wepa/dbms/datatype/Constraint.hpp>

namespace wepa {

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
         Date, /** Tipo de fecha (dependiendo del gestor de base de datos puede contener tambien la hora) */
         TimeStamp /** Tipo para contener simult�neamente la fecha y la hora */
      };
   };

   virtual ~Abstract () {;}

   const char* getName () const noexcept { return m_name.c_str (); }

   int getMaxSize () const noexcept { return m_maxSize; }

   Datatype::_v getType () const noexcept { return m_type; }

   void* getBuffer () noexcept { return m_buffer; }

   bool hasValue () const noexcept { return m_isNull == false; }

   bool canBeNull () const noexcept { return m_constraint == Constraint::CanBeNull; }

   void isNull () throw (adt::RuntimeException);

   void clear () noexcept;

   int compare (const Abstract& other) const throw (adt::RuntimeException);

   operator adt::StreamString () const noexcept { return asString (); }

   virtual adt::StreamString asString () const noexcept;

   virtual Abstract* clone () const noexcept = 0;

   static const char* className () noexcept { return "dbms.datatype.Abstract"; }

protected:
   explicit Abstract (const char* name, const Datatype::_v type, const int maxSize, const Constraint::_v constraint) :
      m_name (name),
      m_type (type),
      m_maxSize (maxSize),
      m_constraint (constraint),
      m_isNull (constraint == Constraint::CanBeNull),
      m_buffer (NULL)
   {;}

   explicit Abstract (const std::string& name, const Datatype::_v type, const int maxSize, const Constraint::_v constraint) :
      m_name (name),
      m_type (type),
      m_maxSize (maxSize),
      m_constraint (constraint),
      m_isNull (constraint == Constraint::CanBeNull),
      m_buffer (NULL)
   {;}

   Abstract (const Abstract& other) :
      m_name (other.m_name),
      m_type (other.m_type),
      m_maxSize (other.m_maxSize),
      m_constraint (other.m_constraint),
      m_isNull (other.m_isNull),
      m_buffer (NULL)
   {;}

   void setBuffer (void* buffer) noexcept { m_buffer = buffer; }

   void isNotNull () noexcept { m_isNull = false; }

   void exceptionWhenIsNull () const throw (adt::RuntimeException);

private:
   const std::string m_name;
   const Datatype::_v m_type;
   const int m_maxSize;
   const bool m_constraint;
   void* m_buffer;
   bool m_isNull;

   virtual void do_clear () noexcept = 0;
   virtual int do_compare (const Abstract& other) const throw (adt::RuntimeException) = 0;
};

#define wepa_declare_datatype_downcast(inherit) \
   static const inherit& downcast (const datatype::Abstract& data,const char* function, const char* file, const int lineno) \
   throw (adt::RuntimeException) { \
      const inherit* result = dynamic_cast <const inherit*> (&data); \
      if (result == NULL) { \
         wepa::adt::StreamString str; \
         str << data.asString () << " | Invalid down cast"; \
         throw wepa::adt::RuntimeException (str, function, file, lineno); \
      } \
      return std::ref (*result); \
   } \
   \
   static inherit& downcast (datatype::Abstract& data,const char* function, const char* file, const int lineno) \
   throw (adt::RuntimeException) { \
      inherit* result = dynamic_cast <inherit*> (&data); \
      if (result == NULL) { \
         wepa::adt::StreamString str; \
         str << data.asString () << " | Invalid down cast"; \
         throw wepa::adt::RuntimeException (str, function, file, lineno); \
      } \
      return std::ref (*result); \
   }

#define wepa_datatype_downcast(inherit,reference) inherit::downcast(reference,WEPA_FILE_LOCATION)

}
}
}

#endif

