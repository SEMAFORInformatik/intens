#include <sstream>
#include <QThread>
#include <QApplication>

/* Local headers */
#include "streamer/BasicStream.h"
#include "datapool/DataVector.h"
#include "datapool/DataIntegerValue.h"
#include "utils/Debugger.h"
#include "utils/Date.h"
#include "utils/FileUtilities.h"
#include "operator/BatchProcess.h"
#include "operator/ProcessWrapper.h"
#include "app/DataPoolIntens.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiFactory.h"

INIT_LOGGER();

std::set<std::string> BasicStream::s_filenames;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

BasicStream::BasicStream( const std::string &name )
  : m_name( name )
  , m_process( 0 )
  , m_appendOption( false )
  , m_noGzOption( false )
  , m_preProcessInChannel( &m_preProcessInChannelListener, "", 0 )
  , m_preProcessOutChannel( &m_preProcessOutChannelListener )
  , m_textwindow( 0 )
  , m_data_buffer( 0 )
  , m_data_valid( 0 )
  , m_data_source_valid( 0 )
  , m_data_filename( 0 )
  , m_items_are_cleared( false )
  , m_targets_are_cleared( false )
  , m_timestamp(0){
}

BasicStream::~BasicStream() {
}

/* --------------------------------------------------------------------------- */
/* installData --                                                              */
/* --------------------------------------------------------------------------- */

void BasicStream::installData(){
  assert( !m_name.empty() );
  std::string varname = "__stream_data_" + m_name;
  DataPoolIntens::getDataPool().AddToDictionary( "", varname
                                               , DataDictionary::type_StructVariable
                                               , INTERNAL_STREAM_STRUCT );

  m_data_buffer = DataPoolIntens::getDataReference( varname + ".Buffer" );
  assert( m_data_buffer != 0 );

  m_data_valid = DataPoolIntens::getDataReference( varname + ".ItemsAreValid" );
  assert( m_data_valid != 0 );
  m_data_source_valid = DataPoolIntens::getDataReference( varname + ".SourceItemsAreValid" );
  assert( m_data_source_valid != 0 );

  m_data_filename = DataPoolIntens::getDataReference( varname + ".Filename" );
  assert( m_data_filename != 0 );
}

/* --------------------------------------------------------------------------- */
/* clearTextWindow --                                                          */
/* --------------------------------------------------------------------------- */

void BasicStream::clearTextWindow(){
  if( m_textwindow != 0 ){
    m_textwindow->clearText();
  }
}

/* --------------------------------------------------------------------------- */
/* addTargetStream --                                                          */
/* --------------------------------------------------------------------------- */

void BasicStream::addTargetStream( BasicStream *s, bool autoClear ){
  assert( s!=0 );
  TargetStreamMap::iterator it;
  for( it = m_targetstreams.begin(); it != m_targetstreams.end(); ++it ){
    if( (*it).first == s ){
      if ( (*it).second && !autoClear)  // autoClear flag FALSE is dominant
        m_targetstreams[s] = false;
      return; // exists already
    }
  }
  BUG_DEBUG("Stream '" << Name() << "' Add TargetStream '" << s->Name() <<"'");
  m_targetstreams.insert ( std::pair<BasicStream *, bool>(s, autoClear) );
  //  m_targetstreams.push_back( s );

  // mich selbst im target stream als source stream hinzufügen
  TargetStreamList::iterator its = std::find(s->m_sourcestreams.begin(), s->m_sourcestreams.end(), this);
  if (its == s->m_sourcestreams.end())
    s->m_sourcestreams.push_back(this);
}

/* --------------------------------------------------------------------------- */
/* addItemStreams --                                                           */
/* --------------------------------------------------------------------------- */

void BasicStream::addItemStreams( TargetStreamList &streamlist ){
  TargetStreamList::iterator it;
  for( it = streamlist.begin(); it != streamlist.end(); ++it ){
    addItemStream( (*it) );
  }
}

/* --------------------------------------------------------------------------- */
/* addItemStream --                                                            */
/* --------------------------------------------------------------------------- */

