
#if !defined(STREAMER_STRING_MATRIX_PARAMETER_H)
#define STREAMER_STRING_MATRIX_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/MatrixDataParameter.h"
#include "xfer/XferConverter.h"

class XferDataItem;

class StringMatrixDataParameter: public MatrixDataParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a StringVectorDataParameter object
  StringMatrixDataParameter( XferDataItem *ditem, int level
			     , int width=0
				 , char delimiter=' ',bool locale=false,bool mandatory=false);
  StringMatrixDataParameter( XferDataItem *dref, int level
			     , StructStreamParameter *p
			     , char delimiter=' '
				 , bool locale=false );

  /// Destructor
  virtual ~StringMatrixDataParameter();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void putValues( StreamDestination *dest, bool transposed );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool readMatrixValues( std::istream &istr, int ndims, int idim, int indexnum, char delimiter=' ' );
  virtual bool writeMatrixValues( std::ostream &ostr, int ndims, DataContainer::SizeList &dims
				  , int idim, int indexnum );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  int putTransposedMatrix( std::vector<std::string>&vals, int ndims, int idim
                         , DataContainer::SizeList  &dims, int indexnum );
  int putMatrix( std::vector<std::string>&vals, int ndims, int idim
               , DataContainer::SizeList &dims, int indexnum );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  MatrixStringConverter     m_conv;
  //  char m_delimiter;
};

#endif
