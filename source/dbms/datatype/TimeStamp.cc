#include <stdio.h>

#include <coffee/dbms/datatype/TimeStamp.hpp>

#include <coffee/config/defines.hpp>


using namespace std;
using namespace coffee;
using namespace coffee::dbms;

datatype::TimeStamp::TimeStamp (const char* name, const Constraint::_v constraint)  :
   Date (name, datatype::Abstract::Datatype::TimeStamp, constraint),
   m_fractionalSecond (0)
{;}

datatype::TimeStamp::TimeStamp (const datatype::TimeStamp& other) :
   Date (other),
   m_fractionalSecond (other.m_fractionalSecond)
{
}

