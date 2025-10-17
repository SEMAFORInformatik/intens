
#include "job/JobIncludes.h"

#include <algorithm>
#include <sstream>
#include <limits>

#include "utils/Date.h"
#include "utils/gettext.h"

#include "job/JobStarter.h"
#include "job/JobAction.h"
#include "job/JobFunction.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataString.h"
#include "job/JobStackDataStructure.h"

#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"
#include "gui/Timer.h"
#include "xfer/XferDataItem.h"
#include "app/UiManager.h"

bool JobStarter::s_wait = false;
std::string JobStarter::s_waitName;
JobStarter::JobQueue JobStarter::s_jobqueue;
JobStarter *JobStarter::s_to_delete=0;

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobStarter::JobStarter( JobAction *fun )
  : m_function( fun )
  , m_reason( JobElement::cll_Function )
  , m_new_dataitem( 0 )
  , m_baseDataitem( 0 )
  , m_sourceDataitem( 0 )
  , m_source2Dataitem( 0 )
  , m_old_data( 0 )
  , m_index( -1 )
  , m_diagramXPos(std::numeric_limits<double>::max())
  , m_diagramYPos(std::numeric_limits<double>::max()){
  assert( fun != 0 );
  m_timer=GuiFactory::Instance()->createTimer( 0 );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
  setName( "StarterOf" + fun->Name() );
}

JobStarter::~JobStarter(){
  BUG_PARA(BugJobStart,"JobStarter::~JobStarter",this);
  if( m_timer ){
    delete m_timer;
    m_timer = 0;
  }
  if( m_task ){
    delete m_task;
    m_task = 0;
  }
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setSource --                                                                */
/* --------------------------------------------------------------------------- */
void JobStarter::setBase( XferDataItem *dataitem ){
  BUG(BugJobStart,"JobStarter::setBase");
  assert( dataitem != 0 );
  // ========================================================
  // Das XferDataItem wird für alle Fälle kopiert. Somit sind
  // wir vor Sideeffects sicher.
  // ========================================================
  m_baseDataitem = new XferDataItem( *dataitem );
}

/* --------------------------------------------------------------------------- */
/* setSource --                                                                */
/* --------------------------------------------------------------------------- */
void JobStarter::setSource( XferDataItem *dataitem ){
  BUG(BugJobStart,"JobStarter::setSource");
  assert( dataitem != 0 );
  // ========================================================
  // Das XferDataItem wird für alle Fälle kopiert. Somit sind
  // wir vor Sideeffects sicher.
  // ========================================================
  m_sourceDataitem = new XferDataItem( *dataitem );
}

/* --------------------------------------------------------------------------- */
/* setSource --                                                                */
/* --------------------------------------------------------------------------- */
void JobStarter::setSource( const std::vector<XferDataItem*>& sourceList ) {
  BUG(BugJobStart,"JobStarter::setSource");
  // ========================================================
  // Das XferDataItem wird für alle Fälle kopiert. Somit sind
  // wir vor Sideeffects sicher.
  // ========================================================
  m_sourceListDataitem.clear();
  for(std::vector<XferDataItem*>::const_iterator it = sourceList.begin();
       it != sourceList.end(); ++it)
    m_sourceListDataitem.push_back(  new XferDataItem( *(*it) ) );
}

/* --------------------------------------------------------------------------- */
/* setSource2 --                                                               */
/* --------------------------------------------------------------------------- */
void JobStarter::setSource2( XferDataItem *dataitem ){
  BUG(BugJobStart,"JobStarter::setSource2");
  assert( dataitem != 0 );
  // ========================================================
  // Das XferDataItem wird für alle Fälle kopiert. Somit sind
  // wir vor Sideeffects sicher.
  // ========================================================
  m_source2Dataitem = new XferDataItem( *dataitem );
}

/* --------------------------------------------------------------------------- */
/* setDataItem --                                                              */
/* --------------------------------------------------------------------------- */

void JobStarter::setDataItem( XferDataItem *dataitem ){
  BUG(BugJobStart,"JobStarter::setDataItem");
  assert( dataitem != 0 );

  // ========================================================
  // Das XferDataItem wird für alle Fälle kopiert. Somit sind
  // wir vor Sideeffects sicher.
  // ========================================================
  m_new_dataitem = new XferDataItem( *dataitem );

  switch( dataitem->getDataType() ){
  case DataDictionary::type_Integer:{
      int i;
      if( dataitem->getValue( i ) ){
        m_old_data = new JobStackDataInteger( i );
        BUG_MSG("Integer Value " << i);
      }
    }
    break;

  case DataDictionary::type_Real:{
      double d;
      if( dataitem->getValue( d ) ){
        m_old_data = new JobStackDataReal( d );
        BUG_MSG("Real Value " << d);
      }
    }
    break;

  case DataDictionary::type_Complex:{
      dComplex c;
      if( dataitem->getValue( c ) ){
        m_old_data = new JobStackDataComplex( c );
        BUG_MSG("Complex Value " << c);
      }
    }
    break;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
      std::string s;
      if( dataitem->getValue( s ) ){
        m_old_data = new JobStackDataString( s );
        BUG_MSG("String Value " << s);
      }
    }
    break;

  case DataDictionary::type_StructVariable:
    m_old_data = new JobStackDataStructure( *dataitem );
    BUG_MSG("Structure Variable.....");
    break;

  default:
    assert( false );
  }
}

