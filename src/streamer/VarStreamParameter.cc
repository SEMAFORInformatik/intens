
#include "utils/Debugger.h"

#include "app/DataPoolIntens.h"
#include "streamer/VarStreamParameter.h"
#include "streamer/IntStreamParameter.h"
#include "streamer/RealStreamParameter.h"
#include "streamer/StringStreamParameter.h"
#include "streamer/StructStreamParameter.h"
#include "streamer/ComplexStreamParameter.h"
#include "streamer/StreamParameterGroup.h"
#include "xfer/XferScale.h"
#include "xml/XMLDocumentHandler.h"

//============================================================================
// Constructor / Destructor
//============================================================================
VarStreamParameter::VarStreamParameter( XferDataItem *varDataItem,
					XferDataItem *dataItem,
					StreamParameterGroup *parent,
					int level, char delimiter, bool locale )
  : DataStreamParameter( varDataItem, level, 0, false, delimiter, locale )
  , m_current( 0 )
  , m_parent( parent )
  , m_item( dataItem )
  , m_length(0)
  , m_prec(-1)
  , m_scale(0)
  , m_tsep(false) {
    assert( parent != 0 );
  }


VarStreamParameter::~VarStreamParameter(){
  if( m_current != 0 )
    delete m_current;
}

//============================================================================
// resolveVars
//============================================================================
void VarStreamParameter::resolveVars(){
  BUG(BugStreamer, "VarStreamParameter::resolveVars()" );
  // delete old parameter
  if( m_current != 0 ){
    m_current->unlinkIndex( m_parent );
    delete m_current;
    m_current = 0;
  }
  // check if dataitem is a string and get the value
  if( m_dataitem->Data()->getDataType() != DataDictionary::type_String )
    return;
  std::string varItemName;
  m_dataitem->getValue( varItemName );
  if( varItemName.empty() )
    return;

  // check if there is a const item bevor the variable part
  std::string prefix = m_item->getFullName( false );
  if( !prefix.empty() )
    prefix+=".";
  // put it all together and get the new datareference
  varItemName = prefix+varItemName+m_suffix;
  BUG_MSG( "varItemName : " << varItemName );
  DataReference *dref = DataPoolIntens::getDataReference( varItemName );
  if( dref == 0 ){
    return;
  }
  // put it together with indexlist
  XferDataItem *xfer = new XferDataItem( *m_item, dref );

  if( xfer == 0 )
    return;

  // create new parameter
  int level = xfer ->getNumberOfLevels()-1;
  // we try to ajust the index level of the last index:
  // (CHECK IT: it might be incorrect!)
  XferDataItemIndex *xi=xfer -> getLastIndex();
  if( xi != 0 ){
    xi -> setLevel( level );
  }
  xfer->setDimensionIndizes();
  BUG_MSG( xfer->getFullName( true ) );

  bool mandatory = false;
  bool isDatasetText = false;
  switch( xfer->Data()->getDataType() ){
  case DataDictionary::type_Integer:
    BUG_MSG( "create IntStreamParameter" );
    m_current = new IntStreamParameter( xfer, level, m_length,
					m_scale != 0 ? m_scale->copy() : 0,
					m_delimiter, mandatory );
    break;
  case DataDictionary::type_Real:
    BUG_MSG( "create RealStreamParameter" );
    m_current = new RealStreamParameter( xfer, level, m_length, m_prec,
										 m_scale != 0 ? m_scale->copy() : 0,
										 m_tsep, mandatory, m_delimiter, isLocale() );
    break;
  case DataDictionary::type_String:
    BUG_MSG( "create StringStreamParameter" );
    m_current = new StringStreamParameter( xfer, level, m_length
					   , m_delimiter, mandatory );
    break;
  case DataDictionary::type_StructVariable:
    BUG_MSG( "create StructStreamParameter" );
    m_current = new StructStreamParameter( xfer, level, 0, m_delimiter, isLocale() );
    {
      UserAttr *attr = static_cast<UserAttr*>(xfer->Data()->getUserAttr());
      if( attr != 0 ){
	static_cast<StructStreamParameter *>(m_current)->setClassname(  attr->Classname() );
      }
      else
	std::cerr << "UserAttr of " << m_current->getName() << " is NULL" << std::endl;
    }
    break;
  case DataDictionary::type_Complex:
    BUG_MSG( "create ComplexStreamParameter" );
    m_current = new ComplexStreamParameter( xfer, level, m_length, m_prec,
											m_scale != 0 ? m_scale->copy() : 0,
											mandatory, isLocale());
    break;
  case DataDictionary::type_StructDefinition:
    std::cerr << "StructDefinition StreamParameter not yet implemented" << std::endl;

  case DataDictionary::type_Bad:

  default:
    break;
  }

//   if( m_current != 0 ){
//     if( !m_current->linkWithIndex( m_parent ) ){
  if( m_parent != 0 ){
    if( !m_parent->linkWithIndex( m_current ) ){

      if( xfer ){
	UserAttr *attr = xfer->getUserAttr();
	if( xfer -> getNumIndexes() > 0 ){
	  m_current -> setScalar( true );
	}
	else {
	  if( attr != 0 ){
	    m_current -> setScalar( attr -> IsScalar() );
	  }
	}
	if( attr != 0 ){
	  m_current -> setCell( attr -> IsCell() );
	}
      }
    }
  }
}

