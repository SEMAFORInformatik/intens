
#include <iostream>

#include "utils/Date.h"
#include "utils/gettext.h"

#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "gui/GuiElement.h"
#include "gui/GuiScrolledText.h"

#include "job/JobFunction.h"

#include "operator/ServerSocket.h"
#include "operator/IntensServerSocket.h"

INIT_LOGGER();

std::vector<IntensServerSocket*> IntensServerSocket::s_servers;
const std::string IntensServerSocket::defaultRequestKey = "@@_DEFAULT_KEY";

// -----------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------
IntensServerSocket::IntensServerSocket( const std::string &id
					, int port
					, IntensSocketListener *listener
					, JobFunction *func )
  : //m_listener( listener )
     m_name( id )
   , m_dataSize(0)
   , m_withHeader(false)
   , m_port(port)
     //    , m_function( func )
{
  m_server = GuiFactory::Instance()->createServerSocket( this, port );
  s_servers.push_back( this );

  if (listener || func) {
    m_requestType.insert( RequestTypeMap::value_type(defaultRequestKey, new RequestType(listener, 0, func) ) );
  }
}

IntensServerSocket::~IntensServerSocket(){
  delete m_server;
  m_server = 0;
}


// -----------------------------------------------------
// start
// -----------------------------------------------------
void IntensServerSocket::start(){
  if( m_server ){
    m_server->start();
    m_withHeader = (m_requestType.find(defaultRequestKey) == m_requestType.end()) ? true : false;
  }
}

// -----------------------------------------------------
// stop
// -----------------------------------------------------
void IntensServerSocket::stop(){
  if( m_server ){
    m_server->stop();
  }
}

// -----------------------------------------------------
// startAll
// -----------------------------------------------------
void IntensServerSocket::startAll(){
  std::vector<IntensServerSocket*>::iterator iter;
  for( iter = s_servers.begin();iter!=s_servers.end();++iter){
    (*iter)->start();
  }
}

// -----------------------------------------------------
// stopAll
// -----------------------------------------------------
void IntensServerSocket::stopAll(){
  std::vector<IntensServerSocket*>::iterator iter;
  for( iter=s_servers.begin();iter!=s_servers.end();++iter){
    (*iter)->stop();
  }
}



// -----------------------------------------------------
// newLine
// -----------------------------------------------------
bool IntensServerSocket::newLine( std::string &line ){
  BUG_DEBUG("newLine(" << line << "): START");
  if( m_requestType.size() ) {
    if (m_withHeader && !m_dataSize) {
      try {
	// brand new data => get header info
	long hsize = 0, dsize = 0;
	std::string hdr_str(line);
	std::string str = hdr_str.substr(0, 2);
	std::istringstream istr( str );
	istr >> hsize;

	std::string hdr = hdr_str.substr(2, hsize);
	str = hdr_str.substr(2+hsize, 8);
	std::string::size_type pos = str.find_first_not_of('0');
	std::istringstream istr2( str );
	istr2 >> dsize;
	m_dataSize = dsize;
	m_header = hdr;
	line = line.substr( 2 + hsize + 8);
      } catch( ... ) {
	m_dataSize = 0;
	m_header = "";
	GuiFactory::Instance()->getLogWindow()->writeText( "invalid tcp data header.\n" );
	m_server->endOfRead();
      }
    }

    //
    if( line.substr(0,3) != "EOF" ) {
      m_input << line;
    }

    if( (m_withHeader && m_input.str().size() >=  m_dataSize) ||
	(!m_withHeader && line.substr(0,3) == "EOF")            ){
      std::istringstream istr( m_input.str() );
      m_input.str("");
      m_dataSize = 0;
      std::string hdr = m_header.size() ? m_header : defaultRequestKey;
      if (m_requestType.find(hdr) == m_requestType.end()) {
	GuiFactory::Instance()->getLogWindow()->writeText( compose(_("tcp server at port '%1' has received unknown data header '%2'.\n"), m_port, hdr) );
        BUG_ERROR("newLine: " << compose(_("tcp server at port '%1' has received unknown data header '%2'."), m_port, hdr) );
	m_server->endOfRead();
	return true;
      }

      // did we receive unexpected data?
      if( !m_requestType[hdr]->m_inListener &&
          istr.str().find_last_not_of(" \t\n\r") != std::string::npos // there is always at least a \n in istr
          ){
        GuiFactory::Instance()->getLogWindow()->writeText( compose(_("tcp server at port '%1' has received unexpected data ('%3') for header '%2'.\n"), m_port, hdr, istr.str()) );
        m_server->endOfRead();
        BUG_ERROR("newLine: " << compose(_("tcp server at port '%1' has received unexpected data ('%3') for header '%2'."), m_port, hdr, istr.str()) );
        return true;
      }
      // did we not receive expected data?
      if( m_requestType[hdr]->m_inListener &&
          !istr.str().size()
          ){
        GuiFactory::Instance()->getLogWindow()->writeText( compose(_("tcp server at port '%1' has not received expected data for header '%2'.\n"), m_port, hdr) );
        m_server->endOfRead();
        BUG_ERROR("newLine: " << compose(_("tcp server at port '%1' has not received expected data for header '%2'."), m_port, hdr) );
        return true;
      }

      if( m_requestType[hdr]->m_inListener ) {
        BUG_DEBUG("newLine: read data to datapool");
        m_requestType[hdr]->m_inListener->read( istr );
        BUG_DEBUG("newLine: read data to datapool DONE");
      }

      GuiManager::Instance().updateLater( GuiElement::reason_Process );
      if( m_requestType[hdr]->m_function ){
        BUG_DEBUG("newLine: trigger->startJob 1");
	Trigger *trigger = new Trigger( this, getFunction(hdr) );
	trigger->startJob();
	// Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
        BUG_DEBUG("newLine: trigger->startJob 1 DONE");
      }
      else{
        BUG_DEBUG("newLine: no function -> endOfRead() 1");
	m_server->endOfRead();
        BUG_DEBUG("newLine: no function -> endOfRead() 1 DONE");
      }
      BUG_DEBUG("newLine: DONE 1");
      return true;
    }
    else{
      BUG_DEBUG("newLine: DONE 2");
      return false;
    }
  }
  else{
    if( getFunction(defaultRequestKey) ){
      BUG_DEBUG("newLine: trigger->startJob 2");
      Trigger *trigger = new Trigger( this, getFunction(defaultRequestKey) );
      trigger->startJob();
      // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
      BUG_DEBUG("newLine: trigger->startJob 2 DONE");
    }
    else{
      BUG_DEBUG("newLine: no function -> endOfRead() 2 DONE");
      m_server->endOfRead();
      BUG_DEBUG("newLine: no function -> endOfRead() 2 DONE");
    }
  }
  BUG_DEBUG("newLine: DONE 3");
  return true;
}

