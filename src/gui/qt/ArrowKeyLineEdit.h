#ifndef ARROW_KEY_LINE_EDIT_H
#define ARROW_KEY_LINE_EDIT_H

#include "gui/qt/GuiQtTextfield.h"

#include <QEvent>

class RealConverter;
class QMenu;

class ArrowKeyListener {
 public:
    virtual void valueChanged()=0;
};

class ArrowKeyLineEdit : public QLineEdit {
  Q_OBJECT
  public:
    ArrowKeyLineEdit( QWidget* parent, GuiQtTextfield *e ) 
      : QLineEdit( parent )
      , m_textfield( e ), m_converter(0), m_listener(0), m_saveInDataPool(false) {
    }
    ArrowKeyLineEdit( QWidget* parent, RealConverter *c ) 
      : QLineEdit( parent )
	, m_textfield( 0 ), m_converter(c), m_listener(0), m_saveInDataPool(false) {
	setAlignment(Qt::AlignRight);
    }
    virtual ~ArrowKeyLineEdit() { }
    
    void attachListener( ArrowKeyListener *l ){
      m_listener=l;
    }

 private:
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void wheelEvent ( QWheelEvent *e );
    void focusInEvent ( QFocusEvent *e);
    void focusOutEvent ( QFocusEvent *e);
    bool event(QEvent* e);
    /** call validator validate method
	to validate user input
     */
    bool makeValidatorCheck();
    bool processCursorStep(bool up);

    // virtual member functions of QLineEdit
    virtual void enterEvent ( QEnterEvent * );
    virtual void leaveEvent ( QEvent * );

  protected:
    QMenu * createPopupMenu ();

  private:
    GuiQtTextfield *m_textfield;
    RealConverter  *m_converter;
    ArrowKeyListener *m_listener;
    bool            m_saveInDataPool;
  };
#endif
