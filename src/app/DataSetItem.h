#if !defined(DATASETITEM_INCLUDED_H)
#define DATASETITEM_INCLUDED_H

#include <string>

class DataSetItem
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  DataSetItem()
    : m_double( 0.0 ){
  }

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  void setValue( const std::string &name, double d )        { m_name = name; m_double = d; }
  void setValue( const std::string &name, const std::string &s ) { m_name = name; m_string = s; }

  const std::string &Name()              { return m_name; }
  double getReal()                  { return m_double; }
  const std::string &getString()         { return m_string; }

  bool isEqual( double d )          { return d == m_double; }
  bool isEqual( int i )             { return (double)i == m_double; }
  bool isEqual( const std::string &s )   { return m_string.empty() ? s == m_name : s == m_string; }

/*******************************************************************************/
/* private Data                                                                */
/*******************************************************************************/
private:
  std::string       m_name;
  double       m_double;
  std::string       m_string;

};

#endif
