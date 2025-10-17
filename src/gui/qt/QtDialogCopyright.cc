
#include <QApplication>
#include <QMessageBox>
#include <QtGui/QPixmap>
#include <QtCore/QDate>
#include <QtCore/QSettings>

#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtDialogCopyright.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/QtMultiFontString.h"

#include "utils/gettext.h"
#include "app/AppData.h"


/* static variables */

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

QtDialogCopyright::QtDialogCopyright()
{
  copyright_pixmap = "semafor";
  copyright_text = compose(_("<p>Copyright (C) 1994-%1<br/>SEMAFOR Informatik & Energie AG<br/>Sperrstrasse 104b<br/>CH-4057 Basel<br/><a href='mailto:info@semafor.ch'>info@semafor.ch</a></p>"), QDate::currentDate().year());
  copyright_text = compose(_("Copyright (C) %1<br/>SEMAFOR Informatik & Energie AG<br/><a href='mailto:info@semafor.ch'>info@semafor.ch</a><br/>All rights reserved."), QDate::currentDate().year());
  copyright_text += "<p>Intens Engineering Manager is made possible by the <a href='http://www.semafor.ch/products/intens/'>Intens OSS</a> open source project and other <a href='http://www.semafor.ch/doc/intens-third-party-libraries.txt'>open source software</a></p>";
    copyright_text += "<p><a href='http://www.semafor.ch/doc/intens-terms.txt'>Terms of Service</a>";
}

QtDialogCopyright::~QtDialogCopyright(){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void QtDialogCopyright::ButtonPressed(){

  if (GuiQtManager::Settings()) {
    copyright_pixmap=GuiQtManager::Settings()->value
      ( "Intens/CopyrightPixmap", QString::fromStdString(copyright_pixmap)).toString().toStdString();
    copyright_text_title=GuiQtManager::Settings()->value
      ( "Intens/CopyrightTitle.text", "").toString().toStdString();
    copyright_text=GuiQtManager::Settings()->value
      ( "Intens/CopyrightBase.text", QString::fromStdString(copyright_text)).toString().toStdString();
    copyright_text_additional=GuiQtManager::Settings()->value
      ( "Intens/Copyright.text", "").toString().toStdString();
    organization_text=GuiQtManager::Settings()->value
      ( "Intens/Organization.text", "").toString().toStdString();
  }
  create();
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void QtDialogCopyright::manage(){
  std::cerr << "QtDialogCopyright::manage()" <<std::endl;
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

void QtDialogCopyright::unmanage(){
  std::cerr << "QtDialogCopyright::unmanage()" <<std::endl;
}

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/

//-----------------------------------------------------------------------------
// create
// ----------------------------------------------------------------------------
void QtDialogCopyright::create(){
  std::string mytitle(AppData::Instance().Title() +  ": Copyright");
  std::string message("");

  if (copyright_text_title.size()) {
    message += copyright_text_title;
  } else {
    message += "<p>INTENS - Engineering Manager<br/>";
    message += "Version: " + AppData::Instance().Version() + "<br/>";
    if (AppData::Instance().Revision().size()) {
      message += "Revision: " + AppData::Instance().Revision() + "<br/>";
    }
    if (AppData::Instance().RevisionDate().size()) {
      message += "Date: " + AppData::Instance().RevisionDate() + "<br/>";
    }
    message += "</p>";
  }

  if (!organization_text.empty()) {
    message += organization_text;
  }

  message += "<p>";
  message += compose(copyright_text, QDate::currentDate().year());
  message += "</p>";

  if (copyright_text_additional.size()) {
    message += "<p>"+copyright_text_additional+"</p>";
  }
  if(AppData::Instance().HeadlessWebMode()) {
    GuiFactory::Instance()->showDialogInformation(0, mytitle, message, 0);
    return;
  }

  QMessageBox mb(QMessageBox::Information,
                 QString::fromStdString(mytitle),
                 QString::fromStdString(message),
                 QMessageBox::NoButton,
                 qApp->activeWindow()
                 );
  mb.addButton(_("OK"), QMessageBox::YesRole);

  // set copyright font
  QFont font =  mb.font();
  mb.setFont( QtMultiFontString::getQFont( "@copyright@", font ) );

  QPixmap icon;
  if( QtIconManager::Instance().getPixmap(copyright_pixmap, icon ) ){
    mb.setIconPixmap( icon );
  }

  GuiFactory::Instance()->installDialogsWaitCursor();
  mb.exec();
  GuiFactory::Instance()->removeDialogsWaitCursor();

  return;
}
