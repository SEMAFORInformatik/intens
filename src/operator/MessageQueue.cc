#include <iostream>
#include <QMetaType>

#include "app/App.h"
#include "streamer/Stream.h"
#include "streamer/StreamManager.h"
#include "operator/MessageQueue.h"
#include "operator/MessageQueueRequest.h"
#include "operator/MessageQueueSubscriber.h"
#include "operator/MessageQueuePublisher.h"
#include "operator/MessageQueueReply.h"
#include "xfer/XferDataItem.h"
#include "utils/FileUtilities.h"

MessageQueue *MessageQueue::s_instance = 0;
MessageQueue::RequestMap    MessageQueue::s_requestMap;
MessageQueue::SubscriberMap MessageQueue::s_subscriberMap;
MessageQueue::PublisherMap  MessageQueue::s_publisherMap;
const std::string MessageQueue::OPENTELEMETRY_METADATA_IN_STREAM("opentelemetry_metadata_in@intens");
const std::string MessageQueue::OPENTELEMETRY_METADATA_OUT_STREAM("opentelemetry_metadata_out@intens");

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */

MessageQueue::MessageQueue(){
}

MessageQueue::~MessageQueue(){
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

MessageQueue &MessageQueue::Instance(){
  if (!s_instance) {
    typedef std::vector<std::string> StringList;
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<StringList>("std::vector<std::string>");
    s_instance = new MessageQueue();
  }
  return *s_instance;
}


/* --------------------------------------------------------------------------- */
/* createRequest --                                                            */
/* --------------------------------------------------------------------------- */

MessageQueueRequest* MessageQueue::createRequest(const std::string &name, const std::string& host, int port, int defaultTimeout, int lineNo, std::string filename) {
  MessageQueueRequest* request = new MessageQueueRequest(host, port, defaultTimeout);
  // add to static request map
  if (port > 0) {
    RequestMap::iterator iter =  s_requestMap.find( name );
    if( iter ==  s_requestMap.end() )
      s_requestMap.insert( RequestMap::value_type( name, request ) );
  }
  request->setLSPLineno(lineNo);
  request->setLSPFilename(filename);
  return request;
}

/* --------------------------------------------------------------------------- */
/* createSubscriber --                                                         */
/* --------------------------------------------------------------------------- */

MessageQueueSubscriber* MessageQueue::createSubscriber(const std::string &name,
						       const std::string& host,
						       int port, int lineNo, std::string filename) {
  MessageQueueSubscriber* subscriber = new MessageQueueSubscriber(name, host, port);
  SubscriberMap::iterator iter =  s_subscriberMap.find( name );
  if( iter ==  s_subscriberMap.end() ){
    s_subscriberMap.insert( SubscriberMap::value_type( name, subscriber ) );
  }
  subscriber->setLSPLineno(lineNo);
  subscriber->setLSPFilename(filename);

  return subscriber;
}

/* --------------------------------------------------------------------------- */
/* createReply --                                                              */
/* --------------------------------------------------------------------------- */

MessageQueueReply* MessageQueue::createReply( const std::string &name,
                                              const std::string& host,
                                              int port,
                                              const std::vector<Stream*>& default_in_streams,
                                              const std::vector<Stream*>& default_out_streams,
                                              JobFunction *default_func, int lineNo, std::string filename) {
  MessageQueueReply* reply = new MessageQueueReply(name, host, port,
                                                   default_in_streams,
                                                   default_out_streams,
                                                   default_func);
  reply->setLSPLineno(lineNo);
  reply->setLSPFilename(filename);
  return reply;
}

/* --------------------------------------------------------------------------- */
/* createPublisher --                                                          */
/* --------------------------------------------------------------------------- */

MessageQueuePublisher* MessageQueue::createPublisher( const std::string &name,
                                                      const std::string& host,
                                                      int port, int lineNo, std::string filename ) {
  MessageQueuePublisher* publisher;
  publisher =  new MessageQueuePublisher(name, host, port);
  // add to static publisher map
  if (port>0) {
    PublisherMap::iterator iter =  s_publisherMap.find( name );
    if( iter ==  s_publisherMap.end() )
      s_publisherMap.insert( PublisherMap::value_type( name, publisher ) );
  }
  publisher->setLSPLineno(lineNo);
  publisher->setLSPFilename(filename);
  return publisher;
}

/* --------------------------------------------------------------------------- */
/* parseIncludeFile --                                                         */
/* --------------------------------------------------------------------------- */

void MessageQueue::parseIncludeFile() {
  std::string token("__PORT_REPLY__");
  std::string tokenPublish("__PORT_PUBLISH__");
  std::string tokenIntensNS(App::TOKEN_INTENS_NAMESPACE);
  std::string intensNS(App::INTENS_NAMESPACE);

  // load and parse MessageQueueReply.inc
  int replyPort = AppData::Instance().ReplyPort();
  if (replyPort > 0) {
    std::vector<std::string> files;
    files.push_back(AppData::Instance().IntensHome() +"/etc/MessageQueueReply.inc");
    files.push_back(AppData::Instance().IntensHome() +"/../etc/MessageQueueReply.inc");
    std::string fn = FileUtilities::GetNewestFile(files);
    std::ostringstream os;
    std::ifstream file(fn.c_str());
    std::string line;
    while(std::getline(file, line)) {
      // find reply port token
      std::string::size_type n = line.find(token);
      if (n != std::string::npos) {
        std::ostringstream osp;
        osp << replyPort;
        line.replace(n, token.size(), osp.str());
      }
      // find publish port token
      n = line.find(tokenPublish);
      if (n != std::string::npos) {
        std::ostringstream osp;
        osp << (replyPort + 1);  // !! Publish Port = Reply Port + 1 !!
        line.replace(n, tokenPublish.size(), osp.str());
      }
      // intens namespace
      while ((n = line.find(tokenIntensNS)) != std::string::npos) {
        std::ostringstream osp;
        line.replace(n, tokenIntensNS.size(), intensNS);
      }
      os << line << std::endl;
    }

    if (!App::Instance().parse(os.str())){
      BUG_ERROR("ERROR parsing mq reply include.");
    }
    else{
      BUG_INFO("MessageQueueReply successfully included");
    }
  }
}

/* --------------------------------------------------------------------------- */
/* initialise --                                                               */
/* --------------------------------------------------------------------------- */

void MessageQueue::initialise() {
  // OpenTelemetry Metadata
  if (AppData::Instance().OpenTelemetryMetadata()) {
     DataReference *refIn  =  DataPoolIntens::Instance().getDataReference( "OPENTELEMETRY_METADATA_IN" );
     DataReference *refOut =  DataPoolIntens::Instance().getDataReference( "OPENTELEMETRY_METADATA_OUT" );

     // meta data in stream
     Stream *in = StreamManager::Instance().newStream(OPENTELEMETRY_METADATA_IN_STREAM);
     Stream *out = StreamManager::Instance().newStream(OPENTELEMETRY_METADATA_OUT_STREAM);
     XferDataItem * dataitemIn = new XferDataItem(refIn);
     dataitemIn->setValue("\"TODO blabla\"");  // TODO api-gateway.py braucht momentan JSON
     XferDataItem * dataitemOut = new XferDataItem(refOut);
     StreamParameter *si = in->addParameter( dataitemIn, 0, 0, 0, false, false, false );
     StreamParameter *so = out->addParameter( dataitemOut, 0, 0, 0, false, false, false );
  }
}

/* --------------------------------------------------------------------------- */
/* getRequest --                                                               */
/* --------------------------------------------------------------------------- */

MessageQueueRequest* MessageQueue::getRequest( const std::string &name ) {
  RequestMap::iterator iter =  s_requestMap.find( name );
  if( iter !=  s_requestMap.end() ){
    return (*iter).second;
  }
  return 0;
}
/* --------------------------------------------------------------------------- */
/* terminateAllRequestThreads --                                               */
/* --------------------------------------------------------------------------- */

void MessageQueue::terminateAllRequestThreads() {
  BUG_DEBUG("MessageQueue::terminateAllRequestThreads");
  RequestMap::iterator iter =  s_requestMap.begin();
  for( ;iter !=  s_requestMap.end(); ++iter ) {
    BUG_DEBUG("Thread terminate name["<<(*iter).first<<"] ["<<(*iter).second<<"]");
//amg 2013-09-27 auskommentiert if (!(*iter).second->checkRequest())
    (*iter).second->terminateRequest();
  }
}

/* --------------------------------------------------------------------------- */
/* hasPendingRequestThreads --                                                 */
/* --------------------------------------------------------------------------- */

bool MessageQueue::hasPendingRequestThreads() {
  BUG_DEBUG("MessageQueue::thasPendingRequestThreads");
  RequestMap::iterator iter =  s_requestMap.begin();
  for( ;iter !=  s_requestMap.end(); ++iter ) {
    BUG_DEBUG("Thread terminate name["<<(*iter).first<<"] ["<<(*iter).second<<"]");
     // ?? is pending ??
    if ( !(*iter).second->checkRequest() )
      return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getPublisher --                                                             */
/* --------------------------------------------------------------------------- */

MessageQueuePublisher* MessageQueue::getPublisher( const std::string &name ) {
  PublisherMap::iterator iter =  s_publisherMap.find( name );
  if( iter !=  s_publisherMap.end() ){
    return (*iter).second;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getSubscriber --                                                            */
/* --------------------------------------------------------------------------- */

MessageQueueSubscriber* MessageQueue::getSubscriber( const std::string &name ) {
  SubscriberMap::iterator iter =  s_subscriberMap.find( name );
  if( iter !=  s_subscriberMap.end() ){
    return (*iter).second;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* addHeader --                                                                */
/* --------------------------------------------------------------------------- */

void MessageQueue::addHeader( std::string header,
			      std::vector<Stream*>& in,
                              std::vector<Stream*>&  out,
			      std::vector<Plugin*> plugins,
			      JobFunction* func ){
  BUG_DEBUG("MessageQueue::setSubscriberTypeData header: "<< header);
  if ( m_headerData.find(header) == m_headerData.end() ){
    m_headerData.insert( HeaderDataMap::value_type(header,
                                                   new HeaderData(in, out, plugins, func) ) );
  }
  else {
    m_headerData[header]->m_inVectorListener  = in;
    m_headerData[header]->m_outVectorListener = out;
    m_headerData[header]->m_pluginVector      = plugins;
    m_headerData[header]->m_function          = func;
  }
}

void MessageQueue::lspWrite( std::ostream &ostr ){
  #define SOCKET_SERIALIZE_FUNC \
    ostr << "<ITEM name=\"" << socket.first << "\""; \
    ostr << " mq=\"1\""; \
    ostr << " file=\"" << socket.second->LSPFilename() << "\""; \
    ostr << " line=\"" << socket.second->LSPLineno() << "\""; \
    ostr << ">" << std::endl; \
    ostr << "</ITEM>" << std::endl;

  for (auto socket : s_subscriberMap) {
    SOCKET_SERIALIZE_FUNC 
  }
  for (auto socket : s_publisherMap) {
    SOCKET_SERIALIZE_FUNC 
  }
  for (auto socket : s_requestMap) {
    SOCKET_SERIALIZE_FUNC 
  }
}

/* --------------------------------------------------------------------------- */
/* getContext --                                                               */
/* --------------------------------------------------------------------------- */

zmq::context_t* MessageQueue::m_context = 0;

zmq::context_t& MessageQueue::getContext() {
  if (!m_context) {
#ifdef WIN32
    m_context = new zmq::context_t(1, 50);
#else
    m_context = new zmq::context_t(1);
#endif
  }
  return *m_context;
}
