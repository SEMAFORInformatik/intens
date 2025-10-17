
#include <qapplication.h>
#include <qpushbutton.h>

#include "gui/qt/GuiQtManager.h"
#include "utils/utils.h"
#include "gui/qt/QtDialogWarning.h"
#include "gui/qt/QtMessageBox.h"
#include "gui/qt/QtMessageBoxExt.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/GuiEventData.h"
#include "app/AppData.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

QtDialogWarning::QtDialogWarning( InformationListener *listener )
  : QtDialogInformation( listener ){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* showDialog --                                                               */
/* --------------------------------------------------------------------------- */

bool QtDialogWarning::showDialog( GuiElement *element
                              , const std::string &title
                              , const std::string &message
                              , InformationListener *listener ){

  std::string mytitle(AppData::Instance().Title() +  ": "  + title);

  bool grabButton( false );
  GuiEventLoopListener *loopcontrol = new GuiEventLoopListener( grabButton );
  GuiManager::Instance().attachEventLoopListener( loopcontrol );

  bool result;
  if (message.size() > 500) {
    std::map<GuiElement::ButtonType, std::string> buttonText;
    QtMessageBoxExt mb( QtMultiFontString::getQString(mytitle),
			QtMultiFontString::getQString(message),
			"dialog-warning",
			QMessageBox::Ok|QMessageBox::NoButton|QMessageBox::NoButton,
			buttonText,
			QtMessageBox::getParentWidget(element)
			);
    result =  mb.exec() == QMessageBox::Yes;
  } else {
    QtMessageBox mb( QtMultiFontString::getQString(mytitle),
		     QtMultiFontString::getQString(message),
		     QMessageBox::Warning,
		     QMessageBox::NoButton,
		     QtMessageBox::getParentWidget(element)
		     );
    QAbstractButton *ok = mb.addButton(_("OK"), QMessageBox::YesRole);
    QFont font = mb.font();
    mb.setFont( QtMultiFontString::getQFont( "@messagebox@", font ) );
    mb.exec();
    result =  mb.clickedButton() == ok;
  }

  loopcontrol->goAway();

  if( listener ){
    listener->informOkButtonPressed();
  }
  return result;
}
