#ifndef COFFEE_CONFIG_RELEASE_H
#define COFFEE_CONFIG_RELEASE_H

#include <string>

namespace coffee  {

namespace config {

class Release {
public:
   /**
    * @brief getVersion
    * @return The version of this compilation
    */
   static std::string getVersion () noexcept;

   /**
    * @brief getArchitecture
    * @return The literal which contains information about this release.
    */
   static std::string getArchitecture () noexcept;
};

}
}

#endif // COFFEE_CONFIG_RELEASE_H
