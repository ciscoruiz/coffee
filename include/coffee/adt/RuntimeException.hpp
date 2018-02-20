#ifndef _coffee_adt_RuntimeException_h
#define _coffee_adt_RuntimeException_h

#include <sstream>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/Exception.hpp>

namespace coffee {

namespace adt {

/**
 * Defines exception used for this library.
 *
 * @see http://www.boost.org/doc/libs/1_39_0/libs/exception/doc/exception_types_as_simple_semantic_tags.html
 */
class RuntimeException : public Exception {
public:
   static const int NullErrorCode = -1;

   RuntimeException (const std::string& str, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      Exception (str, fromMethod, fromFile, fromLine),
      m_errorCode (NullErrorCode)
   {;}

   RuntimeException (const RuntimeException& other) :
      Exception (other),
      m_errorCode (other.m_errorCode)
   {;}

   int getErrorCode () const noexcept { return m_errorCode; }

   void setErrorCode (const int errorCode) noexcept { m_errorCode = errorCode; }

   std::string asString () const noexcept;

private:
   int m_errorCode;
};

}
}

#define COFFEE_THROW_EXCEPTION(msg) do { coffee::adt::StreamString __str; __str << msg; throw coffee::adt::RuntimeException (__str, __PRETTY_FUNCTION__, __FILE__, __LINE__); } while (false)
#define COFFEE_THROW_NAMED_EXCEPTION(name,msg) do { coffee::adt::StreamString __str; __str << msg; throw name(__str, __PRETTY_FUNCTION__, __FILE__, __LINE__); } while (false)


#endif // _coffee_adt_RuntimeException_h
