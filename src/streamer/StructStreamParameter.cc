
#include <string>
#include <assert.h>
#include <vector>
#include <stdarg.h>
#include <complex>

#include "utils/Debugger.h"

#include "app/DataPoolIntens.h"
#include "streamer/StructStreamParameter.h"
#include "streamer/StringStreamParameter.h"
#include "streamer/RealStreamParameter.h"
#include "streamer/ComplexStreamParameter.h"
#include "streamer/NumericMatrixParameter.h"
#include "streamer/ComplexMatrixParameter.h"
#include "streamer/StringMatrixParameter.h"
#include "xml/XMLDocumentHandler.h"

/* --------------------------------------------------------------------------- */
/* getFields --                                                                */
/* --------------------------------------------------------------------------- */

void StructStreamParameter::getFields( DataReference *dref ){
  BUG( BugStreamer, "StructStreamParameter::getFields" );
  StructStreamParameter *pfield=0;
  DataDictionary *dstruct = (dref->GetDict())->structure().getStructure();
  BUG_MSG( dref->nodeName() );
  do {
    DataReference *dfield = DataPoolIntens::getDataReference( dref ,dstruct->getName().c_str() );
    XferDataItem *xdat = 0;
    if( dfield == 0 ){
       std::cerr << "StructStreamParameter: Cannot get DataReference for "
		 << dref->nodeName()<<"." << dstruct->getName();
       std::cerr << std::endl;
    }
    else {
      xdat = new XferDataItem( *m_dataitem );
      xdat -> setDataReference( dfield );
      UserAttr *attr = static_cast<UserAttr*>( dfield->getUserAttr() );
      bool isScalar = attr != 0 ? attr -> IsScalar() : false;
      bool isCell   = attr != 0 ? attr -> IsCell() : false;

      DataStreamParameter *p=0;
      switch( dfield->getDataType() ){
      case DataDictionary::type_Integer:
      case DataDictionary::type_Real:
	//   		cerr << " numeric matrix" << std::endl;
	if( isScalar ){
	  BUG_MSG( dstruct->getName() << " (numeric,scalar)" );
	  p=new RealStreamParameter(xdat, getLevel()+1, this, m_delimiter, isLocale());
	}
	else {
	  BUG_MSG( dstruct->getName() << " (numeric,matrix)" );
	  p=new NumericMatrixDataParameter(xdat, getLevel()+1, this, m_delimiter, isLocale());
	}
	break;

      case DataDictionary::type_CharData:
      case DataDictionary::type_String:
	if( isScalar ){
	  BUG_MSG( dstruct->getName() << " (string,scalar)" );
	  p= new StringStreamParameter(xdat, getLevel()+1, this, m_delimiter, isLocale());
	}
	else {
	  BUG_MSG( dstruct->getName() << " (string,matrix)" );
	  p= new StringMatrixDataParameter(xdat, getLevel()+1, this);
	}
	break;
      case DataDictionary::type_StructVariable:
	  BUG_MSG( dstruct->getName() << " (struct)" );
	  pfield=new StructStreamParameter(xdat, getLevel()+1, this, m_delimiter, isLocale());
	{
	  UserAttr *attr = static_cast<UserAttr*>( dfield->getUserAttr() );
	  if( attr != 0 ){
	    pfield -> setClassname(  attr->Classname() );
	  }
	  else
	    std::cerr << "UserAttr of " << pfield->getName() << " is NULL" << std::endl;
	}
	p=pfield;
	break;
      case DataDictionary::type_Complex:
	if( isScalar ){
	  BUG_MSG( dstruct->getName() << " (complex,scalar)" );
	  p = new ComplexStreamParameter(xdat, getLevel()+1, this, isLocale());
	}
	else {
	  BUG_MSG( dstruct->getName() << " (complex,matrix)" );
	  p=new ComplexMatrixDataParameter(xdat, getLevel()+1, this, ' ', isLocale());
	}
	break;
      case DataDictionary::type_StructDefinition:
      case DataDictionary::type_Bad:

      default:
	delete xdat;
	std::cerr << "StructStreamParameter Unknown Data type" << std::endl;
	break;
      }
      if( p!=0 ){
	p->setCell( isCell );
	m_parfields.push_back( p );
      }
    }
  }while( (dstruct=dstruct->getNext()) != 0);
}

StructStreamParameter::StructStreamParameter( XferDataItem *ditem, int level
					      , StructStreamParameter *p
					      , char delimiter, bool locale )
  : DataStreamParameter(ditem,level,p,false,delimiter,locale)
  , m_recreateIndex(true) {
  m_name = ditem->Data()->nodeName();
  getFields( ditem->Data() );
}

