
#if !defined(STREAMER_TARGETCONTAINER_H)
#define STREAMER_TARGETCONTAINER_H
#include <string>
#include <map>

#include "streamer/BasicStream.h"
#include "xml/Serializable.h"

class TargetContainer : public Serializable
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  TargetContainer();
  virtual ~TargetContainer();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setTargetStream( const std::string &name, BasicStream *stream );
  TargetStreamList *getTargetStreamList( const std::string &name );
  void print();

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void insert( BasicStream *s, TargetStreamList &slist );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  typedef std::map<std::string, TargetStreamList *> TargetLists;

  TargetLists      m_targetlists;
};


#endif
