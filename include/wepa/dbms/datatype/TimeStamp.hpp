// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_TimeStamp_h
#define _wepa_dbms_datatype_TimeStamp_h

#include <wepa/dbms/datatype/Date.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Tipo de datos que permite trabajar con el tipo de dato 'TimeStamp' de un gestor de base de
   datos generico.

   El tipo de dato TimeStamp contiene la informaci�n suficiente para representar una fecha
   incluyendo la hora del d�a.

   \author cisco.tierra@gmail.com.
   \since NemesisRD.dbms 1.6.1
*/
class TimeStamp : public Date {
public:
   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param isNulleable Indica si el dato puede tomar valores nulos.

      \code
      TimeStamp oneTime (false, "%T.%%d")
      \endcode
    */
   explicit TimeStamp (const char* name, const bool isNulleable = false)  :
      Date (name, datatype::Abstract::Datatype::TimeStamp, isNulleable),
      m_fractionalSecond (0)
      {;}

   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param isNulleable Indica si el dato puede tomar valores nulos.
      \code
      TimeStamp oneTime (false, "%T.%%d")
      \endcode
   */
   explicit TimeStamp (const std::string& name, const bool isNulleable = false, const char* format=NULL)  :
      Date (name, datatype::Abstract::Datatype::TimeStamp, isNulleable),
      m_fractionalSecond (0)
      {;}

   /**
    * Devuelve la parte fraccionaria de los segundos asociados a este objeto.
    * \return La parte fraccionaria de los segundos asociados a este objeto.
    */
   int getFractionalSecond () const noexcept { return m_fractionalSecond; }

   /**
    * Establece la parte fraccionaria de los segundos de este objeto.
    * \param fsec Parte fraccionaria de los segundos.
    */
   void setFractionalSecond (const int fsec) noexcept { m_fractionalSecond = fsec; }

   wepa_declare_datatype_downcast(TimeStamp)

private:
   char m_anotherBuffer [MaxDateSize + 1];
   int m_fractionalSecond;

   void do_clear () noexcept { Date::do_clear (); m_fractionalSecond = 0; m_anotherBuffer [0] = 0; }
};

}
}
}

#endif

