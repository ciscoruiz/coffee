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


#include <coffee/config/SCCSRepository.hpp>

#include <coffee/config/SCCS.hpp>
#include <coffee/basis/SCCS.hpp>
#include <coffee/xml/SCCS.hpp>
#include <coffee/logger/SCCS.hpp>
#include <coffee/app/SCCS.hpp>
#include <coffee/networking/SCCS.hpp>
#include <coffee/http/SCCS.hpp>

coffee_sccs_define_tag (http,release-18.05.24);

using namespace coffee;

void http::SCCS::activate()
   noexcept
{
   config::SCCS::activate ();
   basis::SCCS::activate ();
   xml::SCCS::activate ();
   logger::SCCS::activate();
   app::SCCS::activate();
   networking::SCCS::activate();

   config::SCCSRepository::getInstance().registerModule(coffee_sccs_use_tag(http));
}

