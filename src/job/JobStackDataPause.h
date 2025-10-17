
#if !defined(JOB_STACK_DATA_PAUSE_INCLUDED_H)
#define JOB_STACK_DATA_PAUSE_INCLUDED_H

#include "job/JobStackDataInteger.h"

class JobStackDataPause : public JobStackDataInteger
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackDataPause( int value )
    : JobStackDataInteger( value ){
  }
  virtual ~JobStackDataPause(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Die Funktion meldet, ob es sich um ein Pausenzeichen handelt.
      \retval false Ein normales StackItem
      \retval true  Ein Pausenzeichen
   */
  virtual bool isPause() { return true; }

};

#endif
