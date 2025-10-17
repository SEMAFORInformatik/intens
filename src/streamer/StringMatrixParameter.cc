
#include "utils/Debugger.h"

#include "datapool/DataReference.h"
#include "streamer/StringMatrixParameter.h"
#include "gui/GuiIndex.h"
#include "xml/XMLDocumentHandler.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

StringMatrixDataParameter::StringMatrixDataParameter( XferDataItem *ditem
                                                    , int level
						    , int width
						    , char delimiter
						    , bool locale
						    , bool mandatory)
  : MatrixDataParameter( ditem, level, 0, mandatory, delimiter, locale )
  , m_conv( width, true ) {
}

StringMatrixDataParameter::StringMatrixDataParameter( XferDataItem *dref
                                                    , int level
						    , StructStreamParameter *p
						    , char delimiter
							, bool locale)
  : MatrixDataParameter( dref, level, p, false, delimiter, locale ){
}

StringMatrixDataParameter::~StringMatrixDataParameter(){}

/* --------------------------------------------------------------------------- */
/* readMatrixValues --                                                         */
/* --------------------------------------------------------------------------- */

bool StringMatrixDataParameter::readMatrixValues( std::istream &istr
						  , int ndims, int idim, int indexnum, char delimiter ){
  BUG(BugStreamer,"StringMatrixDataParameter::readMatrixValues");

  std::string s;
  int dims = m_dataitem->getMaxIndex( idim+indexnum );
  char c;
  c=istr.get();
  while( c == ' ' || c == '\n' && c != EOF )
    c=istr.get();
  istr.putback(c);
  for( int i = 0; i < dims; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    bool _ok = m_conv.read( istr, s, m_delimiter );
    if( _ok && s != "<NULL>" ){
      m_dataitem->setValue( s );
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

bool StringMatrixDataParameter::writeMatrixValues( std::ostream &os
						  , int ndims
						  , DataContainer::SizeList &dims
						  , int idim, int indexnum ){
  BUG(BugStreamer,"StringMatrixDataParameter::writeMatrixValues");
  std::string s;
  for( int i = 0; i < dims[idim]; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( s ) ){
      m_conv.write( s, os );
      int width = m_conv.getWidth();
      if( width == 0 )
	os << (m_delimiter == '\0' ? ' ' : m_delimiter);
    }
    else{
      int width = m_conv.getWidth();
      if( width == 0 )
	os << (m_delimiter == '\0' ? ' ' : m_delimiter);
      else
	for( int n = 0; n < width; ++n )
	  os << ' ';

    }
  }

  return true;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void StringMatrixDataParameter::putValues( StreamDestination *dest, bool transpose ){
  BUG( BugStreamer, "StringMatrixDataParameter::putValues" );
  if( dest == 0 ) return;

  int indxnum = createDataItemIndexes();
  m_dataitem->setDimensionIndizes();
  DataContainer::SizeList dims;
  int numdims = m_dataitem->Data()->GetAllDataDimensionSize( dims );
  int nlevel = getLevel();

  BUG_MSG("Dimension Array: Size = " << numdims);
  for( int ii=0; ii < numdims; ii++ ){
    BUG_MSG("Size of Dimension " << (numdims - ii) << ": " << dims[ii]);
//     cerr << "Size of Dimension " << (numdims - ii) << ": " << dims[ii]<< endl;
  }

  if( numdims == 0 ){
    if( !m_dataitem->isValid() ){
      BUG_MSG(" NOT VALID! ");
      dest -> putValues(*this,std::vector<int>() , std::vector<std::string>() );
      removeDataItemIndexes();
      return;
    }
    std::string s;
    m_dataitem->getValue( s );
    BUG_MSG( "val='" << s <<"'");
    dest -> putValues(*this,std::vector<int>(1,1) , std::vector<std::string>(1,s) );
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

  std::vector<std::string> vals;
  int k;
  if( transpose ){
    k=putTransposedMatrix( vals, numdims, numdims-1, dims, indxnum );
  }
  else{
    k=putMatrix( vals, numdims, 0, dims, indxnum );
  }
  dest -> putValues(*this, dims, vals );
  removeDataItemIndexes();
  m_dataitem->eraseDataItemIndex(nlevel);
}

/* --------------------------------------------------------------------------- */
/* putTransposedMatrix --                                                      */
/* --------------------------------------------------------------------------- */

int StringMatrixDataParameter::putTransposedMatrix( std::vector<std::string>&vals
						    , int ndims, int idim
						    , std::vector<int>&dims
						    , int indexnum){
//   cerr << "StringMatrixDataParameter::putTransposedMatrix\n";
  BUG( BugStreamer, "StringMatrixDataParameter::putTransposedMatrix" );
  int i,k;

  if( idim > 0 ){
    int isize = dims[idim];
    k=0;
    for( i = 0; i < isize; i++ ){
      m_dataitem->setIndex( idim+indexnum, i );
      if( putTransposedMatrix( vals
			       , ndims   // Dimensions
			       , idim - 1      // current Index in dim
			       , dims
			       , indexnum ) > 0 ){
	k++;
      }
    }
    if( k<isize)
      dims[idim]=k;
    return k;
  }

  std::string s,sinf;
  int isize=dims[0];
  k=0;
  for( i = 0; i < isize; i++ ){
//     cerr << "setIndex " << indexnum << ", " << i << " ==> ";
    m_dataitem->setIndex( indexnum, i );
    if( m_dataitem->getValue( s ) ){
//       cerr << s << endl;
      vals.push_back( s );
    }
    else{
//       cerr << "(empty)" << endl;
      vals.push_back( sinf );
    }
    k++;
  }
  if( k>0 && k<isize){
    dims[0]=k;
  }
  return k;
}

/* --------------------------------------------------------------------------- */
/* putMatrix --                                                                */
/* --------------------------------------------------------------------------- */

int StringMatrixDataParameter::putMatrix( std::vector<std::string>&vals
					   , int ndims, int idim
					   , std::vector<int>&dims, int indexnum ){
  int i,k;

  if( idim < ndims-1 ){
    int isize = dims[idim];
    k=0;
    for( i = 0; i < isize; i++ ){
      m_dataitem->setIndex( idim+indexnum, i );
      if( putMatrix( vals
		  , ndims   // Dimensions
		  , idim + 1      // current Index in dim
		 , dims
		  , indexnum   ) > 0 ){
	k++;
      }
    }
    if( k<isize)
      dims[idim]=k;
    return k;
  }

  std::string s,sinf;
  int isize=dims[idim];
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( s ) ){
      vals.push_back( s );
    }
    else{
      vals.push_back( sinf );
    }
    k++;
  }
  if( k>0 && k<isize){
    dims[0]=k;
  }
  return k;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void StringMatrixDataParameter::marshal( std::ostream &os ){
  os << "<StringMatrixDataParameter";
  if( getLevel() != 0 )
    os << " level=\""<<getLevel()<<"\"";
  if( isMandatory() )
    os << " mand=\"true\"";
  if( isScalar() )
    os << " scalar=\"true\"";
  if( isCell() )
    os << " cell=\"true\"";
  os << " delimiter=\"" << m_delimiter << "\"";
  os << ">\n";
  m_dataitem -> marshal( os );
  m_conv.marshal( os );
  os << "</StringMatrixDataParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *StringMatrixDataParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "StringMatrixDataParameter" ){
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
    s = attributeList["level"];
    if( !s.empty() ){
      int iLevel;
      std::istringstream is( s );
      is >> iLevel;
      setLevel( iLevel );
    } else
      setLevel( 0 );
    s = attributeList["delimiter"];
    m_delimiter = s.size() ? s[0] : ' ';
    return this;
  }
  else if( element == "StringConverter" ){
    m_conv.unmarshal( element, attributeList );
    return this;
  }
  else if( element == "XferDataItem" ){
    m_dataitem -> unmarshal( element, attributeList );
    return m_dataitem;
  }
  else if( element == "/StringMatrixDataParameter" ){
    GuiIndex::registerIndexContainer( this );
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void StringMatrixDataParameter::setText( const std::string &text ){
}
