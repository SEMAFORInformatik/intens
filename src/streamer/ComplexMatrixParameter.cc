
#include "utils/Debugger.h"

#include "utils/NumLim.h"
#include "datapool/DataReference.h"
#include "streamer/ComplexMatrixParameter.h"
#include "gui/GuiIndex.h"
#include "xml/XMLDocumentHandler.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

ComplexMatrixDataParameter::ComplexMatrixDataParameter( XferDataItem *ditem
                                                      , int level
						      , int width
						      , int prec
						      , Scale *scale
						      , bool mandatory
							  , char delimiter
							  , bool locale)
  : MatrixDataParameter( ditem, level, 0, mandatory, delimiter, locale )
  , m_conv( width, prec, scale, true, !locale ){
}

ComplexMatrixDataParameter::ComplexMatrixDataParameter( XferDataItem *dref
                                                      , int level
						      , StructStreamParameter *p
							  , char delimiter
                              , bool locale)
  : MatrixDataParameter( dref, level, p, false, delimiter, locale )
, m_conv( 0, -1, 0, false, !locale ){
}

ComplexMatrixDataParameter::~ComplexMatrixDataParameter(){}

/* --------------------------------------------------------------------------- */
/* readMatrixValues --                                                         */
/* --------------------------------------------------------------------------- */

bool ComplexMatrixDataParameter::readMatrixValues( std::istream &istr
						   , int ndims, int idim, int indexnum, char delimiter ){
  BUG(BugStreamer,"ComplexMatrixDataParameter::readMatrixValues");

  dComplex c;
  int dims = m_dataitem->getMaxIndex( idim+indexnum );
  for( int i = 0; i < dims; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_conv.read( istr, c ) ){
      m_dataitem->setValue( c );
    }
    else{
      m_dataitem->setInvalid();
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeMatrixValues --                                                        */
/* --------------------------------------------------------------------------- */

bool ComplexMatrixDataParameter::writeMatrixValues( std::ostream &ostr
						  , int ndims
						  , DataContainer::SizeList &dims
						  , int idim, int indexnum ){
  BUG(BugStreamer,"ComplexMatrixDataParameter::writeMatrixValues");

  dComplex c;
  for( int i = 0; i < dims[idim]; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( c ) ){
      m_conv.write( c, ostr );
      ostr << " ";
    }
    else{
      ostr << "<NULL> ";
    }
  }
  //ostr << endl;
  return true;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void ComplexMatrixDataParameter::putValues( StreamDestination *dest, bool transpose ){
  BUG( BugStreamer, "ComplexMatrixDataParameter::putValues" );
  if( dest == 0 ) return;

  int indxnum = createDataItemIndexes();
  m_dataitem->setDimensionIndizes();
  DataContainer::SizeList dims;
  int numdims = m_dataitem->Data()->GetAllDataDimensionSize( dims );
  int nlevel = getLevel();

  BUG_MSG("Dimension Array: Size = " << numdims);

  for( int ii=0; ii < numdims; ii++ ){
    BUG_MSG("Size of Dimension " << (numdims - ii) << ": " << dims[ii]);
  }

  if( numdims == 0 ){
    if( !m_dataitem->isValid() ){
      BUG_MSG( "is not valid" );
      dest -> putValues(*this,std::vector<int>() , std::vector<dComplex>(), std::vector<int>() );
      removeDataItemIndexes();
      return;
    }
    double c;
    m_dataitem->getValue( c );
    BUG_MSG( c );
    dest -> putValues(*this,std::vector<int>(1,1) , std::vector<dComplex>(1,dComplex(c,0.0)), std::vector<int>() );
    removeDataItemIndexes();
    return;
  }

  XferDataItemIndex *inx=0;
  for( int i=0; i<numdims; i++ ){
    inx = m_dataitem->newDataItemIndex(nlevel);
    if( inx == 0 ){
      removeDataItemIndexes();
      m_dataitem->eraseDataItemIndex(nlevel);
      return;
    }
    inx -> setIndexNum( i );
    inx -> setLowerbound(0);
  }
  m_dataitem->setDimensionIndizes();

  std::vector<dComplex> vals;
  int k;
  if( transpose ){
    k=putTransposedMatrix( vals, numdims, numdims-1, dims, indxnum );
  }
  else {
    k=putMatrix( vals, numdims, 0, dims, indxnum );
  }
  dest -> putValues(*this, dims, vals, std::vector<int>() );
  removeDataItemIndexes();
  m_dataitem->eraseDataItemIndex(nlevel);
}

/* --------------------------------------------------------------------------- */
/* putTransposedMatrix --                                                      */
/* --------------------------------------------------------------------------- */

int ComplexMatrixDataParameter::putTransposedMatrix( std::vector<dComplex>&vals
						   , int ndims, int idim
						   , std::vector<int>&dims, int indexnum ){
  int i,k;

  if( idim > 0 ){
    int isize = dims[idim];
    int k=0;
    for( i = 0; i < isize; i++ ){
      m_dataitem->setIndex( idim+indexnum, i );
      if( putTransposedMatrix( vals
			       , ndims   // Dimensions
			       , idim - 1      // current Index in dim
			       , dims, indexnum ) > 0 ){
	k++;
      }
    }
    if( k<isize )
      dims[idim]=k;
    return k;
  }

  dComplex c;
  double dinf;
  int isize = dims[0];
  dinf=NumLim::infinity();
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( c ) ){
      vals.push_back( c );
    }
    else{
      vals.push_back( dComplex( dinf, dinf ) );
    }
    k++;
  }
  if( k>0 && k < isize ){
    dims[0]=k;
  }
  return k;
}

