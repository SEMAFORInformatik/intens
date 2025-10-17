
#include <qwt_slider.h>

#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "GuiQwtSlider.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQwtSlider::GuiQwtSlider( GuiElement *parent )
  : GuiQtDataField( parent ),
    m_sliderwidget( 0 )
{
}

GuiQwtSlider::~GuiQwtSlider(){
  delete m_sliderwidget;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
void GuiQwtSlider::setRange( double min, double max, double stepsize ){
  if ( m_sliderwidget == 0 ) return;
#if QWT_VERSION < 0x060100
  m_sliderwidget -> setRange( min, max, stepsize );
#else
  m_sliderwidget -> setScale( min, max );
  m_sliderwidget -> setScaleStepSize(stepsize);
#endif
}

// bool GuiQwtSlider::setLabel( const std::string &name ){
//   m_label = name;
//   m_label_pixmap = false;
//   return true;
// }

// bool GuiQwtSlider::setPixmap( const std::string &name ){
//   m_label = name;
//   m_label_pixmap = true;
//   return true;
// }

void GuiQwtSlider::create(){
  m_sliderwidget = new MyQwtSlider( getParent()->getQtElement()->myWidget(), this );
  m_sliderwidget->setValue( 0 );
  // for demonstration, should be removed
  m_sliderwidget -> setOrientation( Qt::Horizontal );
#if QWT_VERSION < 0x060100
  m_sliderwidget -> setScalePosition( QwtSlider::BottomScale );
#else
  m_sliderwidget -> setScalePosition( QwtSlider::LeadingScale );
#endif
  setRange( Attr()->getMin()
	    , Attr()->getMax()
	    , Attr()->getStep() );
  int width = (int)floor( m_sliderwidget->sizeHint().width() * 1.5 );
  m_sliderwidget -> setMinimumWidth( width );
  // set some default values
  // type of slider representation
#if QWT_VERSION < 0x060000
  m_sliderwidget -> setBgStyle( QwtSlider::BgBoth );
#elif QWT_VERSION < 0x060100
  m_sliderwidget -> setBackgroundStyle( QwtSlider::Trough|QwtSlider::Groove);
#else
  m_sliderwidget -> setTrough(true);
  m_sliderwidget -> setGroove(true);
#endif

  connect( m_sliderwidget, SIGNAL(valueChanged(double)), this, SLOT(valueChange(double)) );
}

/*=============================================================================*/
/* set orientation of the slider, Horizontal or Vertical                       */
/*=============================================================================*/
void GuiQwtSlider::setOrientation( GuiElement::Orientation orientation ){
  if ( m_sliderwidget == 0 ) return;

  if ( orientation == GuiElement::orient_Horizontal ){
    m_sliderwidget -> setOrientation( Qt::Horizontal );
  }
  else if ( orientation == GuiElement::orient_Vertical ){
    m_sliderwidget -> setOrientation( Qt::Vertical );
  }
}

/*=============================================================================*/
/* set the description orientation of the slider, Left, Right, Bottom or Top   */
/*=============================================================================*/
void GuiQwtSlider::setScalePosition( GuiElement::Alignment alignment ){
  if ( m_sliderwidget == 0 ) return;

#if QWT_VERSION < 0x060100
  if ( alignment == GuiElement::align_Left ){
    m_sliderwidget -> setScalePosition( QwtSlider::LeftScale );
  }
  else if ( alignment == GuiElement::align_Right ){
    m_sliderwidget -> setScalePosition( QwtSlider::RightScale );
  }
  else if ( alignment == GuiElement::align_Bottom ){
    m_sliderwidget -> setScalePosition( QwtSlider::BottomScale );
  }
  else if ( alignment == GuiElement::align_Top ){
    m_sliderwidget -> setScalePosition( QwtSlider::TopScale );
  }
#else
  if ( alignment == GuiElement::align_Left ||
       alignment == GuiElement::align_Top ){
    m_sliderwidget -> setScalePosition( QwtSlider::LeadingScale );
  } else if (alignment == GuiElement::align_Right ||
             alignment == GuiElement::align_Bottom) {
    m_sliderwidget -> setScalePosition( QwtSlider::TrailingScale );
  }
#endif
}

void GuiQwtSlider::valueChange(double value){
  if( !m_sliderwidget->mouseDown() )
    FinalWork();
}

/* --------------------------------------------------------------------------- */
/* FinalWork --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQwtSlider::FinalWork(){
  BUG(BugGuiFld,"GuiQwtSlider::FinalWork");

  if( isRunning() ){
     // Falls noch eine Function aktiv ist, sind keine Eingaben moeglich. Diese
     // Erfahrung machen nur schnelle Finger.
     printMessage( _("a function is aktiv"), msg_Warning );
     update( reason_Cancel );
//     if( reason == reason_Activate ){
//       FieldFocusIn(); // Zurueck auf Start
//     } else
     if(disabled()){
       protectField();
     }
     BUG_EXIT("a function is aktiv");
     return;
  }

  switch( checkFormat() ){
  case XferParameter::status_Bad:
    BUG_EXIT("checkFormat failed");
    printMessage( _("Conversion error."), msg_Warning );
    update( reason_Cancel );
    return;
  case XferParameter::status_Unchanged:
     BUG_EXIT("checkFormat unchanged");
     clearMessage();
     update( reason_Cancel );
     return;
  case XferParameter::status_Changed:
    BUG_EXIT("checkFormat changed");
    std::string error_msg;
    clearMessage();
    s_TargetsAreCleared = m_param->DataItem()->StreamTargetsAreCleared(error_msg);
    if( s_TargetsAreCleared ){ // Targets sind ok
      FinalWorkOk();
      return;
    }
    confirm(error_msg);
    return;
  }
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus  GuiQwtSlider::checkFormat(){
  BUG(BugGuiFld,"GuiQtTextfield::checkFormat");
  std::ostringstream os;
  os << m_sliderwidget->value();

  return m_param->checkFormat( os.str() );
}

/* --------------------------------------------------------------------------- */
/* FinalWorkOk --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQwtSlider::FinalWorkOk(){
  BUG(BugGuiFld,"GuiQtTextfield::FinalWorkOk");
  doFinalWork();
  s_DialogIsAktive = false;
}


// Implement this method if you need diffrent types of sliders
void GuiQwtSlider::setBgStyle(){
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQwtSlider::setInputValue(){
  BUG(BugGuiFld,"GuiQwtSlider::setInputValue");
  std::ostringstream os;
  os << m_sliderwidget->value();

  if( m_param->setFormattedValue( os.str() ) ){
    return true;
  }
  BUG_EXIT("setValue failed");
  return false;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQwtSlider::update( UpdateReason reason ){
  BUG(BugGui,"GuiQwtSlider::update");
  if( m_param == 0 || m_sliderwidget == 0 ) return;

  getAttributes();  // vergiss ja nicht diese Funktion aufzurufen!!!
  bool changed = true;

  switch( reason ){
  case reason_FieldInput:
  case reason_Process:
    break;

  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:
    setUpdated();
    break;
  default:
    BUG_MSG("Unhandled Update Reason");
    setUpdated();
    break;
  }

  // --------------------------------------------------------------
  // Zuerst wird nur der Wert geprüft.
  // --------------------------------------------------------------
  if( isUpdated() ){
    std::string text;
    m_param->getFormattedValue( text );
    BUG_MSG("Value of " << m_param->getName() << " is '" << text << "'");
    std::istringstream is( text );
    double value( 0 );
    is >> value;
    m_sliderwidget->setValue( value );
  }
}


void GuiQwtSlider::manage(){
}

double GuiQwtSlider::getValue(){
  return 0;
}

QWidget* GuiQwtSlider::myWidget(){
  return m_sliderwidget;
}

void GuiQwtSlider::serializeXML(std::ostream &os, bool recursive){
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQwtSlider::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  bool updated = GuiQtDataField::serializeJson(jsonObj, onlyUpdated);
  return updated;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQwtSlider::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_data_fields();
  bool updated = GuiQtDataField::serializeProtobuf(element, onlyUpdated);
  return updated;
}
#endif
