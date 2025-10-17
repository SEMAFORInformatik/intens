
#include "utils/Debugger.h"

#include "app/DataPoolIntens.h"
#include "datapool/DataReference.h"
#include "streamer/MatrixDataParameter.h"
#include "streamer/Stream.h"
#include "xfer/XferDataItem.h"

#define SAY(X) // std::cout << "SAY: " << X << std::endl;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

MatrixDataParameter::MatrixDataParameter( XferDataItem *ditem,
					  int level,
					  StructStreamParameter *p,
					  bool mandatory,
					  char delimiter,
					  bool locale )
  : DataStreamParameter( ditem, level, p, mandatory, delimiter, locale ){
}

MatrixDataParameter::~MatrixDataParameter() {}

/* --------------------------------------------------------------------------- */
/* clearRange --                                                               */
/* --------------------------------------------------------------------------- */

void MatrixDataParameter::clearRange( bool incl_itemattr ){
  if( m_dataitem != 0 ){
    m_dataitem->clearRangeMaxLevel( incl_itemattr );
  }
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool MatrixDataParameter::read( std::istream &istr ){
  BUG(BugStreamer,"MatrixDataParameter::read");
  if( m_continuousUpdate ){
    throw bad_call();
  }
  int numdims=0;
  int imax = 0;
  if( !(istr >> numdims) ){
    return false; // read failed
  }
  int dimensionSummery = numdims;
  int nlevel  = getLevel();
  int indxnum = createDataItemIndexes();

  XferDataItemIndex *inx = 0;
  for( int i=0; i<numdims; i++ ){
    inx = m_dataitem->newDataItemIndex( nlevel );
    assert( inx != 0 );

    inx -> setIndexNum( i );
    inx -> setLowerbound(0);
    if( !(istr >> imax) ){
      BUG_EXIT("Bad Dimension Size in Input");
      removeDataItemIndexes();
      return false;
    }
    inx -> setUpperbound( imax );
    dimensionSummery = dimensionSummery*imax;
  }
  m_dataitem->setDimensionIndizes();
  if( dimensionSummery == 0 ){
    char c = istr.get();
    while( c != EOF && c != '\n' )
      c = istr.get();
    removeDataItemIndexes();
    m_dataitem->eraseDataItemIndex( nlevel );
    return true;
  }
  bool rslt = readMatrix( istr, numdims, 0, indxnum );
  removeDataItemIndexes();
  m_dataitem->eraseDataItemIndex( nlevel );
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* readMatrix --                                                               */
/* --------------------------------------------------------------------------- */

bool MatrixDataParameter::readMatrix( std::istream &istr
                                    , int ndims, int idim, int indexnum ){
  BUG(BugStreamer,"MatrixDataParameter::readMatrix");

  if( idim < (ndims-1) ){
    int dims = m_dataitem->getMaxIndex( idim+indexnum );
    for( int i = 0; i < dims; i++ ){
      m_dataitem->setIndex( idim+indexnum, i );
      readMatrix( istr        // input
		, ndims       // Dimensions
	        , idim + 1    // current Index in dim
                , indexnum
	        );
    }
    return true;
  }

  // Nun sind wir beim letzten Index angelangt. Nun endlich werden die Werte
  // gelesen und im Datapool abgelegt.
  return readMatrixValues( istr, ndims, idim, indexnum );
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool MatrixDataParameter::write( std::ostream &ostr ){
  BUG(BugStreamer,"MatrixDataParameter::write");
  int nlevel=getLevel();
  m_dataitem->eraseDataItemIndex(nlevel);
  int indxnum = createDataItemIndexes();
  DataContainer::SizeList dims;
  m_dataitem->setDimensionIndizes();
  int numdims = m_dataitem->Data()->GetAllDataDimensionSize( dims );

  if( numdims == 0 ){
    ostr << "0 ";
    return true;
  }
  createIndexes( dims );
  ostr << numdims << " ";
  for( int i=0; i<numdims; i++ ){
    ostr << dims[i] << " ";
  }
  ostr << std::endl;
  bool b = writeMatrix( ostr, numdims, dims, 0, indxnum );
  removeDataItemIndexes();
  m_dataitem->eraseDataItemIndex( nlevel );
  return b;
}

/* --------------------------------------------------------------------------- */
/* writeMatrix --                                                              */
/* --------------------------------------------------------------------------- */

bool MatrixDataParameter::writeMatrix( std::ostream &ostr
				     , int ndims, DataContainer::SizeList &dims
				     , int idim, int indexnum ){
  BUG(BugStreamer,"MatrixDataParameter::writeMatrix");

  if( idim < ndims-1 ){
    for( int i = 0; i < dims[idim]; i++ ){
      m_dataitem->setIndex( idim+indexnum, i );
      writeMatrix( ostr     // output
		 , ndims    // Dimensions
		 , dims     // Dimension Array
		 , idim + 1 // current Index in dim
		 , indexnum
		 );
    }
    return true;
  }

  // Nun sind wir beim letzten Index angelangt. Nun endlich werden die Werte
  // aus dem Datapool in den Outputstream geschrieben.
  return writeMatrixValues( ostr, ndims, dims, idim, indexnum );
}

/* --------------------------------------------------------------------------- */
/* fixupItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void MatrixDataParameter::fixupItemStreams( BasicStream &stream ){
  DataPoolIntens::Instance().fixupItemStreams( stream, m_dataitem->Data() );
}


/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool MatrixDataParameter::isValid(bool validErrorMsg){
  BUG( BugStreamer, "isValid");

  // simple query to childs
  if( DataStreamParameter::isValid(validErrorMsg) ) {
    return true;
  }

  // is it really invalid?
  // check if 'write'-method has no valid output
  std::ostringstream os;
  write(os);
  size_t pos;
  std::string s = os.str();
  if (s.size() > 0 && s.find("1 0") != 0 ) {
    BUG_EXIT("TRUE (empty output stream)");
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

size_t MatrixDataParameter::getSize()const{
  BUG(BugStreamer,"MatrixDataParameter::getSize");
  int numdims = m_dataitem->getNumDimensions();
  size_t size=1;
  for( int i=0; i<numdims; i++ ){
    size= size*m_dataitem->getDimensionSize(i);
  }
  return size+1+numdims;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void MatrixDataParameter::marshal( std::ostream &os ){
  std::cerr<<"MatrixDataParameter::marshal   not implemented yet !!!"<<std::endl;
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *MatrixDataParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void MatrixDataParameter::setText( const std::string &text ){
}
