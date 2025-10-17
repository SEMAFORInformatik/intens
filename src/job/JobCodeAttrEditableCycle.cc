
#include "job/JobIncludes.h"

#include "job/JobCodeAttrEditableCycle.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"
#include "app/DataPoolIntens.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrEditableCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrEditableCycle::execute");

  // Enable/Disable Flag 1/0
  JobStackDataPtr enable( eng->pop() );
  if( enable.isnt_valid() ) return op_FatalError;

  // Cycle Index
  JobStackDataPtr index( eng->pop() );
  if( index.isnt_valid() ) return op_FatalError;

  OpStatus status = op_Ok;

  int inx = 0;
  if( index->getIntegerValue( inx ) ){
    DataItem *cycleitem = DataPoolIntens::getDataPool().GetRootItem( inx-1 );
    if( cycleitem == 0 ){
      /* Der gewünschte Cycle existiert nicht. */
    }
    else{
      DataItemAttr *attr = cycleitem->getDataItemAttr( true );
      assert( attr != 0 );
      int make_editable = 0;
      if( enable->getIntegerValue( make_editable ) ){
	if( make_editable ){
	  cycleitem->setAttributes( 0, DATAIsReadOnly );
	}
	else{
	  cycleitem->setAttributes( DATAIsReadOnly, 0 );
	}
	DATAAttributeMask mask = cycleitem->getAttributes( 0 );
      }
      else{
	/* Das Flag für Enable/Disable ist ungültig */
	status = op_FatalError;
      }
    }
  }

  BUG_EXIT("Status = " << status );
  return status;
}
