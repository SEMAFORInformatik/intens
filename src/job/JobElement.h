
#if !defined(JOB_ELEMENT_INCLUDED_H)
#define JOB_ELEMENT_INCLUDED_H

#include "gui/GuiElement.h"
#include <vector>

class JobCodeItem;

/** Die JobElement-Klasse bildet die Basis aller Job-Klassen. Sie beinhaltet alle
    Definitionen, welche innerhalb des Job-Handlings von Bedeutung sind.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: JobElement.h,v 1.11 2004/12/14 11:22:18 ked Exp $
*/
class JobElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobElement(){}
  virtual ~JobElement(){}

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  // Code Item Type
  enum CodeType
  { code_Operation
  , code_Integer
  , code_Real
  , code_String
  };

  // Stack Item Type
  enum StackType
  { stck_Data
    //  , stck_Integer
    //  , stck_Real
    //  , stck_String
  , stck_Process
  , stck_ReturnControl
  , stck_Address
  };

  // Operation Status
  enum OpStatus
  { op_Ok
  , op_Branch
  , op_Interrupt
  , op_Wait
  , op_Pause
  , op_EndOfJob
  , op_Warning
  , op_Aborted
  , op_Canceled
  , op_FatalError
  , op_NotImplemented
  };

  // Call Reason
  enum CallReason
  { cll_Input = 1
  , cll_Insert
  , cll_Dupl
  , cll_Clear
  , cll_Delete
  , cll_Pack
  , cll_Select
  , cll_Unselect
  , cll_SelectPoint
  , cll_SelectRectangle
  , cll_Sort
  , cll_Activate
  , cll_Open
  , cll_Close
  , cll_Drop
  , cll_Move
  , cll_NewConnection
  , cll_RemoveConnection
  , cll_RemoveElement
  , cll_CycleClear
  , cll_CycleDelete
  , cll_CycleNew
  , cll_CycleRename
  , cll_CycleSwitch
  , cll_FocusIn
  , cll_FocusOut
  , cll_Function
  , cll_Task
  , cll_GuiUpdate
  , cll_CallReasonEntries
  };

  typedef std::vector<JobCodeItem *> JobCodeStream;

/*=============================================================================*/
/* public Interfaces                                                           */
/*=============================================================================*/
public:
  //  TransactionOwner *transactionOwner(){ assert(false); return 0; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

};

#endif
