
#if !defined(XFER_PARAMETER_COMPLEX_INCLUDED_H)
#define XFER_PARAMETER_COMPLEX_INCLUDED_H

#include "xfer/XferDataParameter.h"
#include "xfer/XferConverter.h"

class XferParameterComplex : public XferDataParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XferParameterComplex( DataReference *dref
		      , int width, int prec, Scale *scale, bool default_radix_char );
  XferParameterComplex( XferParameterComplex &param );
  virtual ~XferParameterComplex(){}

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
  virtual bool setPrecision( int );
  virtual bool setScalefactor( Scale *scale );
  virtual DATAAttributeMask getAttributes( TransactionNumber t );

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
  ComplexConverter m_conv;
};

#endif
