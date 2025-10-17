
#include "utils/Debugger.h"

#include "app/DataSet.h"
#include "app/DataPoolIntens.h"
#include "xfer/XferDataParameter.h"
#include "xfer/XferParameterString.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "streamer/StringDateStreamParameter.h"
#include "xml/XMLDocumentHandler.h"
#include "utils/Date.h"
#include "utils/JsonUtils.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

StringDateStreamParameter::StringDateStreamParameter( char delimiter )
  : DataStreamParameter( 0, 0, 0, false, delimiter ) {
}

StringDateStreamParameter::StringDateStreamParameter( XferDataParameter *param,
                                                      int level,
                                                      int length,
                                                      bool mand,
                                                      UserAttr::STRINGtype type,
                                                      char delimiter )
  : DataStreamParameter( param->DataItem() ,level ,0 ,mand, delimiter )
  , m_converter(length )
  , m_param( param )
  , m_type( type )
{
}

StringDateStreamParameter::~StringDateStreamParameter(){
  delete m_param;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool StringDateStreamParameter::read( std::istream &is ){
  BUG_PARA(BugStreamer, "StringDateStreamParameter::read" ,getName() );
  std::string s;
  if( !m_converter.read( is, s, getDelimiter()) ){
    return false;
  }
  if (m_type == UserAttr::string_kind_date ||
      m_type == UserAttr::string_kind_time ||
      m_type == UserAttr::string_kind_datetime) {
    s = convertFromISODate(s, m_type);
  } else if (m_type == UserAttr::string_kind_value) {
    Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(s);
    if (!valueObject.isNull()) {
      s = valueObject["value"].asString();
    }
    else std::cerr << " not a valid json stream '"<<s<<"'\n";
  } else
    assert(false); // TODO
  return s.size() ? m_param->setFormattedValue( s ) : false;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool StringDateStreamParameter::write( std::ostream &os ){
  BUG( BugStreamer, "StringDateStreamParameter::write");
  createDataItemIndexes();

  std::string s;
  m_param->getFormattedValue( s );
  if (m_type == UserAttr::string_kind_date ||
      m_type == UserAttr::string_kind_time ||
      m_type == UserAttr::string_kind_datetime) {
    if( !s.empty() ){
      s = convertFromISODate(s, m_type);
    }
  }
  else if (m_type == UserAttr::string_kind_value) {
    Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(s);
    if (!valueObject.isNull()) {
      s = valueObject["value"].asString();
    }
  }
  if( s.empty() ){
    removeDataItemIndexes();
    BUG_MSG("not valid or empty");
    // is it ok to silently ignore this?
  }
  bool rslt = m_converter.write( s, os );
  if( m_converter.getWidth() == 0 ){
    os << getDelimiter();
  }
  removeDataItemIndexes();
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void StringDateStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  assert( false );
//   BUG( BugStreamer, "StringDateStreamParameter::putValues" );
//   if( dest != 0 ){
//     BUG_MSG( getName() );
//     std::string s;
//     createDataItemIndexes();
//     setDataItemIndexes();
//     if( m_dataitem->getValue(s) ){
//       dest -> putValue(*this, s );
//     }
//     else
//       dest -> putValue( *this, "" );
//     removeDataItemIndexes();
//   }
}
