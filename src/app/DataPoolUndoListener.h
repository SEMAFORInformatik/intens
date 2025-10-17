
#if !defined(_DATAPOOL_UNDO_LISTENER_INCLUDED_H_)
#define _DATAPOOL_UNDO_LISTENER_INCLUDED_H_

#include <string>
#include <map>

#include "datapool/DataTTrailUndoListener.h"
#include "gui/GuiMenuButtonListener.h"

class JobAction;

class DataPoolUndoListener : public DataTTrailUndoListener
                           , public GuiMenuButtonListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DataPoolUndoListener( bool undo );
  virtual ~DataPoolUndoListener();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void undoStatus( bool enabled );
  virtual void redoStatus( bool enabled );
  virtual void ButtonPressed();
  virtual JobAction *getAction(){ return 0; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  bool m_is_undo_listener;
};

#endif
