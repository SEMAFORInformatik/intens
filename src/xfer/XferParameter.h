
#if !defined(XFER_PARAMETER_INCLUDED_H)
#define XFER_PARAMETER_INCLUDED_H

#include <string>

class XferParameter
{
/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum InputStatus
  { status_Changed
  , status_Unchanged
  , status_Bad
  };

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool read( std::istream &is ) = 0;
  virtual bool write( std::ostream &os ) = 0;
  virtual void getFormattedValue( std::string & ) = 0;
  virtual bool setFormattedValue( const std::string & ) = 0;
  virtual InputStatus checkFormat( const std::string & ) = 0;
  virtual bool convertValue( const std::string &text ) = 0;
  virtual void clear(){}
  virtual void clearRange(){}
  virtual bool isEmpty( const std::string &s ){
	  return s.find_first_not_of( ' ' ) == std::string::npos;
  }

};

#endif
