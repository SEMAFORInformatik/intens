
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QBoxLayout>
#include <QApplication>
#include <QStyle>
#include <iostream>

#include "gui/GuiElement.h"
#include "gui/qt/QtDialogInputExt.h"
#include "utils/gettext.h"

//-------------------------------------------------
// QtDialogInputExt Constructor
//-------------------------------------------------
QtDialogInputExt::QtDialogInputExt(QWidget * parent, const QString& title, const QString& label,
				   const QString&  textLabel, QLineEdit::EchoMode mode,
				   const QString& yesButtonText,
				   const QString& cancelButtonText,
				   const QString& applyButtonText )
  : QDialog(parent, Qt::WindowCloseButtonHint)
  , m_applyButton( 0 )
{
  setWindowTitle( title );

  // (info) label
  m_label = new QLabel(label);

  // (text) label
  m_textLabel = new QLabel(textLabel);
  m_lineEdit = new QLineEdit;
  m_textLabel->setBuddy(m_lineEdit);

  // yes button
  m_yesButton = new QPushButton( yesButtonText.size() ? yesButtonText : QString(_("Yes") ) );
  m_yesButton->setDefault(true);
  QStyle* style = QApplication::style();
  m_yesButton->setIcon(style->standardIcon(QStyle::SP_DialogYesButton));

  // cancel button
  m_cancelButton = new QPushButton( cancelButtonText.size() ? cancelButtonText : QString(_("Cancel") ) );
  m_cancelButton->setIcon(style->standardIcon(QStyle::SP_DialogCancelButton));
  //  m_cancelButton->setDefault(true);

  // apply button
  if ( applyButtonText.size() ) {
    m_applyButton = new QPushButton( applyButtonText );
    if (applyButtonText.contains( _("new"), Qt::CaseInsensitive) || applyButtonText.contains( _("save"), Qt::CaseInsensitive))
      m_applyButton->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
  }

  // extension button
  m_moreButton = new QPushButton(_("&More"));
  m_moreButton->setCheckable(true);

  // optional extension widget
  m_extension = new QWidget;

  // buttonbox
  m_buttonBox = new QDialogButtonBox(Qt::Horizontal);
  m_buttonBox->addButton(m_yesButton, QDialogButtonBox::ActionRole);
  if (m_applyButton) {
    m_buttonBox->addButton(m_applyButton, QDialogButtonBox::ActionRole);
  }
  m_buttonBox->addButton(m_cancelButton, QDialogButtonBox::ActionRole);
  m_buttonBox->addButton(m_moreButton, QDialogButtonBox::ActionRole);

  connect(m_moreButton, SIGNAL(toggled(bool)), m_extension, SLOT(setVisible(bool)));
  connect(m_yesButton, SIGNAL(clicked(bool)), this, SLOT(slot_yesButtonClicked(bool)));
  connect(m_cancelButton, SIGNAL(clicked(bool)), this, SLOT(slot_cancelButtonClicked(bool)));
  if (m_applyButton)
    connect(m_applyButton, SIGNAL(clicked(bool)), this, SLOT(slot_applyButtonClicked(bool)));

  // layout
  QVBoxLayout *extensionLayout = new QVBoxLayout;
  extensionLayout->setContentsMargins(0,0,0,0);
  m_extension->setLayout(extensionLayout);

  QVBoxLayout *leftLayout = new QVBoxLayout;
  leftLayout->addWidget(m_label);
  leftLayout->addWidget(m_textLabel);
  leftLayout->addWidget(m_lineEdit);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addLayout(leftLayout, 0, 0, 1, -1);
  mainLayout->addWidget(m_buttonBox, 1, 0, 1, -1);
  mainLayout->addWidget(m_extension, 2, 0, 1, -1);
  mainLayout->setRowStretch(3, 1);

  setLayout(mainLayout);

  // hide
  m_extension->hide();
  m_moreButton->hide();
  setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

//-------------------------------------------------
// getText
//-------------------------------------------------
std::string QtDialogInputExt::getText() {
  if (m_lineEdit) {
    return m_lineEdit->text().toStdString();
  }

  return "";
}

//-------------------------------------------------
// slot_yesButtonClicked
//-------------------------------------------------
void QtDialogInputExt::slot_yesButtonClicked(bool clicked) {
  done(GuiElement::button_Yes);
}

//-------------------------------------------------
// slot_cancelButtonClicked
//-------------------------------------------------
void QtDialogInputExt::slot_cancelButtonClicked(bool clicked) {
  done(GuiElement::button_Cancel);
}

//-------------------------------------------------
// slot_applyButtonClicked
//-------------------------------------------------
void QtDialogInputExt::slot_applyButtonClicked(bool clicked) {
  done(GuiElement::button_Apply);
}