StructStreamParameter::~StructStreamParameter(){
  std::vector< DataStreamParameter * >::iterator I;
  for( I=m_parfields.begin(); I!=m_parfields.end(); ++I ){
    delete *I;
  }
}

void StructStreamParameter::setValue( double v ){
  std::cerr << "StructStreamParameter::setValue:"<< std::endl;
  std::cerr << "  Warning: cannot set Value for " << getName()<< std::endl;
}
bool StructStreamParameter::getValue( double &v )const{
  std::cerr << "StructStreamParameter::getValue:"<< std::endl;
  std::cerr << "  Warning: cannot get Value for " << getName()<< std::endl;
  return false;
}
void StructStreamParameter::setValue( int v ){
  std::cerr << "StructStreamParameter::setValue:"<< std::endl;
  std::cerr << "  Warning: cannot set Value for " << getName()<< std::endl;
}
bool StructStreamParameter::getValue( int &v )const{
  std::cerr << "StructStreamParameter::getValue:"<< std::endl;
  std::cerr << "  Warning: cannot get Value for " << getName()<< std::endl;
  return false;
}
void StructStreamParameter::setValue( const std::string &v ){
  std::cerr << "StructStreamParameter::setValue:"<< std::endl;
  std::cerr << "  Warning: cannot set Value for " << getName()<< std::endl;
}
bool StructStreamParameter::getValue( std::string &v )const{
  std::cerr << "StructStreamParameter::getValue:"<< std::endl;
  std::cerr << "  Warning: cannot get Value for " << getName()<< std::endl;
  return false;
}

bool StructStreamParameter::isValid(bool validErrorMsg){
  std::vector< DataStreamParameter * >::iterator I;
  for( I=m_parfields.begin(); I!=m_parfields.end(); ++I ){
    if( (*I)->isValid(validErrorMsg) )
      return true;
  }
  return false;
}

bool StructStreamParameter::write( std::ostream &os ){
  std::vector< DataStreamParameter * >::iterator I;
  for( I=m_parfields.begin(); I!=m_parfields.end(); ++I ){
    if( !(*I)->write( os ) )
      return false;
  }
  return true;
}

bool StructStreamParameter::read( std::istream &is ){
  std::vector< DataStreamParameter * >::iterator I;
  for( I=m_parfields.begin(); I!=m_parfields.end(); ++I ){
    if( !(*I)->read( is ) )
      return false;
  }
  return true;
}
//-------------------------------------------------------------------------------
// getValues
//   gets the values for all fields from src
//
void StructStreamParameter::getValues( StreamSource *src ){
  if( src == 0 )
    return;
  src->getValues( *this, m_parfields );
}
//-------------------------------------------------------------------------------
// putValues
//   determines the dimensions, creates a vector and sends it to the destination
//
void StructStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest == 0 )
    return;
  int indxnum=0;
  BUG( BugStreamer, "StructStreamParameter::putValues " );
  //  std::cerr << "------> StructStreamParameter::putValues " << getName()<< std::endl;
  BUG_MSG( getName() );
  if( m_parent != 0 ){
    indxnum=m_parent->createDataItemIndexes(m_dataitem);
  }
  m_dataitem->setDimensionIndizes();
  DataContainer::SizeList dims;
  int numdims = m_dataitem->getAllDataDimensionSize(dims);
  //  std::cerr << "      dimensions: " << numdims << std::endl;
  if( m_dataitem->lastLevelHasIndices() || m_dataitem->getNumDimensions()==0 ){
    BUG_MSG( getFullName(true) << " is scalar" );
    //std::cerr << getFullName(true) << " is scalar"<< std::endl;
    if( dims.size()==0 || dims[0]==0 ){
      BUG_MSG( " is not valid" );
      //std::cerr << " is not valid!" << std::endl;
      dest -> putValues(*this, m_parfields , std::vector<int>() );
      if( m_parent != 0 ){
	m_parent->removeDataItemIndexes(m_dataitem);
      }
      return;
    }
    //std::cerr << "getAllDataDimensionSize:" << dims.size() << std::endl;
    dest -> putValues(*this, m_parfields , std::vector<int>(1,1) );
    if( m_parent != 0 ){
      m_parent->removeDataItemIndexes(m_dataitem);
    }
    return;
  }
  BUG_MSG( m_dataitem->Data()->fullName(true) << " has " << numdims << " dimensions" );
  //std::cerr << "Dimensions:\n" ;
  // copy( dims.begin(), dims.end(), ostream_iterator<int>( std::cerr, " " ) );
  // std::cerr << std::endl;

  int level=getLevel();
  if( m_parent != 0 ){
    level = m_parent->getLevel()+1;
  }
  if( m_recreateIndex ){
    XferDataItemIndex *inx=0;
    for( int i=0; i<numdims; i++ ){
      inx = m_dataitem->newDataItemIndex( level );

      inx->setLowerbound( 0 );
    }
  }

  m_dataitem->setDimensionIndizes();

  setDimensionSize( numdims );

  dest->putValues( *this, m_parfields, dims );

  if( m_recreateIndex ){
    BUG_MSG( "erase index on level " << level );
    m_dataitem->eraseDataItemIndex(level);
    DataStreamParameter::removeDataItemIndexes();
  }
  if( m_parent != 0 ){
    m_parent->removeDataItemIndexes(m_dataitem);
  }
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void StructStreamParameter::setIndex( int pos, int i ){
  assert( pos < m_indexvec.size() );
  m_indexvec[pos]=i;
}

