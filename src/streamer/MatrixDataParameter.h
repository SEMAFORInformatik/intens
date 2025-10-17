
#if !defined(STREAMER_MATRIX_DATA_PARAMETER_H)
#define STREAMER_MATRIX_DATA_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/DataStreamParameter.h"
#include "datapool/DataContainer.h"

class XferDataItem;
class StructStreamParameter;

class MatrixDataParameter: public DataStreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  MatrixDataParameter( XferDataItem *ditem, int level, StructStreamParameter *p,
					   bool mandatory, char delimiter, bool locale );
  virtual ~MatrixDataParameter();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void clearRange( bool incl_itemattr=false );
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

  virtual size_t getSize()const;

  virtual void putValues( StreamDestination *dest, bool transposed ) = 0;
  virtual void fixupItemStreams( BasicStream &stream );
  virtual bool isValid(bool validErrorMsg=false);

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool readMatrixValues( std::istream &istr, int ndims, int idim, int indexnum, char delimiter=' ' ) = 0;
  virtual bool writeMatrixValues( std::ostream &ostr, int ndims, DataContainer::SizeList &dims
				  , int idim, int indexnum ) = 0;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  bool readMatrix( std::istream &istr, int ndims, int idim, int indexnum );
  bool writeMatrix( std::ostream &ostr, int ndims, DataContainer::SizeList &dims
                  , int idim, int indexnum );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
};

#endif
