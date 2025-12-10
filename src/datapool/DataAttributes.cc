
#include "datapool/DataGlobals.h"
#include "datapool/DataAttributes.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAttributes::DataAttributes()
  : m_attr_mask( 0 ){
}

DataAttributes::DataAttributes( DATAAttributeMask mask )
  : m_attr_mask( mask ){
}

DataAttributes::DataAttributes( const DataAttributes &attr )
  : m_attr_mask( attr.m_attr_mask ){
}

DataAttributes::~DataAttributes(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getAttributes --                                                            */
/* --------------------------------------------------------------------------- */

DATAAttributeMask DataAttributes::getAttributes() const{
  return m_attr_mask;
}

/* --------------------------------------------------------------------------- */
/* setAttributes --                                                            */
/* --------------------------------------------------------------------------- */

bool DataAttributes::setAttributes( DATAAttributeMask set_mask
                                  , DATAAttributeMask reset_mask )
{
  DATAAttributeMask attr = m_attr_mask;
  m_attr_mask = m_attr_mask | set_mask;
  if( reset_mask != 0 ){
    m_attr_mask = ( m_attr_mask | reset_mask ) ^ reset_mask;
  }
  return !( attr == m_attr_mask);
}

/* --------------------------------------------------------------------------- */
/* resetAttributes --                                                          */
/* --------------------------------------------------------------------------- */

bool DataAttributes::resetAttributes( DATAAttributeMask mask ){
  // Das valid-Bit darf hier nicht manipuliert werden.
  mask = resetAttrBits( mask, DATAisValid );
  bool is_valid = isAttributeSet( DATAisValid );
  DATAAttributeMask old_mask = resetAttrBits( m_attr_mask, DATAisValid );

  if( old_mask == mask ){
    return false;
  }
  m_attr_mask = mask;
  if( is_valid ){
    setAttribute( DATAisValid );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setAttrBits --                                                              */
/* --------------------------------------------------------------------------- */

DATAAttributeMask DataAttributes::setAttrBits( DATAAttributeMask mask
                                             , DATAAttributeMask set_mask )
{
  if( set_mask != 0 ){
    mask = ( mask | set_mask );
  }
  return mask;
}

/* --------------------------------------------------------------------------- */
/* resetAttrBits --                                                            */
/* --------------------------------------------------------------------------- */

DATAAttributeMask DataAttributes::resetAttrBits( DATAAttributeMask mask
                                               , DATAAttributeMask reset_mask )
{
  if( reset_mask != 0 ){
    mask = ( mask | reset_mask ) ^ reset_mask;
  }
  return mask;
}

/* --------------------------------------------------------------------------- */
/* setAttribute --                                                             */
/* --------------------------------------------------------------------------- */

bool DataAttributes::setAttribute( DATAAttributeMask mask, bool set ){
  if( set ){
    return setAttributes( mask, 0 );
  }
  else{
    return setAttributes( 0, mask );
  }
}

/* --------------------------------------------------------------------------- */
/* resetAttribute --                                                           */
/* --------------------------------------------------------------------------- */

bool DataAttributes::resetAttribute( DATAAttributeMask reset_mask ){
  return setAttributes( 0, reset_mask );
}

/* --------------------------------------------------------------------------- */
/* isAttributeSet --                                                           */
/* --------------------------------------------------------------------------- */

bool DataAttributes::isAttributeSet( DATAAttributeMask mask ) const{
  return (m_attr_mask & mask) != 0;
}

/* --------------------------------------------------------------------------- */
/* writeDPattributes --                                                        */
/* --------------------------------------------------------------------------- */

void DataAttributes::writeDPattributes( std::ostream &ostr ) const{
  if( isAttributeSet( DATAisLocked ) ){
    ostr << " lock=\"1\"";
  }
  if( isAttributeSet( DATAisEqual ) ){
    ostr << " eql=\"" << 1 << "\"";
  }
  if( isAttributeSet( DATAisDifferent ) ){
    ostr << " neq=\"" << 1 << "\"";
  }
  //if( isAttributeSet( DATAisLeftOnly ) ){
  //  ostr << " left=\"" << 1 << "\"";
  //}
  //if( isAttributeSet( DATAisRightOnly ) ){
  //  ostr << " right=\"" << 1 << "\"";
  //}
  writeDPattributeBits( ostr );
}

/* --------------------------------------------------------------------------- */
/* writeDPattributeBits --                                                     */
/* --------------------------------------------------------------------------- */

void DataAttributes::writeDPattributeBits( std::ostream &ostr ) const{
  for( int x = 0; x < 32; x++ ){
    if( isAttributeSet( (1L<<x) ) ){
      ostr << " bit" << x << "=\"1\"";
    }
  }
}

/* --------------------------------------------------------------------------- */
/* attributeBits --                                                            */
/* --------------------------------------------------------------------------- */

void DataAttributes::attributeBits( DATAAttributeMask mask, std::ostream &ostr ){
  ostr << ">>" << mask << " = ";
  for( int x = 0; x < 32; x++ ){
    DATAAttributeMask m = (1L<<x);
    if( (mask & m) != 0 ){
      ostr << "[" << x << "]";
    }
  }
  ostr << std::endl;
}

/* --------------------------------------------------------------------------- */
/* setStylesheet--                                                             */
/* --------------------------------------------------------------------------- */

bool DataAttributes::setStylesheet(const std::string &stylesheet) {
  bool ret(m_stylesheet != stylesheet);
  m_stylesheet = stylesheet;
  return ret;
}

/* --------------------------------------------------------------------------- */
/* getStylesheet--                                                             */
/* --------------------------------------------------------------------------- */

const std::string& DataAttributes::getStylesheet() const {
  return m_stylesheet;
}

/* --------------------------------------------------------------------------- */
/* setMediaType--                                                              */
/* --------------------------------------------------------------------------- */

void DataAttributes::setMediaType(const std::string &media_type) {
  m_media_type = media_type;
}

/* --------------------------------------------------------------------------- */
/* getMediaType --                                                             */
/* --------------------------------------------------------------------------- */

const std::string DataAttributes::getMediaType() const {
  return m_media_type;
}
