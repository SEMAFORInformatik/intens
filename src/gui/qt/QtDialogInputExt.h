
#if !defined(QT_INPUTDIALOGEXT_H)
#define QT_INPUTDIALOGEXT_H

#include <QDialog>
#include <QLineEdit>

#include "utils/gettext.h"

class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

class QtDialogInputExt : public QDialog
 {
Q_OBJECT
  public:

    QtDialogInputExt(QWidget * parent, const QString & title, const QString & label,
		       const QString & textLabel, QLineEdit::EchoMode mode = QLineEdit::Normal,
		       const QString& yesButtonText=QString(_("OK")),
		       const QString& cancelButtonText=QString(_("Cancel")),
		       const QString& applyButtonText=QString()
		       );

std::string getText();

private slots:
     void slot_yesButtonClicked(bool clicked);
     void slot_cancelButtonClicked(bool clicked);
     void slot_applyButtonClicked(bool clicked);

 private:
     QLabel           *m_label;
     QLabel           *m_textLabel;
     QLineEdit        *m_lineEdit;
     QDialogButtonBox *m_buttonBox;
     QPushButton      *m_yesButton;
     QPushButton      *m_cancelButton;
     QPushButton      *m_applyButton;
     QPushButton      *m_moreButton;
     QWidget          *m_extension;
 };

 #endif
