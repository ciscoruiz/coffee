#ifndef _coffee_dbms_datatype_Constraint_hpp
#define _coffee_dbms_datatype_Constraint_hpp

namespace coffee {

namespace dbms {

namespace datatype {

struct Constraint {
   enum _v { CanBeNull, CanNotBeNull };
};

}
}
}

#endif

