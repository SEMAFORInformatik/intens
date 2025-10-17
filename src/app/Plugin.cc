
#include <ltdl.h>
#include <sys/types.h>

#include "app/AppData.h"
#include "app/Plugin.h"
#include "utils/Debugger.h"
#include "app/UiManager.h"
#include "app/HardCopyListener.h"
#include "app/ModuleFactory.h"
#include "app/IntensInterface.h"
#include "app/SmartPluginInterface.h"
#include "app/SmartIntensInterface_Impl.h"
#include "streamer/Stream.h"
  #include "streamer/StreamManager.h"
#include "operator/ConnectionListener.h"
#ifdef HAVE_QT
#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtManager.h"
#endif

#define HAVE_DLOPEN_WITH_UNRESOLVED_REFERENCES
#define SHLIB_EXT ".so"

Plugin::~Plugin() {
  if( m_hModule != 0 ) lt_dlclose( (lt_dlhandle) m_hModule );
}

Plugin* Plugin::open( const std::string &name ) {
#ifdef HAVE_DLOPEN_WITH_UNRESOLVED_REFERENCES
  return ModuleFactory::Instance().createPlugin(name);
#else
  if( name =="topbuild" ){
//     return new Plugin( name, 0, new RailnetPlugin() );
  }
  else if ( name == "compbuild" ){
    //    return new Plugin( name, 0, new ComponentBuilderPlugin() );
  } else {
    SmartPluginInterface* plugin = GuiManager::Instance().loadPlugin(name);
    return plugin ? new Plugin(name, plugin) : 0;
  }
  return 0;
#endif
}

std::string Plugin::getDescription() {
  std::string ret =  m_pObj->getDescription();
  return ret;
}

int Plugin::initialize() {
  return 0;
}

int Plugin::update() {
  return m_pObj->update();
}

Plugin::Plugin(const std::string &name, void *hModule, void *pObj)
  : GuiElement( NULL, name )
  , m_hModule( hModule )
  , m_width( 0 )
  , m_height( 0 ), m_intensInterface(this)
, m_pluginElement( NULL )
{
  m_pObj = reinterpret_cast<PluginInterface*>(pObj);
  // Hardcopy Listener installieren
  UImanager::Instance().addHardCopy( m_pObj->getName(), this );
}

Plugin::Plugin(const std::string &name, SmartPluginInterface* plugin)
  : GuiElement( NULL, name ), m_intensInterface(this) {
  m_pSmartObj = plugin;
}

