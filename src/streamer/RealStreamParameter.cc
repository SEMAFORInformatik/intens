
#include <limits>
#include <cmath>
#include "utils/NumLim.h"

#include "utils/Debugger.h"

#include "datapool/DataReference.h"
#include "streamer/RealStreamParameter.h"
#include "gui/GuiIndex.h"
#include "xml/XMLDocumentHandler.h"

RealStreamParameter::RealStreamParameter(XferDataItem *ditem, int level
	, int width, int prec, Scale *scale, bool thousand_sep
    , bool mand, char delimiter, bool locale):
  DataStreamParameter(ditem,level,0,mand, delimiter, locale)
  , m_conv( width, prec, scale, false, !locale ) {
  if( thousand_sep )
    m_conv.setThousandSep();
  m_conv.setAppendDot();
}

RealStreamParameter::RealStreamParameter( XferDataItem *ditem, int level
					  , StructStreamParameter *p
					  , char delimiter, bool locale ):
  DataStreamParameter( ditem, level, p, false, delimiter, locale )
  , m_conv( 0, -1, 0, false, true ) {
}

RealStreamParameter::~RealStreamParameter(){}

bool RealStreamParameter::read( std::istream &is ){
  BUG_PARA(BugStreamer, "RealStreamParameter::read( istream & )", getName() );
  double d;
  bool rslt = true;
  // int indxnum=createDataItemIndexes();
  if( m_conv.read( is, d, m_delimiter ) ){
    BUG_MSG( d );
    if (!std::isnan(d) && d!=std::numeric_limits<double>::quiet_NaN()) {
      m_dataitem->setValue(d);
    }
    BUG_MSG( "std::isnan["<<std::isnan(d)<<"] varname:'" <<  m_dataitem->getFullName(true) << "' val='" << d <<"'" );
    //    cerr << m_dataitem -> getFullName( true ) << " = " << d << endl;
  }
  // Ein 'nan' wird als undefinierter Wert interpretiert ! ( Walter lässt Grüssen )
  else{
#if ( __GNUC__ < 3 )
    std::streampos pos = is.tellg();
#else
    std::istream::pos_type pos = is.tellg();
#endif
    char c= '\n';
    int len = m_conv.getWidth(), x=0;
    while( is.get(c) ){
      if( !( c == ' ' || c == '\t' || c == '-' || c == m_delimiter) ){
	is.unget();
	break;
      }
      if (++x == len) break;
    }
    if (c == '\n' || (len > 0 && x == len)) { // eoln or all whitespaces => set invalid
      m_dataitem->setInvalid();
      return c == '\n' ? false : rslt;
    }

   std::string nan("nan");
    int n = 0;
    for( n = 0; n < 3; ++n ){
      is.get( c );
      if( !( c == nan[n] || tolower(c) == nan[n] ) )
	break;
    }
    if( !( n == 3 && isspace( is.peek() ) ) ){
      is.clear();
      is.seekg( pos );
       rslt = false;
    }
    else {
      m_dataitem->setInvalid();
    }
  }
  return rslt;
}

bool RealStreamParameter::getValue( std::string &v ) const{
  std::ostringstream os; double d;

  if( m_dataitem->getValue(d) ){
    os << d; v=os.str();
    return true;
  }
  return false;
}

void RealStreamParameter::setValue( const std::string &v ){
  std::istringstream is(v);
  double d;
  //int inxdnum = createDataItemIndexes();
  if( m_conv.read( is, d, m_delimiter ) ){
    m_dataitem->setValue(d);
  }
  else
    std::cerr << "RealStreamParameter::setValue( " << v << " ) REAL CONVERSION ERROR" << std::endl;
  //removeDataItemIndexes();
}

bool RealStreamParameter::write( std::ostream &os ){
  BUG_PARA(BugStreamer, "RealStreamParameter::write ", m_dataitem->getFullName( true ) );
  double d;
  bool rslt = true;
  if( m_dataitem->getValue(d) ){
    rslt = m_conv.write( d, os );
    if( m_conv.getWidth() == 0 &&
        m_format != HardCopyListener::URL // no delimiter for URLs
        )
      os << (m_delimiter == '\0' ? ' ' : m_delimiter);
  }
  else {
    int w=m_conv.getWidth();
    if( w>0 )
      for( int iw=0 ; iw<w; ++iw )
	os << ' ';
    else {
      if ( m_format != HardCopyListener::URL ) // no delimiter for URLs
        os << (m_delimiter == '\0' ? ' ' : m_delimiter);
    }
  }
  return rslt;
}

void RealStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 ){
    double d;
    int indxnum = createDataItemIndexes();
    if( isScalar() ){
      if( m_dataitem->getValue(d) ){
	dest -> putValue(*this, d );
      }
      else {
	dest -> putValues(*this, std::vector<int>(), std::vector<double>(), isCell(), std::vector<int>() );
      }
      removeDataItemIndexes();
    }
    else {
      m_dataitem->setDimensionIndizes();
      DataContainer::SizeList dims;
      int numdims = m_dataitem->Data()->GetAllDataDimensionSize( dims );
      int nlevel = getLevel();


      if( numdims == 0 ){
	if( !m_dataitem->isValid() ){
	  dest -> putValues(*this,std::vector<int>() , std::vector<double>(), isCell(), std::vector<int>() );
	  removeDataItemIndexes();
	  return;
	}
	double d;
	m_dataitem->getValue( d );
	dest -> putValues(*this,std::vector<int>(1,1) , std::vector<double>(1,d), isCell(), std::vector<int>() );
	removeDataItemIndexes();
	return;
      }
      int firstIndexNum = m_dataitem->getFirstIndexNum(nlevel)+1;
      XferDataItemIndex *inx=0;
      for( int i=0; i<numdims; i++ ){
	inx = m_dataitem->newDataItemIndex(nlevel);
	if( inx == 0 ){
	  removeDataItemIndexes();
	  m_dataitem->eraseDataItemIndex(nlevel);
	  return;
	}
	inx -> setIndexNum( firstIndexNum + i );
	inx -> setLowerbound(0);
      }
      m_dataitem->setDimensionIndizes();

      std::vector<double> vals;
      int k;
      if( transposed ){
	if( isCell() ){
	  std::vector<int> celldims, newdims;
	  k=putTransposedCellMatrix( vals, numdims, numdims-1, dims, celldims, indxnum );
	  newdims.push_back(dims.size());
	  std::copy( dims.begin(), dims.end()-1, std::back_inserter( newdims ) );
	  newdims.push_back(1);
	  std::copy( celldims.begin(), celldims.end(), std::back_inserter( newdims ) );
	  dims=newdims;
	}
	else
	  k=putTransposedMatrix( vals, numdims, numdims-1, dims, indxnum );
      }
      else{
	k=putMatrix( vals, numdims, 0, dims, indxnum );
      }
      dest -> putValues(*this, dims, vals, isCell(), std::vector<int>() );
      removeDataItemIndexes();
      m_dataitem->eraseDataItemIndex(nlevel);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* putTransposedMatrix --                                                      */
/* --------------------------------------------------------------------------- */

int RealStreamParameter::putTransposedMatrix( std::vector<double>&vals
					     , int ndims, int idim
					     , DataContainer::SizeList &dims, int indexnum ){
  BUG_PARA( BugStreamer,"RealStreamParameter::putTransposedMatrix"
	    , "ndims=" << ndims << ", idim=" << idim );
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

  double d,dinf;
  int isize = dims[0];
  dinf=NumLim::infinity();
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( indexnum, i );
    if( m_dataitem->getValue(d) ){
      vals.push_back( d );
    }
    else{
      vals.push_back( dinf );
    }
    k++;
  }
  if( k>0 && k < isize ){
    dims[0]=k;
  }
  return k;
}

int RealStreamParameter::putTransposedCellMatrix( std::vector<double>&vals
						  , int ndims, int idim
						  , DataContainer::SizeList &dims
						  , std::vector<int> &celldims
						  , int indexnum ){
  BUG_PARA( BugStreamer,"RealStreamParameter::putTransposedCellMatrix"
	    , "ndims=" << ndims << ", idim=" << idim );
  int i,k;

  if( idim > 0 ){
    int isize = dims[idim];
    int k=0;
    for( i = 0; i < isize; i++ ){
      m_dataitem->setIndex( idim+indexnum, i );
      if( putTransposedCellMatrix( vals
			       , ndims   // Dimensions
			       , idim - 1      // current Index in dim
				   , dims, celldims, indexnum ) > 0 ){
	k++;
      }
    }
    if( k<isize )
      dims[idim]=k;
    return k;
  }

  double d,dinf;
  int isize = m_dataitem->Data()->GetDataDimensionSizeOfLevel_PreInx(getLevel(), 0);
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( indexnum, i );
    if( m_dataitem->getValue(d) ){
      vals.push_back( d );
      k++;
    }
  }
  celldims.push_back( k );
  return k;
}

/* --------------------------------------------------------------------------- */
/* putMatrix --                                                                */
/* --------------------------------------------------------------------------- */

int RealStreamParameter::putMatrix( std::vector<double>&vals
				   , int ndims, int idim
				   , std::vector<int>&dims, int indexnum ){
  BUG_PARA( BugStreamer,"RealStreamParameter::putMatrix"
	    , "ndims=" << ndims << ", idim=" << idim );
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

  double d,dinf;
  int isize = dims[idim];
  dinf=NumLim::infinity();
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( d ) ){
      vals.push_back( d );
    }
    else{
      vals.push_back( dinf );
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
void RealStreamParameter::marshal( std::ostream &os ){
  os << "<RealStreamParameter";
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
  os << "</RealStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *RealStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "RealStreamParameter" ){
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
    s = attributeList["delimiter"];
    m_delimiter = s.size() ? s[0] : ' ';
   return this;
  }
  else if( element == "RealConverter" ){
    return m_conv.unmarshal( element, attributeList );
  }
  else if( element == "XferDataItem" ){
    m_dataitem -> unmarshal( element, attributeList );
    return m_dataitem;
  }
  else if( element == "/RealStreamParameter" ){
    GuiIndex::registerIndexContainer( this );
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void RealStreamParameter::setText( const std::string &text ){
}
