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

#ifndef _coffee_dbms_datatype_Abstract_h
#define _coffee_dbms_datatype_Abstract_h

#include <functional>
#include <memory>

#include <coffee/basis/StreamString.hpp>
#include <coffee/basis/RuntimeException.hpp>
#include <coffee/dbms/datatype/Constraint.hpp>

#include <coffee/dbms/InvalidDataException.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
 * Base class for input/output variables into the c++ scope associated to
 * different SQL sentences.
 */
class Abstract  {
public:
   struct Datatype {
      enum _v {
         Integer, /**< Integer data type*/
         String,  /**< String data type */
         Float, /**< Floating point type */
         ShortBlock,  /**< Memory block datatype */
         LongBlock,  /**< Big memory block datatype */
         Date, /** Date data type */
         TimeStamp, /** Epoch data type */
         MultiString /** A data type which has a lot of values */
      };
   };

   /**
    * Destructor
    */
   virtual ~Abstract() {;}

   /**
    * \return the name for this instance.
    */
   const char* getName() const noexcept { return m_name.c_str(); }

   /**
    * \return Max size for this instance
    */
   int getMaxSize() const noexcept { return m_maxSize; }

   /**
    * \return the data type of this instance
    */
   Datatype::_v getType() const noexcept { return m_type; }

   /**
    * \return the buffer associated to this instance.
    * \warning You should not need this method.
    */
   void* getBuffer() noexcept { return m_buffer; }

   /**
    * \return the buffer associated to this instance.
    * \warning You should not need this method.
    */
   const void* getBuffer() const noexcept { return m_buffer; }

   /**
    * \return \b true if the data has a value assigned or \b false otherwise.
    */
   bool hasValue() const noexcept { return m_isNull == false; }

   /**
    * \return \b true it this instance can contain a null value or \b false otherwise.
    */
   bool canBeNull() const noexcept { return m_constraint == Constraint::CanBeNull; }

   /**
    * Set to null the value of this instance.
    * \warning It will throw an exception in case of this instance could not contain a null value.
    */
   void isNull() throw(basis::RuntimeException);

   /**
    * Clear the value of this instance.
    */
   void clear() noexcept;

   /**
    * Compare the value of this instance with the value of the received instance.
    */
   int compare(const std::shared_ptr<Abstract>& other) const throw(basis::RuntimeException);

   /**
    * Compare the value of this instance with the value of the received instance.
    */
   int compare(const Abstract& other) const throw(basis::RuntimeException);

   operator basis::StreamString() const noexcept { return asString(); }

   /**
    * \return Summarize information of this instance in a coffee::basis::StreamString.
    */
   virtual basis::StreamString asString() const noexcept;

   /**
    * \return a duplicate of this instance.
    */
   virtual std::shared_ptr<Abstract> clone() const noexcept = 0;

   /**
    * \return the hash code for this instance.
    */
   virtual size_t hash() const noexcept = 0;

protected:
   Abstract(const char* name, const Datatype::_v type, const int maxSize, const Constraint::_v constraint) :
      m_name(name),
      m_type(type),
      m_maxSize(maxSize),
      m_constraint(constraint),
      m_isNull(constraint == Constraint::CanBeNull),
      m_buffer(NULL)
   {;}

   Abstract(const std::string& name, const Datatype::_v type, const int maxSize, const Constraint::_v constraint) :
      m_name(name),
      m_type(type),
      m_maxSize(maxSize),
      m_constraint(constraint),
      m_isNull(constraint == Constraint::CanBeNull),
      m_buffer(NULL)
   {;}

   Abstract(const Abstract& other) :
      m_name(other.m_name),
      m_type(other.m_type),
      m_maxSize(other.m_maxSize),
      m_constraint(other.m_constraint),
      m_isNull(other.m_isNull),
      m_buffer(NULL)
   {;}

   void setBuffer(void* buffer) noexcept { m_buffer = buffer; }

   void isNotNull() noexcept { m_isNull = false; }

   void exceptionWhenIsNull() const throw(basis::RuntimeException);

private:
   const std::string m_name;
   const Datatype::_v m_type;
   const int m_maxSize;
   const bool m_constraint;
   void* m_buffer;
   bool m_isNull;

   virtual void do_clear() noexcept = 0;
   virtual int do_compare(const Abstract& other) const throw(basis::RuntimeException) = 0;
};

#define coffee_declare_datatype_downcast(inherit) \
   static const inherit& downcast(const datatype::Abstract& data,const char* function, const char* file, const int lineno) \
         throw(coffee::dbms::InvalidDataException) { \
      try { \
         return dynamic_cast <const inherit&>(data); \
      } \
      catch(std::bad_cast& ex) { \
      } \
      coffee::basis::StreamString str; \
      str << data.asString() << " | Invalid down cast"; \
      throw coffee::dbms::InvalidDataException(str, function, file, lineno); \
   } \
   static inherit& downcast(datatype::Abstract& data,char* function, char* file, int lineno) \
         throw(coffee::dbms::InvalidDataException) { \
      try { \
         return dynamic_cast <inherit&>(data); \
      } \
      catch(std::bad_cast& ex) { \
      } \
      coffee::basis::StreamString str; \
      str << data.asString() << " | Invalid down cast"; \
      throw coffee::dbms::InvalidDataException(str, function, file, lineno); \
   } \
   \
   static const std::shared_ptr<inherit> downcast(const std::shared_ptr<datatype::Abstract>& data,const char* function, const char* file, const int lineno) \
         throw(coffee::dbms::InvalidDataException) { \
      const std::shared_ptr<inherit> result = std::dynamic_pointer_cast<inherit>(data); \
      if(!result) { \
         coffee::basis::StreamString str; \
         str << data->asString() << " | Invalid down cast"; \
         throw coffee::dbms::InvalidDataException(str, function, file, lineno); \
      } \
      return result; \
   } \
   \
   static std::shared_ptr<inherit> downcast(std::shared_ptr<datatype::Abstract>& data,const char* function, const char* file, const int lineno) \
   throw(dbms::InvalidDataException) { \
      std::shared_ptr<inherit> result = std::dynamic_pointer_cast<inherit>(data); \
      if(!result) { \
         coffee::basis::StreamString str; \
         str << data->asString() << " | Invalid down cast"; \
         throw coffee::dbms::InvalidDataException(str, function, file, lineno); \
      } \
      return result; \
   }

#define coffee_datatype_downcast(inherit,data) inherit::downcast(data,COFFEE_FILE_LOCATION)

}
}
}

#endif

