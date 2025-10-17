
/* Local headers */
#include "streamer/TargetContainer.h"
#include "streamer/StreamManager.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

TargetContainer::TargetContainer(){
}

TargetContainer::~TargetContainer(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setTargetStream --                                                          */
/* --------------------------------------------------------------------------- */

void TargetContainer::setTargetStream( const std::string &name, BasicStream *stream ){
  BUG_DEBUG("targetContainer::setTargetStream, name: "<< name << " to stream named: " << stream->Name());
  TargetLists::iterator it = m_targetlists.find( name );
  if( it == m_targetlists.end() ){
    TargetStreamList *slist = new TargetStreamList();
    ///    TargetLists::iterator beg=m_targetlists.begin();
    m_targetlists.insert( TargetLists::value_type(name,slist) );
    insert( stream, *slist );
    return;
    //  assert( it != m_targetlists.end() );
  }
  insert( stream, *(*it).second );
}

/* --------------------------------------------------------------------------- */
/* getTargetStreamList --                                                      */
/* --------------------------------------------------------------------------- */

TargetStreamList *TargetContainer::getTargetStreamList( const std::string &name ){
  TargetLists::iterator it = m_targetlists.find( name );
  if( it == m_targetlists.end() ){
    return 0;
  }
  return (*it).second;
}

/* --------------------------------------------------------------------------- */
/* insert --                                                                   */
/* --------------------------------------------------------------------------- */

void TargetContainer::insert( BasicStream *s, TargetStreamList &slist ){
  assert( s!=0 );
  TargetStreamList::iterator it;

  for( it = slist.begin(); it != slist.end(); ++it ){
    if( (*it) == s ){
      return; // exists already
    }
  }
  slist.push_back( s );
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void TargetContainer::print(){
  TargetStreamList::iterator it;
  TargetLists::iterator i;
  std::cout << "================= TargetContainer =================" << std::endl;
  for( i = m_targetlists.begin(); i != m_targetlists.end(); ++i ){
    std::cout << "Item: " << (*i).first << std::endl;
    TargetStreamList *slist = (*i).second;
    for( it = slist->begin(); it != slist->end(); ++it ){
      std::cout << "  - " << (*it)->Name() << std::endl;
      (*it)->printTargetStreams(std::cout);
      (*it)->printItemStreams(std::cout);
    }
  }
  std::cout << "================= TargetContainer =================" << std::endl;

  StreamManager::Instance().printTargetStreamInfo(std::cout);
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void TargetContainer::marshal( std::ostream &os ){
  std::cerr<<"TargetContainer::marshal   NOT IMPLEMENTED YET !!"<<std::endl;

}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *TargetContainer::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void TargetContainer::setText( const std::string &text ){
}
