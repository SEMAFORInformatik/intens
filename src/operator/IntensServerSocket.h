
#ifndef INTENS_SERVER_SOCKET
#define INTENS_SERVER_SOCKET

#include <vector>
#include <map>

#include "job/JobStarter.h"
#include "job/JobFunction.h"
#include "operator/ServerSocket.h"

class JobFunction;
class IntensSocketListener;

class IntensServerSocket : public ServerSocketListener{
// -----------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------
public:
  IntensServerSocket( const std::string &, int, IntensSocketListener * =0, JobFunction * =0);
  virtual ~IntensServerSocket();

private:
  IntensServerSocket();
 public:
// -----------------------------------------------------
// private definitions
// -----------------------------------------------------
  class Trigger : public JobStarter
  {
  public:
    Trigger( IntensServerSocket *server, JobFunction *f )
      : JobStarter( f )
      , m_server( server ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    IntensServerSocket  *m_server;
  };

// -----------------------------------------------------
// public member functions
// -----------------------------------------------------
public:
  static void startAll();
  static void stopAll();
  static IntensServerSocket *getServer( const std::string &id );
  void accept();
  void start();
  void stop();
  void backFromJobStarter( JobAction::JobResult rslt );
  virtual bool newLine( std::string & );
  virtual bool response( std::ostream &os );
  void setRequestTypeData( std::string header, IntensSocketListener *in,
			   IntensSocketListener *out, JobFunction* func);
  void setRequestListener( IntensSocketListener *, std::string header=defaultRequestKey);
  IntensSocketListener *getRequestListener(std::string header=defaultRequestKey);
  ServerSocket *getServer(){ return m_server; }
  Trigger *getTrigger( JobFunction *func=0 );
  const std::string &getName(){ return m_name; }
  virtual XferDataItem* getXfer(){ assert(false); return 0; }
  virtual XferDataItem* getThumbXfer(){ assert(false); return 0; }
  virtual int getThumbWidth(){ assert(false); return 0; }
  virtual int getThumbHeight(){ assert(false); return 0; }
  static void serialize( std::ostream &, AppData::SerializeType type, bool recursive=false );
  JobFunction *getFunction(std::string header=defaultRequestKey);

// -----------------------------------------------------
// private data
// -----------------------------------------------------
private:
  class RequestType{
  public:
    RequestType(IntensSocketListener* in, IntensSocketListener* out, JobFunction* func)
    : m_inListener(in), m_outListener(out), m_function(func)
    {}
    IntensSocketListener                   *m_inListener;
    IntensSocketListener                   *m_outListener;
    JobFunction                            *m_function;
  };
  typedef std::map< std::string, RequestType* > RequestTypeMap;
  typedef RequestTypeMap::value_type RequestTypePair;
  RequestTypeMap m_requestType;
  static const std::string defaultRequestKey;

  std::string                             m_name;
  ServerSocket                           *m_server;
  //  JobFunction                            *m_function;
  int                                     m_port;
  //  IntensSocketListener                   *m_listener;
  static std::vector<IntensServerSocket*> s_servers;
  std::ostringstream                      m_input;
  std::string                             m_header;
  long                                    m_dataSize;
  bool                                    m_withHeader;
};

class IntensSocketListener{
public:
  IntensSocketListener(){}
  ~IntensSocketListener(){}

  virtual bool read( std::istream & )=0;
  virtual bool write( std::ostream &os ) =0;
};

#endif
