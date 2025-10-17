
#include <limits.h>
#include "utils/Debugger.h"

#include "xml/XMLFactory.h"
#include "streamer/XMLStreamDocumentHandler.h"
#include "xfer/XferDataItem.h"
#include "app/DataPoolIntens.h"

#include "streamer/XMLStreamParameter.h"

void def(std::istream &source, std::ostream &dest );

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XMLStreamParameter::XMLStreamParameter( XferDataItem *xfer
					, bool stand_alone
					, bool valid_items_only )
  : StreamParameter()
  , m_xfer( xfer )
  , m_allCycles( false )
  , m_stand_alone( stand_alone )
  , m_valid_items_only( valid_items_only ){
}

XMLStreamParameter::~XMLStreamParameter(){}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool XMLStreamParameter::read( std::istream &is ){
  BUG_DEBUG( "read(std::istream &)" );

  XMLFactory &factory = XMLFactory::Instance();
  factory.reset();
  XMLStreamDocumentHandler *handler =
    new XMLStreamDocumentHandler; // will be deleted by the factory

  handler->setRoot( m_xfer );
  if( m_allCycles )
    handler->withAllCycles();
  factory.setSAXDocumentHandler( handler );
  factory.saxParse( is );
  return true;
}

#ifdef __MINGW32__
bool XMLStreamParameter::read(const  std::string &filename ){
  BUG_DEBUG( "read(std::string &)" );

  XMLFactory &factory = XMLFactory::Instance();
  factory.reset();
  XMLStreamDocumentHandler *handler =
    new XMLStreamDocumentHandler; // will be deleted by the factory
  handler->setRoot( m_xfer );
  if( m_allCycles )
    handler->withAllCycles();
  factory.setSAXDocumentHandler( handler );
  factory.saxParse( filename );
  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool XMLStreamParameter::write( std::ostream &os ){
  bool debug = true;
  BUG(BugStreamer, "write(ostream &)" );

  if( m_stand_alone ){
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    if ( !m_stylesheet.empty() ) {
      os << "<?xml-stylesheet type=\"text/xsl\" href=\""+m_stylesheet+"\"?>" << std::endl;
    }
  }

  DataPoolIntens &dpi =  DataPoolIntens::Instance();

  std::vector<std::string> attrs = m_xmlAttributes;
  if( !m_schema.empty() ){
    if( !m_namespace.empty() ){
      attrs.push_back("xsi:schemaLocation=\""+m_namespace+" "+m_schema+"\"");
    }
    else{
      attrs.push_back("xsi:schemaLocation=\""+m_schema+"\"");
    }
    attrs.push_back("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" );
  }
  if( !m_namespace.empty() ){
    attrs.push_back( "xmlns=\""+m_namespace+"\"" );
  }
  if( !m_version.empty() ){
      attrs.push_back("version=\""+m_version+"\"");
  }

  if( m_xfer ){
    BUG_MSG( "internal DataItem is " << m_xfer->getFullName( true ) );

    int level = 0;
    if( !m_xfer->writeXML( os, attrs, level, debug ) ){
      std::string tag = m_xfer->getName();
      os << "  <" << tag << ">" << std::endl;
      os << "  </" << tag << ">" << std::endl;
    }
  }
  else{
    BUG_MSG( "DATAPOOL" );

    os << "<DATAPOOL>" << std::endl;

    int  numCycles = 1;
    int currentCycle = dpi.currentCycle();
    if( m_allCycles )
      numCycles = dpi.numCycles();
    if( numCycles > 1 )
      os << "<vector>" << std::endl;

    for( int i = 0; i < numCycles; ++i ){
      os << "<contents>" << std::endl;
      if( m_allCycles ){
        dpi.goCycle( i, false ); // do not clear undo stack
      }
      DataDictionary *dD = DataPoolIntens::Instance().getDataPool().GetRootDict()->getStructure();
      while ( dD ){
        switch (dD->getDataType() ){
        case DataDictionary::type_Integer:
        case DataDictionary::type_Real:
        case DataDictionary::type_String:
        case DataDictionary::type_Complex:
        case DataDictionary::type_StructVariable:{
          std::string name = dD->getName();
          if( name.find_first_of( "@" ) == std::string::npos
              && name.find( "__stream_data_" ) == std::string::npos
              && name.find( "__filestream_" ) == std::string::npos ){
            int level = 2;
            dpi.getDataReference( name )->writeXML( os, m_xmlAttributes, level, debug );
          }
          break;
        }
        default:
          break;
        }
        dD = dD->getNext();
      }
      os << "</contents>" << std::endl;
    }

    if( numCycles > 1 ){
      os << "</vector>" << std::endl;
    }
    os << "</DATAPOOL>" << std::endl;

    if( m_allCycles ){
      dpi.goCycle( currentCycle, false ); // do not clear undo stack
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setindex --                                                                 */
/* --------------------------------------------------------------------------- */

void XMLStreamParameter::setIndex( const std::string &name, int index ) {
  BUG_PARA( BugStreamer, "setIndex", "name=" << name << ", index=" << index );
  if( m_xfer!=0 ){
    m_xfer->setIndex( name, index );
  }
  else{
    BUG_EXIT( "no DataItem" );
  }
}

/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */

std::string XMLStreamParameter::getName()const{
  return "XMLStream";
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void XMLStreamParameter::marshal( std::ostream &os ){
  os << "<XMLStreamParameter>\n";
  if( !m_xmlAttributes.empty() ){
    for( int i=0; i<m_xmlAttributes.size(); ++i ){
      os << "<xmlAttributes value=\""
	 << m_xmlAttributes[i]
	 << "\"></xmlAttributes>\n";
    }
  }
  if( m_xfer != 0 )
    m_xfer -> marshal( os );
  os << "</XMLStreamParameter>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *XMLStreamParameter::unmarshal( const std::string &element,
                                             const XMLAttributeMap &attributeList ){
  if( element == "xmlAttributes" ){
    m_xmlAttributes.push_back( attributeList["value"] );
    return this;
  }
  if( element == "XferDataItem" ){
    m_xfer = new XferDataItem();
    return m_xfer -> unmarshal( element, attributeList );
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void XMLStreamParameter::setText( const std::string &text ){
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool XMLStreamParameter::isValid(bool validErrorMsg){
  return  m_xfer == 0 ? false : m_xfer->isValid();
}
