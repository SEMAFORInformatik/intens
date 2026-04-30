
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(QT_DIALOGWARNING_H)
#define QT_DIALOGWARNING_H

#include "gui/qt/QtDialogInformation.h"

class QtDialogWarning : public QtDialogInformation
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  QtDialogWarning( InformationListener *listener );
  virtual ~QtDialogWarning() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static bool showDialog( GuiElement *, const std::string &, const std::string &
                        , InformationListener * );

};

#endif