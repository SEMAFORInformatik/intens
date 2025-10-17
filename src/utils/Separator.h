
#if !defined(_UTILITIES_SEPARATOR_H_)
#define _UTILITIES_SEPARATOR_H_

#include <string>
#include <iostream>

///
class Separator
{
// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //
public:
  Separator( char first='(', char next=',', char last=')' )
    : m_first( first )
    , m_next( next )
    , m_last( last ){
  }
  virtual ~Separator(){}

// ================================================================ //
// public Functions                                                 //
// ================================================================ //
public:
  void write( std::ostream &ostr ){
    if( m_first != 0 ) ostr << m_first;
    m_first = m_next;
  }
  void init( char first='(', char next=',', char last=')' ){
    m_first = first;
    m_next  = next;
    m_last  = last;
  }
  std::string close(){
    std::string s;
    if( m_first != m_next ) if( m_first != 0 ) s = m_first;
    if( m_last != 0 ) s += m_last;
    m_first = 0;
    m_next  = 0;
    m_last  = 0;
    return s;
  }

// ================================================================ //
// private Data                                                     //
// ================================================================ //
protected:
  char   m_first;
  char   m_next;
  char   m_last;
};

inline std::ostream &operator<<( std::ostream &ostr,Separator &sep ){
  sep.write( ostr );
  return ostr;
}

#endif
