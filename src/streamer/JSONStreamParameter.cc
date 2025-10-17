
#include <QTime>
#include <limits.h>
#include "utils/Debugger.h"
#include "utils/utils.h"

#include <iterator>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include <stdexcept>

#define RAPIDJSON_PARSE_DEFAULT_FLAGS kParseNanAndInfFlag
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
using namespace rapidjson;
using namespace std;

#include "xfer/XferDataItem.h"
#include "app/DataPoolIntens.h"

#include "streamer/JSONStreamParameter.h"
#include "datapool/DataVector.h"
#include "datapool/DataRealValue.h"
#include "datapool/DataStringValue.h"
#include "datapool/DataComplexValue.h"
#include "datapool/DataIntegerValue.h"
#include "datapool/DataAlterReadJSON.h"
#include "datapool/DataAlterReadRapidJSON.h"

Json::Value JSONStreamParameter::s_root;   // will contain the root value after parsing.

void def(std::istream &source, std::ostream &dest );

INIT_LOGGER();

/*=============================================================================*/
/* MessageHandler for RapidJson-Parser                                         */
/*=============================================================================*/

typedef map<string, XferDataItem *> ItemMap;

struct MessageHandler: public BaseReaderHandler<UTF8<>, MessageHandler> {
  MessageHandler(ItemMap &itemMap, bool tryDbAttrAsKey)
    : m_itemMap( itemMap )
    , m_tryDbAttrAsKey(tryDbAttrAsKey)
    , m_level( 0 )
    , m_ignore( false )
    , m_alterdata( 0 )
    , m_multi_items( false ){
    assert( m_itemMap.size() >= 1 );
    m_multi_items = m_itemMap.size() > 1;
  }

