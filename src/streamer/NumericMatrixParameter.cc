
#include "utils/Debugger.h"

#include "utils/NumLim.h"
#include "datapool/DataReference.h"
#include "streamer/NumericMatrixParameter.h"
#include "gui/GuiIndex.h"
#include "xml/XMLDocumentHandler.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

NumericMatrixDataParameter::NumericMatrixDataParameter( XferDataItem *ditem
							, int level
							, int width
							, int prec
							, Scale *scale
							, bool thousand_sep
							, bool mandatory
							, char delimiter
                            , bool locale)
  : MatrixDataParameter( ditem, level, 0, mandatory, delimiter, locale )
  , m_conv( width, prec, scale, true, !locale ){
  if( thousand_sep )
    m_conv.setThousandSep();
  if( ditem->Data()->getDataType() == DataDictionary::type_Real )
    m_conv.setAppendDot();
}

NumericMatrixDataParameter::NumericMatrixDataParameter( XferDataItem *dref
                              , int level
						      , StructStreamParameter *p
							  , char delimiter
                              , bool locale)
  : MatrixDataParameter( dref, level, p, false, delimiter, locale )
  , m_conv( 0, -1, 0, false, !locale ) {
}

NumericMatrixDataParameter::~NumericMatrixDataParameter(){}

/* --------------------------------------------------------------------------- */
/* readMatrixValues --                                                         */
/* --------------------------------------------------------------------------- */