/* --------------------------------------------------------------------------- */
/* putMatrix --                                                                */
/* --------------------------------------------------------------------------- */

int ComplexMatrixDataParameter::putMatrix( std::vector<dComplex>&vals
					    , int ndims, int idim
					    , std::vector<int>&dims, int indexnum ){
  int i,k;

  if( idim < ndims-1 ){
    int isize = dims[idim];
    int k=0;
    for( i = 0; i < isize; i++ ){
      m_dataitem->setIndex( idim+indexnum, i );
      if( putMatrix( vals
		     , ndims   // Dimensions
		     , idim + 1      // current Index in dim
		     , dims, indexnum ) > 0 ){
		       k++;
		     }
    }
    if( k<isize )
      dims[idim]=k;
    return k;
  }

  dComplex c;
  double dinf;
  int isize = dims[idim];
  dinf=NumLim::infinity();
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( c ) ){
      vals.push_back( c );
    }
    else{
      vals.push_back( dComplex( dinf, dinf ) );
    }
    k++;
  }
  if( k>0 && k < isize ){
    dims[idim]=k;
  }
  return k;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void ComplexMatrixDataParameter::marshal( std::ostream &os ){
  os << "<ComplexMatrixDataParameter";
  if( getLevel() != 0 )
    os << " level=\"" << getLevel() << "\"";
  if( isMandatory() )
    os << " mand=\"true\"";
  if( isScalar() )
    os << " scalar=\"true\"";
  if( isCell() )
    os << " cell=\"true\"";
  os << ">\n";
  m_dataitem -> marshal( os );
  m_conv.marshal( os );
  os << "</ComplexMatrixDataParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *ComplexMatrixDataParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "ComplexMatrixDataParameter" ){
    GuiIndex::clearIndexContainer();
    std::string s;
    s = attributeList["mand"];
    if( !s.empty() )
      setMandatory();
    s = attributeList["scalar"];
    if( !s.empty() )
      setScalar( true );
    s = attributeList["cell"];
    if( !s.empty() )
      setCell( true );
    s=attributeList["level"];
    if( !s.empty() ){
      int iLevel;
      std::istringstream is( s );
      is >> iLevel;
      setLevel( iLevel );
    } else
      setLevel( 0 );
    return this;
  }
  else if( element == "ComplexConverter" ){
    return m_conv.unmarshal( element, attributeList );
  }
  else if( element == "XferDataItem" ){
    m_dataitem -> unmarshal( element, attributeList );
    return m_dataitem;
  }
  else if( element == "/ComplexMatrixDataParameter" ){
    GuiIndex::registerIndexContainer( this );
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void ComplexMatrixDataParameter::setText( const std::string &text ){
}
