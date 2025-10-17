
#if !defined(STREAMER_STREAM_MANAGER_H)
#define STREAMER_STREAM_MANAGER_H

#include <string>
#include <fstream>
#include <map>
#include "plot/PSPlot.h"
#include "xml/Serializable.h"

class Stream;
class BasicStream;

class StreamManager
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  StreamManager() {}
  virtual ~StreamManager() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static StreamManager &Instance();

  Stream *newStream( const std::string &name );
  Stream *newStream( bool check = true );
  void attachStream( BasicStream * );
  Stream *getStream( const std::string &name );
  PSPlot::PSStream *newPSStream( const std::string &name );
  PSPlot::PSStream *newPSStream();
  PSPlot::PSStream *getPSStream( const std::string &name );
  void deleteStream( const std::string &name );
  void deleteStream( Stream *stream );

  void readInitFile( const std::string &filename );
  void fixupAllItemStreams();
  void printTargetStreamInfo(std::ostream& os);
  void lspWrite( std::ostream &ostr );
  void deleteFiles();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void initialize( std::ifstream &istr );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  typedef std::map<std::string, BasicStream *> StreamMap;

  static StreamManager *s_instance;

  StreamMap        m_streams;

};

#endif
