// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#ifndef _coffee_basis_NamedObject_h
#define _coffee_basis_NamedObject_h

#include <coffee/basis/StreamString.hpp>

namespace coffee {

namespace basis {

/**
 * Class to define a object with an associated name.
 */
class NamedObject {
public:
   /**
    * Destructor.
    */
  virtual ~NamedObject() { ;}

  /**
   * \return The logical name of this instance.
   */
  const std::string& getName() const noexcept { return m_name; }

  /**
   * \return \b true if the name of this instance is equal to the received name or \b false otherwise.
   */
  bool isEqual(const std::string& name) const noexcept { return m_name == name; }

  /**
   * \return \b true if the name of both instances are the same or \b false otherwise.
   */
  bool isEqual(const NamedObject& other) const noexcept { return isEqual(other.m_name); }

  /**
   * \return the value of isEqual.
   */
  bool operator ==(const std::string& name) const noexcept { return isEqual(name); }

  /**
   * \return the value of isEqual.
   */
  bool operator ==(const NamedObject& other) const noexcept { return isEqual(other.m_name); }

  /**
   * Operator conversion.
   * \return Summarize information of this instance in a StreamString.
  */
  operator StreamString() const noexcept { return asString(); }

  /**
   * \return Summarize information of this instance in a StreamString.
  */
  virtual StreamString asString() const noexcept { StreamString result("basis.NamedObject { Name: "); return result << m_name << " }"; }

protected:
  /**
   * Constructor.
   * \param name Logical name
   */
  explicit NamedObject(const std::string& name) : m_name(name) {;}

private:
   const std::string m_name;
};

}
}


#endif

