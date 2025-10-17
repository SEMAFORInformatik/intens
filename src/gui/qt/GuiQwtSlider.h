
#if !defined(GUI_QWT_SLIDER_H)
#define GUI_QWT_SLIDER_H

#include "gui/qt/GuiQtDataField.h"
#include "gui/GuiSlider.h"

#include <qwt_slider.h>

class GuiQwtSlider : public GuiQtDataField, public GuiSlider {
    Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQwtSlider( GuiElement *parent );
  virtual ~GuiQwtSlider();

  class MyQwtSlider : public QwtSlider{
  public:
    MyQwtSlider( QWidget *parent, GuiQwtSlider *slider )
    : QwtSlider( parent )
      , m_slider( slider )
      , m_mouseDown( false ){}
    ~MyQwtSlider(){}
  public:
    virtual void mousePressEvent ( QMouseEvent * e ){
      m_mouseDown = true;
      QwtSlider::mousePressEvent( e );
    }
    virtual void mouseReleaseEvent ( QMouseEvent * e ){
      m_mouseDown = false;
      QwtSlider::mouseReleaseEvent( e );
      m_slider->valueChange( value() );
    }
#if QWT_VERSION < 0x060100
    virtual void valueChange(){
      QwtSlider::valueChange();
      m_slider->valueChange( value() );
    }
#endif
    bool mouseDown(){
      return m_mouseDown;
    }
  private:
    bool m_mouseDown;
    GuiQwtSlider *m_slider;
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
/* protected member functions of GuiDatafield                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue();
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
private slots:
  void valueChange(double value);
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
  MyQwtSlider              *m_sliderwidget;
};

#endif