bool NumericMatrixDataParameter::readMatrixValues( std::istream &istr
						   , int ndims, int idim, int indexnum, char delimiter ){
  BUG(BugStreamer,"NumericMatrixDataParameter::readMatrixValues");
  double d;
  int dims = m_dataitem->getMaxIndex( idim+indexnum );
  for( int i = 0; i < dims; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_conv.read( istr, d, delimiter ) ){
      m_dataitem->setValue( d );
    }
  // Ein 'nan' wird als undefinierter Wert interpretiert ! ( Walter lässt Grüssen )
    else{
#if ( __GNUC__ < 3 )
    std::streampos pos = istr.tellg();
#else
    std::istream::pos_type pos = istr.tellg();
#endif
      char c;
      while( istr.get(c) ){
	if( !( c == ' ' || c == '\t' || c == '-' ) ){
	  istr.unget();
	  break;
	}
      }
      std::string nan("nan");
      int n = 0;
      for( n = 0; n < 3; ++n ){
	istr.get( c );
	if( !( c == nan[n] || tolower(c) == nan[n] ) )
	  break;
      }
      if( !( n == 3 && isspace( istr.peek() ) ) ){
	istr.clear();
	istr.seekg( pos );
      }
      m_dataitem->setInvalid();
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeMatrixValues --                                                        */
/* --------------------------------------------------------------------------- */

bool NumericMatrixDataParameter::writeMatrixValues( std::ostream &ostr
						  , int ndims
						  , DataContainer::SizeList &dims
						  , int idim, int indexnum ){
  BUG(BugStreamer,"NumericMatrixDataParameter::writeMatrixValues");

  double d;
  for( int i = 0; i < dims[idim]; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( d ) ){
      m_conv.write( d, ostr );
      if( m_conv.getWidth() == 0 )
	ostr << ' ';
    }
    else{
      int width = m_conv.getWidth();
      if( width == 0 )
	ostr << "<NULL> ";
      else if( width >= 6 ){
	ostr << "<NULL>";
	for( int n = 0; n < width - 6; ++n )
	  ostr << ' ';
      }
      else
	for( int n = 0; n < width; ++n )
	  ostr << ' ';
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void NumericMatrixDataParameter::putValues( StreamDestination *dest, bool transpose ){
  BUG(BugStreamer,"NumericMatrixDataParameter::putValues");
  //  std::cerr << "NumericMatrixDataParameter::putValues " << getName();
  if( dest == 0 ){
    //std:cerr << " (no destination)\n";
    return;
  }
  int indxnum = createDataItemIndexes();
  m_dataitem->setDimensionIndizes();
  DataContainer::SizeList dims;
  int numdims = m_dataitem->Data()->GetAllDataDimensionSize( dims );
  int nlevel = getLevel();

  BUG_MSG("Dimension Array: Size = " << numdims);
  //std::cerr << "Dimension Array: Size = " << numdims << std::endl;
  for( int ii=0; ii < numdims; ii++ ){
    BUG_MSG("Size of Dimension " << (numdims - ii) << ": " << dims[ii]);
    //std::cerr << " " << numdims - ii << ": " << dims[ii];
  }
  //std::cerr << std::endl;

  if( numdims == 0 ){
    //std::cerr << "\n";
    if( !m_dataitem->isValid() ){
      BUG_MSG( "is not valid" );
      //std::cerr << " NOT VALID\n";
      dest -> putValues(*this,std::vector<int>() , std::vector<double>(), isCell(), std::vector<int>() );
      removeDataItemIndexes();
      return;
    }
    double d;
    m_dataitem->getValue( d );
    BUG_MSG( d );
    dest -> putValues(*this,std::vector<int>(1,1) , std::vector<double>(1,d), isCell(), std::vector<int>() );
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

  std::vector<double> vals;
  std::vector<int> inds;
  int k;
  if( transpose ){
    if( isCell() ){
	  std::vector<int> celldims, newdims;
	  k=putTransposedCellMatrix( vals, numdims, numdims-1, dims, celldims, indxnum );
	  newdims.push_back(dims.size());
	  std::copy( dims.begin(), dims.end()-1, std::back_inserter( newdims ) );
	  newdims.push_back(1);
	  std::copy( celldims.begin(), celldims.end(), std::back_inserter( newdims ) );
	  dims=newdims;
//         copy( celldims.begin(), celldims.end(), std::ostream_iterator<int>( cerr, " " ) );
//         cerr << endl;
//         copy( vals.begin(), vals.end(), std::ostream_iterator<double>( cerr, " " ) );
//         cerr << endl;
    }
    else
      k=putTransposedMatrix( vals, numdims, numdims-1, dims, indxnum );
  }
  else{
    if( isCell() ){
      std::vector<int> celldims, newdims;
      k=putCellMatrix( vals, numdims, 0, dims, celldims, indxnum );
      newdims.push_back(dims.size());
      std::copy( dims.begin(), dims.end()-1, std::back_inserter( newdims ) );
      newdims.push_back(1);
      std::copy( celldims.begin(), celldims.end(), std::back_inserter( newdims ) );
      dims=newdims;
//         copy( celldims.begin(), celldims.end(), std::ostream_iterator<int>( cerr, " " ) );
//         cerr << endl;
//         copy( vals.begin(), vals.end(), std::ostream_iterator<double>( cerr, " " ) );
//         cerr << endl;
    }
    else {
      k=putMatrix( vals, numdims, 0, dims, indxnum, inds );
    }
  }
  dest -> putValues(*this, dims, vals, isCell(), inds );
  removeDataItemIndexes();
  m_dataitem->eraseDataItemIndex(nlevel);
}

/* --------------------------------------------------------------------------- */
/* putTransposedMatrix --                                                      */
/* --------------------------------------------------------------------------- */

int NumericMatrixDataParameter::putTransposedMatrix( std::vector<double>&vals
						   , int ndims, int idim
						   , std::vector<int>&dims, int indexnum ){
  BUG_PARA( BugStreamer,"NumericMatrixDataParameter::putTransposedMatrix"
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

int NumericMatrixDataParameter::putTransposedCellMatrix( std::vector<double>&vals
						     , int ndims, int idim
						     , DataContainer::SizeList &dims
						     , std::vector<int> &celldims
						     , int indexnum ){
  BUG_PARA( BugStreamer,"NumericMatrixParameter::putTransposedCellMatrix"
	    , "ndims=" << ndims << ", idim=" << idim );
  int i,k;

  if( idim > 0 ){
    int isize = dims[ndims-idim-1];
    int k=0;
    for( i = 0; i < isize; i++ ){
      m_dataitem->setIndex( ndims-idim-1+indexnum, i );
      if( putTransposedCellMatrix( vals
			       , ndims   // Dimensions
			       , idim - 1      // current Index in dim
			       , dims, celldims, indexnum ) > 0 ){
	k++;
      }
    }
    return k;
  }

  double d;
  int isize = m_dataitem->getSize(ndims-1+indexnum);
  //   cerr << "SIZE (" << ndims-1+indexnum << ") is: " << isize << endl;
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( ndims-1+indexnum, i );
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
/*   vals   vector containing the numeric values                               */
/*   ndims  number of dimensions                                               */
/*   idim   current dimension                                                  */
/*   dims   vector of size ndims containing the dimension sizes                */
/*   indexnum                                                                  */
/*   inds   vector containing the indicator values (1 valid -1 invalid)        */
/* --------------------------------------------------------------------------- */

int NumericMatrixDataParameter::putMatrix( std::vector<double>&vals
					   , int ndims, int idim
					   , std::vector<int>&dims
					   , int indexnum
					   , std::vector<int> &inds ){
  BUG_PARA( BugStreamer,"NumericMatrixDataParameter::putMatrix"
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
		     , dims, indexnum, inds ) > 0 ){
	k++;
      }
    }
    if( k<isize )
      dims[idim]=k;
    return k;
  }

  double d;
  int isize = dims[idim];
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( d ) ){
      inds.push_back( 1 );
    }
    else{
      d=0;
      inds.push_back( -1 );
    }
    vals.push_back( d );
    k++;
  }
  if( k>0 && k < isize ){
    dims[idim]=k;
  }
  return k;
}
/* --------------------------------------------------------------------------- */
/* putCellMatrix --                                                                */
/* --------------------------------------------------------------------------- */

int NumericMatrixDataParameter::putCellMatrix( std::vector<double>&vals
						     , int ndims, int idim
						     , DataContainer::SizeList &dims
						     , std::vector<int> &celldims
						     , int indexnum ){
  BUG_PARA( BugStreamer,"NumericMatrixDataParameter::putCellMatrix"
	    , "ndims=" << ndims << ", idim=" << idim );
  int i,k;

  if( idim < ndims-1 ){
    int isize = dims[idim];
    int k=0;
    for( i = 0; i < isize; i++ ){
      m_dataitem->setIndex( idim+indexnum, i );
      if( putCellMatrix( vals
			 , ndims   // Dimensions
			 , idim + 1      // current Index in dim
			 , dims, celldims, indexnum ) > 0 ){
	k++;
      }
    }
    return k;
  }

  double d;
  int isize = dims[idim];
  k=0;
  for( i = 0; i < isize; i++ ){
    m_dataitem->setIndex( idim+indexnum, i );
    if( m_dataitem->getValue( d ) ){
      vals.push_back( d );
      k++;
    }
  }
  celldims.push_back( k );
  return k;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void NumericMatrixDataParameter::marshal( std::ostream &os ){
  os << "<NumericMatrixDataParameter";
  if( getLevel() != 0 )
    os << " level=\""<<getLevel()<<"\"";
  if( isMandatory() )
    os << " mand=\"true\"";
  if( isScalar() )
    os << " scalar=\"true\"";
  if( isCell() )
    os << " cell=\"true\"";
  os << ">\n";
  m_dataitem -> marshal( os );
  m_conv.marshal( os );
  os << "</NumericMatrixDataParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *NumericMatrixDataParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "NumericMatrixDataParameter" ){
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
  else if( element == "RealConverter" ){
    return m_conv.unmarshal( element, attributeList );
  }
  else if( element == "XferDataItem" ){
    m_dataitem -> unmarshal( element, attributeList );
    return m_dataitem;
  }
  else if( element == "/NumericMatrixDataParameter" ){
    if( m_dataitem->Data()->getDataType() == DataDictionary::type_Real )
      m_conv.setAppendDot();
    GuiIndex::registerIndexContainer( this );
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void NumericMatrixDataParameter::setText( const std::string &text ){
}
