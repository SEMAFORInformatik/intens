
#include "utils/Debugger.h"

#include "operator/OpProcess.h"
#include "streamer/StreamManager.h"
#include "app/AppData.h"
#include "utils/gettext.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiFactory.h"

INIT_LOGGER();

Process::ProcessMap Process::s_procmap;
Process::BasicStreamList Process::s_basicstreams;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

Process::Process( const std::string &name )
  : m_name( name )
  , m_is_running( false )
  , m_ExitStatus( 0 )
  , m_installed( false )
  , m_uiUpdateInterval( 0 ){
  if( name.empty() )
    return;
  if( find(name)==0 ){
    add(name,this);
  }
  setExecCmd(name);
}

Process::Process( const Process &process )
  : m_name( process.m_name )
  , m_is_running( false )
  , m_ExitStatus( 0 )
  , m_exec_cmd( process.m_exec_cmd )
  , m_installed( false ){
}

Process::~Process(){
  ProcessMap::iterator pi;
  for( pi = s_procmap.begin(); pi != s_procmap.end(); pi++ ){
    if( (*pi).second == this ){
      s_procmap.erase( pi );
    }
  }
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* find --                                                                     */
/* --------------------------------------------------------------------------- */

Process* Process::find( const std::string &name ){
 ProcessMap::iterator iter = s_procmap.find( name );
 if( iter != s_procmap.end() )
   return (*iter).second;
 return 0;
}

/* --------------------------------------------------------------------------- */
/* getUninstalled --                                                           */
/* --------------------------------------------------------------------------- */

Process* Process::getUninstalled( const std::string &name ){
  Process *process = find( name );
  if( process == 0 ){
    return 0;
  }
  if( process->isInstalled() ){
    process = process->clone();
  }
  process->setInstalled();
  return process;
}

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

void Process::add(const std::string &name, Process *p ){
  s_procmap.insert( ProcessMap::value_type(name, p) );
}

/* --------------------------------------------------------------------------- */
/* hasTerminated --                                                            */
/* --------------------------------------------------------------------------- */

bool Process::hasTerminated( ){
  return true;
}

/* --------------------------------------------------------------------------- */
/* registerBasicStream --                                                      */
/* --------------------------------------------------------------------------- */

void Process::registerBasicStream( BasicStream *s, const std::string &fifo ){
  BUG_PARA( BugOperator, "Process::registerBasicStream", "'" << fifo << "'" );
  s_basicstreams.push_back( std::make_pair( s, fifo) );
}

/* --------------------------------------------------------------------------- */
/* addInputStream --                                                           */
/* --------------------------------------------------------------------------- */

Process::error_type
Process::addInputStream( const std::string &streamid, const std::string &fifo ){
  return addInputStream( StreamManager::Instance().getStream( streamid ), fifo);
}

/* --------------------------------------------------------------------------- */
/* addInputStream --                                                           */
/* --------------------------------------------------------------------------- */

Process::error_type
Process::addInputStream( Stream *s, const std::string &fifo ){
  if( s==0 ){
    return O_STREAM_UNDECLARED;
  }
  m_instreams.push_back( s );
  return O_NO_ERROR;
}

/* --------------------------------------------------------------------------- */
/* setOutputBasicStreams --                                                    */
/* --------------------------------------------------------------------------- */

void Process::setOutputBasicStreams(){
  BUG( BugOperator, "Process::setOutputBasicStreams" );
  BasicStreamList::iterator it;
  for( it = s_basicstreams.begin(); it != s_basicstreams.end(); ++it ){
    m_outstreams.push_back( (*it).first );
  }
  s_basicstreams.clear();
}

/*------------------------------------------------------------
  setOutputStream --
    adds a new output format to the output stream list.
  ------------------------------------------------------------
*/
Process::error_type
Process::setOutputStreams( const std::vector< std::pair<std::string,std::string> > &outstr )
{
  Stream *s;
  std::vector< std::pair<std::string,std::string> >::const_iterator oi;

  for( oi=outstr.begin(); oi!=outstr.end(); ++oi ){
    if ( (s = StreamManager::Instance().getStream( (*oi).first )) == 0 ){
      return O_STREAM_UNDECLARED;
    }
    m_outstreams.push_back( s );
  }
  return O_NO_ERROR;
}

/*------------------------------------------------------------
  addOutputStream --
    adds a new output format to the output stream list.
  ------------------------------------------------------------
*/
Process::error_type
Process::addOutputStream( Stream *s )
{
  if( s==0 )
    return O_STREAM_UNDECLARED;

  m_outstreams.push_back( s );
  return O_NO_ERROR;;
}

/*------------------------------------------------------------
 */
bool Process::checkInput(){
  std::vector<Stream *>::iterator striter;
//   MultiLanguage &mls = MultiLanguage::Instance();
  bool chok=true;
  for( striter=m_instreams.begin(); striter!=m_instreams.end(); ++striter ){
    std::vector<std::string> invaliditems;

    if( (*striter)->getInvalidItems(invaliditems) ){
      chok=false;
      std::ostringstream errs;
//       errs << mls.getString( "ML_CH_INCOMP_INP", ML_CONST_STRING );
      errs << compose(_("\nIncomplete input for the following items \nin stream '%1':\n"), (*striter)->Name());

      for( int i=0; i<invaliditems.size(); ++i ){
        errs << "   " << invaliditems[i] << std::endl;
      }
      m_messages=errs.str();
      GuiFactory::Instance()->getLogWindow()->writeText( errs.str() );
    }
  }
  return chok;
}

/*------------------------------------------------------------
 */
void Process::putStreamValues(bool transposed ){
  std::vector< Stream * >::const_iterator I;
  for( I=m_instreams.begin(); I!=m_instreams.end(); ++I ){
    (*I)->putValues(this, transposed);
  }
}

/*------------------------------------------------------------
 */
int Process::getInputParameters( std::ostream &os )const{
  std::vector< Stream * >::const_iterator I;
  int sum=0;
  for( I=m_instreams.begin();I!=m_instreams.end(); ++I ){
    if( (*I) != 0 ){
      sum += (*I)->getParameters(os);
    }
  }
  return sum;
}

/* --------------------------------------------------------------------------- */
/* getOutputParameters --                                                      */
/* --------------------------------------------------------------------------- */

int Process::getOutputParameters( std::ostream &os )const{
  OutputStreamList::const_iterator I;
  int sum=0;
  for( I = m_outstreams.begin(); I != m_outstreams.end(); ++I ){
    if( (*I) != 0 ){
      sum += (*I)->getParameters(os);
    }
  }
  return sum;
}

/* --------------------------------------------------------------------------- */
/* getStreamValues --                                                          */
/* --------------------------------------------------------------------------- */

void Process::getStreamValues(){
  OutputStreamList::const_iterator I;
  for( I = m_outstreams.begin(); I != m_outstreams.end(); ++I ){
    if( *I != 0 ){
      (*I)->getValues(this);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* clearTextWindows --                                                         */
/* --------------------------------------------------------------------------- */

void Process::clearTextWindows(){
  OutputStreamList::iterator I;
  BUG(BugOperator,"Process::clearTextWindows" );
  for( I = m_outstreams.begin(); I != m_outstreams.end(); ++I ){
    (*I)->clearTextWindow();
  }
}

/* --------------------------------------------------------------------------- */
/* writeInputStreams --                                                        */
/* --------------------------------------------------------------------------- */

void Process::writeInputStreams(){
  InputStreamList::iterator I;
  BUG_DEBUG("PseudoProcess writeInputStreams, name: " << m_name );
  for( I=m_instreams.begin();I!=m_instreams.end(); ++I ){
    BUG_DEBUG("Streams, name: " << (*I)->Name());
    std::ostringstream os;
    (*I)->write(os);
  }
}

/* --------------------------------------------------------------------------- */
/* clearOutputStreams --                                                       */
/* --------------------------------------------------------------------------- */

void Process::clearOutputStreams(){
  BUG_DEBUG("PseudoProcess clearOutputStreams, name: " << m_name );
  OutputStreamList::iterator I;
  for( I = m_outstreams.begin(); I != m_outstreams.end(); ++I ){
    BUG_DEBUG("targetStreams, name: " << (*I)->Name());
    std::istringstream is( (dynamic_cast<Stream*>(*I)->hasJSONFlag() ? "{}" : "") );
    (*I)->read(is);
  }
}

/* --------------------------------------------------------------------------- */
/* setTargets --                                                               */
/* --------------------------------------------------------------------------- */

void Process::setTargets(bool autoClear){
  InputStreamList::iterator iiter;
  OutputStreamList::iterator oiter;
  BUG( BugOperator, "Process::setTargets" );

  for( iiter=m_instreams.begin(); iiter!=m_instreams.end(); ++iiter ){
    for( oiter=m_outstreams.begin(); oiter!=m_outstreams.end(); ++oiter ){
      (*iiter)->addTargetStream( (*oiter), autoClear );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getNumInputParameters --                                                    */
/* --------------------------------------------------------------------------- */

size_t Process::getNumInputParameters() const {
  std::vector<Stream *>::const_iterator iiter;
  size_t num = 0;
  for( iiter=m_instreams.begin(); iiter != m_instreams.end(); ++iiter ){
    num += (*iiter)->size();
  }
  return num;
}

/* --------------------------------------------------------------------------- */
/* workFailed --                                                               */
/* --------------------------------------------------------------------------- */

void Process::workFailed(){
  BUG( BugOperator, "Process::workFailed" );
  if (getErrorMessage().size() == 0) {
    BUG_EXIT("empty message, iam Silent");
    return;
  }
  // truncate error message, get last error message
  std::size_t epos = 0;
  std::string msg(getErrorMessage());
  if (msg.size()) {
    epos = msg.rfind("Error");
    if (epos == std::string::npos)
      epos = msg.rfind("ERROR");
    if (epos != std::string::npos) {
      if (epos > 50) {
        epos = msg.rfind("\n", epos);
      } else {
        epos = 0;
      }
    }
  }
  GuiFactory::Instance() -> showDialogWarning
    ( 0
      , _("Error")
      , compose(_("Runtime error in '%2': %1\n(For details see log window)"),
                msg.substr(epos), m_name)
      , 0 );
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void Process::serializeXML(std::ostream &os, bool recursive){
  os << "<process name=\"" << m_name << "\">" << std::endl;
  if( recursive ){
  }
  os << "</process" <<  std::endl;
}
