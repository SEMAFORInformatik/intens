
#if !defined(CONNECTION_LISTENER)
#define CONNECTION_LISTENER

class ConnectionListener
{
/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  virtual void connectionClosed(bool abort=false) = 0;
};

#endif
