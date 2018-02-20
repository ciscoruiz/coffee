#ifndef __coffee_mock_MockLowLevelRecord_hpp
#define __coffee_mock_MockLowLevelRecord_hpp

#include <map>
#include <coffee/adt/Second.hpp>

namespace coffee {
namespace mock {

struct MockLowLevelRecord {
   int m_id;
   std::string m_name;
   int m_integer;
   float m_float;
   adt::Second m_time;
};

typedef std::map <int, mock::MockLowLevelRecord> MockLowLevelContainer;

}
}
#endif