  ~MessageHandler(){
    if( m_alterdata ){
      delete m_alterdata;
    }
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::badBaseLevelValue --                                      */
  /* ------------------------------------------------------------------------- */

  bool badBaseLevelValue() {
    if( m_level == 0 ){
      if( !m_alterdata ){
        if( m_itemMap.size() != 1 ){
          BUG_DEBUG("bad baselevel value");
          return true;
        }
      }
    }
    return false;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::getAlterData --                                           */
  /* ------------------------------------------------------------------------- */

  DataAlterReadRapidJSON *getAlterData(XferDataItem *xfer){
    assert( xfer );
    DataAlterReadRapidJSON *alterdata = new DataAlterReadRapidJSON();
    bool rslt = xfer->alterData( *alterdata );
    if( !rslt ){ // item doesnt exist in datapool
      delete alterdata;
      m_ignore = true;
      return 0;
    }
    return alterdata;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::getAlterDataForSimpleValue --                             */
  /* ------------------------------------------------------------------------- */

  DataAlterReadRapidJSON *getAlterDataForSimpleValue(){
    BUG_DEBUG("Simple Value in JSON-Stream");

    assert( m_level == 0 );
    assert( m_itemMap.size() == 1 );

    ItemMap::iterator it = m_itemMap.begin();
    assert( it != m_itemMap.end() );

    return getAlterData( it->second );
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::StartObject --                                            */
  /* ------------------------------------------------------------------------- */

  bool StartObject() {
    BUG_DEBUG("Begin of MessageHandler::StartObject: Level = " << m_level);

    if( m_level == 0 ){
      BUG_DEBUG("Start Base Object with " << m_itemMap.size() << " items");
      assert( !m_alterdata );
    }

    if( m_alterdata ){
      m_ignore = m_alterdata->setStartObject( m_level );
    }

    BUG_DEBUG("End of MessageHandler::StartObject: Level = " << m_level);
    m_level++;
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::EndObject --                                              */
  /* ------------------------------------------------------------------------- */

  bool EndObject(SizeType) {
    m_level--;
    BUG_DEBUG("Begin of MessageHandler::EndObject: Level = " << m_level);

    if( m_level == 0 ){
      BUG_DEBUG("End of Base Object with " << m_itemMap.size() << " items");
    }

    if( m_alterdata ){
      m_ignore = m_alterdata->setEndObject( m_level );
    }
    BUG_DEBUG("End of MessageHandler::EndObject: Level = " << m_level);
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::Key --                                                    */
  /* ------------------------------------------------------------------------- */

  bool Key(const Ch* str, SizeType length, bool) {
    m_key = string(str, length);
    BUG_DEBUG("Begin of MessageHandler::Key: '" << m_key << "', Level = " << m_level);

    if( m_alterdata ){
      // Der vorherige  Key im selben Objekt wird immer gelöscht,
      // unabhängig vom Level.
      BUG_DEBUG(" --> remove previous key of Object");
      m_ignore = m_alterdata->setEndKey( m_level );
    }

    if( m_level == 1 ){
      BUG_DEBUG(" ==> Base-Level Key <==");
      m_ignore = false;

      if( m_alterdata && m_multi_items ){
        // Auf dem Base-Level werden bei Mehrfach-Elementen
        // immer neue Funktoren für den Datapool-Zugriff erstellt
        delete m_alterdata;
        m_alterdata = 0;
      }

      ItemMap::iterator it = m_itemMap.find(m_key);
      if( it != m_itemMap.end() ){
        // Auch falls nur ein Element in der Liste ist, wird dieser
        // Fall als 'multiple elements' behandelt.
        BUG_DEBUG(" --> Key " << m_key << " IS available: Stream with multiple elements");
        m_multi_items = true;

        XferDataItem *xfer = it->second;
        assert( xfer );
        m_alterdata = new DataAlterReadRapidJSON();
        bool rslt = xfer->alterData( *m_alterdata );
        if( !rslt ){ // item doesnt exist in datapool
          delete m_alterdata;
          m_alterdata = 0;
          m_ignore = true;
        }

        BUG_DEBUG("End of MessageHandler::Key: '" << m_key << "' (Base-Level)");
        return true;
      }

      BUG_DEBUG(" --> Key " << m_key << " IS NOT available");
      if( m_multi_items ){
        BUG_DEBUG(" --> ... but stream with multiple elements");
        m_ignore = true;
        BUG_DEBUG("End of MessageHandler::Key: '" << m_key << "' (Base-Level)");
        return true;
      }

      // One element stream! Falls der Funktor bereits vorhanden
      // ist, wird er wieder verwendet
      if( !m_alterdata ){
        // Erster Key im Objekt
        ItemMap::iterator it = m_itemMap.begin();
        assert( it != m_itemMap.end() );

        XferDataItem *xfer = it->second;
        assert( xfer );
        // check for strukt item
        if( xfer->getDataType() != DataDictionary::type_StructVariable ){
          m_ignore = true;
          return false;
        }

        m_alterdata = new DataAlterReadRapidJSON();
        bool rslt = xfer->alterData( *m_alterdata );
        if( !rslt ){
          m_ignore = true;
        }
      }
      if( !m_ignore ){
        m_ignore = m_alterdata->setKey( m_key, m_level, m_tryDbAttrAsKey );
      }
      BUG_DEBUG("End of MessageHandler::Key: '" << m_key << "' (Base-Level)");
      return true;
    }

    BUG_DEBUG(" ==> Next Level Key <==");
    if( !m_alterdata ){
      // override
      BUG_DEBUG(" <== no alterData-Object: override");
      return true;
    }

    if( !m_ignore ){
      m_ignore = m_alterdata->setKey( m_key, m_level, m_tryDbAttrAsKey );
    }

    BUG_DEBUG("End of MessageHandler::Key: '" << m_key << "' (Next-Level)");
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::String --                                                 */
  /* ------------------------------------------------------------------------- */

  bool String(const char* str, SizeType length, bool) {
    if( m_ignore ){
      return true;
    }
    BUG_DEBUG("String");

    if( badBaseLevelValue() ){
      return false;
    }

    string s(str, length);

    if( !m_alterdata ){ // Spezialfall: Nur ein Wert im Stream
      m_alterdata = getAlterDataForSimpleValue();
      if( m_alterdata ){ // item exists in datapool
        m_alterdata->setString( s );
        delete m_alterdata;
        m_alterdata = 0;
      }
      return true;  // the end
    }

    m_alterdata->setString( s );
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::Int --                                                    */
  /* ------------------------------------------------------------------------- */

  bool Int(int i) {
    if( m_ignore ){
      return true;
    }
    BUG_DEBUG("Int");

    if( badBaseLevelValue() ){
      return false;
    }

    if( !m_alterdata ){ // Spezialfall: Nur ein Wert im Stream
      m_alterdata = getAlterDataForSimpleValue();
      if( m_alterdata ){ // item exists in datapool
        m_alterdata->setInteger( i );
        delete m_alterdata;
        m_alterdata = 0;
      }
      return true;  // the end
    }

    assert( m_alterdata );
    m_alterdata->setInteger( i );
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::Uint --                                                   */
  /* ------------------------------------------------------------------------- */

  bool Uint(unsigned i) {
    if( m_ignore ){
      return true;
    }
    BUG_DEBUG("Uint");

    if( badBaseLevelValue() ){
      return false;
    }

    if( !m_alterdata ){ // Spezialfall: Nur ein Wert im Stream
      m_alterdata = getAlterDataForSimpleValue();
      if( m_alterdata ){ // item exists in datapool
        m_alterdata->setUInteger( i );
        delete m_alterdata;
        m_alterdata = 0;
      }
      return true;  // the end
    }

    assert( m_alterdata );
    m_alterdata->setUInteger( i );
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::int64 --                                                  */
  /* ------------------------------------------------------------------------- */

  bool Int64(int64_t i) {
    if( m_ignore ){
      return true;
    }
    BUG_DEBUG("Int64");

    if( badBaseLevelValue() ){
      return false;
    }

    if( !m_alterdata ){ // Spezialfall: Nur ein Wert im Stream
      m_alterdata = getAlterDataForSimpleValue();
      if( m_alterdata ){ // item exists in datapool
        m_alterdata->setDouble( i );
        delete m_alterdata;
        m_alterdata = 0;
      }
      return true;  // the end
    }

    assert( m_alterdata );
    m_alterdata->setDouble( i );
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::Uint64 --                                                 */
  /* ------------------------------------------------------------------------- */

  bool Uint64(uint64_t i) {
    if( m_ignore ){
      return true;
    }
    BUG_DEBUG("Uint64");

    if( badBaseLevelValue() ){
      return false;
    }

    if( !m_alterdata ){ // Spezialfall: Nur ein Wert im Stream
      m_alterdata = getAlterDataForSimpleValue();
      if( m_alterdata ){ // item exists in datapool
        m_alterdata->setDouble( i );
        delete m_alterdata;
        m_alterdata = 0;
      }
      return true;  // the end
    }

    assert( m_alterdata );
    m_alterdata->setDouble( i );
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::Double --                                                 */
  /* ------------------------------------------------------------------------- */

  bool Double(double d) {
    if( m_ignore ){
      return true;
    }
    BUG_DEBUG("Double");

    if( badBaseLevelValue() ){
      return false;
    }

    if( !m_alterdata ){ // Spezialfall: Nur ein Wert im Stream
      m_alterdata = getAlterDataForSimpleValue();
      if( m_alterdata ){ // item exists in datapool
        if( std::isfinite(d) ) {
          m_alterdata->setDouble( d );
        }
        else{
          m_alterdata->setInvalid();
        }
        delete m_alterdata;
        m_alterdata = 0;
      }
      return true;  // the end
    }

    assert( m_alterdata );
    if( std::isfinite(d) ) {
      m_alterdata->setDouble( d );
    }
    else{
      m_alterdata->setInvalid();
    }
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::Null --                                                   */
  /* ------------------------------------------------------------------------- */

  bool Null(){
    if( m_ignore ){
      return true;
    }
    BUG_DEBUG("Null");

    if( badBaseLevelValue() ){
      return false;
    }

    if( !m_alterdata ){ // Spezialfall: Nur ein Wert im Stream
      m_alterdata = getAlterDataForSimpleValue();
      if( m_alterdata ){ // item exists in datapool
        m_alterdata->setInvalid();
        delete m_alterdata;
        m_alterdata = 0;
      }
      return true;  // the end
    }

    assert( m_alterdata );
    m_alterdata->setInvalid();
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::Bool --                                                   */
  /* ------------------------------------------------------------------------- */

  bool Bool(bool b){
    if( m_ignore ){
      return true;
    }
    BUG_DEBUG("Bool");

    if( badBaseLevelValue() ){
      return false;
    }

    if( !m_alterdata ){ // Spezialfall: Nur ein Wert im Stream
      m_alterdata = getAlterDataForSimpleValue();
      if( m_alterdata ){ // item exists in datapool
        m_alterdata->setInteger( b ? 1 : 0 );
        delete m_alterdata;
        m_alterdata = 0;
      }
      return true;  // the end
    }

    assert( m_alterdata );
    m_alterdata->setInteger( b ? 1 : 0 );
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::StartArray --                                             */
  /* ------------------------------------------------------------------------- */

  bool StartArray() {
    BUG_DEBUG("Begin of MessageHandler::StartArray: level = " << m_level);

    if( badBaseLevelValue() ){
      return false;
    }

    if( m_level == 0 ){
      BUG_DEBUG(" --> Base Level Array ");
      assert( m_itemMap.size() == 1 );
      assert( !m_alterdata );

      ItemMap::iterator it = m_itemMap.begin();
      m_ignore = false;
      m_alterdata = new DataAlterReadRapidJSON();
      XferDataItem *xfer = it->second;
      assert( xfer );
      bool rslt = xfer->alterData( *m_alterdata );
      if( !rslt ){
        BUG_DEBUG(" --> alterData failed: the end");
        BUG_DEBUG("End of MessageHandler::StartArray (abort)");
        return false; // the end
      }
    }

    m_level++;
    if( m_ignore ){
      BUG_DEBUG("End of MessageHandler::StartArray (override)");
      return true;
    }

    assert( m_alterdata );
    m_alterdata->setStartArray( m_level );
    BUG_DEBUG("End of MessageHandler::StartArray");
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::EndArray --                                               */
  /* ------------------------------------------------------------------------- */

  bool EndArray(SizeType) {
    BUG_DEBUG("MessageHandler::EndArray");
    m_level--;
    if( m_ignore ){
      BUG_DEBUG("End of MessageHandler::EndArray (override)");
      return true;
    }

    assert( m_alterdata );
    m_alterdata->setEndArray();

    BUG_DEBUG("End of MessageHandler::EndArray: level = " << m_level);
    return true;
  }

  /* ------------------------------------------------------------------------- */
  /* MessageHandler::Default --                                                */
  /* ------------------------------------------------------------------------- */

  bool Default() {
    BUG_DEBUG("Default");
    return false;
  } // All other events are invalid.

  ItemMap                  m_itemMap;
  bool                     m_tryDbAttrAsKey;
  std::string              m_key;
  int                      m_level;
  bool                     m_ignore;
  DataAlterReadRapidJSON  *m_alterdata;
  bool                     m_multi_items;
};

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JSONStreamParameter::JSONStreamParameter( XferDataItem *xfer, int i )
  : StreamParameter()
  , m_xfer( xfer )
  , m_indentation( i )
  , m_allCycles( false )
{
}

JSONStreamParameter::~JSONStreamParameter(){}

/* --------------------------------------------------------------------------- */
/* getChildType --                                                             */
/* --------------------------------------------------------------------------- */

Json::ValueType JSONStreamParameter::getChildType( Json::Value &value ){
  int size = value.size();
  for ( int index =0; index < size; ++index ){
    if ( value[index].type() == Json::nullValue){
      continue;
    }
    return value[index].type();
  }
  return Json::nullValue;
}

/* --------------------------------------------------------------------------- */
/* clearRange --                                                               */
/* --------------------------------------------------------------------------- */

void JSONStreamParameter::clearRange( bool incl_itemattr ){
  BUG(BugStreamer,"clearRange");

  if( m_xfer != 0 ){
    m_xfer->clearRangeMaxLevel( incl_itemattr );
  }
}

/* --------------------------------------------------------------------------- */
/* setDbItemsNotModified --                                                    */
/* --------------------------------------------------------------------------- */

void JSONStreamParameter::setDbItemsNotModified(){
  BUG(BugStreamer,"setDbItemsNotModified");

  if( m_xfer == 0 ){
    BUG_EXIT( "no item available" );
    return;
  }

  m_xfer->setDbItemsNotModified();
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::isValid(bool validErrorMsg){
  return  m_xfer == 0 ? false : m_xfer->isValid();
}

/* --------------------------------------------------------------------------- */
/* isElementStream --                                                          */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::isElementStream(){
  return m_xfer != 0;
}

/* --------------------------------------------------------------------------- */
/* isDatapoolStream --                                                         */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::isDatapoolStream(){
  if( isElementStream() ){
    return false;
  }
  return m_allCycles;
}

/* --------------------------------------------------------------------------- */
/* isCycleStream --                                                            */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::isCycleStream(){
  if( isElementStream() ){
    return false;
  }
  return !m_allCycles;
}

/* --------------------------------------------------------------------------- */
/* readJson --                                                                 */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::readJson( std::istream &is,
                                    ParameterList &param_list )
{
  BUG_DEBUG("Begin of JSONStreamParameter::readJson");
  clearErrorMessageStream();
  clearWarningMessageStream();

  if( !is ){
    // empty stream
    BUG_DEBUG("End of JSONStreamParameter::readJson: no json-stream available to parse");
    return true;
  }

  if( isDatapoolStream() || isCycleStream() ){
    BUG_DEBUG("is Datapool or Cycle");
    return read( is );
  }

  assert( isElementStream() );

  std::string input;
  char c;
  c = is.get();
  while(is){
    input += c;
    c = is.get();
  }

  ItemMap itemMap;
  ParameterList::iterator it = param_list.begin();
  for ( ; it != param_list.end(); it++ ){
    if( !(*it)->isJson() ){
      BUG_DEBUG("End of JSONStreamParameter::readJson: Parameter is not Json");
      return false;
    }
    JSONStreamParameter *p = (*it)->getJSONStreamParameter();
    assert( p );
    XferDataItem *xfer = p->getXferDataItem();
    assert( xfer );
    string name( xfer->getName() );
    if((m_serialize_flags & HIDEtransient) &&  // stream hides TRANSIENT items, use DBATTR if given
       !p->getDbAttr().empty()) {  // DBATTR is given
      name = p->getDbAttr();
    }
    itemMap.insert(ItemMap::value_type(name, xfer));
  }

  Reader reader;
  MessageHandler handler(itemMap, m_serialize_flags & HIDEtransient);
  StringStream inputstream( input.c_str() );

  BUG_DEBUG("===== parse JSON =====");
  if( reader.Parse(inputstream, handler) ){
    BUG_DEBUG("===== successfully parsed =====");
    return true;
  }
  BUG_DEBUG("===== NOT successfully parsed =====");

  ParseErrorCode e = reader.GetParseErrorCode();
  size_t o = reader.GetErrorOffset();

  getErrorMessageStream() << "Failed to parse json '"
                          << (m_xfer == 0 ? "" : m_xfer->getFullName(false))
                          << "'" << std::endl
                          << "Error: " << GetParseError_En(e) << std::endl
                          << " at offset " << o
                          << " near '" << input.substr(o, 10) << "...'" << std::endl;
  /* for debugging only (22.7.2020/hob)
  if (hasErrorMessage()) {
    std::cerr << getErrorMessageStream().str();
    std::cerr << "\n\n json[" << input << "]\n\n";
  }
  */
  BUG_DEBUG("End of JSONStreamParameter::readJson: not successful");
  return false;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::read( std::istream &is ){ // old style
  BUG_DEBUG("read json inputstream");
  assert( !isElementStream() );

  if( !parse_json_stream( is ) ){
    // Parser failed
    return false;
  }

  if( isCycleStream() ){
    return read_cycle();
  }
  if( isDatapoolStream() ){
    return read_datapool();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* parse_json_stream --                                                        */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::parse_json_stream( std::istream &is ){
  Json::Reader reader;
  std::string input;

  if( is ){
    // assume a new read
    BUG_DEBUG("parse stream");
    char c;
    c = is.get();
    while(is){
      input += c;
      c = is.get();
    }
    BUG_DEBUG( "Input :" << input );

    bool parsingSuccessful = reader.parse( input, s_root );
    if ( !parsingSuccessful ){
      // report to the user the failure and their locations in the document.
      getErrorMessageStream() << "Failed to parse json '"
                              << (m_xfer == 0 ? "" : m_xfer->getFullName(false))
                              << "'" << std::endl
                              << reader.getFormattedErrorMessages()
                              << std::endl;
#ifdef _DEBUG
      getErrorMessageStream() << input << std::endl;
#endif
      if (hasErrorMessage()) {
        std::cerr << getErrorMessageStream().str();
        std::cerr << "\n\n json[" << input << "]\n\n";
      }
      BUG_DEBUG("not successful");
      return false;
    }
    BUG_DEBUG("successfully parsed");
    return true;
  }

  BUG_DEBUG("nothing to parse");
  return true;
}

/* --------------------------------------------------------------------------- */
/* read_datapool --                                                            */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::read_datapool(){
  BUG_DEBUG( "read_datapool()" );
  assert( isDatapoolStream() );

  if( s_root.isObject() ) {
    std::cerr << "reading DATAPOOL expects an array, but found an object" << std::endl;
    return false;
  }

  DataPoolIntens &dpi = DataPoolIntens::Instance();

  // Initialise datapool
  dpi.goCycle( 0 );
  for( int i = 1; i < dpi.numCycles(); ++i ){
    dpi.removeCycle( i );
  }

  assert( s_root.isArray() );
  int numCycles = s_root.size();

  Json::Value *cycle = 0;
  for( int i = 0; i < numCycles; ++i ){
    if ( i > 0 ) {
      dpi.newCycle( "" );
    }
    else{
      dpi.clearCycle( dpi.currentCycle() );
    }
    cycle = &(s_root[i]);
    if ( (*cycle).isMember( INTERNAL_CYCLE_LIST ) ){
      Json::Value &cyclelist = (*cycle)[INTERNAL_CYCLE_LIST];
      if( cyclelist.isArray() ){
        if( i < cyclelist.size() ){
          dpi.setCycleName( i, cyclelist[i].asString() );
        }
      }
    }
  }

  for( int i = 0; i < numCycles; ++i ){
    cycle = &(s_root[i]);
    dpi.goCycle( i );

    // read one cycle
    Json::Value::Members members( cycle->getMemberNames() );
    for ( Json::Value::Members::iterator it = members.begin();
          it != members.end();
          ++it ) {
      const std::string &name = *it;
      BUG_DEBUG( "#" << i << ": Load Item '" << name << "'");

      DataReference *dref = dpi.getDataReference( name );
      if( dref != 0 ){
        XferDataItem *xfer = new XferDataItem( dref );
        DataAlterReadJSON readJson( xfer->getName(), (*cycle)[name] );
        bool rslt = xfer->alterData( readJson );
        delete xfer;
      }
      else {
        BUG_DEBUG("#" << i << ": Ignore Item '" << name << "'");
      }
    }
  } // numCycles

  if( isDatapoolStream() ){
    dpi.goCycle( 0 );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* read_cycle --                                                               */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::read_cycle(){
  BUG_DEBUG( "read_cycle()" );
  assert( isCycleStream() );

  if ( s_root.isArray() ) {
    std::cerr << "reading CYCLE expects an object, but found an array" << std::endl;
    return false;
  }

  DataPoolIntens &dpi = DataPoolIntens::Instance();

  // read one cycle
  int currCycle = dpi.currentCycle();
  dpi.clearCycle( currCycle );

  Json::Value *cycle = &s_root; // CYCLE object
  Json::Value::Members members( cycle->getMemberNames() );

  for ( Json::Value::Members::iterator it = members.begin();
        it != members.end();
        ++it ) {
    const std::string &name = *it;
    DataReference *dref = dpi.getDataReference( name );
    if( dref != 0 ){
      if( !dref->dictionary().isInternalName() ){
        XferDataItem *xfer = new XferDataItem( dref );
        DataAlterReadJSON readJson( xfer->getName(), (*cycle)[name] );
        bool rslt = xfer->alterData( readJson );
        delete xfer;
      }
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */
#ifdef __MINGW32__
bool JSONStreamParameter::read(const  std::string &filename ){
  BUG(BugStreamer, "JSONStreamParameter::read(std::istream &)" );
  /*
  JSONFactory &factory = JSONFactory::Instance();
  factory.reset();
  JSONStreamDocumentHandler *handler =
    new JSONStreamDocumentHandler; // will be deleted by the factory
  handler->setRoot( m_xfer );
  if( m_allCycles )
    handler->withAllCycles();
  factory.setSAXDocumentHandler( handler );
  factory.saxParse( filename );
  */
  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* readString --                                                               */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::readString(const std::string& s){
  Json::Reader reader;
  bool parsingSuccessful = reader.parse( s, s_root );
  if ( !parsingSuccessful ){
    // report to the user the failure and their locations in the document.
    getErrorMessageStream() << "Failed to parse json '"
                            << (m_xfer == 0 ? "" : m_xfer->getFullName(false))
                            << "'" << std::endl
                            << reader.getFormattedErrorMessages()
                            << std::endl;
#ifdef _DEBUG
    getErrorMessageStream() << s << std::endl;
#endif
    if( hasErrorMessage() ){
      std::cerr << getErrorMessageStream().str();
    }
    return false;
  }

  if (m_xfer) {
    DataAlterReadJSON readJson( m_xfer->getName(), s_root );
    bool rslt = m_xfer->alterData( readJson );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* write_element --                                                            */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::write_element( std::ostream &os ){
  assert( isElementStream() );
  BUG_DEBUG( "write_element() for " << m_xfer->getFullName( true ) );

  if( !m_xfer->writeJSON( os, m_level, m_indentation, m_serialize_flags ) ){
    os << "[]" << std::endl;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* write_a_cycle --                                                            */
/* --------------------------------------------------------------------------- */

void JSONStreamParameter::write_a_cycle( std::ostream &os, int level, int num ){
  DataPoolIntens &pool = DataPoolIntens::Instance();

  std::vector<DataReference *> dlist;
  DataDictionary *dict = pool.getDataPool().GetRootDict()->getStructure();
  while ( dict ){
    if( !dict->isInternalName() ){
      switch (dict->getDataType() ){
      case DataDictionary::type_Integer:
      case DataDictionary::type_Real:
      case DataDictionary::type_String:
      case DataDictionary::type_Complex:
      case DataDictionary::type_StructVariable: {
        std::string name = dict->getName();
        if( name == INTERNAL_CYCLE_NAME ){
          break;
        }
        if( name == INTERNAL_CYCLE_LIST ){
          if( isCycleStream() || num != 0 ){ /* nur für cycle 0 in datapool */
            break;
          }
        }

        if( name.find( "__stream_data_" ) == std::string::npos &&
            name.find( "__filestream_" ) == std::string::npos ){
          DataReference *dref = pool.getDataReference( name );
          if( dref != 0 && dref->isSerializable( m_serialize_flags ) ) {
            dlist.push_back( dref );
          }
        }
        break;
      }
      default:
        break;
      }
    }
    dict = dict->getNext();
  }

  int i = level * m_indentation;
  int i_next = (level+1) * m_indentation;
  indent( i, os ) << "{" << std::endl;

  std::string cyclename;
  pool.getCycleName( num, cyclename );
  indent( i_next, os ) << "\"" << INTERNAL_CYCLE_NAME << "\": "
                       << "\"" << cyclename << "\"";

  std::vector<DataReference *>::const_iterator it;
  for( it = dlist.begin(); it!=dlist.end(); ++it ){
    os << "," << std::endl;
    indent( i_next, os ) << "\"" << (*it)->GetDict()->getName() << "\": ";
    (*it)->writeJSON( os, level+1, m_indentation, m_serialize_flags, false );
  }
  os << std::endl;
  indent( i, os ) << "}";
}

/* --------------------------------------------------------------------------- */
/* write_datapool --                                                           */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::write_datapool( std::ostream &os ){
  assert( isDatapoolStream() );
  BUG_DEBUG( "write_datapool()" );

  DataPoolIntens &pool = DataPoolIntens::Instance();
  int currentCycle = pool.currentCycle();
  int numCycles = pool.numCycles();

  os << "[" << std::endl;

  for( int i = 0; i < numCycles; ++i ){
    if ( i > 0 ) {
      os << "," << std::endl;
    }
    pool.goCycle( i, false ); // do not clear undo stack
    write_a_cycle( os, 1, i );
  }

  os << std::endl << "]" << std::endl;
  pool.goCycle( currentCycle, false ); // do not clear undo stack
  return true;
}

/* --------------------------------------------------------------------------- */
/* write_cycle --                                                              */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::write_cycle( std::ostream &os ){
  assert( isCycleStream() );
  BUG_DEBUG( "write_cycle()" );

  DataPoolIntens &pool = DataPoolIntens::Instance();
  int currentCycle = pool.currentCycle();

  write_a_cycle( os, 0, currentCycle );
  os << std::endl;
  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool JSONStreamParameter::write( std::ostream &os ){
  BUG(BugStreamer, "write(ostream &)" );

  if( isElementStream() ){
    return write_element( os );
  }
  if( isDatapoolStream() ){
    return write_datapool( os );
  }
  return write_cycle( os );
}

/* --------------------------------------------------------------------------- */
/* setindex --                                                                 */
/* --------------------------------------------------------------------------- */

void JSONStreamParameter::setIndex( const std::string &name, int index ) {
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

std::string JSONStreamParameter::getName()const{
  return m_xfer == 0 ? "" : m_xfer->getName();
}

/* --------------------------------------------------------------------------- */
/* getFullName --                                                              */
/* --------------------------------------------------------------------------- */

std::string JSONStreamParameter::getFullName(bool withIndexes) const{
  return m_xfer == 0 ? "" : m_xfer->getFullName(withIndexes);
}

/* --------------------------------------------------------------------------- */
/* getLabel --                                                                 */
/* --------------------------------------------------------------------------- */

const std::string JSONStreamParameter::getLabel() const{
  return m_xfer == 0 ? "" : m_xfer->getLabel();
}

/* --------------------------------------------------------------------------- */
/* getDbAttr --                                                                  */
/* --------------------------------------------------------------------------- */

std::string JSONStreamParameter::getDbAttr() const{
  return m_xfer == 0 ? "" : m_xfer->getDbAttr();
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void JSONStreamParameter::marshal( std::ostream &os ){
  os << "<JSONStreamParameter>\n";
  if( m_xfer != 0 ){
    m_xfer -> marshal( os );
  }
  os << "</JSONStreamParameter>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *JSONStreamParameter::unmarshal( const std::string &element,
                                              const XMLAttributeMap &attributeList ){
  if( element == "XferDataItem" ){
    m_xfer = new XferDataItem();
    return m_xfer -> unmarshal( element, attributeList );
  }
  return this;
}


/* --------------------------------------------------------------------------- */
/* getDataReference_Sub --                                                      */
/* --------------------------------------------------------------------------- */

DataReference *JSONStreamParameter::getDataReference_Sub(const std::string& subvarname) const {
  if(!m_xfer) {
    return NULL;
  }
  std::string vn = m_xfer->getFullName(false) + "." + subvarname;
  return DataPoolIntens::Instance().getDataReference(vn);
}

/* --------------------------------------------------------------------------- */
/* getItemLabel --                                                             */
/* --------------------------------------------------------------------------- */

std::string JSONStreamParameter::getItemLabel(const std::string& subvarname) const {
  std::string ret;

  DataReference *dref = getDataReference_Sub(subvarname);
  if(dref) {
    XferDataItem xfer(dref);
    ret = xfer.getLabel();
  }

  // not found, subvarname could be a dbAttr
  if (ret.empty()) {
    DataReference * ref = m_xfer ? m_xfer->Data() : 0;
    if (ref) {
      ret = getDbAttrItemLabel(ref, subvarname);
    }
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/* getDataSetValue --                                                          */
/* --------------------------------------------------------------------------- */
// if data item subvarname has a DataSet, return the input string that belongs to value
// return empty string otherwise

std::string JSONStreamParameter::getDataSetValue( const std::string& subvarname,
                                                  const std::string& value) const
{
  DataReference *dref = getDataReference_Sub(subvarname);
  if(dref) {
    XferDataItem xfer(dref);
    std::string str;
    if(xfer.getDataSetInputValue(str, value)) {
      return str;
    }
  }

  return std::string();
}

/* --------------------------------------------------------------------------- */
/* getDbAttrItemLabel --                                                       */
/* --------------------------------------------------------------------------- */
std::string JSONStreamParameter::getDbAttrItemLabel(DataReference* ref, const std::string& subvarname) const {
  std::string ret;
  if (ref) {
      DataReference::StructIterator structIter;
      UserAttr *userAttr = 0;
      for( structIter = ref->begin(); structIter != ref->end() && ret.empty(); ++structIter){
        DataReference* newRef = structIter.NewDataReference();
        std::string dbAttr = newRef->dbAttr();
        if (newRef->getDataType() == DataDictionary::type_StructVariable) {
          ret = getDbAttrItemLabel(newRef, subvarname);
        }
        if (dbAttr == subvarname) {
          ret = static_cast<UserAttr&>(newRef->userAttr()).Label( newRef );
        }
        delete newRef;
      }
  }
  return ret;
}