/* --------------------------------------------------------------------------- */
/* setDataItem --                                                              */
/* --------------------------------------------------------------------------- */

void JobStarter::setDataItem( XferDataItem *dataitem, std::string &txt ){
  BUG(BugJobStart,"JobStarter::setDataItem");

  assert( dataitem != 0 );

  // ========================================================
  // Das XferDataItem wird für alle Fälle kopiert. Somit sind
  // wir vor Sideeffects sicher.
  // ========================================================
  m_new_dataitem = new XferDataItem( *dataitem );

  m_old_data = new JobStackDataString( txt );
}

/* --------------------------------------------------------------------------- */
/* startJob --                                                                 */
/* --------------------------------------------------------------------------- */

void JobStarter::startJob( bool block_undo ){
  BUG_PARA(BugJobStart,"startJob", "this=" << this );
  // Als erstes wird der Starter in einer Queue abgelegt. Falls bereits
  // ein Job läuft, wird dieser den effektiven Start übernehmen.
  if( block_undo ){
    BUG_MSG("block undo");
    setBlockUndo();
  }

  push( this );

  if( s_wait ){
    BUG_EXIT("function " << m_function->Name() << " will start later because function "
             << s_waitName << " is running. Queue size is " << s_jobqueue.size());
    return;
  }
  s_wait=true;
  s_waitName=m_function->Name();
  if( m_function->highPriority() ){
    tryAgain( this );
  }
  else{
    m_timer->start();
  }
}

/* --------------------------------------------------------------------------- */
/* tryAgain --                                                                 */
/* --------------------------------------------------------------------------- */

void JobStarter::tryAgain( JobStarter *caller ) {
  BUG(BugJobStart,"JobStarter::tryAgain");
  if( s_to_delete ){
    delete s_to_delete;
    s_to_delete=0;
  }
  JobStarter *s = pop();
  if( s == 0 ){
    BUG_EXIT("no more Starters available");
    s_wait=false;
    s_to_delete=caller;
    return;
  }
  s_waitName=s->m_function->Name();
  BUG_MSG("function " << s->m_function->Name() << " starts");
  BUG_INFO(">> function " << s->m_function->Name() << " starts (queue size " << s_jobqueue.size() << ")");
  s->startNow();
  if( s != caller ){
    s_to_delete=caller;
  }
}

/* --------------------------------------------------------------------------- */
/* startNow --                                                                 */
/* --------------------------------------------------------------------------- */