// -----------------------------------------------------
// response
// -----------------------------------------------------
bool IntensServerSocket::response(std::ostream &os){
  RequestTypeMap::iterator it = m_requestType.find(m_header);
  if (it == m_requestType.end() ||
      (*it).second->m_outListener == NULL) {
  // reset header info
    m_header = "";
    m_dataSize = 0;
    return (it == m_requestType.end()) ? true : false;
  }
  (*it).second->m_outListener->write(os);
  // reset header info
  m_header = "";
  m_dataSize = 0;
  return true;

}

// -----------------------------------------------------
// getServer
// -----------------------------------------------------
IntensServerSocket *IntensServerSocket::getServer( const std::string &id ){
  std::vector<IntensServerSocket*>::iterator iter;
  for( iter=s_servers.begin(); iter!=s_servers.end(); ++iter ){
    if( (*iter)->m_name==id ){
      return *iter;
    }
  }
  return 0;
}

// -----------------------------------------------------
// setRequestTypeData
// -----------------------------------------------------
void IntensServerSocket::setRequestTypeData( std::string header, IntensSocketListener *in,
					     IntensSocketListener *out, JobFunction* func){
  if ( m_requestType.find(header) == m_requestType.end() )
    m_requestType.insert( RequestTypeMap::value_type(header, new RequestType(in, out, func) ) );
  else {
    m_requestType[header]->m_inListener  = in;
    m_requestType[header]->m_outListener = out;
    m_requestType[header]->m_function    = func;
  }
}

// -----------------------------------------------------
// setRequestListener
// -----------------------------------------------------
void IntensServerSocket::setRequestListener( IntensSocketListener *listener, std::string header ){
  if ( m_requestType.find(header) != m_requestType.end() )
    m_requestType[header]->m_inListener = listener;
  else
    std::cout << " Request Header '"<<header<<"' data not found!\n";
  //    assert(false);
}

// -----------------------------------------------------
// getRequestListener
// -----------------------------------------------------
IntensSocketListener* IntensServerSocket::getRequestListener(std::string header){
  if ( m_requestType.find(header) == m_requestType.end() )
    return 0;
  return m_requestType[header]->m_inListener;
}

// -----------------------------------------------------
// getFunction
// -----------------------------------------------------
JobFunction* IntensServerSocket::getFunction(std::string header){
  if ( m_requestType.find(header) != m_requestType.end() )
    return m_requestType[header]->m_function;
  return 0;
}

// -----------------------------------------------------
// backFromJobStarter
// -----------------------------------------------------
void IntensServerSocket::backFromJobStarter( JobAction::JobResult rslt  ){
  if (m_server)
    m_server->endOfRead();
}

// -----------------------------------------------------
// serialize
// -----------------------------------------------------
void IntensServerSocket::serialize( std::ostream &os, AppData::SerializeType type, bool recursive ){
  if (type != AppData::serialize_XML)
       return;
  std::vector<IntensServerSocket*>::iterator iter;
  for( iter=s_servers.begin(); iter!=s_servers.end(); ++iter ){
    os << "<socket name=\"" << (*iter)->getName() <<"\"";
    if( (*iter)->getFunction() ){
      os << " function=\"" << (*iter)->getFunction()->Name() << "\"";
    }
    os << "/>" << std::endl;
  }
}


// -----------------------------------------------------
// getTrigger
// -----------------------------------------------------

IntensServerSocket::Trigger *IntensServerSocket::getTrigger( JobFunction *func ){
  // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
  if( func == 0 ){
    func =  getFunction();
  }
  if( func )
    return new Trigger( this, func );
  return 0;
}

// -----------------------------------------------------
// backFromJobStarter
// -----------------------------------------------------
void IntensServerSocket::Trigger::backFromJobStarter( JobAction::JobResult rslt  ){
  if ( m_server)
    m_server->backFromJobStarter( rslt );
}
