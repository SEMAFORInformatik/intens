
#if !defined(GUI_QTTOGGLE_INCLUDED_H)
#define GUI_QTTOGGLE_INCLUDED_H

#include "gui/qt/GuiQtDataField.h"

#include <qcheckbox.h>
class GuiQtToggleButton;

class GuiQtToggle : public GuiQtDataField
{
  Q_OBJECT

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtToggle( GuiElement *parent );
  GuiQtToggle( const GuiQtToggle &toggle );
  virtual ~GuiQtToggle();

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  class MyQCheckBox : public QCheckBox {
  public:
    MyQCheckBox( QWidget* parent, GuiQtElement *e );
    virtual ~MyQCheckBox() {}
 private:
    // virtual member functions of QCheckBox
    virtual void enterEvent ( QEnterEvent * );
    virtual void leaveEvent ( QEvent * );
    virtual void paintEvent ( QPaintEvent * );

  private:
    GuiQtElement *m_element;
  };
/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Toggle; }

  virtual void create();
  virtual void manage() {}
  virtual bool destroy();

  virtual void enable();
  virtual void disable();
  virtual void getSize( int &x, int &y );

  virtual void update( UpdateReason );
  virtual QWidget *myWidget();

  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtToggle( *this ); }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList *eles, bool onlyUpdate = false);
#endif

/*=============================================================================*/
/* public member functions of GuiField                                         */
/*=============================================================================*/
public:
  virtual bool setScalefactor( Scale *scale );

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual GuiQtDataField *CloneForFieldgroupTable();
  void ValueChanged( bool state );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue();
/*   virtual void overrideTranslations( Widget w ); */
/*   virtual Widget createWidget( Widget parent ); */

  void setColors();
  void setAlarmColors();

  void FinalWork();
  void FinalWorkOk();
  QCheckBox*  getToggle() { return m_toggle; }

private slots:
  void valueChanged( bool state );
/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QCheckBox  *m_toggle;
};

#endif
