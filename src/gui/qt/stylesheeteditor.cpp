/****************************************************************************
**
** Copyright (C) 2006-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QStyleFactory>
#include <QRegularExpression>

#include "stylesheeteditor.h"
#include "app/AppData.h"
#include "gui/qt/GuiQtManager.h"

StyleSheetEditor::StyleSheetEditor(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    QRegularExpression regExp("Q(.*)Style");
    QString defaultStyle = QApplication::style()->metaObject()->className();
#if QT_VERSION > 0x050800
    QRegularExpressionMatch match = regExp.match(defaultStyle);
    if (match.hasMatch())
        defaultStyle = match.captured(1);
#else
    if (regExp.exactMatch(defaultStyle))
        defaultStyle = regExp.cap(1);
#endif

    ui.styleCombo->addItems(QStyleFactory::keys());
    ui.styleCombo->setCurrentIndex(ui.styleCombo->findText(defaultStyle));
    ui.styleSheetCombo->setCurrentIndex(ui.styleSheetCombo->findText("Coffee"));
    //    loadStyleSheet("Coffee");

    // load stylesheet combobox
    std::set<std::string> list = GuiManager::Instance().getAvailableStylesheets();
    std::set<std::string>::const_iterator it;
    ui.styleSheetCombo->addItem("");
    for (it = list.begin(); it != list.end(); ++it) {
      ui.styleSheetCombo->addItem(QApplication::translate("StyleSheetEditor", (*it).c_str(), 0));
    }
    int idx = ui.styleSheetCombo->findText(QString::fromStdString( GuiManager::Instance().stylesheetName() ));
    if (idx >= 0 ) {
      ui.styleSheetCombo->setCurrentIndex(idx);
      loadStyleSheet(QString::fromStdString( GuiManager::Instance().stylesheetName() ));
    }
}

void StyleSheetEditor::on_styleCombo_activated(const QString &styleName)
{
    qApp->setStyle(styleName);
    ui.applyButton->setEnabled(false);
    GuiManager::Instance().update( GuiElement::reason_Always );
}

void StyleSheetEditor::on_styleSheetCombo_activated(const QString &sheetName)
{
  loadStyleSheet(sheetName);
}

void StyleSheetEditor::on_styleTextEdit_textChanged()
{
    ui.applyButton->setEnabled(true);
}

void StyleSheetEditor::on_applyButton_clicked()
{
#if  QT_VERSION >= 0x040200
    qApp->setStyleSheet(ui.styleTextEdit->toPlainText());
    ui.applyButton->setEnabled(false);
    GuiManager::Instance().update( GuiElement::reason_Always );
#endif
}

void StyleSheetEditor::loadStyleSheet(const QString &sheetName)
{
  std::string sname = GuiManager::Instance().setStylesheetName( sheetName.toStdString() );
  if (sname.size() == 0) return;
  
#if  QT_VERSION >= 0x040200
  QFile file( QString::fromStdString(sname) );
  if (file.exists()) {
    file.open(QFile::ReadOnly);
    QString styleSheet = file.readAll();
    ui.styleTextEdit->setPlainText(styleSheet);
//     qApp->setStyleSheet(styleSheet);
    ui.applyButton->setEnabled(false);
    file.close();
  }
  GuiManager::Instance().update( GuiElement::reason_Always );
 #endif
}
