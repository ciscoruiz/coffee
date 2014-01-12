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

   /**
    * Nombre l�gico del dato, indicado en el contructor.
    * \return Nombre l�gico del dato, indicado en el contructor.
    */
   const char* getName () const noexcept { return m_name.c_str (); }

   /**
      Devuelve el tamano maximo de este dato que coincidiria con el indicado en el constructor.
      \return El tamano maximo de este dato que coincidiria con el indicado en el constructor.
   */
   int getMaxSize () const noexcept { return m_maxSize; }

   /**
      Devuelve el tipo de dato.
      \return El tipo de datos.
   */
   Datatype::_v getType () const noexcept { return m_type; }

   /**
      Devuelve el area de memoria asociada a esta variable.
   */
   void* getBuffer () noexcept { return m_buffer; }

   /**
      Devuelve el indicador de nulo de esta instancia.
      \return El indicador de nulo de esta instancia.
   */
   bool hasValue () const noexcept { return m_isNull == false; }

   /**
      Devuelve el valor que indica si este dato puede tomar valores nulos.
      \return El valor que indica si este dato puede tomar valores nulos.
   */
   bool canBeNull () const noexcept { return m_constraint == Constraint::CanBeNull; }

   /**
      Establece el indicador de nulo de esta instancia.
      \param isNull Indicador de nulo de esta instancia.
      \warning Slo tendr�efecto en caso de haber indicado en el constructor que
      el dato puede tomar valores nulos.
   */
   void isNull () throw (adt::RuntimeException);

   /**
      Incorpora el m�todo clear para todos tipos de datos con lo que podemos obtener informaci�n
      del medio f�sico.

      Si el dato est� definido como "nuleable" activar� el indicador que indica que el dato est� vac�o,
      en otro caso se asignar� un valor adecuado dependiendo del tipo del dato, cero para los n�meros,
      "" para las cadenas, etc.
   */
   void clear () noexcept;

   operator adt::StreamString () const noexcept { return asString (); }

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      @return Una cadena con la informacion referente a esta instancia.
   */
   virtual adt::StreamString asString () const noexcept;

   /**
    * Devuelve el nombre l�gico de esta clase
    * \return el nombre l�gico de esta clase
    */
   static const char* className () noexcept { return "dbms::type::Abstract"; }

protected:
   /**
      Constructor.
      \param name Nombre l�gico del dato.
      \param type Tipo de dato de esta instancia.
      \param maxSize Tamao maximo que puede tener este dato. Deberia coincidir con el indicado
      por la columna con la que vaya a corresponder en la sentencia.
      \param constraint Indica si el dato puede tomar valores nulos.

      \warning los tipos de datos complejos deberia reimplementar los metodos #code and #decode.
   */
   explicit Abstract (const char* name, const Datatype::_v type, const int maxSize, const Constraint::_v constraint) :
      m_name (name),
      m_type (type),
      m_maxSize (maxSize),
      m_constraint (constraint),
      m_isNull (constraint == Constraint::CanBeNull),
      m_buffer (NULL)
   {;}

   /**
      Constructor.
      \param name Nombre l�gico del dato.
      \param type Tipo de dato de esta instancia.
      \param maxSize Tamao maximo que puede tener este dato. Deberia coincidir con el indicado
      por la columna con la que vaya a corresponder en la sentencia.
      \param constraint Indica si el dato puede tomar valores nulos.

      \warning los tipos de datos complejos deberia reimplementar los metodos #code and #decode.
   */
   explicit Abstract (const std::string& name, const Datatype::_v type, const int maxSize, const Constraint::_v constraint) :
      m_name (name),
      m_type (type),
      m_maxSize (maxSize),
      m_constraint (constraint),
      m_isNull (constraint == Constraint::CanBeNull),
      m_buffer (NULL)
   {;}

   /**
      Constructor copia.
      \param other Instancia de la que copiar.
      \since NemesisRD.dbms 1.1.1
   */
   Abstract (const Abstract& other) :
      m_name (other.m_name),
      m_type (other.m_type),
      m_maxSize (other.m_maxSize),
      m_constraint (other.m_constraint),
      m_isNull (other.m_isNull),
      m_buffer (other.m_buffer)
   {;}

   /**
      Establece el area de memoria asociada a esta variable.
      \param buffer Direccion de memoria donde comienza el contenido esta variable.
   */
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

