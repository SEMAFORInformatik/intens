
#include <typeinfo>

#include "utils/Debugger.h"

#include "operator/MatlabProcess.h"
#include "operator/MatlabInitProcess.h"
#include "operator/ProcessGroup.h"
#include "operator/BatchProcess.h"
#include "gui/GuiFactory.h"
#include "gui/GuiPulldownMenu.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiSeparator.h"
#include "utils/gettext.h"
#include "utils/Date.h"

ProcessGroup::ProcessGroupMap ProcessGroup::s_procgroupmap;

INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

ProcessGroup::ProcessGroup( const std::string &name )
  : JobAction(name)
  , m_failed( false )
  , m_stopped( true )
  , m_ExitStatus( 0 )
  , m_uiUpdateInterval( 0 ){
  if( name.empty() )
    return;
  m_label = name;
  if( find(name)==0 ){
    s_procgroupmap.insert( ProcessGroupMap::value_type(name, this) );
  }
  m_current = m_processes.begin();
}

ProcessGroup::~ProcessGroup(){
  ProcessGroupMap::iterator pi;
  for( pi = s_procgroupmap.begin(); pi != s_procgroupmap.end(); pi++ ){
    if( (*pi).second == this ){
      s_procgroupmap.erase( pi );
    }
  }
//   vector<Process *>::iterator iter;
//   for( iter=m_processes.begin(); iter!=m_processes.end(); ++iter )
//     delete *iter;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/


/* --------------------------------------------------------------------------- */
/* find --                                                                     */
/* --------------------------------------------------------------------------- */

ProcessGroup *ProcessGroup::find( const std::string &name ){
 ProcessGroupMap::iterator iter = s_procgroupmap.find( name );
 if( iter != s_procgroupmap.end() )
   return (*iter).second;
 return 0;
}

/* --------------------------------------------------------------------------- */
/* appendProcess --                                                            */
/* --------------------------------------------------------------------------- */

Process *ProcessGroup::appendProcess( const std::string &name ){
  Process *p=0;
  if( !name.empty() ){
    if( (p=Process::getUninstalled( name ))==0 )
      return 0;
  }
  else{
    p=new BatchProcess();
  }
  if( typeid( *p ) == typeid( MatlabProcess ) ){
    MatlabInitProcess *mip = &MatlabInitProcess::Instance();
    if( m_processes.empty() ){
      m_processes.push_back( mip );
    }else{
      if(dynamic_cast<MatlabInitProcess*>(m_processes.front())){
        m_processes.insert(m_processes.begin(), mip);
      }
    }
  }
  m_processes.push_back(p);
  return p;
}

/* --------------------------------------------------------------------------- */
/* prepareStart --                                                             */
/* --------------------------------------------------------------------------- */

void ProcessGroup::prepareStart(){
  BUG(BugOperator,"ProcessGroup::prepareStart" );
  m_current      = m_processes.begin();
  m_failed       = false;
  m_ExitStatus   = 0;
  m_stopped      = false;
  std::vector<Process *>::iterator pi;
  for( pi=m_processes.begin(); pi!=m_processes.end(); ++pi )
    (*pi)->clearTextWindows();
  m_inputCheckOk=true;
}

/* --------------------------------------------------------------------------- */
/* stop --                                                                     */
/* --------------------------------------------------------------------------- */

void ProcessGroup::stop(){
  if( m_current != m_processes.end() ){
    if( !(*m_current)->stop() ){
      std::string s( compose(_("stop_processgroup %1"), m_label) );
      perror( s.c_str() );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* work --                                                                     */
/* --------------------------------------------------------------------------- */

JobAction::JobResult ProcessGroup::work(){
  BUG( BugOperator, "ProcessGroup::work");
  if( getExitStatus() ){
    m_failed = true;
    BUG_EXIT( "return JobAction::job_Aborted, failed");
    return JobAction::job_Aborted;
  }

  if ( !isRunning() ) { /* start a new process */
    BUG_MSG(" start a new process" );

    Process *p = current();
    if( p != 0 ){
      if( !p->checkInput() ){
        m_inputCheckOk=false;
        m_failed=true;
        m_ExitStatus=-99;
        BUG_EXIT( "return JobAction::job_Aborted");
        return JobAction::job_Aborted;
      }
      if (p->getExecCmd() == "echo" || p->getExecCmd() == "echo {}") {
        BUG_MSG( "pseudo process, do nothing");
        p->writeInputStreams();
        p->clearOutputStreams();
        return JobAction::job_Ok;
      }

      p->setUiUpdateInterval( m_uiUpdateInterval );
      BUG_MSG( "start the process");
      if( ! p->start() ){
	BUG_EXIT( "return JobAction::job_Aborted, start");
	return JobAction::job_Aborted;
      }
      if (!p->isDaemon())
	return JobAction::job_Wait; // Es geht weiter
    }
    return JobAction::job_Ok;  /* end of process list */
  }

  if( (*(m_current))->hasTerminated() ){
    m_ExitStatus = (*(m_current))->getExitStatus();
    BUG_MSG("m_ExitStatus: " << m_ExitStatus);
    if( m_ExitStatus == 0 ){
      if( next() == 0 ){
	BUG_EXIT("return JobAction::job_Ok ");
	return JobAction::job_Ok;  /* end of process list */
      }
    }
    else{
      BUG_EXIT("return JobAction::job_Aborted");
      return JobAction::job_Aborted;
    }
  }
  return JobAction::job_Wait; // Es geht weiter
}

/* --------------------------------------------------------------------------- */
/* next --                                                                     */
/* --------------------------------------------------------------------------- */

Process *ProcessGroup::next(){
  if( m_processes.empty() ) return 0;
  if( m_current == m_processes.end() ) return 0;
  ++m_current;
  if( m_current == m_processes.end() ) return 0;
  return *m_current;
}

/* --------------------------------------------------------------------------- */
/* backFromWorker --                                                           */
/* --------------------------------------------------------------------------- */

void ProcessGroup::backFromWorker( JobAction::JobResult rslt ){
  BUG_PARA( BugJobStart, "ProcessGroup::backFromWorker", "THIS=" << this );
  switch( rslt ){
  case job_Aborted :
    BUG_MSG( "job_Aborted" );

#if ! HAVE_QT
    if( m_current != m_processes.end() )
      (*m_current)->workFailed();
#endif

    if( m_stopped ){
      endJobAction( job_Canceled );
    }
    else{
      endJobAction( job_Aborted );
    }
    break;

   case job_Ok :
    if( m_stopped ){
      BUG_MSG( "Worker ok, but job_Canceled" );
      endJobAction( job_Canceled );
    }
    else{
     BUG_MSG( "job_Ok" );
     endJobAction( job_Ok );
    }
    break;

   case job_Canceled :
    BUG_MSG( "job_Canceled" );
    endJobAction( rslt );
    break;

   case job_FatalError :
    BUG_MSG( "job_FatalError" );
    endJobAction( rslt );
    break;

  default :
    assert( false );
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void ProcessGroup::startJobAction(){
  BUG( BugJobStart, "ProcessGroup::startJobAction" );
  startWorker();

  if (CallingActionName().empty()) {
    PYLOG_INFO(compose(PYLOG_CALL_PROCESS, Name(), "ProcessGroup"));
    DES_INFO(compose("  RUN(%1); // ProcessGroup Label: %2", Name(), m_label));
  }
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void ProcessGroup::stopJobAction(){
  BUG( BugJobStart, "ProcessGroup::stopJobAction" );

  if( m_stopped ){
    return;
  }

  m_stopped = true;
  stop();
}

/* --------------------------------------------------------------------------- */
/* startWorker --                                                              */
/* --------------------------------------------------------------------------- */

void ProcessGroup::startWorker(){
  BUG( BugJobStart, "ProcessGroup::startWorker");
  prepareStart();
  bool use_timer( true );
  Worker *worker = new Worker( this, 50 );
  worker->start();
  BUG_EXIT("Worker started");
}

/* --------------------------------------------------------------------------- */
/* createButtonListener --                                                     */
/* --------------------------------------------------------------------------- */

GuiButtonListener *ProcessGroup::createButtonListener(){
  return new JobActionButtonListener( this );
}

/* --------------------------------------------------------------------------- */
/* createProcessMenu --                                                        */
/* --------------------------------------------------------------------------- */

void ProcessGroup::createProcessMenu( GuiPulldownMenu *menu ){
  ProcessGroupMap::iterator i;
  for( i = s_procgroupmap.begin(); i != s_procgroupmap.end(); ++i ){
    if( !(*i).second->MenuInstalled() ){
      GuiButtonListener *listener = (*i).second->getButtonListener();
      GuiMenuButton *pushbutton = GuiFactory::Instance()->createMenuButton( menu->getElement(), listener );
      menu->attach( pushbutton->getElement() );
      if( (*i).second->getLabel().empty() )
        pushbutton->setLabel( (*i).first );
      else
        pushbutton->setLabel( (*i).second->getLabel() );
      (*i).second->setMenuInstalled();
    }
  }
#if __MINGW32__ && defined HAVE_MATLAB
  if ( MatlabProcess::needMatlab() ) {
    GuiSeparator *sep = GuiFactory::Instance()->createSeparator(  menu->getElement() );
    menu->attach( sep->getElement() );
    GuiMenuButtonListener *listener = &MatlabInitProcess::Instance().m_matlabConnClose;
    GuiMenuButton *pushbutton = GuiFactory::Instance()->createMenuButton( menu->getElement(), listener );
    pushbutton->setLabel( _("Close Matlab Connection") );
    menu->attach( pushbutton->getElement() );
  }
#endif
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void ProcessGroup::serializeXML(std::ostream &os, bool recursive){
  os << "<processgroup name=\"" << Name() << "\">" << std::endl;
  if( recursive ){
    ProcessList::iterator it;
    for( it = m_processes.begin(); it != m_processes.end(); ++it ){
      (*it)->serializeXML(os, recursive);
    }
  }
  os << "</processgroup>" <<  std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeAll --                                                             */
/* --------------------------------------------------------------------------- */

void ProcessGroup::serializeAll( std::ostream &os ){
  static ProcessGroupMap s_procgroupmap;
  ProcessGroupMap::iterator it;
  for( it = s_procgroupmap.begin(); it != s_procgroupmap.end(); ++it ){
    (*it).second->serializeXML(os, true);
  }
}
