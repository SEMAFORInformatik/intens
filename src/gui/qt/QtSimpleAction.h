
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


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