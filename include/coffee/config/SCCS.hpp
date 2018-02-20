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
