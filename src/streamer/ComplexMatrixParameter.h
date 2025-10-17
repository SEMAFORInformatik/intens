
#if !defined(STREAMER_COMPLEX_MATRIX_PARAMETER_H)
#define STREAMER_COMPLEX_MATRIX_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/MatrixDataParameter.h"
#include "xfer/XferConverter.h"

class XferDataItem;

class ComplexMatrixDataParameter: public MatrixDataParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a ComplexMatrixDataParameter object
  ComplexMatrixDataParameter( XferDataItem *dref, int level
			      , int width
			      , int prec
			      , Scale *scale
			      , bool mandatory
			      , char delimiter
				  , bool locale );

  ComplexMatrixDataParameter( XferDataItem *dref, int level, StructStreamParameter *p,
							  char delimiter, bool locale );

  /// Destructor
  virtual ~ComplexMatrixDataParameter();

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
  int putTransposedMatrix( std::vector<dComplex>&vals, int ndims, int idim
                         , DataContainer::SizeList &dims, int indexnum );
  int putMatrix( std::vector<dComplex>&vals, int ndims, int idim
               , DataContainer::SizeList &dims, int indexnum );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  ComplexConverter     m_conv;
};

#endif