void BasicStream::addItemStream( BasicStream *s ){
  assert( s!=0 );
  if( s == this ) return;

  TargetStreamList::iterator it;
  for( it = m_itemstreams.begin(); it != m_itemstreams.end(); ++it ){
    if( (*it) == s ){
      return; // exists already
    }
  }
  m_itemstreams.push_back( s );
}

/* --------------------------------------------------------------------------- */
/* checkAsTargetStream --                                                      */
/* --------------------------------------------------------------------------- */

bool BasicStream::checkAsTargetStream( TransactionNumber timestamp ){
  BUG_DEBUG("checkAsTargetStream, name: " << m_name);

  if( checkItemStreams( timestamp ) ){
    // Dieses Feature ist neu. Bis zum 19.11.2002 haben wir die Targetstreams
    // der Itemstreams nicht kontrolliert, jedoch bei Bedarf gelöscht.
    return true;
  }
  return checkTargets( timestamp );
}

/* --------------------------------------------------------------------------- */
/* clearAsTargetStream --                                                      */
/* --------------------------------------------------------------------------- */

void BasicStream::clearAsTargetStream( TransactionNumber timestamp ){
  BUG_DEBUG("clearAsTargetStream, name: " << m_name);
  clearItemsAndTargets( timestamp );
}

/* --------------------------------------------------------------------------- */
/* checkTargetStreams --                                                       */
/* --------------------------------------------------------------------------- */

bool BasicStream::checkTargetStreams( TransactionNumber timestamp ){
  BUG_DEBUG("checkTargetStreams, name: " << m_name );
  if (!DataItemsAreValid(false)) {  // test source stream
    BUG_DEBUG(compose(_("unused target, input stream named '%1' is not valid (%2)\n"), Name()));
    return false;
  }
  getErrorMessage(); // clear this message from input stream
  // Diese Funktion prüft, ob irgendwelche TargetStreams dieses Streams
  // gültig sind. Ob die eigenen Werte gültig sind, interessiert uns nicht.

  return checkTargets( timestamp );
}

/* --------------------------------------------------------------------------- */
/* checkItemsAndTargets --                                                     */
/* --------------------------------------------------------------------------- */

bool BasicStream::checkItemsAndTargets( TransactionNumber timestamp ){
  BUG_DEBUG("BasicStream::checkItemsAndTargets, name: " << m_name);
  // Diese Funktion prüft, ob die Items des Streams gültig sind sowie die
  // eigenen TargetStreams. Die TargetStreams sind nur interessant, wenn die
  // Items gültig sind.

  if( m_timestamp == timestamp ){
    // Dieser Stream wurde bereits überprüft. Wir brechen ab.
    BUG_DEBUG("already checked => false");
    return false;
  }
  m_timestamp = timestamp;

  if( checkItemStreams( timestamp ) ){
    // Dieses Feature ist neu. Bis zum 19.11.2002 haben wir die Targetstreams
    // der Itemstreams nicht kontrolliert, jedoch bei Bedarf gelöscht.
    return true;
  }

  if( DataItemsAreValid() ){
    // Die Data-Items dieses Streams sind gültig. Wir brechen die Suche ab.
    BUG_DEBUG("DataItems of '" << Name() << "' are valid => true");
    return true;
  }
  return checkTargets( timestamp );
}

/* --------------------------------------------------------------------------- */
/* checkTargets --                                                             */
/* --------------------------------------------------------------------------- */

