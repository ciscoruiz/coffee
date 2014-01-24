// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef __wepa_persistence_AutoObject_hpp
#define __wepa_persistence_AutoObject_hpp

#include <wepa/adt/Exception.hpp>

#include <wepa/persistence/Object.hpp>
#include <wepa/persistence/Storage.hpp>

namespace wepa {
namespace persistence {

class Object;

template <typename _T> class AutoObject {
public:
   AutoObject () : m_object (NULL) {;}
   AutoObject (Object& object) throw (adt::RuntimeException) :
      m_object (NULL)
   {
      set (object);
   }
   ~AutoObject () { release (); }

   bool hasValue () const noexcept { return m_object != NULL; }

   bool isNull () const noexcept { return hasValue () == false; }

   AutoObject& operator= (Object& object) throw (adt::RuntimeException) {
      release ();
      set (object);
      return *this;
   }

   _T& get () throw (adt::RuntimeException) { return std::ref (*operator-> ()); }

   _T* operator-> () throw (adt::RuntimeException) {
      if (isNull ())
         WEPA_THROW_EXCEPTION(typeid (*this).name () << " does not have associated value");

      return static_cast <_T*> (m_object);
   }

   AutoObject (const AutoObject <_T>&) = delete;

private:
   Object* m_object;

   void set (Object& object) throw (adt::RuntimeException) {
      _T* result = dynamic_cast <_T*> (&object);

      if (result == NULL) {
         WEPA_THROW_EXCEPTION(object.asString () << " | This object can not be treated by " << typeid (*this).name ());
      }

      m_object = result;
   }
   void release () noexcept {
      if (m_object != NULL) {
         GuardClass guard (m_object->getClass ());
         m_object->getStorageOwner ().release (guard, *m_object);
         m_object = NULL;
      }
   }
};

} /* namespace persistence */
} /* namespace wepa */
#endif
