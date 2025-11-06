
#include <qapplication.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include "utils/gettext.h"
#include "app/AppData.h"
#include "gui/GuiEventData.h"
#include "gui/GuiManager.h"
#include "gui/qt/QtDialogConfirmation.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtMessageBox.h"
#include "gui/qt/QtMessageBoxExt.h"
#include "gui/qt/GuiQtElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

QtDialogConfirmation::QtDialogConfirmation( ConfirmationListener *listener ) {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* showDialog --                                                               */
/* --------------------------------------------------------------------------- */

GuiElement::ButtonType QtDialogConfirmation::showDialog( GuiElement *element
				       , const std::string &title
				       , const std::string &message
				       , ConfirmationListener *listener
				       , const std::map<GuiElement::ButtonType, std::string>& buttonText
               , bool withCancelBtn
               , int defaultBtn ) {
  std::string mytitle(AppData::Instance().Title() +  ": " + title);

  bool grabButton( false );
  GuiEventLoopListener *loopcontrol = new GuiEventLoopListener( grabButton );
  GuiManager::Instance().attachEventLoopListener( loopcontrol );

  int result;
  QAbstractButton *pushed = 0,
    *yesButton = 0,
    *saveButton = 0,
    *okButton = 0,
    *noButton = 0,
    *applyButton = 0,
    *openButton = 0,
    *cancelButton = 0,
    *abortButton = 0,
    *discardButton = 0;

  QMessageBox::StandardButton firstBtn = QMessageBox::Yes;
  QMessageBox::StandardButton secondBtn = QMessageBox::No;
  QMessageBox::StandardButton thirdBtn = withCancelBtn ? QMessageBox::Cancel : QMessageBox::NoButton;
  if (buttonText.size() > 0) {
    // first
    if (buttonText.find(GuiElement::button_Save) != buttonText.end())
      firstBtn = QMessageBox::Save;
    if (buttonText.find(GuiElement::button_Ok) != buttonText.end())
      firstBtn = QMessageBox::Ok;

    // second
    // if (buttonText.find(GuiElement::button_Discard) != buttonText.end())
    //   secondBtn = QMessageBox::Discard;
    if (buttonText.find(GuiElement::button_Apply) != buttonText.end())
      secondBtn = QMessageBox::Apply;
    else if (buttonText.find(GuiElement::button_Open) != buttonText.end())
      secondBtn = QMessageBox::Open;

    // third
    if (withCancelBtn) {
      if (buttonText.find(GuiElement::button_Cancel) != buttonText.end())
        thirdBtn = QMessageBox::Cancel;
      else if (buttonText.find(GuiElement::button_Abort) != buttonText.end())
        thirdBtn = QMessageBox::Abort;
      else if (buttonText.find(GuiElement::button_Discard) != buttonText.end())
        thirdBtn = QMessageBox::Discard;
    }
  }

  if (message.size() > 500) {
    QtMessageBoxExt mb( QtMultiFontString::getQString(mytitle),
			QtMultiFontString::getQString(message),
			"dialog-information",
			firstBtn|secondBtn|thirdBtn,
			buttonText,
			QtMessageBox::getParentWidget(element)
			);
    result =  mb.exec();

    // QMessageBox::ButtonRole => GuiElement::ButtonType
    switch(result) {
    case QMessageBox::Yes:
      return GuiElement::button_Yes;
    case QMessageBox::Save:
      return GuiElement::button_Save;
    case QMessageBox::Discard:
      return GuiElement::button_Discard;
    case QMessageBox::Apply:
      return GuiElement::button_Apply;
    case QMessageBox::Open:
      return GuiElement::button_Open;
    case QMessageBox::Cancel:
      return GuiElement::button_Cancel;
    case QMessageBox::Abort:
      return GuiElement::button_Abort;
    default:
      std::cerr << "Unknown Button pushed: " << result << std::endl;
      return GuiElement::button_Cancel;
    }
  } else {
    QtMessageBox mb( QtMultiFontString::getQString(mytitle),
                     QtMultiFontString::getQString(message),
                     QMessageBox::NoIcon,//Question,
                     QMessageBox::NoButton, ///firstBtn|secondBtn|thirdBtn,
                     QtMessageBox::getParentWidget(element)
                     );
    // first button
    if (firstBtn == QMessageBox::Yes){
      yesButton =
      mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Yes,
                                                   _("&Yes"), buttonText),
                   QMessageBox::YesRole);
    }else if (firstBtn == QMessageBox::Save){
      saveButton =
      mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Save,
                                                   _("&Save"), buttonText),
                   QMessageBox::AcceptRole);
    }else if (firstBtn == QMessageBox::Ok){
      okButton =
      mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Ok,
                                                   _("&Ok"), buttonText),
                   QMessageBox::AcceptRole);
    }

    // second button
    // No Button
    if (secondBtn == QMessageBox::No){
      noButton =
      mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_No,
                                                   _("&No"), buttonText),
                 QMessageBox::NoRole);
    // }else if(secondBtn == QMessageBox::Discard) {
    //   // Discard Button
    //   mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Discard,
    //                                                _("&Discard"), buttonText),
    //                QMessageBox::DestructiveRole);
    }else if(secondBtn == QMessageBox::Apply) {
      // Apply Button
      applyButton =
      mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Apply,
                                                   _("&Apply"), buttonText),
                   QMessageBox::ApplyRole);
    }else if(secondBtn == QMessageBox::Open) {
      // Open Button
      openButton =
      mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Open,
                                                   _("&Open"), buttonText),
                   QMessageBox::AcceptRole);
    }

    // third button
    if ( withCancelBtn ) {
      if (thirdBtn == QMessageBox::Cancel){
        // Cancel Button
        cancelButton =
        mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Cancel,
                                                     _("&Cancel"), buttonText),
                     QMessageBox::RejectRole);

      }else if(thirdBtn == QMessageBox::Abort) {
        // Abort Button
        abortButton =
        mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Abort,
                                                     _("&Abort"), buttonText),
                     QMessageBox::RejectRole);
      }else if(thirdBtn == QMessageBox::Discard) {
        discardButton =
        // Discard Button
        mb.addButton(QtMessageBoxExt::getButtonLabel(GuiElement::button_Discard,
                                                     _("&Discard"), buttonText),
                     QMessageBox::RejectRole);
      }
    }

    mb.setDefaultButton(defaultBtn == 1 ? firstBtn : defaultBtn == 2 ? secondBtn : defaultBtn == 3 ? thirdBtn : firstBtn);
    mb.setEscapeButton(secondBtn);

    result = mb.exec();
    pushed = mb.clickedButton();
  }

  loopcontrol->goAway();

  if( listener){
    if (pushed &&
        (pushed == okButton || pushed == yesButton || pushed == saveButton ||
         pushed == applyButton ||pushed == openButton))
      listener->confirmYesButtonPressed();
    else
      if (pushed && pushed == noButton)
        listener->confirmNoButtonPressed();
      else
        listener->confirmCancelButtonPressed();
  }

  // clickedButton => GuiElement::ButtonType
  if (pushed){
    if (pushed == okButton)
      return GuiElement::button_Ok;
    if (pushed == yesButton)
      return GuiElement::button_Yes;
    if (pushed == saveButton)
      return GuiElement::button_Save;
    if (pushed == noButton)
      return GuiElement::button_No;
    if (pushed == applyButton)
      return GuiElement::button_Apply;
    if (pushed == openButton)
      return GuiElement::button_Open;
    if (pushed == cancelButton)
      return GuiElement::button_Cancel;
    if (pushed == abortButton)
      return GuiElement::button_Abort;
    if (pushed == discardButton)
      return GuiElement::button_Discard;
  }
  return GuiElement::button_Cancel;
}