/* --------------------------------------------------------------------------- */
/* setItemIndex --                                                             */
/* --------------------------------------------------------------------------- */

void StructStreamParameter::setItemIndex( int pos, int v ){
  assert( pos < m_indexvec.size() );
  if( m_dataitem!=0 ){
    int base=m_dataitem->getFirstIndexNum( getLevel() );
    if( base >= 0 ){
      m_dataitem->setIndex(base+pos,v);
    }
  }
  m_indexvec[pos]=v;
}

/* --------------------------------------------------------------------------- */
/* setDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

void StructStreamParameter::setDimensionSize( int numdims ){
  m_indexvec = std::vector<int>( numdims, 0 );
}

/* --------------------------------------------------------------------------- */
/* createDataItemIndexes --                                                    */
/* --------------------------------------------------------------------------- */

int StructStreamParameter::createDataItemIndexes( XferDataItem *d ){
  int ip=0;
  int i;

  if( !m_recreateIndex ){
    return d==0?0:d->getNumIndexes();
  }
  if( m_parent != 0 ){
    ip=m_parent->createDataItemIndexes( d );
  }

  XferDataItemIndex *inx;
  int nlevel=getLevel();
  for( i=0; i<m_indexvec.size(); ++i ){
    inx = d->newDataItemIndex(nlevel);
    if( inx == 0 ){
      // std::cerr << "HELP CANNOT CREATE ITEM INDEX!! " << std::endl;
      return ip;
    }
    inx -> setLowerbound( m_indexvec[i] );
  }
  return i+ip;
}

/* --------------------------------------------------------------------------- */
/* removeDataItemIndexes --                                                    */
/* --------------------------------------------------------------------------- */

void StructStreamParameter::removeDataItemIndexes( XferDataItem *d ){
  if( m_indexvec.size()>0 && m_recreateIndex ){
    d->eraseDataItemIndex( getLevel() );
  }
  if( m_parent != 0 ){
    m_parent->removeDataItemIndexes(d);
  }
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void StructStreamParameter::marshal( std::ostream &os ){
  os << "<StructStreamParameter";
  if( isMandatory() )
    os << " mand=\"true\"";
  if( isScalar() )
    os << " scalar=\"true\"";
  if( isCell() )
    os << " cell=\"true\"";


  os << " classname=\"" << m_classname << "\"";
  os << " name=\""      << m_name      << "\"";
  if( m_recreateIndex )
    os << " recreateIndex=\"true\"";


  os << ">\n";
  m_dataitem -> marshal( os );
  os << "</StructStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *StructStreamParameter::unmarshal( const std::string &element
                                              , const XMLAttributeMap &attributeList )
{
  if( element == "StructStreamParameter" ){
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

    m_classname=attributeList["classname"];
    m_name=attributeList["name"];
    s = attributeList["recreateIndex"];
    if( !s.empty() )
      m_recreateIndex = true ;

  }
  else if( element == "XferDataItem" ){
    m_dataitem -> unmarshal( element, attributeList );
    m_name = m_dataitem->Data()->nodeName();
    getFields( m_dataitem->Data() );
    UserAttr *attr = static_cast<UserAttr*>(m_dataitem->Data()->getUserAttr());
    if( attr != 0 ){
      setClassname( attr->Classname() );
    }
    return m_dataitem;
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void StructStreamParameter::setText( const std::string &text ){
}