void JobStarter::startNow(){
  BUG_PARA(BugJobStart,"JobStarter::startNow", this );
  assert( m_function != 0 );

  // should index value be logged?
  std::vector<std::string> noLogFunc;
  noLogFunc.push_back("INIT");
  noLogFunc.push_back("QUIT");
  noLogFunc.push_back("Enable Undo");
  noLogFunc.push_back("AFTER_UPDATE_FORMS");
  //noLogFunc.push_back("AFTER_DB_LOGON");
  //noLogFunc.push_back("AFTER_PROGRESS_DIALOG_ABORTED");
  //noLogFunc.push_back("ON_CYCLE_SWITCH");
  noLogFunc.push_back("@FileStream@");
  std::vector<std::string>::iterator it = std::find(noLogFunc.begin(), noLogFunc.end(), m_function->Name());

  prepareToStartJob();

  m_function->setReason( m_reason );
  m_function->setDiagramXPos( m_diagramXPos );
  m_function->setDiagramYPos( m_diagramYPos );
  m_function->setSortCriteria( m_sortCriteria );

  if( m_new_dataitem != 0 ){
    m_function->setData( m_new_dataitem, m_old_data );
    if( m_index >= 0 ){
      m_function->setIndex( m_index );
    }
    m_new_dataitem = 0;
    m_old_data     = 0;
  }
  else{
    m_function->setIndex( m_index );
    if (m_old_data) {
      JobFunction *func = dynamic_cast<JobFunction*>(m_function);
      if (func) func->setOldData(m_old_data);
    }
	// write index value into log file
	if (it == noLogFunc.end()) {
	  std::ostringstream ss;
	  ss << m_index;
	  std::string s(ss.str());
	  std::string key(Name());
	  UImanager::Instance().addDescriptionFunctionArgument(key, s);
	  DES_DEBUG(compose("  //JobStarter SET_RESOURCE(\"Arg@%1\", \"%2\"); // -- Name: %3, actionName: %4",
						key, s, Name(), m_function->Name()));
	}
  }
  if( m_baseDataitem != 0 ){
    m_function->setBaseDataItem( m_baseDataitem );
  }
  if( m_sourceDataitem != 0 ){
    m_function->setSourceDataItem( m_sourceDataitem );
  }
  if( m_sourceListDataitem.size() ){
    m_function->setSourceDataItem( m_sourceListDataitem );
  }
  if( m_source2Dataitem != 0 ){
    m_function->setSource2DataItem( m_source2Dataitem );
  }
  m_function->setStartTime( Date::currentIsoTime() );

  if (it == noLogFunc.end() &&
      m_function->Name() != "ON_CYCLE_SWITCH") {
    if (false && AppData::Instance().PyLogMode()) {
      PYLOG_INFO(compose(PYLOG_CALL_PROCESS, m_function->Name(), "FunctionXX"));
      std::cerr << "<abort> JobStarter::startNow()\n";
      return;
    }
    DES_INFO(compose("  RUN(%1);", m_function->Name()));
  }
  startNextJobAction( m_function );
}

/* --------------------------------------------------------------------------- */
/* startTransaction --                                                         */
/* --------------------------------------------------------------------------- */

void JobStarter::startTransaction(){
  assert( m_function != 0 );
  JobAction::startTransaction( m_function );
}

/* --------------------------------------------------------------------------- */
/* backFromJobController --                                                    */
/* --------------------------------------------------------------------------- */

void JobStarter::backFromJobController( JobAction::JobResult rslt ){
  BUG_PARA(BugJobStart,"JobStarter::backFromJobController", this );

  assert( s_wait );
  BUG_MSG("function " << m_function->Name() << " is done");
  BUG_INFO("<< function " << m_function->Name() << " is done");

  backFromJobStarter( rslt );

  if( rslt != job_Ok ){
    s_wait = false;
    cleanup( rslt );
  }
  else{
    if( m_function->highPriority() ){
      tryAgain( this );
    }
    else{
      m_timer->start();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* push --                                                                     */
/* --------------------------------------------------------------------------- */

void JobStarter::push( JobStarter *starter ){
  if (s_jobqueue.size()) {
    JobStarter *s_front = s_jobqueue.front();
    if (s_front->Name() == starter->Name() &&
        s_front->m_baseDataitem && starter->m_baseDataitem &&
        s_front->m_baseDataitem->getFullName(true) == starter->m_baseDataitem->getFullName(true) &&
        s_front->getReason() == starter->getReason()) {
      BUG_DEBUG("push: IGNORE Name: " << starter->Name());
      return;
    }
  }
  s_jobqueue.push( starter );
  BUG_DEBUG("push: " << s_jobqueue.size() << ", Name: " << starter->Name());
}

/* --------------------------------------------------------------------------- */
/* pop --                                                                      */
/* --------------------------------------------------------------------------- */

JobStarter *JobStarter::pop(){
  if( s_jobqueue.size() == 0 ) return 0;
  JobStarter *s = s_jobqueue.front();
  s_jobqueue.pop();
  BUG_DEBUG("pop: " << s_jobqueue.size() << ", Name: " << s->Name());
  return s;
}

/* --------------------------------------------------------------------------- */
/* cleanup --                                                                  */
/* --------------------------------------------------------------------------- */

void JobStarter::cleanup( JobAction::JobResult rslt ){
  JobStarter *s;
  while( (s=pop()) != 0 ){
    s->backFromJobStarter( rslt );
  }
}
