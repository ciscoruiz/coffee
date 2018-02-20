#ifndef __coffee_xml_Wrapper_hpp
#define __coffee_xml_Wrapper_hpp

#include <functional>

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace xml {

class Compiler;

template <typename _Handler> class  Wrapper {
public:
   typedef std::function <void (_Handler*)> Deleter;
   typedef _Handler* Handler;
   typedef std::function <const char* (const Handler)> NameExtractor;

   virtual ~Wrapper () {
      releaseHandler ();
   }

   Handler getHandler () noexcept { return m_handler; }
   Handler getHandler () const noexcept { return m_handler; }

   const std::string& getName () const {
      if (m_handler == NULL) {
         m_name.clear ();
      }
      else if (m_name.empty() == true) {
         m_name = m_nameExtractor(m_handler);
      }

      return m_name;
   }

   operator Handler () noexcept { return m_handler; }

protected:
   Wrapper () : m_handler (NULL), m_deleter (NULL), m_nameExtractor (NULL) {}
   explicit Wrapper (Handler handler) : m_handler (handler), m_deleter (NULL), m_nameExtractor (NULL) {}

   void setDeleter (Deleter deleter) noexcept { m_deleter = deleter; }
   void setNameExtractor (NameExtractor nameExtractor) noexcept { m_nameExtractor = nameExtractor; }
   Handler setHandler (Handler handler) noexcept { m_handler = handler; m_name.clear (); return m_handler; }

   void releaseHandler ()
      noexcept
   {
      if (m_handler != NULL) {
         if (m_deleter) {
            m_deleter (m_handler);
         }
         m_handler = NULL;
         m_name.clear ();
      }
   }

   virtual void compile (Compiler& compiler) const throw (adt::RuntimeException) {;} ;

private:
   Handler m_handler;
   NameExtractor m_nameExtractor;
   Deleter m_deleter;
   mutable std::string m_name;
};

}

}

#endif
