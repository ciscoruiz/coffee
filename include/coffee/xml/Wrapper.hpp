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
