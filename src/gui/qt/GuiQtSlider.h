
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(GUI_QT_SLIDER_H)
#define GUI_QT_SLIDER_H

#include "gui/qt/GuiQtDataField.h"
#include "gui/GuiSlider.h"

#include <QSlider>

class GuiQtSlider : public GuiQtDataField, public GuiSlider {
    Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtSlider( GuiElement *parent );
  GuiQtSlider( const GuiQtSlider &toggle );
  virtual ~GuiQtSlider();

private:
  class QtSlider : public QSlider{
  public:
    QtSlider( QWidget *parent, GuiQtSlider *slider )
    : QSlider( parent )
      , m_slider( slider )
      , m_mouseDown( false ){}
    ~QtSlider(){}
  public:
    virtual void mousePressEvent ( QMouseEvent * e ){
      m_mouseDown = true;
      QSlider::mousePressEvent( e );
    }
    virtual void mouseReleaseEvent ( QMouseEvent * e ){
      m_mouseDown = false;
      QSlider::mouseReleaseEvent( e );
      m_slider->valueChange( value() );
    }
// #if QWT_VERSION < 0x060100
//     virtual void valueChange(){
//       QSlider::valueChange();
//       m_slider->valueChange( value() );
//     }
// #endif
    bool mouseDown(){
      return m_mouseDown;
    }
  private:
    bool m_mouseDown;
    GuiQtSlider *m_slider;
  };
/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed() { assert( false ); }
  virtual void confirmNoButtonPressed() { assert( false ); }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Slider; }
  virtual void create();
  virtual void enable();
  virtual void disable();
  virtual void manage();
  virtual double getValue();
  virtual QWidget* myWidget();
  virtual void setRange( double min, double max, double stepsize );
  virtual void setOrientation( GuiElement::Orientation orientation );
  virtual void setScalePosition( GuiElement::Alignment alignment );
  virtual void setBgStyle();

  virtual GuiElement * getElement() { return this; }

  virtual void update( UpdateReason );
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
/*=============================================================================*/
/* public member functions of GuiDatafield                                  */
/*=============================================================================*/
public:
  virtual GuiQtDataField *CloneForFieldgroupTable();
/*=============================================================================*/
/* protected member functions of GuiDatafield                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue();
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
private slots:
  void valueChange(int value);
/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
  void FinalWork();
  void FinalWorkOk();
  XferParameter::InputStatus checkFormat();


/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QtSlider              *m_sliderwidget;
};

#endif
