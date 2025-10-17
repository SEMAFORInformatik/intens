
#if !defined(_DATA_TTRAIL_UNDO_DISABLER_INCLUDED_)
#define _DATA_TTRAIL_UNDO_DISABLER_INCLUDED_

#include "datapool/DataDebugger.h"

/** Das Objekt DataTTrailUndoDisable setzt das Undo-Feature im Konstruktor
    ausser Kraft und im Destruktor wieder in Kraft, sofern das Feature überhaupt
    aktiviert ist.
 */
class DataTTrailUndoDisable
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \param doit true -> Das Undo-Feature muss im Destruktor wieder aktiviert werden.
   */
  DataTTrailUndoDisable( bool doit );

  /** Destruktor
   */
  virtual ~DataTTrailUndoDisable();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  bool   m_doit;
};

#endif
/** \file */
