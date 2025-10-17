
#if !defined(REQUEST_INTERFACE_H)
#define REQUEST_INTERFACE_H

#include <vector>
#include <string>

class Stream;
class ConnectionListener;

class RequestInterface {
 public:
  RequestInterface();
  virtual ~RequestInterface(){};

  /** set request output Streams for next operation
   */
  void setRequestOutStreams( const std::vector<Stream*>& out_streams );

  /** set request input Streams for next operation
   */
  void setRequestInStreams( const std::vector<Stream*>& in_streams );

  /** set request header for next operation
   */
  void setRequestHeader( const std::string header );

  /** set request timeout for next operation
   */
  void setRequestTimeout( const int timeout );

  /** set connection listener
   */
  void setRequestListener( ConnectionListener *listener );

  /** check input for next request
   */
  virtual bool checkRequest() = 0;

  /** start next request
   */
  virtual void startRequest() = 0;

  /** terminate request
      (no longer waiting for response)
   */
  virtual void terminateRequest() = 0;

  /** get error message
   */
  virtual std::string getErrorMessage() const = 0;

  const std::vector<Stream*>& getRequestInStreams() const;
  const std::vector<Stream*>& getRequestOutStreams() const;
  const std::string&          getRequestHeader() const;
  int                         getRequestTimeout() const;

 protected:
  ConnectionListener*         getRequestListener();

 private:

  // used by RequestInterface
  std::vector<Stream*>   m_in_streams;
  std::vector<Stream*>   m_out_streams;
  std::string            m_header;
  int                    m_timeout;
  ConnectionListener    *m_listener;
};

#endif
