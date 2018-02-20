
#include <coffee/dbms/datatype/Abstract.hpp>
#include <coffee/dbms/binder/Input.hpp>

using namespace coffee;
using namespace coffee::dbms;

adt::StreamString binder::Input::asString() const
   noexcept
{
   adt::StreamString result("binder.Input {");

   result += getData()->asString();

   return result += "}";
}
