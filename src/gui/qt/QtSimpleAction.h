
#ifndef GUI_QT_SIMPLE_ACTION_H
#define GUI_QT_SIMPLE_ACTION_H

#include <qtimer.h>
#include "gui/SimpleAction.h"


class QtSimpleAction : public QTimer, public SimpleAction
{
Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  QtSimpleAction( SimpleAction::Object *obj );
  virtual ~QtSimpleAction(){}

/*=============================================================================*/
/* Slots                                                                       */
/*=============================================================================*/
private slots:
  void workproc();
};

#endif