//============================================================================
// AddLevel
//============================================================================
void VarStreamParameter::addLevel( const std::string &name ){
  m_suffix += '.' + name;
}

//============================================================================
// SetIndex
//============================================================================

void VarStreamParameter::setIndex( const std::string &name, int val ){
  BUG_PARA( BugStreamer, "setIndex", "name=" << name << ", val=" << val );
  DataStreamParameter::setIndex( name, val );
  resolveVars();
}

void VarStreamParameter::setIndex( int pos, int val ){
  BUG_PARA( BugStreamer, "setIndex", "pos=" << pos << ", val=" << val );
  DataStreamParameter::setIndex( pos, val );
  resolveVars();
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void VarStreamParameter::marshal( std::ostream &os ){
  os << "<VarStreamParameter";
  os << " suffix=\"" << m_suffix << "\"";
  os << " length=\"" << m_length << "\"";
  os << " prec=\""   << m_prec   << "\"";
  if( m_tsep )
    os << " tsep=\"true\"";
  os << ">\n";

//   if( m_current != 0 ){
//     os << "<current>\n";
//     m_current -> marshal( os );
//     os << "</current>\n";
//   }

  // m_parent

//   m_item  -> marshal( os );
  DataStreamParameter::marshal( os );

  m_scale -> marshal( os );

  os << "</VarStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *VarStreamParameter::unmarshal( const std::string &element,
					     const XMLAttributeMap &attributeList ){
  if( element == "VarStreamParameter" ){
    std::string s=attributeList["suffix"];
    if( !s.empty() ){
      m_suffix = s;
    }
    s=attributeList["length"];
    if( !s.empty() ){
      int iVal;
      std::istringstream is( s );
      is >> iVal;
      m_length = iVal;
    }
    s=attributeList["prec"];
    if( !s.empty() ){
      int iVal;
      std::istringstream is( s );
      is >> iVal;
      m_prec = iVal;
    }
    s=attributeList["tsep"];
    if( !s.empty() )
      m_tsep = true;
    else
      m_tsep = false;
    return this;
  }
  if( element == "DataStreamParameter" ){
    std::string s=attributeList["indexNum"];
    if( !s.empty() ){
      int iVal;
      std::istringstream is( s );
      is >> iVal;
      m_indexNum = iVal;
    }
    s=attributeList["level"];
    if( !s.empty() ){
      int iVal;
      std::istringstream is( s );
      is >> iVal;
      setLevel( iVal );
    }
    return this;
  }
  if( element == "XferDataItem" ){
    m_dataitem = new XferDataItem();
    m_dataitem -> unmarshal( element, attributeList );
    return m_dataitem;
  }
  if( element == "Scale" ){
    m_scale = new Scale();
    m_scale->unmarshal( element, attributeList );
    return this;
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void VarStreamParameter::setText( const std::string &text ){
}
