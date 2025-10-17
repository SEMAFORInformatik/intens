
#if !defined(XFER_PARAMETER_STRING_INCLUDED_H)
#define XFER_PARAMETER_STRING_INCLUDED_H

#include "xfer/XferConverter.h"
#include "xfer/XferDataParameter.h"

class XferParameterString : public XferDataParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XferParameterString(DataReference *dref=0,int width=0, char delimiter=' ');
  XferParameterString( XferParameterString &param );
  virtual ~XferParameterString(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual XferDataParameter *clone();
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void getFormattedValue( std::string & );
  virtual bool setFormattedValue( const std::string & );
  virtual InputStatus checkFormat( const std::string & );
  virtual bool convertValue( const std::string &text );
  virtual bool setLength( int len );
  virtual int getLength(){ return m_conv.getWidth(); }
  virtual bool setScalefactor( double scale ){ return false; }
  virtual bool setScalefactor( Scale *scale );
  virtual DATAAttributeMask getAttributes( TransactionNumber t ){ return Data()->getAttributes( t ); }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  virtual bool isConverterUpdated( TransactionNumber trans, bool gui = false ){
    return m_conv.isUpdated( trans, gui );
  }
/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  StringConverter m_conv;
  char            m_delimiter;
};

#endif
