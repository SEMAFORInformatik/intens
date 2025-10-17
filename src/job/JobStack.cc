
#include "utils/Debugger.h"
#include "job/JobIncludes.h"
#include "job/JobStackItem.h"
#include "job/JobStack.h"
#include "job/JobStackDataReal.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobStack::JobStack(){
}

JobStack::~JobStack(){
  clear();
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* push --                                                                     */
/* --------------------------------------------------------------------------- */

void JobStack::push( const JobStackItemPtr &item ){
  BUG_DEBUG( "JobStack::push: " << item );
  m_stack.push( item );
}

/* --------------------------------------------------------------------------- */
/* get --                                                                      */
/* --------------------------------------------------------------------------- */

const JobStackItemPtr JobStack::get(){
  BUG_DEBUG( "JobStack::get" );
  if( size() == 0 ){
    BUG_DEBUG( "Stack is empty" );
    return 0;
  }
  JobStackItemPtr item = m_stack.top();
  BUG_DEBUG( "top Item '" << item << "'" );
  return item;
}

/* --------------------------------------------------------------------------- */
/* pop --                                                                      */
/* --------------------------------------------------------------------------- */

const JobStackItemPtr JobStack::pop(){
  BUG_DEBUG( "JobStack::pop" );
  if( size() == 0 ){
    BUG_DEBUG( "Stack is empty" );
    return 0;
  }
  JobStackItemPtr item = m_stack.top();
  m_stack.pop();
  BUG_DEBUG( "pop Item '" << item << "'" );
  return item;
}

/* --------------------------------------------------------------------------- */
/* size --                                                                     */
/* --------------------------------------------------------------------------- */

int JobStack::size(){
  return (int)m_stack.size();
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStack::print( std::ostream &ostr ){
  ostr << "=== Stack size: " << size() << " ===" << std::endl;
  if( size() > 0 ){
    std::cout << m_stack.top() << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStack::clear(){
  BUG_DEBUG( "JobStack::clear" << size() << " elements");

  while( size() > 0 ){
    m_stack.pop();
  }
}
