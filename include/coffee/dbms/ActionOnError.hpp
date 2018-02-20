#ifndef __coffee_dbms_ActionOnError_hpp
#define __coffee_dbms_ActionOnError_hpp

namespace coffee {

namespace dbms {
/**
 * Operation to do when the execution of this statement fails.
 */
struct ActionOnError { enum _v { Ignore, Rollback }; };

}
}


#endif
