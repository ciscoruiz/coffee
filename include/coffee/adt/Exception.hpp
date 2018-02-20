#ifndef __coffee_adt_Exception_hpp
#define __coffee_adt_Exception_hpp

#include <stdexcept>

namespace coffee {

namespace adt {

class Exception : std::logic_error {
public:
   using std::logic_error::what;

   virtual ~Exception () noexcept {;}

   const char* getMethod () const noexcept { return m_fromMethod; }
   const char* getFile () const noexcept { return m_fromFile; }
   const unsigned getLine () const noexcept { return m_fromLine; }

protected:
   Exception (const std::string& str, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      std::logic_error (str),
      m_fromMethod (fromMethod),
      m_fromFile (fromFile),
      m_fromLine (fromLine)
   {;}

   Exception (const Exception& other) :
      std::logic_error (other),
      m_fromMethod (other.m_fromMethod),
      m_fromFile (other.m_fromFile),
      m_fromLine (other.m_fromLine)
   {;}

   std::string filePosition () const noexcept;

private:
   const char* m_fromMethod;
   const char* m_fromFile;
   const unsigned m_fromLine;
};

}
}

#endif
