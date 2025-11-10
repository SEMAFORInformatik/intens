#ifndef MESSAGE_QUEUE_INCLUDED_H
#define MESSAGE_QUEUE_INCLUDED_H

#include <string>
#include <map>
#include <zmq.hpp>
#include "operator/IntensServerSocket.h"

class XferDataItem;
class JobFunction;
class Stream;
class Plugin;
class MessageQueueRequest;
class MessageQueueSubscriber;
class MessageQueueReply;
class MessageQueuePublisher;

class MessageQueue {
public:
  enum Type
  { type_Publisher = 0
  , type_Subscriber
  , type_Request
  , type_Reply
  , type_Undefined
  };
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  static MessageQueue &Instance();
  virtual ~MessageQueue();

  MessageQueueRequest* createRequest(const std::string &name,
				     const std::string& host,
                                     int port,
				     int defaultTimeout, int lineNo = 0, std::string filename = "");
  MessageQueueSubscriber* createSubscriber(const std::string &name,
					   const std::string& host,
					   int port, int lineNo = 0, std::string filename = "");
  MessageQueueReply* createReply(const std::string &name,
				 const std::string& host,
                                 int port,
                                 const std::vector<Stream*>& default_in_streams,
                                 const std::vector<Stream*>& default_out_streams,
                                 JobFunction *default_func,
                                 int lineNo = 0, std::string filename = "");
  MessageQueuePublisher* createPublisher(const std::string &name,
					 const std::string& host,
                               int port, int lineNo = 0, std::string filename = "");
  static void parseIncludeFile();
  static void initialise();

  static MessageQueueRequest *getRequest( const std::string &name );
  static MessageQueueSubscriber *getSubscriber( const std::string &name );
  static MessageQueuePublisher *getPublisher( const std::string &name );

  void addHeader( std::string header, std::vector<Stream*>& in,
                  std::vector<Stream*>& out, std::vector<Plugin*> plugins, JobFunction* func);
  void lspWrite( std::ostream &ostr );

  /** terminate all request threads */
  static void terminateAllRequestThreads();
  /** has pending request threads */
  static bool hasPendingRequestThreads();

private:
  MessageQueue();
  typedef std::map<std::string, MessageQueueRequest *> RequestMap;
  typedef std::map<std::string, MessageQueueSubscriber *> SubscriberMap;
  typedef std::map<std::string, MessageQueuePublisher *>  PublisherMap;

public:
  class HeaderData{
  public:
  HeaderData(std::vector<Stream*>& in, std::vector<Stream*>& out,
             std::vector<Plugin*>  plugins, JobFunction* func)
    : m_inVectorListener(in), m_outVectorListener(out), m_pluginVector(plugins), m_function(func)
    {}
    std::vector<Stream*> m_inVectorListener;
    std::vector<Stream*> m_outVectorListener;
    std::vector<Plugin*> m_pluginVector;
    JobFunction *m_function;
  };
  typedef std::map< std::string, HeaderData* > HeaderDataMap;
  typedef HeaderDataMap::value_type HeaderDataPair;

  static zmq::context_t& getContext();

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  HeaderDataMap            m_headerData;
  static MessageQueue*     s_instance;
  static RequestMap        s_requestMap;
  static SubscriberMap     s_subscriberMap;
  static PublisherMap      s_publisherMap;

  MessageQueueSubscriber*  m_subscriber;
  static zmq::context_t* m_context;
/*=============================================================================*/
/* public const data                                                           */
/*=============================================================================*/
public:
  static const std::string OPENTELEMETRY_METADATA_IN_STREAM;
  static const std::string OPENTELEMETRY_METADATA_OUT_STREAM;
};

#endif
