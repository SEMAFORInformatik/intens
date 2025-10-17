// ****************************************************************
// DataException
//
// $Id: DataException.cpp 1.18 2012/09/28 12:53:23Z bho Exp $
// ****************************************************************

#include "DataException.h"
#include <sstream>

// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //

DataException::DataException( const std::string &func
                            , const std::string &obj
	                          , const std::string &msg
                            , const std::string &file
                            , int line )
{
  std::ostringstream what;
  if( !obj.empty() ){
    what << obj << "::";
  }
  what << func << " in " << file << " at @" << line << ": " << msg << '\0';
  m_what = what.str();
}

DataException::DataException( const DataException &ex )
: m_what( ex.m_what ){
}

DataException::~DataException() throw() {
}

// ================================================================ //
// Member Functions                                                 //
// ================================================================ //

// ---------------------------------------------------------------- //
// operator <<  --                                                  //
// ---------------------------------------------------------------- //

std::ostream &operator<<( std::ostream &ostr, const DataException &ex ) {
  ostr << ex.get_what();
  return ostr;
}

// ---------------------------------------------------------------- //
// what --                                                          //
// ---------------------------------------------------------------- //

const char* DataException::what() const throw() {
  return m_what.c_str();
}

// ---------------------------------------------------------------- //
// get_what --                                                      //
// ---------------------------------------------------------------- //

const std::string DataException::get_what() const{
  return m_what;
}
