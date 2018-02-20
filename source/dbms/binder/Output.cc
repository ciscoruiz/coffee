
#include <coffee/logger/Logger.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>

#include <coffee/dbms/binder/Output.hpp>

using namespace coffee;
using namespace coffee::dbms;

adt::StreamString binder::Output::asString() const
   noexcept
{
   adt::StreamString result("binder.Output {");
   result += getData()->asString();
   return result += "}";
}