bool BasicStream::checkTargets( TransactionNumber timestamp ){
  BUG_DEBUG("BasicStream::checkTargets, name: " << m_name);
  TargetStreamMap::iterator it;
  bool ret(false);
  for( it = m_targetstreams.begin(); it != m_targetstreams.end(); ++it ){
    if( (*it).first->checkItemsAndTargets( timestamp ) ){
      // Ist die TransactionNumber des targetStream in der TransactionNumberList
      // unseres Input Streams NICHT enthalten, wird dieser hier ignoriert.

      // Vergleich der TransactionNumber zwischen Target und Source Stream
      std::vector<int> ownTransIds;
      getLastTransactionNumberFromSource(ownTransIds);
      std::ostringstream ostr;
      for (const auto& i: ownTransIds) ostr << i << ' ';
      BUG_DEBUG("TransactionNum TargetStream[" << (*it).first->getLastTransactionNumber()
                << "][" << (*it).first->Name() << "] > Source["
                << ostr.str() << "][" << Name() << "]");
      int targetTransId = (*it).first->getLastTransactionNumber();
      BUG_DEBUG("Try to find my transId: " << targetTransId);
      if (ownTransIds.size() == 0 || targetTransId == 0 ||
          std::find(ownTransIds.begin(),ownTransIds.end(), targetTransId)  == std::end(ownTransIds)) {
        getErrorMessage(); // clear Message
        BUG_DEBUG("  Not found, continue");
        continue;
      }
      if ((*it).second) {
        BUG_DEBUG("CLEAR automatically targetStream  '"<<(*it).first->Name()<<"' !!!");
        TransactionNumber timestamp = DataPoolIntens::CurrentTransaction();
        (*it).first->clearAsTargetStream( timestamp );
        (*it).first->getErrorMessage();   // clear
        continue;
      }
      BUG_INFO("checkTargets (ConsistencyCheck) Stream '"<<Name()<<"', CLEAR targetStream '"<<(*it).first->Name()<<"' !!!");
      ret = true;
    }
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/* clearTargetStreams --                                                       */
/* --------------------------------------------------------------------------- */

void BasicStream::clearTargetStreams( TransactionNumber timestamp ){
  BUG_DEBUG("BasicStream::clearTargetStreams, name: " << m_name
            << ",  timestampIn: " << timestamp);
  clearTargets(timestamp);
  setDataItemsInvalid(false);  // clear input stream timestamps
}

/* --------------------------------------------------------------------------- */
/* checkItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

bool BasicStream::checkItemStreams( TransactionNumber timestamp ){
  BUG_DEBUG("BasicStream::checkItemStreams, name: " << m_name);
  TargetStreamList::iterator it;
  for( it = m_itemstreams.begin(); it != m_itemstreams.end(); ++it ){
    if( (*it)->checkTargets( timestamp ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* clearItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void BasicStream::clearItemStreams( TransactionNumber timestamp ){
  BUG_DEBUG("BasicStream::clearItemStreams, name: " << m_name);
  TargetStreamList::iterator it;
  for( it = m_itemstreams.begin(); it != m_itemstreams.end(); ++it ){
    (*it)->clearTargets( timestamp );
  }
}

/* --------------------------------------------------------------------------- */
/* clearItemsAndTargets --                                                     */
/* --------------------------------------------------------------------------- */

void BasicStream::clearItemsAndTargets( TransactionNumber timestamp ){
  BUG_DEBUG("BasicStream::clearItemsAndTargets, name: " << m_name);
  if( m_items_are_cleared != timestamp ||
      (m_items_are_cleared == 0 && timestamp == 0)){
    m_items_are_cleared = timestamp;
    clearRange();
    clearItemStreams( timestamp );
    setDataItemsInvalid();
  }
  else{
    BUG_DEBUG( "Items already cleared");
  }
  if( m_targets_are_cleared != timestamp ){
    m_targets_are_cleared = timestamp;
    clearTargets( timestamp );
  }
}

/* --------------------------------------------------------------------------- */
/* clearTargets --                                                             */
/* --------------------------------------------------------------------------- */

void BasicStream::clearTargets(TransactionNumber timestamp){
  std::vector<int> ownTransIds;
  getLastTransactionNumberFromSource(ownTransIds);
  std::ostringstream ostr;
  for (const auto& i: ownTransIds) ostr << i << ' ';
  BUG_DEBUG("clearTargets, name: " << m_name << ", : TransactionNumber own: " << ostr.str());

  TargetStreamMap::iterator it;
  for( it = m_targetstreams.begin(); it != m_targetstreams.end(); ++it ){
    if (std::find(ownTransIds.begin(),ownTransIds.end(), timestamp)  != std::end(ownTransIds)) {
      BUG_DEBUG("clearTargets, ignore outputstram name: " << (*it).first->Name() <<
                ", TransactionNumber last[" << (*it).first->getLastTransactionNumber() << "] != actual[" << timestamp << "]");
      continue;
    }
    BUG_DEBUG("clearTargets, outputstram name: " << (*it).first->Name() <<
              ", TransactionNumber last[" << (*it).first->getLastTransactionNumber() << "] == actual[" << timestamp << "]");
    (*it).first->clearItemsAndTargets( timestamp );
  }
}

/* --------------------------------------------------------------------------- */
/* printTargetStreams --                                                       */
/* --------------------------------------------------------------------------- */

void BasicStream::printTargetStreams(std::ostream& os){
  TargetStreamMap::iterator it;
  for( it = m_targetstreams.begin(); it != m_targetstreams.end(); ++it ){
    os << "    - Targetstream: " << (*it).first->Name()
       << " transActionNr[" << (*it).first->getLastTransactionNumber() << "]"
       << std::endl;
    (*it).first->printSourceStreams(os);
  }
}

/* --------------------------------------------------------------------------- */
/* printSourceStreams --                                                       */
/* --------------------------------------------------------------------------- */

void BasicStream::printSourceStreams(std::ostream& os){
  std::vector<int> ownTransIds;
  getLastTransactionNumberFromSource(ownTransIds);
  std::ostringstream ostr;
  for (const auto& i: ownTransIds) ostr << i << ' ';

  TargetStreamList::iterator it;
  for( it = m_sourcestreams.begin(); it != m_sourcestreams.end(); ++it ){
    os << "       - Sourcestream: " << (*it)->Name();
    int i = (*it)->getLastTransactionNumber();
    if (i ) os << " transActionNr[" << i << "]";
    if (ostr.str().size()) os << " SourceTransActionNr[" << ostr.str() << "]";
    os  << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* printItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void BasicStream::printItemStreams(std::ostream& os){
  TargetStreamList::iterator it;
  for( it = m_itemstreams.begin(); it != m_itemstreams.end(); ++it ){
    os << "    - Itemstream: " << (*it)->Name()
       << " transActionNr[" << (*it)->getLastTransactionNumber() << "]" << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* SetProcess --                                                               */
/* --------------------------------------------------------------------------- */

void BasicStream::setProcess( const std::string &cmd  ){
  BUG_DEBUG("BasicStream::setProcess, name: " << m_name);
  m_processCmd = cmd;
}

/* --------------------------------------------------------------------------- */
/* hasProcess --                                                               */
/* --------------------------------------------------------------------------- */

bool BasicStream::hasProcess(){
  return  m_processCmd.size() ? true : false;
}

/* --------------------------------------------------------------------------- */
/* runProcess --                                                               */
/* --------------------------------------------------------------------------- */

bool BasicStream::runProcess(std::ostream &inostr, std::ostream &outstr){
  BUG_DEBUG("runProcess, name: " << m_name );
  if (m_processCmd.size()) {
    m_preProcessInChannelListener.setInStream( inostr );
    if (!m_process) {
      m_process = new BatchProcess();
#ifdef HAVE_QT
      m_preProcessInChannel.setQProcess(  m_process->getQProcess() );
#endif
      m_process->addChannel( &m_preProcessInChannel );
      m_process->addChannel( &m_preProcessOutChannel );
      m_process->setExecCmd( m_processCmd );
    }

    int rt =
    m_process->start();
    m_process->waitForFinished();
    if (m_process->getExitStatus() != 0) {
      BUG_DEBUG("Aborted");
      BUG_DEBUG( compose(_("Runtime error in '%1' stream process : \n\n%2\n\n(For details see log window)"),m_name, m_processCmd) );
      // nicht möglich:  GuiFactory::Instance() -> showDialogWarning
      // da wir hier evtl in einem Thread
      return false;
    }

    std::string& str  = m_preProcessOutChannel.getStreamBuffer();
    outstr << str;
    str.clear(); // we do this
    BUG_DEBUG( "Finished" );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getErrorMessage --                                                          */
/* --------------------------------------------------------------------------- */

std::string BasicStream::getErrorMessage(bool target) {
  if (m_process)
    return m_process->getErrorMessage();
  std::string msg;
  TargetStreamMap::iterator it;
  for( it = m_targetstreams.begin(); it != m_targetstreams.end(); ++it ){
    msg += (*it).first->getErrorMessage(true);
  }
  return msg;
}

/* --------------------------------------------------------------------------- */
/* setAppendOption --                                                          */
/* --------------------------------------------------------------------------- */

void  BasicStream::setAppendOption( bool appendOption ){
  m_appendOption = appendOption;
}

/* --------------------------------------------------------------------------- */
/* hasAppendOption --                                                          */
/* --------------------------------------------------------------------------- */

bool  BasicStream::hasAppendOption() const {
  return m_appendOption;
}

/* --------------------------------------------------------------------------- */
/* setNoGzOption --                                                            */
/* --------------------------------------------------------------------------- */

void  BasicStream::setNoGzOption( bool noGzOption ){
  m_noGzOption = noGzOption;
}

/* --------------------------------------------------------------------------- */
/* hasNoGzOption --                                                            */
/* --------------------------------------------------------------------------- */

bool  BasicStream::hasNoGzOption() const {
  return m_noGzOption;
}

/* --------------------------------------------------------------------------- */
/* getLastTransactionNumber --                                                 */
/* --------------------------------------------------------------------------- */
int BasicStream::getLastTransactionNumber(){
  int i = 0;
  if( m_data_valid != 0 )
    m_data_valid->GetValue( i );
  return i;
}

/* --------------------------------------------------------------------------- */
/* getLastTransactionNumberFromSource --                                       */
/* --------------------------------------------------------------------------- */
bool BasicStream::getLastTransactionNumberFromSource(std::vector<int>& res){
  if( m_data_source_valid != 0 ) {
    DataVector vec;
    int iVal = 0;
    int id[1] = { -1  };
    m_data_source_valid->getDataVector(vec, 1, id);
    for (DataVector::iterator it = vec.begin();it != vec.end(); ++it) {
      if ((*it)->getValue(iVal)) {
        res.push_back(iVal);
      }
    }
  }
  return res.size() > 0;
}

/* --------------------------------------------------------------------------- */
/* DataItemsAreValid --                                                        */
/* --------------------------------------------------------------------------- */

bool BasicStream::DataItemsAreValid(bool target){
  BUG_DEBUG("BasicStream::DataItemsAreValid, name: " << m_name);
  int i = 0;
  if (target) {
    if( m_data_valid != 0 )
      m_data_valid->GetValue( i );
  } else {
    if( m_data_source_valid != 0 )
      m_data_source_valid->GetValue( i );
  }

  // additional check for existing valid data
  // only return true if stream has valid data
  bool valid = hasValidData();
  BUG_DEBUG("Stream: " << Name() << ", TransactionNum: " << i << ", Valid: " << valid);
  return i >= 0 ? valid : false;
}

/* --------------------------------------------------------------------------- */
/* setDataItemsValid --                                                        */
/* --------------------------------------------------------------------------- */

void BasicStream::setDataItemsValid(bool target){
  BUG_DEBUG("BasicStream::setDataItemsValid, name: " << m_name);
  if (QThread::currentThread() != qApp->thread()) {
    // Wir sind nicht im MainThread
    // Und sollten wir hier keine Schreiboperation machen.
    // (können das leider nicht vorher unterbinden)
    BUG_DEBUG("Return, no valid setDataItemsValid call");
    return;
  }
  int n = target ? DataPoolIntens::LastSourceStreamTransaction() : DataPoolIntens::CurrentTransaction();
  BUG_DEBUG("CurrentTransaction: " << DataPoolIntens::CurrentTransaction()
            << ", use useTransaction: << " << n << ", Name:  " << Name()
            << (target ? ", TARGET, " : ", SOURCE")
            << ", m_data_valid: " << m_data_valid
            << ", m_data_source_valid: " << m_data_source_valid);
  if( target && m_data_valid != 0 ) {
    m_data_valid->SetValue( n );
  }
  if( !target && m_data_source_valid != 0 ) {
#if 1
    // set our transaction id for following target streams
    DataPoolIntens::setLastSourceStreamTransaction(n);

    DataVector vec;
    int iVal(0);
    int id[1] = { -1  };
    m_data_source_valid->getDataVector(vec, 1, id);
    vec.getValue(iVal);
    BUG_DEBUG("BasicStream::setDataItemsValid, name: " << m_name << ", ts: " << n << ", iVal:" << iVal);
    if (iVal <= 0) {
      vec.clear();   // clear
    }
    DataValue::Ptr ptr = new DataIntegerValue( n );
    vec.appendValue(ptr);
    m_data_source_valid->setDataVector( vec,  1, id);
#else
    m_data_source_valid->SetValue( n );
#endif
  }
}

/* --------------------------------------------------------------------------- */
/* setDataItemsInvalid --                                                      */
/* --------------------------------------------------------------------------- */

void BasicStream::setDataItemsInvalid(bool target){
  BUG_DEBUG("BasicStream::setDataItemsInvalid, name: " << m_name);
  if( m_data_buffer != 0 ){
    m_data_buffer->eraseAllElements();
  }
  if( target && m_data_valid != 0 ){
    m_data_valid->SetValue( -1 );
  }
  if( !target && m_data_source_valid != 0 ){
    m_data_source_valid->SetValue( -1 );
  }
  if( m_data_filename != 0 ){
    m_data_filename->SetItemInvalid();
  }
}

/* --------------------------------------------------------------------------- */
/* writeBuffer --                                                              */
/* --------------------------------------------------------------------------- */

bool BasicStream::writeBuffer( const std::string &s, int idx ){
  assert( m_data_buffer != 0);
  return m_data_buffer->SetValue( s, idx );
}

/* --------------------------------------------------------------------------- */
/* readBuffer --                                                               */
/* --------------------------------------------------------------------------- */

bool BasicStream::readBuffer( std::string &s, int idx ){
  assert( m_data_buffer != 0 );
  return m_data_buffer->GetValue( s, idx );
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool BasicStream::isDataItemUpdated( TransactionNumber timestamp ){
  assert( m_data_buffer != 0 );
  return m_data_buffer->isDataItemUpdated( DataReference::ValueUpdated, timestamp );
}

/* --------------------------------------------------------------------------- */
/* ProcessInputChannelListener::write --                                       */
/* --------------------------------------------------------------------------- */

bool BasicStream::ProcessInputChannelListener::write( InputChannelEvent& event) {
    BUG_DEBUG("ProcessInputChannelListener::write" );
    #ifdef HAVE_QT
    std::ostringstream *os = dynamic_cast<std::ostringstream*>(m_ostr);
    return event.write( *os );
    #else
    return true; // TODO
    #endif
  }

/* --------------------------------------------------------------------------- */
/* setStreamFilename --                                                        */
/* --------------------------------------------------------------------------- */

std::string BasicStream::newStreamFilename(){
  assert( m_data_filename != 0 );
  // delete old file
  std::string filename = getStreamFilename();
  if (filename.size()){
    // file can only be deleted if only one cycle exists.
    // if more cycle exists, we deleted the files at the end in quit function
    if (DataPoolIntens::Instance().numCycles() == 1){
      FileUtilities::RemoveFile(filename);
      s_filenames.erase(filename);
    }
  }
  std::string fn = std::string("/tmp/") + Name() + "_" + Date::currentIsoTime();
  m_data_filename->SetValue(fn);
  s_filenames.insert(fn);
  return fn;
}

/* --------------------------------------------------------------------------- */
/* getStreamFilename --                                                        */
/* --------------------------------------------------------------------------- */

std::string BasicStream::getStreamFilename(){
  assert( m_data_filename != 0 );
  std::string fn;
  if (m_data_filename->GetValue(fn))
    return fn;
  return "";
}

/* --------------------------------------------------------------------------- */
/* deleteFiles --                                                                */
/* --------------------------------------------------------------------------- */

void BasicStream::deleteFiles(){
  std::set<std::string>::iterator si;
  for( si = BasicStream::s_filenames.begin(); si != BasicStream::s_filenames.end(); ++si ){
    std::string filename = (*si);
    if (!filename.empty()) {
      BUG_DEBUG("Delete file: " << filename);
      FileUtilities::RemoveFile(filename);
    }
  }
}
