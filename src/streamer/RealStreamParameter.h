
#if !defined(REAL_STREAM_PARAMETER_H)
#define REAL_STREAM_PARAMETER_H

#include "streamer/DataStreamParameter.h"

//*************************************************
// REAL_STREAM_PARAMETER
//*************************************************

class RealStreamParameter: public DataStreamParameter {
 public:
  /// constructs a RealStreamParameter object
  RealStreamParameter( XferDataItem *ditem, int level
		       , int width, int prec, Scale *scale, bool thousand_sep
			   , bool mand, char delimiter, bool locale);
  RealStreamParameter( XferDataItem *ditem, int level
		       , StructStreamParameter *p, char delimiter, bool locale );

  /// Destructor
  virtual ~RealStreamParameter();

  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

  virtual void setValue( const std::string &v );
  virtual bool getValue( std::string &v )const;
  virtual void setValue( int v ){ setValue((double)v); }
  virtual bool getValue( int &v )const{
    double d; if(DataStreamParameter::getValue(d)){ v=static_cast<int>(d);return true;} return false;
  }
  virtual void setValue( double v ){ DataStreamParameter::setValue(v); }
  virtual bool getValue( double &v )const{ return DataStreamParameter::getValue(v); }

  virtual void putValues( StreamDestination *dest, bool transposed );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

 private:
  int putTransposedMatrix( std::vector<double>&vals, int ndims, int idim
                         , DataContainer::SizeList &dims, int indexnum );
  int putTransposedCellMatrix( std::vector<double>&vals, int ndims, int idim
			       , DataContainer::SizeList &dims, std::vector<int> &celldims
			       , int indexnum );
  int putMatrix( std::vector<double>&vals, int ndims, int idim
               , DataContainer::SizeList &dims, int indexnum );
  RealConverter m_conv;
  //  char m_delimiter;
};
#endif
