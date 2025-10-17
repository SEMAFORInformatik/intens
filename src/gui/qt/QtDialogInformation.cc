
// #include <qapplication.h>
// #include <qmessagebox.h>
// #include <QGridLayout>
// #include <QTextEdit>
// #include <QScrollBar>
// #include <QDesktopWidget>

#include "gui/qt/QtDialogInformation.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtMessageBox.h"
#include "gui/qt/QtMessageBoxExt.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtDialogProgressBar.h"
#include "gui/GuiEventData.h"
#include "gui/GuiManager.h"
#include "utils/gettext.h"
#include "app/AppData.h"

/*=============================================================================*/
/* Constructor                                                                 */
/*=============================================================================*/

QtDialogInformation::QtDialogInformation( InformationListener *listener ) {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* showDialog --                                                               */
/* --------------------------------------------------------------------------- */
#include "app/QuitApplication.h"
bool QtDialogInformation::showDialog( GuiElement *element
                                    , const std::string &title
                                    , const std::string &message
                            	    , InformationListener *listener ){

  BUG_PARA( BugGui, "QtDialogInformation::showDialog message: ", message );
  bool result;
  std::string mytitle(/*AppData::Instance().Title() +  ": "  +*/ title);
  bool grabButton( false );
  GuiEventLoopListener *loopcontrol = new GuiEventLoopListener( grabButton );
  GuiManager::Instance().attachEventLoopListener( loopcontrol );

  // if test mode => exit
  if (AppData::Instance().hasTestModeFunc()) {
    std::cerr << "=== TestMode MessageBox\n"
	      << "=== title: '"<<title<<"'\n"
	      << "=== message: '"<<message<<"' "
	      << std::endl << std::flush;
    AppData::Instance().setTestModeExitCode( AppData::messageExitCode );

    // silently
    loopcontrol->goAway();
    if( listener ){
      listener->informOkButtonPressed();
    }
    return true;
  }

  // create messagebox
  if (message.size() > 500) {
    std::map<GuiElement::ButtonType, std::string> buttonText;
    QtMessageBoxExt mb( QtMultiFontString::getQString(mytitle),
			QString::fromStdString(message),
			"dialog-information",
			QMessageBox::Ok|QMessageBox::NoButton|QMessageBox::NoButton,
			buttonText,
			QtMessageBox::getParentWidget(element)
			);
    result = mb.exec() == QMessageBox::Yes;
  }else {
    QtMessageBox mb( QtMultiFontString::getQString(mytitle),
		     QString::fromStdString(message),
		     QMessageBox::Information,
		     QMessageBox::Ok|QMessageBox::NoButton|QMessageBox::NoButton,
		     QtMessageBox::getParentWidget(element)
		     );

    result = mb.exec() == QMessageBox::Yes;
  }

  loopcontrol->goAway();

  if( listener ){
    listener->informOkButtonPressed();
  }

  return result;
}
