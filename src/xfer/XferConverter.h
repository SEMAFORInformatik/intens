#ifndef XFER_CONVERTER_H
#define XFER_CONVERTER_H

#include <complex>
#include <iostream>
#include <string>
#include <sstream>
#include "datapool/DataPool.h"
#include "xfer/Scale.h"
#include "xml/Serializable.h"

class GuiIndexListener;


class Converter: public Serializable
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  Converter( bool ignore_newline );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setWidth( int w );
  int getWidth()const { return m_width; }

  /** set the adjustement
   */
  void setAdjustLeft( bool a ) { m_adjustleft = a; }

  /** returns true if left adjusted
   */
  bool isLeftAdjusted() const { return m_adjustleft; }

  /** writes an INVALID pattern to the ostream
   */
  bool writeInvalid( std::ostream &os ) const;

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

  void setScaleIndex( int wildcardNumberBackwards, int value );
  virtual Scale *scale() const { return 0; }

  /** returns true if Scale is updated
   */
  virtual bool isUpdated( TransactionNumber trans, bool gui = false )=0;

protected:
  int deblank( std::istream &, const char delimiter=' '
	       , bool remove_multiple_delimiter=false ) const;
  bool readInfinite( char c, std::istream &is ) const {
    return  readPattern( c, is, "INF", 3 ); }
  bool readInvalid( char c, std::istream &is ) const {
    return  readPattern( c, is, "<NULL>", 6 ); }
private:
  bool readPattern( char c, std::istream &is, const char *, int len ) const;

private:
  bool   m_ignore_newline;
  int    m_width;
  bool   m_adjustleft;
};


/**
 class RealConverter provides facilities for formatted extraction
  and insertion of double values
 */
class RealConverter : public Converter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// Constructor
  RealConverter( int width, int prec, Scale *scale, bool ignore_newline, bool default_radix_char );
  RealConverter( const RealConverter &c )
    : Converter( false ){
    m_prec = c.m_prec;
    m_scale = c.scale() != 0 ? c.scale()->copy() : 0;
    m_thousand_sep = c.m_thousand_sep;
    m_decimal_point = c.m_decimal_point;
    m_default_radix_char = c.m_default_radix_char;
    m_appendDot = c.m_appendDot;
    setWidth( c.getWidth() );
    setAdjustLeft(c.isLeftAdjusted());
  }
  virtual ~RealConverter(){ if( m_scale != 0 ) delete m_scale; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// extracts characters from istr and converts them to the double value d
  bool read( std::istream &istr, double &d, const char delimiter=' ' ) const ;
  bool readAll( std::istream &istr, double &d, char delimiters=' ' ) const ;

  /// converts the value d into characters and inserts them into ostr
  bool write( double d, std::ostream &ostr ) const;

  virtual void setPrecision( int p ) { m_prec = p; }
  //  void setScale( double s ) { m_scale->setValue( s ); }
  void setScale( Scale *scale ){ if( m_scale != 0 ) delete m_scale; m_scale = scale; }

  void setThousandSep(char s='\''){ m_thousand_sep=s; }
  /** returns precision
   */
  int getPrecision() const { return m_prec; }
  /** returns the scale factor
   */

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

  double getScale() const { return m_scale != 0 ? m_scale->getValue() : 1. ; }
  double getShift() const { return m_scale != 0 ? m_scale->getShift() : 0; }

  virtual Scale *scale() const { return m_scale; }
  virtual bool isUpdated( TransactionNumber trans, bool gui = false ){
    return m_scale != 0 ? m_scale->isUpdated( trans, gui ) : false;
  }
  /** appends dot on output */
  void setAppendDot( ){ m_appendDot=true; }
  DATAAttributeMask getAttributes( TransactionNumber t );

  void registerIndex( GuiIndexListener *listener );
  void unregisterIndex( GuiIndexListener *listener );
  bool acceptIndex( const std::string &, int );
  bool setIndex( const std::string &, int );
  static char decimalPoint();
  static char defaultDecimalPoint();
  static void resetDecimalPoint();

 protected:
  char getDecimalPoint() const;
private:
  int    m_prec;
  Scale *m_scale;
  char   m_thousand_sep;
  bool   m_default_radix_char;
  char   m_decimal_point;
  bool   m_appendDot;     /// appends a dot on output if true
};

