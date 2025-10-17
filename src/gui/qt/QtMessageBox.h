
#if !defined(QT_MESSAGEBOX_H)
#define QT_MESSAGEBOX_H

#include <QMessageBox>
#include <QTextEdit>

class GuiElement;

class QtMessageBox : public QMessageBox{
public:
  QtMessageBox( const QString& caption
		, const QString& text
		, Icon icon
    , QMessageBox::StandardButtons buttons
		, QWidget *parent = 0 );

  virtual ~QtMessageBox(){}
  static QWidget* getParentWidget(GuiElement* element);

  void init();
  bool isMultiFont();
  bool setMultiFontMsg();
  bool hasBrTag();
  int exec(); // call exec and restore activeWindow

protected:
  virtual void closeEvent( QCloseEvent*){}
  std::string             m_msg;
  QTextEdit*  m_textEdit;
};

#endif
