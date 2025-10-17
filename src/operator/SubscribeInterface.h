
#if !defined(SUBSCRIBE_INTERFACE_H)
#define SUBSCRIBE_INTERFACE_H

#include <vector>
#include <string>

class Stream;
class JobFunction;
class ConnectionListener;

class SubscribeInterface {
 public:
  SubscribeInterface();
  virtual ~SubscribeInterface(){};

  /** set subscribe output Streams for next operation
   */
  void setSubscribeOutStreams( const std::vector<Stream*>& out_streams );

  /** set subscribe function for next operation
   */
  void setSubscribeFunction( JobFunction* func );

  /** set subscribe header for next operation
   */
  void setSubscribeHeader( const std::string header );

  /** set connection listener
   */
  void setSubscribeListener( ConnectionListener *listener );

  /** check input for next subscribe
   */
  virtual bool checkSubscribe() = 0;

  /** do subscribe
   */
  virtual void doSubscribe() = 0;

  /** get error message
   */
  virtual std::string getErrorMessage() const = 0;

 protected:
  std::vector<Stream*>&       getSubscribeOutStreams();
  std::string&                getSubscribeHeader();
  JobFunction*                getSubscribeFunction();
  ConnectionListener*         getSubscribeListener();

 private:

  // used by SubscribeInterface
  JobFunction           *m_function;
  std::vector<Stream*>   m_out_streams;
  std::string            m_header;
  ConnectionListener    *m_listener;
};

#endif
