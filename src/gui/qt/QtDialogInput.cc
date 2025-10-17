
#include "gui/qt/QtDialogInput.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtInputDialog.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/GuiEventData.h"
#include "gui/GuiManager.h"
#include "utils/gettext.h"
#include "app/AppData.h"

/*=============================================================================*/
/* Constructor                                                                 */
/*=============================================================================*/

QtDialogInput::QtDialogInput( ConfirmationListener *listener ) {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* showDialog --                                                               */
/* --------------------------------------------------------------------------- */
#include "gui/qt/GuiQtManager.h"
std::string QtDialogInput::getText( GuiElement *element
				    , const std::string &title
                                    , const std::string &message
                                    , const std::string &label
                            	    , ConfirmationListener *listener
				    , bool& ok){

  std::string mytitle(AppData::Instance().Title() +  ": "  + title);
  QtInputDialog inDlg(QtMultiFontString::getQString(mytitle),
		      QString::fromStdString(message),
		      QString::fromStdString(label),
		      QtInputDialog::getParentWidget(element)
		      );

  bool grabButton( false );
  GuiEventLoopListener *loopcontrol = new GuiEventLoopListener( grabButton );
  GuiManager::Instance().attachEventLoopListener( loopcontrol );

  ok = inDlg.exec() == QDialog::Accepted;

  loopcontrol->goAway();

  if( listener ){
    if (ok)
      listener->confirmYesButtonPressed();
    else
      listener->confirmNoButtonPressed();
  }

  return inDlg.textValue().toStdString();
}
