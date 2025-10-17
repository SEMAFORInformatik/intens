
#include "job/JobIncludes.h"

#include "operator/ClientSocket.h"

#include "job/JobEngine.h"
#include "job/JobClientSocket.h"
#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobClientSocket::startJobAction(){
  BUG(BugJob,"JobClientSocket::startJobAction" );
  assert( m_socket != 0 );
  m_socket->setListener( this );
  m_socket->start();
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobClientSocket::stopJobAction(){
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* connectionClosed --                                                         */
/* --------------------------------------------------------------------------- */

void JobClientSocket::connectionClosed(bool abort){
  BUG(BugJob,"JobClientSocket::connectionClosed" );
  endJobAction( JobAction::job_Ok_ignore );
}

bool JobClientSocket::checkInput(){
  return m_socket -> checkInput();
}

const std::string &JobClientSocket::getErrorMessage() const{
  return m_socket -> getErrorMessage();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void JobClientSocket::serializeXML(std::ostream &os, bool recursive ){
  os << "<socket name=\"" << Name() << "\">" << std::endl;
  os << "</socket>" <<  std::endl;
}
