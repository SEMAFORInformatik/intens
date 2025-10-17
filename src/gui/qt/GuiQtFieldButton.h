
#if !defined(GUI_QT_FIELDBUTTON_INCLUDED_H)
#define GUI_QT_FIELDBUTTON_INCLUDED_H

#include "gui/qt/GuiQtDataField.h"

class QAbstractButton;

class GuiQtFieldButton : public GuiQtDataField
{
  Q_OBJECT

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtFieldButton( GuiElement *parent );
  GuiQtFieldButton( GuiQtFieldButton &button );

  virtual ~GuiQtFieldButton(){}

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_FieldButton; }
  virtual void create( );
  virtual void manage() {}
  virtual void getSize( int &x, int &y );
  virtual void stretchWidth( int width );
  virtual void update( UpdateReason );
  virtual void enable();
  virtual void disable();
  virtual QWidget * myWidget() { return (QWidget*)m_buttonwidget; }
  virtual void addTabGroup();
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtFieldButton ( *this ); }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual GuiElement::Orientation getContainerExpandPolicy();
  virtual void setOrientation( GuiElement::Orientation orientation ) { m_orientation = orientation; };

  // GuiIndexListener
  virtual bool acceptIndex( const std::string &name, int inx );
  virtual void setIndex( const std::string &name, int inx );

  // GuiQtElement
  Qt::Orientations getExpandPolicy();

/*=============================================================================*/
/* public member functions of GuiDataField                                     */
/*=============================================================================*/
public:
  virtual bool setLength( int len );
  virtual int getLength(){ return m_length; }

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed() { assert( false ); }
  virtual void confirmNoButtonPressed() { assert( false ); }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool setLabel( const std::string &name );
  virtual bool setPixmap( const std::string &name, bool withLabel=false );
  virtual void setElementSize( int width, int height );
  const std::string &Label() { return m_label; }
  virtual GuiQtDataField *CloneForFieldgroupTable();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue();
  virtual void lock();
  virtual void unlock();
  virtual void switchlock();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void setColors();
  void setButtonSize();
  int  getButtonHeight();
  int  getButtonWidth();

private slots:
  void slot_activate();
  void slot_pressed();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QAbstractButton   *m_buttonwidget;
  std::string        m_label;
  std::string        m_pixmap_name;
  GuiElement::GuiButtonType m_label_pixmap;
  int                m_width_element;
  int                m_height_element;
  int                m_length;
  Orientation        m_orientation;

  friend class MyQtFieldButton;
};

#endif
