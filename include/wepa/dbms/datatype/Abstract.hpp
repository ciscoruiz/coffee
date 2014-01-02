// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_type_Abstract_h
#define _wepa_dbms_type_Abstract_h

#include <wepa/adt/StreamString.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Clase base para las variables de entrada/salida asociadas a las sentencias SQL.

   @author  cisco.tierra@gmail.com.
*/
class Abstract  {
public:
   struct Type {
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
   const char* getName () const throw () { return m_name.c_str (); }

   /**
      Devuelve el tamano maximo de este dato que coincidiria con el indicado en el constructor.
      \return El tamano maximo de este dato que coincidiria con el indicado en el constructor.
   */
   int getMaxSize () const throw () { return m_maxSize; }

   /**
      Devuelve el tipo de dato.
      \return El tipo de datos.
   */
   Type::_v getType () const throw () { return m_type; }

   /**
      Devuelve el area de memoria asociada a esta variable.
   */
   void* getBuffer () throw () { return m_buffer; }

   /**
      Devuelve el indicador de nulo de esta instancia.
      \return El indicador de nulo de esta instancia.
   */
   bool isNull () const throw () { return m_isNull; }

   /**
      Devuelve el valor que indica si este dato puede tomar valores nulos.
      \return El valor que indica si este dato puede tomar valores nulos.
   */
   bool isNulleable () const throw () { return m_isNulleable; }

   /**
      Establece el indicador de nulo de esta instancia.
      \param isNull Indicador de nulo de esta instancia.
      \warning Slo tendr�efecto en caso de haber indicado en el constructor que
      el dato puede tomar valores nulos.
   */
   void setNull (const bool isNull) throw () { if (m_isNulleable == true) m_isNull = isNull; }

   /**
      Incorpora el m�todo clear para todos tipos de datos con lo que podemos obtener informaci�n
      del medio f�sico.

      Si el dato est� definido como "nuleable" activar� el indicador que indica que el dato est� vac�o,
      en otro caso se asignar� un valor adecuado dependiendo del tipo del dato, cero para los n�meros,
      "" para las cadenas, etc.

      \since NemesisRD.dbms 1.10.2
   */
   void clear () throw () {
      setNull (true);
      do_clear ();
   }

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      @return Una cadena con la informacion referente a esta instancia.
   */
   virtual adt::StreamString asString () const throw ();

   /**
    * Devuelve el nombre l�gico de esta clase
    * \return el nombre l�gico de esta clase
    * \since NemesisRD.dbms 2.10.16.04
    */
   static const char* className () throw () { return "dbms::type::Abstract"; }

protected:
   /**
      Constructor.
      \param name Nombre l�gico del dato.
      \param type Tipo de dato de esta instancia.
      \param maxSize Tamao maximo que puede tener este dato. Deberia coincidir con el indicado
      por la columna con la que vaya a corresponder en la sentencia.
      \param isNulleable Indica si el dato puede tomar valores nulos.

      \warning los tipos de datos complejos deberia reimplementar los metodos #code and #decode.
   */
   explicit Abstract (const char* name, const Type::_v type, const int maxSize, const bool isNulleable) :
      m_name (name),
      m_type (type),
      m_maxSize (maxSize),
      m_isNulleable (isNulleable),
      m_isNull (isNulleable),
      m_buffer (NULL)
   {;}

   /**
      Constructor.
      \param name Nombre l�gico del dato.
      \param type Tipo de dato de esta instancia.
      \param maxSize Tamao maximo que puede tener este dato. Deberia coincidir con el indicado
      por la columna con la que vaya a corresponder en la sentencia.
      \param isNulleable Indica si el dato puede tomar valores nulos.

      \warning los tipos de datos complejos deberia reimplementar los metodos #code and #decode.
   */
   explicit Abstract (const std::string& name, const Type::_v type, const int maxSize, const bool isNulleable) :
      m_name (name),
      m_type (type),
      m_maxSize (maxSize),
      m_isNulleable (isNulleable),
      m_isNull (isNulleable),
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
      m_isNulleable (other.m_isNulleable),
      m_isNull (other.m_isNull),
      m_buffer (other.m_buffer)
   {;}

   /**
      Establece el area de memoria asociada a esta variable.
      \param buffer Direccion de memoria donde comienza el contenido esta variable.
   */
   void setBuffer (void* buffer) throw () { m_buffer = buffer; }

private:
   const std::string m_name;
   const Type::_v m_type;
   const int m_maxSize;
   const bool m_isNulleable;
   void* m_buffer;
   bool m_isNull;

   virtual void do_clear () throw () = 0;
};

}
}
}

#endif

