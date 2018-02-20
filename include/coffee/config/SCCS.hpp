
#ifndef _coffee_config_sccs_hpp_
#define _coffee_config_sccs_hpp_

#ifndef _DEBUG
   #define coffee_sccs_define_tag(module,release)  const char* coffee_sccs_##module = "@(#)COFFEE."#module" VERSION 1.18.02."#release"/O";
   #define coffee_sccs_define_tag_ex(module,ex,release)  const char* coffee_sccs_##module = "@(#)COFFEE."#ex" VERSION 1.18.02."#release"/O";
#else
   #define coffee_sccs_define_tag(module,release)  const char* coffee_sccs_##module = "@(#)COFFEE."#module" VERSION 1.18.02."#release"/D";
   #define coffee_sccs_define_tag_ex(module,ex,release)  const char* coffee_sccs_##module = "@(#)COFFEE."#ex" VERSION 1.18.02."#release"/D";
#endif

#define coffee_sccs_use_tag(module) (const char *) coffee_sccs_##module
#define coffee_sccs_import_tag(module) extern const char* coffee_sccs_##module

namespace coffee {
namespace config {

class SCCS {
public:
   static void activate () noexcept;
};

}
}

#endif /* _coffee_config_sccs_hpp_ */
