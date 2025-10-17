
#include "utils/Debugger.h"
#include "job/JobIncludes.h"
#include "job/JobStackItem.h"
#include "job/JobStackAddress.h"
#include "job/JobStackReturn.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobStackItem::JobStackItem(){
#ifdef _DEBUG
  s_item_count++;
#endif
}

JobStackItem::~JobStackItem(){
#ifdef _DEBUG
  s_item_count--;
#endif
}

/*=============================================================================*/
/* public Functions of JobStackItem                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getItemCount --                                                             */
/* --------------------------------------------------------------------------- */

int JobStackItem::getItemCount(){
#ifdef _DEBUG
  return s_item_count;
#else
  return 0;
#endif
}

/*=============================================================================*/
/* protected Functions of JobStackItem                                         */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* address --                                                                  */
/* --------------------------------------------------------------------------- */

JobStackAddress &JobStackItem::address(){
  assert( false );
  // the following dummy code is never executed (because of above assert)
  // but it avoids the compiler warning: no return statement
  JobStackAddress *addr = new JobStackAddress(0);
  return *addr;
}

/* --------------------------------------------------------------------------- */
/* returncontrol --                                                            */
/* --------------------------------------------------------------------------- */

JobStackReturn &JobStackItem::returncontrol(){
  assert( false );
  // the following dummy code is never executed (because of above assert)
  // but it avoids the compiler warning: no return statement
  JobStackReturn *ret = new JobStackReturn(0, 0, "");
  return *ret;
}


/*=============================================================================*/
/* Constructor / Destructor of JobStackitemPtr                                 */
/*=============================================================================*/

JobStackItemPtr::JobStackItemPtr(){}

JobStackItemPtr::JobStackItemPtr( JobStackItem *p )
  : Pointer( p ){
}

JobStackItemPtr::JobStackItemPtr( const JobStackItemPtr &ptr )
  : Pointer( ptr ){
}

/*=============================================================================*/
/* public Functions of JobStackitemPtr                                         */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* address --                                                                  */
/* --------------------------------------------------------------------------- */

JobStackAddress &JobStackItemPtr::address(){
  return m_data->address();
}

/* --------------------------------------------------------------------------- */
/* returncontrol --                                                            */
/* --------------------------------------------------------------------------- */

JobStackReturn &JobStackItemPtr::returncontrol(){
  return m_data->returncontrol();
}
