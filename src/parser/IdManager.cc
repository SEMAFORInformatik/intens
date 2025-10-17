
#include <string>

#include "parser/IdManager.h"

/* initialize */
IdManager *IdManager::s_instance = 0;

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

IdManager &IdManager::Instance(){
  if( s_instance == 0 ){
    s_instance = new IdManager();
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* remove --                                                                   */
/* --------------------------------------------------------------------------- */

void IdManager::remove(){
  if( s_instance != 0 ){
    delete s_instance;
    s_instance = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* registerId --                                                               */
/* --------------------------------------------------------------------------- */
bool IdManager::registerId( const std::string &id, IdentifierType type ){
  return m_id_list.insert( IdentifierList::value_type( id, type ) ).second;
}

/* --------------------------------------------------------------------------- */
/* getType --                                                                  */
/* --------------------------------------------------------------------------- */

IdManager::IdentifierType IdManager::getType( const std::string &id ){
  IdentifierList::iterator ii = m_id_list.find( id );
  if( ii == m_id_list.end() ){
    return id_None;
  }
  return (*ii).second;
}
