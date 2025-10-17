
#ifndef GUI_QT_NAV_TEXTFIELD_H
#define GUI_QT_NAV_TEXTFIELD_H

#include "xfer/XferConverter.h"
#include "gui/qt/GuiQtDataField.h"

class Scale;
class QLineEdit;

class GuiQtNavTextfield : public GuiQtDataField{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
 public:
  GuiQtNavTextfield ( GuiElement *parent, XferDataItem *dataitem );
  GuiQtNavTextfield ( GuiElement *parent, const std::string& label, const std::string& labelColor );
  virtual ~GuiQtNavTextfield();

 private:
  GuiQtNavTextfield( const GuiQtNavTextfield & );
  GuiQtNavTextfield &operator=( const GuiQtNavTextfield &textfield );

  /*=============================================================================*/
  /* private definitions                                                         */
  /*=============================================================================*/
 protected:
  enum CB_Reason
  { reason_Validate };

  /*=============================================================================*/
  /* public member functions of GuiElement                                       */
  /*=============================================================================*/
 public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Textfield; }
  virtual void create(){}
  virtual void manage() {}
  virtual void update( UpdateReason ){}
  virtual QWidget *myWidget() { return 0; }

/*=============================================================================*/
/* public member functions of GuiField                                         */
/*=============================================================================*/
public:
  virtual bool setPrecision( int prec );
  virtual bool setScalefactor( Scale *scale );
  virtual bool setThousandSep();
/*=============================================================================*/
/* public member functions of GuiQtDataField                                */
/*=============================================================================*/
public:
  virtual bool FieldIsEditable();
  virtual bool installDataItem( XferDataItem *dataitem );

  /*=============================================================================*/
  /* public member functions of ConfirmationListener                             */
  /*=============================================================================*/
 public:
  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed(){}

  /*=============================================================================*/
  /* public member functions                                                     */
  /*=============================================================================*/
 public:
  void finalWork( const std::string &value );
  bool startRename(){ return FieldIsEditable(); }
  virtual void validate( const std::string &value ){}
  virtual bool getFormattedValue( std::string &s );
  bool getColor( std::string &color );
  virtual void setWidth( int width );
  virtual bool setInputValue();
  XferParameter *getParam(){ return m_param; }
  virtual int getLength(){ return m_param->getLength(); }
  bool getValue( std::string &s );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  void finalWorkOk();
  virtual XferParameter::InputStatus checkFormat();

  /*=============================================================================*/
  /* private member functions                                                    */
  /*=============================================================================*/
 private:
  virtual void setAlarmColors(){}

  /*=============================================================================*/
  /* private data                                                                */
  /*=============================================================================*/
 private:
  std::string m_tmpValue;
  std::string m_label;
  std::string m_labelColor;
};

#endif