int Plugin::execute( ){
  return m_pObj -> execute( this, m_args );
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void Plugin::create(){
  int  width = 0, height = 0;
  BUG( BugPlugin, "Plugin::create" );
  if (m_pSmartObj) {
    m_pSmartObj->initialize( m_intensInterface );
  } else {
    IntensInterface_Impl* ii = new IntensInterface_Impl;
    m_pObj->initialize(ii);
  }

  if (m_pSmartObj) {
#ifdef HAVE_QT
    m_pluginElement = m_pSmartObj->createWindow( getParent()->getQtElement()->myWidget(), &width, &height );
    for (GuiElementList::iterator it = m_childElements.begin();
         it != m_childElements.end(); ++it) {
      (*it)->create();
      int x,y;
      (*it)->getLayoutPosition(x, y);
      m_pSmartObj->insertSubWindow( (*it)->getQtElement()->myWidget()
                                    , x, y, (*it)->getColSpan(), (*it)->getRowSpan() );
    }
#endif
  } else
     m_pluginElement = m_pObj->createWindow( getParent(), &width, &height );
  m_width  = (int)width;
  m_height = (int)height;
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void Plugin::manage(){
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void Plugin::getSize( int &x, int &y, bool hint){
  x = m_width;
  y = m_height;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void Plugin::update( UpdateReason reason ){
  // update childs
  for (GuiElementList::iterator it = m_childElements.begin();
       it != m_childElements.end(); ++it) {
    (*it)->update(reason);
  }

  if (m_pSmartObj) {
    //m_pSmartObj->update();
  } else
    m_pObj->update( reason );
}

/* --------------------------------------------------------------------------- */
/* isShown --                                                                  */
/* --------------------------------------------------------------------------- */

bool Plugin::isShown() {
  return m_pluginElement != 0;
}

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void Plugin::startJobAction(){
  if( execute() != 0 ){
    endJobAction( job_Aborted );
  }
}

void Plugin::setExitCode( JobResult res ){
    endJobAction( res );
}

/* --------------------------------------------------------------------------- */
/* getFileFormat --                                                                */
/* --------------------------------------------------------------------------- */
HardCopyListener::FileFormat Plugin::getFileFormat() {
  assert( m_pObj != 0 );
  return m_pObj->getFileFormat();
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported --                                                    */
/* --------------------------------------------------------------------------- */
bool Plugin::isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ){
  assert( m_pObj != 0 );
  return m_pObj->isFileFormatSupported( fileFormat );
}

/* --------------------------------------------------------------------------- */
/* MenuLabel --                                                                */
/* --------------------------------------------------------------------------- */
const std::string &Plugin::MenuLabel() {
  return getName();
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool Plugin::write( InputChannelEvent &event ){
  assert( m_pObj != 0 );
#if defined HAVE_QT
  std::ostringstream ostr;
  bool ret = m_pObj->write( ostr );
  event.write( ostr );
  return ret;
#else
  return m_pObj->write( event.getOstream() );
#endif
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool Plugin::write( std::ostream &os ){
  assert( m_pObj != 0 );
  return m_pObj->write( os );
}


/* --------------------------------------------------------------------------- */
/* getDefaultSettings --                                                       */
/* --------------------------------------------------------------------------- */
bool Plugin::getDefaultSettings( HardCopyListener::PaperSize &size,
                                 HardCopyListener::Orientation &orient,
                                 int &quantity,
                                 HardCopyListener::FileFormat &fileFormat,
                                 double &lm, double &rm,
                                 double &tm, double &bm ){

  return m_pObj->getDefaultSettings( size, orient, quantity, fileFormat, lm, rm, tm, bm );
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void Plugin::serializeXML(std::ostream &os, bool recursive){
  os << "<Plugin";
  os << ">" << std::endl;
  os<<"</Plugin>"<<std::endl;
}

/* --------------------------------------------------------------------------- */
/* messageQueueSubscribe --                                                    */
/* --------------------------------------------------------------------------- */

bool Plugin::messageQueueSubscribe(const std::string& header , const std::vector<std::string> &dataList) {
  if (m_pSmartObj) {
    m_pSmartObj->messageQueueSubscribe(header, dataList);
    return true;
  }

  assert(false);
  return false;
}

/* --------------------------------------------------------------------------- */
/* attach (GuiElement) --                                                      */
/* --------------------------------------------------------------------------- */

void Plugin::attach( GuiElement *elem) {
  if (m_pSmartObj) {
    m_childElements.push_back(elem);
  } else
    assert(false);
}

/* --------------------------------------------------------------------------- */
/* startRequest                                                                */
/* --------------------------------------------------------------------------- */

void Plugin::startRequest() {
  BUG( BugPlugin, "Plugin::startRequest" );
  assert( m_pSmartObj);

  std::vector<std::string> inDataList;
  std::vector<std::string> outDataList;

  // write input data
  for (std::vector<Stream*>::const_iterator it = getRequestInStreams().begin();
       it != getRequestInStreams().end(); ++it) {
    std::ostringstream os;
    (*it)->write( os );
    inDataList.push_back( os.str() );
  }

  m_pSmartObj->requestData(getRequestHeader(), inDataList, outDataList);
  BUG_MSG("requestData inSize["<<inDataList.size()<<"] outSize["<<outDataList.size()<<"]");

  // read output data
  int idx = 0;
  for (std::vector<Stream*>::const_iterator it = getRequestOutStreams().begin();
       it != getRequestOutStreams().end(); ++it, ++idx) {
    (*it)->clearRange( );
    if (idx < outDataList.size()) {
      std::stringstream is( outDataList[idx] );
      (*it)->read( is );
    }
  }

  if( getRequestListener() )
    getRequestListener()->connectionClosed();
  setRequestListener( 0 );
}

//-------------------------------------------------
// checkRequest
//-------------------------------------------------
bool Plugin::checkRequest(){
  return true;
}

//-------------------------------------------------
// getErrorMessage
//-------------------------------------------------
std::string Plugin::getErrorMessage() const {
  return "";
}

/* --------------------------------------------------------------------------- */
/* doSubscribe                                                                 */
/* --------------------------------------------------------------------------- */

void Plugin::doSubscribe() {
  BUG( BugPlugin, "Plugin::startSubscribe" );
  assert( m_pSmartObj);

  if (m_pSmartObj->subscribeData(getSubscribeHeader()))
    m_subscriberData.push_back( new SubscribeData(getSubscribeHeader(), getSubscribeOutStreams(),
                                                  getSubscribeFunction()) );

  BUG_MSG("subscribed Header["<<getSubscribeHeader()<<"]");

  if( getSubscribeListener() )
    getSubscribeListener()->connectionClosed();
  setSubscribeListener( 0 );
}

//-------------------------------------------------
// checkSubscribe
//-------------------------------------------------
bool Plugin::checkSubscribe(){
  return true;
}

//-------------------------------------------------
// getSubscribeData
//-------------------------------------------------
bool Plugin::getSubscribeData(const std::string& header, std::vector<Stream*>& streams, JobFunction*& func) {
  streams.clear();
  func = 0;
  for ( std::vector<SubscribeData*>::iterator it = m_subscriberData.begin();
        it != m_subscriberData.end(); ++it) {
    if ((*it)->m_header == header ) {
      streams = (*it)->m_out_streams;
      func = (*it)->m_function;
      return true;
    }
  }
  return false;
}
