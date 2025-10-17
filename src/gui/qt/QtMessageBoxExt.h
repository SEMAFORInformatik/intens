
#if !defined(QT_MESSAGEBOXEXT_H)
#define QT_MESSAGEBOXEXT_H

#include <QDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <map>
#include "gui/GuiElement.h"

class QCloseEvent;

class QtMessageBoxExt : public QDialog{
  Q_OBJECT
public:
  QtMessageBoxExt( const QString& caption
		, const QString& text
		, const char* dialog_icon
		, QMessageBox::StandardButtons buttons
		, const std::map<GuiElement::ButtonType, std::string>& buttonText
		, QWidget *parent = 0 );

  virtual ~QtMessageBoxExt(){}
  static QWidget* getParentWidget(GuiElement* element);
  static QString getButtonLabel(GuiElement::ButtonType buttonType,
                                const char *default_label,
                                const std::map<GuiElement::ButtonType, std::string>& buttonText);

  void closeEvent ( QCloseEvent * e );
  void reject();

  bool setMultiFontMsg();
  bool hasBrTag();
  int exec(); // call exec and restore activeWindow

public slots:
  void slot_OkButtonPressed();
  void slot_YesButtonPressed();
  void slot_SaveButtonPressed();
  void slot_NoButtonPressed();
  void slot_DiscardButtonPressed();
  void slot_ApplyButtonPressed();
  void slot_OpenButtonPressed();
  void slot_CancelButtonPressed();
  void slot_AbortButtonPressed();
public:

private:
  std::string             m_msg;
  QTextEdit*  m_textEdit;
};

#endif
