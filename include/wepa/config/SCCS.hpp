// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//

#ifndef _wepa_config_sccs_hpp_
#define _wepa_config_sccs_hpp_

#ifndef _DEBUG
   #define wepa_sccs_define_tag(module,release)  const char* wepa_sccs_##module = "@(#)WEPA."#module" VERSION alfa-13.05."#release"/O";
   #define wepa_sccs_define_tag_ex(module,ex,release)  const char* wepa_sccs_##module = "@(#)WEPA."#ex" VERSION alfa-13.05."#release"/O";
#else
   #define wepa_sccs_define_tag(module,release)  const char* wepa_sccs_##module = "@(#)WEPA."#module" VERSION alfa-13.05."#release"/D";
   #define wepa_sccs_define_tag_ex(module,ex,release)  const char* wepa_sccs_##module = "@(#)WEPA."#ex" VERSION alfa-13.05."#release"/D";
#endif

#define wepa_sccs_use_tag(module) (const char *) wepa_sccs_##module
#define wepa_sccs_import_tag(module) extern const char* wepa_sccs_##module

namespace wepa {
namespace config {

class SCCS {
public:
   static void activate () throw ();
};

}
}

#endif /* _wepa_config_sccs_hpp_ */
