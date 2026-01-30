
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobFunction.h"
#include "job/JobStackReturn.h"
#include "job/JobStackAddress.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataString.h"
#include "utils/Date.h"

INIT_LOGGER();

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

int JobEngine::s_id_counter = 0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobEngine::JobEngine( JobFunction *function )
  : m_id( ++s_id_counter )
  , m_function( function )
  , m_code( 0 )
  , m_current_addr( 0 )
  , m_error( false )
  , m_messagestream( 0 ){
  BUG_DEBUG("Begin of FUNC "<< function->Name());
  pushReturnControl( function->Name() );
  m_code = function->getCodeStream();
}
JobEngine::~JobEngine(){
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* get_id --                                                                   */
/* --------------------------------------------------------------------------- */

int JobEngine::get_id() const{
  return m_id;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobEngine::execute(){
  BUG_DEBUG("JobEngine::execute");
  assert( m_code != 0 );
  int exe_counter = 0;

  OpStatus status = op_Ok;

  while( status == op_Ok ){
    assert( (*m_code)[m_current_addr] != 0); //!!!!!
    status = (*m_code)[m_current_addr]->execute( this );
    BUG_DEBUG( "Status of execute() is " << status );
    exe_counter++;
    switch( status ){
    case op_Branch: // Die Code Adresse wurde bereits neu gesetzt. (GoTo)
      status = op_Ok;
      break;

    case op_Ok:
      // Nach 50 execute()-Aufrufen wird die Arbeit kurz unterbrochen, damit
      // Intens ein paar Dinge erledigen kann (z.B. GUI-Updates), bevor es
      // mit dem nächsten OpCode weitergeht.
      if( exe_counter > 50 ){
        status = op_Interrupt;
      }
      else{
        m_current_addr++;
      }
      break;

    case op_Pause:
      // Verlangt ein OpCode eine Pause, wird auch ein Interrupt ausgelöst.
      // Jedoch wird bei Wiederaufnahme der Arbeit dersselbe Opcode wieder
      // aufgerufen.
      status = op_Interrupt;
      m_current_addr--;
      break;

    default:
      break;
    }
  }

  // Warum wurde die Arbeit unterbrochen ??
  switch( status ){
  case op_Interrupt:
  case op_Warning:
  case op_Wait:
    // Normale Verschnaufpause. Wir geben dem Aufrufer die Moeglichkeit
    // zwischendurch etwas anderes zu tun.
    m_current_addr++;
    break;

  case op_EndOfJob:
    BUG_DEBUG( "EndOfJob" );
    m_code = 0; // Es gibt kein zurueck.
    break;

  case op_Aborted:
    BUG_DEBUG( "Aborted" );
    m_code = 0; // Es gibt kein zurueck.
    break;

  case op_FatalError:
    BUG_DEBUG( "FatalError" );
    m_code = 0; // Es gibt kein zurueck.
    break;

  default:
    assert(false);
  }

  BUG_DEBUG("End of execute: status = " << status);
  return status;
}

/* --------------------------------------------------------------------------- */
/* cancel --                                                                   */
/* --------------------------------------------------------------------------- */

void JobEngine::cancel(){
  if( (*m_code)[m_current_addr] != 0){
    (*m_code)[m_current_addr]->cancel( this );
  }
}

/* --------------------------------------------------------------------------- */
/* restoreCode --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobEngine::restoreCode( JobStackReturn &ret ){
  std::string name;
  OpStatus status = ret.getCodeStream( m_code, m_current_addr, name );
  int msec = ret.getStartTime().msecsTo(QTime::currentTime());
  if (msec < 1000){
    BUG_DEBUG("End of FUNC " << name << Date::durationAsString(msec));
  }else{
    BUG_INFO("End of FUNC " << name << Date::durationAsString(msec));
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* push --                                                                     */
/* --------------------------------------------------------------------------- */

void JobEngine::push( const JobStackItemPtr &item ){
  m_stack.push( item );
}

/* --------------------------------------------------------------------------- */
/* pushData --                                                                 */
/* --------------------------------------------------------------------------- */

void JobEngine::pushData( const JobStackDataPtr &data ){
  m_stack.push( data.itemPtr() );
}

/* --------------------------------------------------------------------------- */
/* get --                                                                      */
/* --------------------------------------------------------------------------- */

const JobStackItemPtr JobEngine::get(){
  return m_stack.get();
}

/* --------------------------------------------------------------------------- */
/* pop --                                                                      */
/* --------------------------------------------------------------------------- */

const JobStackItemPtr JobEngine::pop(){
  return m_stack.pop();
}

/* --------------------------------------------------------------------------- */
/* sizeOfStack --                                                              */
/* --------------------------------------------------------------------------- */

int JobEngine::sizeOfStack(){
  return m_stack.size();
}

/* --------------------------------------------------------------------------- */
/* pushTrue --                                                                 */
/* --------------------------------------------------------------------------- */

void JobEngine::pushTrue( bool yes ){
  if( yes ){
    m_stack.push( new JobStackDataInteger( 1 ) );
  }
  else{
    m_stack.push( new JobStackDataInteger( 0 ) );
  }
}

/* --------------------------------------------------------------------------- */
/* pushAddress --                                                              */
/* --------------------------------------------------------------------------- */

void JobEngine::pushAddress( int addr ){
  m_stack.push( new JobStackAddress( addr ) );
}

/* --------------------------------------------------------------------------- */
/* pushReason --                                                               */
/* --------------------------------------------------------------------------- */

void JobEngine::pushReason( CallReason reason ){
  pushTrue( m_function->reasonIsEqual( reason ) );
}

/* --------------------------------------------------------------------------- */
/* pushInvalid --                                                              */
/* --------------------------------------------------------------------------- */

void JobEngine::pushInvalid(){
  m_stack.push( new JobStackDataInteger() );
}

/* --------------------------------------------------------------------------- */
/* callFunction --                                                             */
/* --------------------------------------------------------------------------- */

void JobEngine::callFunction( JobFunction *func ){
  assert( !func->notImplemented() );

  pushReturnControl( func->Name() );
  BUG_INFO("Begin of FUNC "<< func->Name());
  m_code = func->getCodeStream();
  m_current_addr = 0;
}

/* --------------------------------------------------------------------------- */
/* pushReturnControl --                                                        */
/* --------------------------------------------------------------------------- */

void JobEngine::pushReturnControl( const std::string &name ){
  BUG_DEBUG("PushReturnControl Function Addr: " << m_current_addr);
  push( new JobStackReturn( m_code, m_current_addr, name ) );
}

/* --------------------------------------------------------------------------- */
/* popAddress --                                                               */
/* --------------------------------------------------------------------------- */

int JobEngine::popAddress(){
  JobStackItemPtr addr( pop() );
  assert( addr.is_valid() );
  assert( addr->Type() == stck_Address );
  return addr.address().getAddress();
}

/* --------------------------------------------------------------------------- */
/* pushIndex --                                                                */
/* --------------------------------------------------------------------------- */

void JobEngine::pushIndex( int level, int index ){
  int value = 0;
  if( m_function->getIndex( value, level, index ) ){
    m_stack.push( new JobStackDataInteger( value ) );
  }
  else{
    pushInvalid();
  }
}

/* --------------------------------------------------------------------------- */
/* pushIndexNumber --                                                          */
/* --------------------------------------------------------------------------- */

void JobEngine::pushIndexNumber(){
  int value = 0;
  if( m_function->getIndex( value ) ){
    m_stack.push( new JobStackDataInteger( value ) );
  }
  else{
    pushInvalid();
  }
}

/* --------------------------------------------------------------------------- */
/* pushDiagramXPos                                                             */
/* --------------------------------------------------------------------------- */

void JobEngine::pushDiagramXPos(){
  double value = 0;
  if( m_function->getDiagramXPos( value ) ){
    m_stack.push( new JobStackDataReal( value ) );
  }
  else{
    pushInvalid();
  }
}

/* --------------------------------------------------------------------------- */
/* pushDiagramYPos --                                                          */
/* --------------------------------------------------------------------------- */

void JobEngine::pushDiagramYPos(){
  double value = 0;
  if( m_function->getDiagramYPos( value ) ){
    m_stack.push( new JobStackDataReal( value ) );
  }
  else{
    pushInvalid();
  }
}

/* --------------------------------------------------------------------------- */
/* pushSortCriteria--                                                          */
/* --------------------------------------------------------------------------- */

void JobEngine::pushSortCriteria(){
  std::string value;
  if( m_function->getSortCriteria( value ) ){
    m_stack.push( new JobStackDataString( value ) );
  }
  else{
    pushInvalid();
  }
}

/* --------------------------------------------------------------------------- */
/* MessageStream --                                                            */
/* --------------------------------------------------------------------------- */

std::ostream &JobEngine::MessageStream(){
  if( m_messagestream == 0 ){
    m_messagestream = new std::ostringstream();
  }
  return *m_messagestream;
}

/* --------------------------------------------------------------------------- */
/* setMessage --                                                               */
/* --------------------------------------------------------------------------- */

void JobEngine::setMessage(){
  m_message = m_messagestream->str();
  delete m_messagestream;
  m_messagestream = 0;
}

/* --------------------------------------------------------------------------- */
/* ownerId --                                                                  */
/* --------------------------------------------------------------------------- */

std::string JobEngine::ownerId() const{
  return "JobEngine";
}
