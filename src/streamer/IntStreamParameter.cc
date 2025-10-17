
#include <cmath>
#include <limits>

#include "utils/Debugger.h"

#include "utils/NumLim.h"
#include "datapool/DataReference.h"
#include "streamer/IntStreamParameter.h"
#include "gui/GuiIndex.h"
#include "xml/XMLDocumentHandler.h"

IntStreamParameter::IntStreamParameter(XferDataItem *dref, int level, StructStreamParameter *p, char delimiter ):
  DataStreamParameter(dref,level,p,false,delimiter)
, m_conv( 0, 0, false ) {
}

IntStreamParameter::IntStreamParameter(XferDataItem *ditem, int level, int width, Scale *scale, char delimiter, bool mand ):
  DataStreamParameter(ditem,level,0,mand,delimiter)
  , m_conv( width, scale, false ) {
}

IntStreamParameter::~IntStreamParameter(){}

bool IntStreamParameter::read( std::istream &is ){
  BUG_PARA( BugStreamer, "IntStreamParameter::read( istream &is ) ",getName() );
  int i;
  bool rslt = true;
  if( m_conv.read( is, i, m_delimiter ) ){
    if (!std::isnan(i) && i!=std::numeric_limits<int>::min()) {
      m_dataitem->setValue(i);
    }
    BUG_MSG( "std::isnan["<<std::isnan(i)<<"] ???["<<(i==std::numeric_limits<int>::quiet_NaN())<<"]  varname:'" <<  m_dataitem->getFullName(true) << "' val='" << i <<"'" );
  }
  // Ein 'nan' wird als undefinierter Wert interpretiert ! ( Walter lässt Grüssen )
  else{
#if ( __GNUC__ < 3 )
    std::streampos pos = is.tellg();
#else
    std::istream::pos_type pos = is.tellg();
#endif
    char c = '\n';
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
      return  c=='\n' ? false : rslt;
    }

    std::string nan("nan");
    int n = 0;
    if (is.gcount() > 2)
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

bool IntStreamParameter::write( std::ostream &os ){
  int i;
  bool rslt = true;
  if( m_dataitem->getValue(i) ){
    rslt =  m_conv.write( i, os );
    if( m_conv.getWidth() == 0 &&
        m_format != HardCopyListener::URL // no delimiter for URLs
        )
      os << (m_delimiter == '\0' ? ' ' : m_delimiter);
  }
  else {
    int w=m_conv.getWidth();
    if( w>0 ){
      for( int iw = 0; iw<w; ++iw )
	os << ' ';
    }
    else {
      if ( m_format != HardCopyListener::URL ) // no delimiter for URLs
        os << (m_delimiter == '\0' ? ' ' : m_delimiter);
    }
  }
  return rslt; //
}
void IntStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 ){
    int indxnum = createDataItemIndexes();
    if( isScalar() ){
      double d;
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
      if ( m_dataitem->getFirstIndexNum(nlevel)<0 ){
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
      }
      m_dataitem->setDimensionIndizes();

      std::vector<double> vals;
      int k;
      if( transposed ){
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

int IntStreamParameter::putTransposedMatrix( std::vector<double>&vals
					     , int ndims, int idim
					     , std::vector<int>&dims, int indexnum ){
  BUG_PARA( BugStreamer,"IntStreamParameter::putTransposedMatrix"
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

/* --------------------------------------------------------------------------- */
/* putMatrix --                                                                */
/* --------------------------------------------------------------------------- */

int IntStreamParameter::putMatrix( std::vector<double>&vals
				   , int ndims, int idim
				   , std::vector<int>&dims, int indexnum ){
  BUG_PARA( BugStreamer,"IntStreamParameter::putMatrix"
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

  double dinf;
  int d;
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

bool IntStreamParameter::getValue( std::string &v )const{
  std::ostringstream os; int i;
  if( m_dataitem->getValue(i) ){
    os << i; v=os.str(); return true;
  }
  return false;
}


void IntStreamParameter::setValue( const std::string &v ){
  std::istringstream is(v);
  int i;
  if( m_conv.read( is, i, m_delimiter ) ){
    if (i==std::numeric_limits<int>::quiet_NaN())
      m_dataitem->setInvalid();
    else
      m_dataitem->setValue(i);
  }
  else {
    std::cerr << " INT CONVERSION ERROR for "<< v << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void IntStreamParameter::marshal( std::ostream &os ){
  os << "<IntStreamParameter";
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
  os << "</IntStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *IntStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "IntStreamParameter" ){
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
  else if( element == "IntConverter" ){
    return m_conv.unmarshal( element, attributeList );
  }
  else if( element == "XferDataItem" ){
    m_dataitem -> unmarshal( element, attributeList );
    return m_dataitem;
  }
  else if( element == "/IntStreamParameter" ){
    GuiIndex::registerIndexContainer( this );
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void IntStreamParameter::setText( const std::string &text ){
}
