
#include "utils/Debugger.h"

#include "utils/NumLim.h"
#include "datapool/DataReference.h"
#include "streamer/StreamParameter.h"
#include "streamer/ComplexStreamParameter.h"
#include "streamer/StructStreamParameter.h"
#include "gui/GuiIndex.h"
#include "xml/XMLDocumentHandler.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

ComplexStreamParameter::ComplexStreamParameter( XferDataItem *dref, int level
												, StructStreamParameter *p, bool locale )
  : DataStreamParameter( dref, level, p, false, ' ', locale )
  , m_complete( true )
  , m_conv( 0, -1, 0, false, true ){
}

ComplexStreamParameter::ComplexStreamParameter( XferDataItem *ditem, int level
					      , int width, int prec, Scale *scale, bool mand, bool locale )
  : DataStreamParameter( ditem, level, 0, mand, ' ', locale )
  , m_conv( width, prec, scale, false, true )
  , m_complete( true ){
}

ComplexStreamParameter::~ComplexStreamParameter(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool ComplexStreamParameter::read( std::istream &is ){
  dComplex c;
  BUG(BugStreamer, "ComplexStreamParameter::read( istream & )" );
  BUG_MSG( getName() );
  if( m_conv.read( is, c ) ){
    m_dataitem->setValue(c);
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

size_t ComplexStreamParameter::getDimensionSize(int ndim) const {
  if( m_dataitem == 0 ){
    return 0;
  }
  return 2*m_dataitem->getDimensionSize(ndim);
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool ComplexStreamParameter::getValue( std::string &v )const{
//   m_complete=true;
  dComplex d;
  std::ostringstream os;
  if( m_dataitem->getValue(d) ){
    os << d; v=os.str(); return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void ComplexStreamParameter::setValue( const std::string &v ){
  std::istringstream is(v);
  dComplex c;
  if( m_conv.read( is, c ) ){
    m_dataitem->setValue(c);
  }
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool ComplexStreamParameter::getValue( double &v )const{
  if( m_complete ){
    const_cast<bool &>(m_complete)=false;
    if( m_dataitem->getValue(const_cast<dComplex &>(m_val)) ){
      v=m_val.real();
      return true;
    }
    return false;
  }
  v=m_val.imag();
  const_cast<bool &>(m_complete)=true;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void ComplexStreamParameter::setValue( double v ){
  if( m_complete ){
    m_complete=false;
    m_val=v;
    return;
  }
  m_dataitem->setValue(dComplex(m_val.real(),v));
  m_complete=true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool ComplexStreamParameter::write( std::ostream &os ){
  dComplex c;
  if( m_dataitem->getValue(c) ){
    return m_conv.write( c, os );
  }
  return m_conv.writeInvalid(os);
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void ComplexStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 ){
    int indxnum = createDataItemIndexes();
    if( isScalar() ){
      dComplex d;
      if( m_dataitem->getValue(d) ){
	dest -> putValues(*this, std::vector<int>(1,1), std::vector<dComplex>(1,d), std::vector<int>() );
      }
      else {
	dest -> putValues(*this, std::vector<int>(), std::vector<dComplex>(), std::vector<int>() );
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

      std::vector<dComplex> vals;
      int k;
      if( transposed ){
	k=putTransposedMatrix( vals, numdims, numdims-1, dims, indxnum );
      }
      else{
	k=putMatrix( vals, numdims, 0, dims, indxnum );
      }
      dest -> putValues(*this, dims, vals, std::vector<int>() );
      removeDataItemIndexes();
      m_dataitem->eraseDataItemIndex(nlevel);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* putTransposedMatrix --                                                      */
/* --------------------------------------------------------------------------- */

int ComplexStreamParameter::putTransposedMatrix( std::vector<dComplex>&vals
					     , int ndims, int idim
					     , std::vector<int>&dims, int indexnum ){
  BUG_PARA( BugStreamer,"ComplexStreamParameter::putTransposedMatrix"
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
  dComplex c;
  int isize = dims[0];
  const double dinf=NumLim::infinity();
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( indexnum, i );
    if( m_dataitem->getValue(c) ){
      vals.push_back( c );
    }
    else{
      vals.push_back( dComplex( dinf, 0.0 ) );
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

int ComplexStreamParameter::putMatrix( std::vector<dComplex>&vals
				   , int ndims, int idim
				   , std::vector<int>&dims, int indexnum ){
  BUG_PARA( BugStreamer,"ComplexStreamParameter::putMatrix"
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
  dComplex c;
  int isize = dims[idim];
  const double dinf=NumLim::infinity();
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( c ) ){
      vals.push_back( c );
    }
    else{
      vals.push_back( dComplex(dinf, 0.0) );
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
void ComplexStreamParameter::marshal( std::ostream &os ){
  os << "<ComplexStreamParameter";
  if( isMandatory() )
    os << " mand=\"true\"";
  if( isScalar() )
    os << " scalar=\"true\"";
  if( isCell() )
    os << " cell=\"true\"";
  os << ">\n";
  m_dataitem -> marshal( os );
  m_conv.marshal( os );
  os << "</ComplexStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *ComplexStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "ComplexStreamParameter" ){
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
    return this;
  }
  else if( element == "ComplexConverter" ){
    return m_conv.unmarshal( element, attributeList );
  }
  else if( element == "XferDataItem" ){
    m_dataitem -> unmarshal( element, attributeList );
    return m_dataitem;
  }
  else if( element == "/ComplexStreamParameter" ){
    GuiIndex::registerIndexContainer( this );
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void ComplexStreamParameter::setText( const std::string &text ){
}
