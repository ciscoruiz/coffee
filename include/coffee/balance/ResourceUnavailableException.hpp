#ifndef _coffee_balance_ResourceUnavailableException_hpp
#define _coffee_balance_ResourceUnavailableException_hpp

#include <coffee/adt/Exception.hpp>

namespace coffee {

namespace balance {

/**
 * Defines exception used for this library.
 *
 * @see http://www.boost.org/doc/libs/1_39_0/libs/exception/doc/exception_types_as_simple_semantic_tags.html
 */
class ResourceUnavailableException : public adt::Exception {
public:
   ResourceUnavailableException (const std::string& str, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      adt::Exception (str, fromMethod, fromFile, fromLine)
   {;}

   ResourceUnavailableException (const ResourceUnavailableException& other) : adt::Exception (other) {;}

   std::string asString () const noexcept {
      adt::StreamString str (this->filePosition());
      str << what ();
      return str;
   }
};

}
}


#endif

