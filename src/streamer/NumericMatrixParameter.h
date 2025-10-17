
#if !defined(STREAMER_NUMERIC_MATRIX_PARAMETER_H)
#define STREAMER_NUMERIC_MATRIX_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/MatrixDataParameter.h"
#include "xfer/XferConverter.h"

class XferDataItem;

class NumericMatrixDataParameter: public MatrixDataParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a NumericMatrixDataParameter object
  NumericMatrixDataParameter( XferDataItem *ditem, int level
			      , int width
			      , int prec
			      , Scale *scale
			      , bool thousand_sep
			      , bool mandatory
			      , char delimiter
				  , bool locale);
  NumericMatrixDataParameter( XferDataItem *dref, int level
			      , StructStreamParameter *p
                  , char delimiter
				  , bool locale);

  /// Destructor
  virtual ~NumericMatrixDataParameter();

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
  int putTransposedMatrix( std::vector<double>&vals, int ndims, int idim
                         , DataContainer::SizeList &dims, int indexnum );
  int putTransposedCellMatrix( std::vector<double>&vals
			       , int ndims, int idim
			       , DataContainer::SizeList &dims
			       , std::vector<int> &celldims
			       , int indexnum );
  int putMatrix( std::vector<double>&vals, int ndims, int idim
               , DataContainer::SizeList &dims, int indexnum, std::vector<int> &inds );
  int putCellMatrix( std::vector<double>&vals
		     , int ndims, int idim
		     , DataContainer::SizeList &dims
		     , std::vector<int> &celldims
		     , int indexnum );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  RealConverter          m_conv;
};

#endif
