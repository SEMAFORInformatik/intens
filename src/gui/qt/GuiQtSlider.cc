
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "GuiQtSlider.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtSlider::GuiQtSlider( GuiElement *parent )
  : GuiQtDataField( parent ),
    m_sliderwidget( 0 )
{
}

GuiQtSlider::GuiQtSlider( const GuiQtSlider &slider )
  : GuiQtDataField( slider )
  , m_sliderwidget( 0 )
{}

GuiQtSlider::~GuiQtSlider(){
  delete m_sliderwidget;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
void GuiQtSlider::setRange( double min, double max, double stepsize ){
  if ( m_sliderwidget == 0 ) return;
  m_sliderwidget -> setRange( min, max );
  m_sliderwidget -> setSingleStep(stepsize);
}

// bool GuiQtSlider::setLabel( const std::string &name ){
//   m_label = name;
//   m_label_pixmap = false;
//   return true;
// }

// bool GuiQtSlider::setPixmap( const std::string &name ){
//   m_label = name;
//   m_label_pixmap = true;
//   return true;
// }

void GuiQtSlider::create(){
  getAttributes();
  m_sliderwidget = new QtSlider( getParent()->getQtElement()->myWidget(), this );
  m_sliderwidget->setEnabled( isEditable() );
  m_sliderwidget->setValue( 0 );
  // for demonstration, should be removed
  m_sliderwidget -> setOrientation( Qt::Horizontal );
  m_sliderwidget -> setTickPosition(QSlider::TicksBelow);
  setRange( Attr()->getMin()
	    , Attr()->getMax()
	    , Attr()->getStep() );
  int width = (int)floor( m_sliderwidget->sizeHint().width() * 1.5 );
  m_sliderwidget -> setMinimumWidth( width );

  updateWidgetProperty();
  m_param->DataItem()->setDimensionIndizes();

  connect( m_sliderwidget, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)) );
}

/*=============================================================================*/
/* set orientation of the slider, Horizontal or Vertical                       */
/*=============================================================================*/
void GuiQtSlider::setOrientation( GuiElement::Orientation orientation ){
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
void GuiQtSlider::setScalePosition( GuiElement::Alignment alignment ){
  if ( m_sliderwidget == 0 ) return;

  if ( alignment == GuiElement::align_Left ){
    m_sliderwidget -> setOrientation(Qt::Vertical);
    m_sliderwidget -> setTickPosition(QSlider::TicksLeft);
  }
  else if ( alignment == GuiElement::align_Right ){
    m_sliderwidget -> setOrientation(Qt::Vertical);
    m_sliderwidget -> setTickPosition(QSlider::TicksRight);
  }
  else if ( alignment == GuiElement::align_Bottom ){
    m_sliderwidget -> setOrientation(Qt::Horizontal);
    m_sliderwidget -> setTickPosition(QSlider::TicksBelow);
  }
  else if ( alignment == GuiElement::align_Top ){
    m_sliderwidget -> setOrientation(Qt::Horizontal);
    m_sliderwidget -> setTickPosition(QSlider::TicksBelow);
  }
}

void GuiQtSlider::valueChange(int value){
  if( !m_sliderwidget->mouseDown() )
    FinalWork();
}

/* --------------------------------------------------------------------------- */
/* FinalWork --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtSlider::FinalWork(){
  BUG(BugGuiFld,"GuiQtSlider::FinalWork");

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

XferParameter::InputStatus  GuiQtSlider::checkFormat(){
  BUG(BugGuiFld,"GuiQtTextfield::checkFormat");
  std::ostringstream os;
  os << m_sliderwidget->value();

  return m_param->checkFormat( os.str() );
}

/* --------------------------------------------------------------------------- */
/* FinalWorkOk --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtSlider::FinalWorkOk(){
  BUG(BugGuiFld,"GuiQtTextfield::FinalWorkOk");
  doFinalWork();
  s_DialogIsAktive = false;
}


// Implement this method if you need diffrent types of sliders
void GuiQtSlider::setBgStyle(){
}

/* --------------------------------------------------------------------------- */
/* CloneForFieldgroupTable --                                                  */
/* --------------------------------------------------------------------------- */

GuiQtDataField *GuiQtSlider::CloneForFieldgroupTable(){
  GuiQtSlider *txt = new GuiQtSlider( *this );
  return txt;
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtSlider::setInputValue(){
  BUG(BugGuiFld,"GuiQtSlider::setInputValue");
  std::ostringstream os;
  os << m_sliderwidget->value();

  m_param->DataItem()->setValue(m_sliderwidget->value());
  // if( m_param->setFormattedValue( os.str() ) ){
  //   return true;
  // }
  BUG_EXIT("setValue failed");
  return false;
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtSlider::enable(){
  setDisabled(false);
  if( m_sliderwidget == 0 ) return;

  getAttributes();
  m_sliderwidget->setEnabled( isEditable() );
  //  setColors();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtSlider::disable(){
  setDisabled(true);
  if( m_sliderwidget == 0 ) return;

  getAttributes();
  m_sliderwidget->setEnabled( false );
  //setColors();
}


/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtSlider::update( UpdateReason reason ){
  BUG(BugGui,"GuiQtSlider::update");
  if( m_param == 0 || m_sliderwidget == 0 ) return;

  updateWidgetProperty();
  getAttributes();  // vergiss ja nicht diese Funktion aufzurufen!!!

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
  if( GuiDataField::isUpdated() ){
    std::string text;
    m_param->getFormattedValue( text );
    BUG_MSG("Value of " << m_param->getName() << " is '" << text << "'");
    std::istringstream is( text );
    double value( 0 );
    is >> value;
    m_sliderwidget->setValue( value );
  }
}


void GuiQtSlider::manage(){
}

double GuiQtSlider::getValue(){
  return 0;
}

QWidget* GuiQtSlider::myWidget(){
  return m_sliderwidget;
}

void GuiQtSlider::serializeXML(std::ostream &os, bool recursive){
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtSlider::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  bool updated = GuiQtDataField::serializeJson(jsonObj, onlyUpdated);
  return updated;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtSlider::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_data_fields();
  bool updated = GuiQtDataField::serializeProtobuf(element, onlyUpdated);
  return updated;
}
#endif