/**
 class ComplexConverter provides facilities for formatted extraction
  and insertion of dComplex values
 */
class ComplexConverter : public RealConverter
{
 /*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// Constructor
  ComplexConverter( int width, int prec, Scale *scale, bool ignore_newline, bool default_radix_char );
  ComplexConverter( const ComplexConverter &c )
    : RealConverter( 0, -1, 0, false, c.m_default_radix_char ){
    setPrecision( c.getPrecision() );
    setScale( c.scale() != 0 ? c.scale()->copy() : 0 );
    setWidth( c.getWidth() );
    setAdjustLeft(c.isLeftAdjusted());
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// extracts characters from istr and converts them to the double value c
  bool read( std::istream &istr, std::complex<double> &c ) const ;
  bool readAll( std::istream &istr, std::complex<double> &c, char delimiter=' ' ) const ;

  /// converts the value c into characters and inserts them into ostr
  bool write( std::complex<double> &c, std::ostream &ostr ) const;

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

 private:
  bool m_default_radix_char;
};

/**
 class IntConverter provides facilities for formatted extraction
  and insertion of int values
 */
class IntConverter : public Converter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// Constructor
  IntConverter( int width, Scale *scale, bool ignore_newline );
  IntConverter( const IntConverter &c )
    : Converter( false ){
    m_scale = c.scale() != 0 ? c.scale()->copy() : 0;
    setWidth( c.getWidth() );
    setAdjustLeft(c.isLeftAdjusted());
  }

  virtual ~IntConverter(){ if( m_scale != 0 ) delete m_scale; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// extracts characters from istr and converts them to the int value i
  bool read( std::istream &istr, int &i, const char delimiter=' ' );
  bool readAll( std::istream &istr, int &i, const char delimiter=' ' );

  /// converts the value i into characters and inserts them into ostr
  bool write( int i, std::ostream &ostr );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

  /** returns the scale factor
   */
  double getScale() const { return m_scale != 0 ? m_scale->getValue() : 1; }
  virtual Scale *scale() const { return m_scale; }
  void setScale( Scale *scale ){ if( m_scale != 0 ) delete m_scale; m_scale = scale; }
  double getShift() const { return m_scale != 0 ? m_scale->getShift() : 0; }
  virtual bool isUpdated( TransactionNumber trans, bool gui = false ){
    return m_scale != 0 ? m_scale->isUpdated( trans, gui ) : false;
  }
  DATAAttributeMask getAttributes( TransactionNumber t ){
    return m_scale != 0 ? m_scale->getAttributes( t ) : 0;
  }

  void registerIndex( GuiIndexListener *listener );
  void unregisterIndex( GuiIndexListener *listener );
  bool acceptIndex( const std::string &, int );
  bool setIndex( const std::string &, int );

private:
  Scale *m_scale;
};

/**
 class StringConverter provides facilities for formatted extraction
  and insertion of string values
 */
class StringConverter : public Converter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// Constructor
  StringConverter( int width=0, bool ignore_newline=false );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// extracts characters from istr and converts them to the string value s
  virtual bool read( std::istream &istr, std::string &s, const char delimiter ) const ;
  bool readAll( std::istream &istr, std::string &s, const char delimiter=' ' ) const ;

  /// converts the value s into characters and inserts them into ostr
  bool write( const std::string &s, std::ostream &ostr ) const;

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

  virtual bool isUpdated( TransactionNumber trans, bool gui = false ){
    return false;
  }
  /** sets the Index of scale dataitem if there is a wildcard
   */
 private:
};


class MatrixStringConverter : public StringConverter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  MatrixStringConverter( int width=0, bool ignore_newline=false )
    : StringConverter( width, ignore_newline ){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool read( std::istream &istr, std::string &s, const char delimiter ) const ;
};

#endif
