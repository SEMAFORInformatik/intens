
#if !defined(GUI_QT_COMBOBOX_H)
#define GUI_QT_COMBOBOX_H

#include <qobject.h>
#include <qcombobox.h>

#include "gui/GuiComboBox.h"
#include "gui/qt/GuiQtDataField.h"

class DataReference;

class GuiQtComboBox : public GuiQtDataField, public GuiComboBox{
  Q_OBJECT
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  GuiQtComboBox( GuiElement *parent );
  GuiQtComboBox( GuiQtComboBox &option );

  virtual ~GuiQtComboBox();

  /*=============================================================================*/
  /* public definitions                                                          */
  /*=============================================================================*/
public:
  class MyQComboBox : public QComboBox {
  public:
    MyQComboBox( bool rw, QWidget* parent, GuiQtElement *e );
    virtual ~MyQComboBox();

    void setFont( const QFont& font );

    // virtual member functions of QComboBox
    virtual bool event(QEvent* e);
    virtual void enterEvent ( QEnterEvent * );
    virtual void leaveEvent ( QEvent * );
    virtual void keyPressEvent ( QKeyEvent * );
    virtual void wheelEvent(QWheelEvent* e);
  private:
    GuiQtElement *m_element;
  };

  /*=============================================================================*/
  /* private slots                                                               */
  /*=============================================================================*/
private slots:
 void activateCB( int index );
 void textChanged();

/*=============================================================================*/
/* member functions of GuiComboBox                                             */
/*=============================================================================*/
protected:
  virtual XferDataParameter *param(){ return m_param; }
private:
  virtual GuiDataField *datafield(){ return this; }
  virtual GuiElement *getElement(){ return this; }
  virtual bool created(){ return m_combobox != 0; }
  virtual void fill( int n );

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_ComboBox; }
  virtual void create();
  virtual void manage() { if( m_combobox )
    m_combobox->show();
  }
  virtual bool destroy();
  virtual void enable();
  virtual void disable();
  virtual void update( UpdateReason );
  virtual QWidget *myWidget() { return m_combobox; }
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtComboBox( *this ); }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  /** JSF Corba:: get all visible Datapool Values */
  virtual void getVisibleDataPoolValues( GuiValueList& vmap );

/*=============================================================================*/
/* public member functions of GuiDataField                                     */
/*=============================================================================*/
public:
  virtual bool setScalefactor( Scale *scale ){
    return GuiComboBox::setScalefactor( scale );
  }
  virtual bool setSetName( const std::string &name ){
    return GuiComboBox::setSetName( name );
  }

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed(){
    GuiComboBox::confirmYesButtonPressed();
  }
  virtual void confirmNoButtonPressed(){
    GuiComboBox::confirmNoButtonPressed();
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool installDataItem( XferDataItem *dataitem ){
    return GuiComboBox::installDataItem( dataitem );
  }
  virtual XferDataItem *replaceDataItem( XferDataItem *dataitem ){
    return GuiComboBox::replaceDataItem( dataitem );
  }
  virtual GuiQtDataField *CloneForFieldgroupTable();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue(){
    return GuiComboBox::setInputValue();
  }
  virtual void lock(){
    GuiComboBox::lock();
  }
  virtual void unlock(){
    GuiComboBox::unlock();
  }
  virtual void switchlock(){
    GuiComboBox::switchlock();
  }

  virtual void setColors();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void deletePickList();
  void rebuildPickList();
  int getPickListIndex( const std::string &value );
  int getOptionIndex( const std::string &value );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  MyQComboBox          *m_combobox;
};

#endif
