
#if !defined(COMPLEX_STREAM_PARAMETER_H)
#define COMPLEX_STREAM_PARAMETER_H

#include "streamer/DataStreamParameter.h"

//*************************************************
// COMPLEX_STREAM_PARAMETER
//*************************************************

class ComplexStreamParameter: public DataStreamParameter {
 public:
  /// constructs a ComplexStreamParameter object
  ComplexStreamParameter( XferDataItem *ditem, int level, int width, int prec, Scale *scale, bool mand, bool locale);
  ComplexStreamParameter( XferDataItem *ditem, int level, StructStreamParameter *p, bool locale );

  /// Destructor
  virtual ~ComplexStreamParameter();

  virtual size_t getDimensionSize(int ndim) const;
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

  virtual void setValue( const std::string &v );
  virtual bool getValue( std::string &v )const;
  virtual void setValue( int v ){ setValue((double)v); }
  virtual bool getValue( int &v )const{
    double d; if(getValue(d)){ v=static_cast<int>(d);return true;} return false;
  }
  virtual void setValue( double v );
  virtual bool getValue( double &v )const;

  virtual void putValues( StreamDestination *dest, bool transposed );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

 private:
  int putTransposedMatrix( std::vector<dComplex>&vals, int ndims, int idim
                         , DataContainer::SizeList &dims, int indexnum );
  int putMatrix( std::vector<dComplex>&vals, int ndims, int idim
               , DataContainer::SizeList &dims, int indexnum );
  bool             m_complete;
  dComplex         m_val;
  ComplexConverter m_conv;
};
#endif
