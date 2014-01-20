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
#ifndef __wepa_persistence_Class_hpp
#define __wepa_persistence_Class_hpp

#include <mutex>

#include <boost/ptr_container/ptr_vector.hpp>

#include <wepa/adt/NamedObject.hpp>
#include <wepa/adt/RuntimeException.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

namespace xml {
   class Node;
}

namespace persistence {

class Object;
class Accessor;
class GuardClass;

class Class : public adt::NamedObject {
   typedef boost::ptr_vector <dbms::datatype::Abstract> Members;

public:
   virtual ~Class ();

   int member_size () const noexcept { return m_members.size (); }

   adt::StreamString asString () const noexcept;

   xml::Node& asXML (xml::Node& parent) const noexcept;

   Class (const Class&) = delete;

protected:
   Class (const char* name) : adt::NamedObject (name) {;}

   dbms::datatype::Abstract& getMember (const int columnNumber) throw (adt::RuntimeException);
   const dbms::datatype::Abstract& getMember (const int columnNumber) const throw (adt::RuntimeException);

private:
   Members m_members;
   std::recursive_mutex m_mutex;

   void createMembers () throw (adt::RuntimeException);

   virtual dbms::datatype::Abstract* do_createMember (const int columnNumber) const noexcept = 0;
   virtual Object* createObject () noexcept = 0;

   friend class Accessor;
   friend class GuardClass;
};

} /* namespace persistence */
} /* namespace wepa */
#endif
