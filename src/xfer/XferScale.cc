
#include <assert.h>
#include <limits>

#include "xfer/XferScale.h"
#include "xfer/XferDataItem.h"
#include "gui/GuiIndex.h"
#include "gui/GuiManager.h"
#include "xml/XMLDocumentHandler.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XferScale::XferScale( XferDataItem *item, char _operator )
  : Scale( 0., _operator ){
  assert( item != 0 );
  assert( _operator == '*' || _operator == '/' );
  item->setDimensionIndizes();
  m_dataItem = new XferDataItem( *item );
}

XferScale::XferScale( char _operator )
  : Scale( 0., _operator ),  m_dataItem(0) {}

XferScale::XferScale( const XferScale &scale ){
  *this = scale;
  m_dataItem = new XferDataItem( *(scale.m_dataItem) );
}

XferScale::~XferScale(){
  delete m_dataItem;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* operator= --                                                                */
/* --------------------------------------------------------------------------- */

Scale &XferScale::operator=( const Scale &scale ){
  if( this == &scale )
    return *this;
  Scale::operator=( scale );
  return *this;
}

/* --------------------------------------------------------------------------- */
/* scale --                                                                    */
/* --------------------------------------------------------------------------- */
Scale *XferScale::copy(){
  return new XferScale( *this );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

double XferScale::getValue() const{
  BUG_DEBUG(  "getValue" );
  double value = m_value;
  if( m_dataItem != 0 ){
    if( !( m_dataItem->getValue( value ) ) ){
      if (AppData::Instance().DefaultScaleFactor1()) {
        value = 1;
      } else {
        return std::numeric_limits<double>::quiet_NaN();
      }
    }
  }
  if( m_operator == '*' ){
    BUG_DEBUG(  "operator is : * , value is : " << value );
    return value;
  }
  BUG_DEBUG(  "operator is : / , value is : " << value );
  return 1./value;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void XferScale::setIndex( int wildcardNumberBackwards, int value ){
  assert( wildcardNumberBackwards > 0 );
  assert( value >= 0 );
  BUG_DEBUG(  "setIndex" );
  int wildcardNumberForwards = m_dataItem->getNumOfWildcards() - wildcardNumberBackwards + 1;
  if( wildcardNumberForwards < 1 )
    return;
  m_dataItem->getDataItemIndexWildcard( wildcardNumberForwards )->setIndex( m_dataItem->Data(), value );
}

/* --------------------------------------------------------------------------- */
/* isUpdated --                                                                */
/* --------------------------------------------------------------------------- */

bool XferScale::isUpdated( TransactionNumber trans, bool gui ){
  BUG_DEBUG(  "isUpdated" );
  bool rslt = true;
  if( m_dataItem == 0 ){
    BUG_DEBUG(  "no xfer" );
    rslt =  false;
  }
  else{
    rslt =  m_dataItem->isUpdated( trans, gui );
    if( rslt ){
      BUG_DEBUG(  m_dataItem->getFullName( true ) << "is updated" );
    }
    else {
      BUG_DEBUG(  m_dataItem->getFullName( true ) << "is not updated" );
    }
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/*  getAttributes --                                                           */
/* --------------------------------------------------------------------------- */

DATAAttributeMask XferScale::getAttributes( TransactionNumber trans ){
  BUG_DEBUG(  "getAttributes" );
  if( m_dataItem == 0 ){
    BUG_DEBUG(  "no dataitem");
    return 0;
  }
  BUG_DEBUG( "this = " << this << " dataitem : " << m_dataItem->getFullName( true ) );
  return m_dataItem->Data()->getAttributes( trans );
}

/* --------------------------------------------------------------------------- */
/* registerIndex --                                                            */
/* --------------------------------------------------------------------------- */

void XferScale::registerIndex( GuiIndexListener *listener ){
  assert( m_dataItem != 0 );
  XferDataItemIndex *itemindex = 0;
  int i = 0;
  GuiElement *element = 0;
  GuiIndex *index = 0;

  while( (itemindex=m_dataItem->getDataItemIndexIndexed(++i)) != 0 ){
    element = GuiElement::findElement( itemindex->IndexName() );
    // Lokale GuiIndex Elemente in einer Fieldgroup sind im GuiManager nicht
    // registiert und muessen nicht beachtet werden. Die GuiFieldgroup ueber-
    // nimmt diese Arbeit.
    if( element != 0 ){
      assert( element->Type() == GuiElement::type_Index );
//       index = dynamic_cast<GuiIndex *>(element);
      index = element->getGuiIndex();
      index->registerIndexedElement( listener );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* unregisterIndex --                                                          */
/* --------------------------------------------------------------------------- */

void XferScale::unregisterIndex( GuiIndexListener *listener ){
  assert( m_dataItem != 0 );
  XferDataItemIndex *itemindex = 0;
  int i = 0;
  GuiElement *element = 0;
  GuiIndex *index = 0;

  while( (itemindex=m_dataItem->getDataItemIndexIndexed(++i)) != 0 ){
    element = GuiElement::findElement( itemindex->IndexName() );
    // Lokale GuiIndex Elemente in einer Fieldgroup sind im GuiManager nicht
    // registiert und muessen nicht beachtet werden. Die GuiFieldgroup ueber-
    // nimmt diese Arbeit.
    if( element != 0 ){
      assert( element->Type() == GuiElement::type_Index );
//       index = static_cast<GuiIndex *>(element);
      index = element->getGuiIndex();
      index->unregisterIndexedElement( listener );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool XferScale::acceptIndex( const std::string &name, int inx ){
  assert( m_dataItem != 0 );
  return m_dataItem->acceptIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferScale::setIndex( const std::string &name, int inx ){
  assert( m_dataItem != 0 );
  assert( inx >= 0 );
  return m_dataItem->setIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void XferScale::marshal( std::ostream &os ){
  os << "<XferScale";
  os << " operator=\"" << m_operator << "\"";
  os << " value=\"" << m_value << "\"";
  os << ">\n";
  m_dataItem -> marshal( os );
  os << "</XferScale>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable * XferScale::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "XferScale" ){
    std::string s;
    s=attributeList["operator"];
    if( s.empty() ){
    } else {
      char c;
      std::istringstream is( s );
      is >> c;
      m_operator = c;
    }
    s=attributeList["value"];
    if( !s.empty() ){
      std::istringstream is( s );
      is >> m_value;
    }
    return this;
  }
  else if( element == "XferDataItem" ){
    m_dataItem = new XferDataItem();
    return m_dataItem -> unmarshal( element, attributeList );
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void XferScale::setText( const std::string &text ){
}
