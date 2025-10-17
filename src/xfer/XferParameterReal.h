
#if !defined(XFER_PARAMETER_REAL_INCLUDED_H)
#define XFER_PARAMETER_REAL_INCLUDED_H

#include "xfer/XferDataParameter.h"
#include "xfer/XferConverter.h"

class XferParameterReal : public XferDataParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XferParameterReal( DataReference *dref
		   , int width, int prec, Scale *scale, bool default_radix_char
		   , char delimiter=' ' );
  XferParameterReal( XferParameterReal &param );
  virtual ~XferParameterReal(){}

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
  virtual int  getPrecision();
  virtual bool setThousandSep();
  virtual bool setScalefactor( Scale *scale );
  virtual Scale* getScalefactor();
  virtual DATAAttributeMask getAttributes( TransactionNumber t );
  void setAppendDot(){ m_conv.setAppendDot(); }

  virtual void registerIndex( GuiIndexListener *listener );
  virtual void unregisterIndex( GuiIndexListener *listener );
  virtual bool acceptIndex( const std::string &, int );
  virtual bool setIndex( const std::string &, int );

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
  RealConverter m_conv;
  char          m_delimiter;
};

#endif
