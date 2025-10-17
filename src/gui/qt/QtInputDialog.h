
#if !defined(QT_INPUTDIALOG_H)
#define QT_INPUTDIALOG_H

#include <QInputDialog>
#include <QTextEdit>

class GuiElement;

class QtInputDialog : public QInputDialog{
public:
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
  QtInputDialog( const QString& caption,
		 const QString& textMessage,
		 const QString& textLabel,
		 QWidget *parent = 0 );

  virtual ~QtInputDialog(){}
  static QWidget* getParentWidget(GuiElement* element);
  int exec(); // call exec and restore activeWindow
/*=============================================================================*/
/* private member functions                                                     */
/*=============================================================================*/
 private:
  void init();
  bool isMultiFont();
  bool setMultiFontMsg();
  virtual void closeEvent( QCloseEvent*){}

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
 private:
  std::string  m_msg;
  QTextEdit*   m_textEdit;
  QString      m_label;
};

#endif
