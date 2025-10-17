#include <qstringlist.h>
#include <qsettings.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QEnterEvent>

#include "utils/Debugger.h"

#include "utils/gettext.h"
#include "utils/StringUtils.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtToggle.h"
#include "app/DataPoolIntens.h"

INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtToggle::GuiQtToggle( GuiElement *parent )
  : GuiQtDataField( parent )
  , m_toggle( 0 ) {
  setLength(1); /* means dafault is visible */
}

GuiQtToggle::GuiQtToggle( const GuiQtToggle &toggle )
  : GuiQtDataField( toggle )
  , m_toggle( 0 )
{}

GuiQtToggle::~GuiQtToggle() {
  delete m_toggle;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
// void GuiQtToggle::create( Widget parent ){
void GuiQtToggle::create(){
  BUG(BugGui,"GuiQtToggle::create");

  getAttributes();

  QWidget *parent = getParent() ? getParent()->getQtElement()->myWidget() :0;
  assert( m_toggle == 0 );

  m_toggle = new MyQCheckBox( parent, this );
  m_toggle->setEnabled( isEditable() );
  m_toggle->setMaximumSize( m_toggle->sizeHint() );

  connect( m_toggle, SIGNAL(toggled(bool)), this, SLOT(valueChanged(bool)) );

  updateWidgetProperty();

  setColors();

  m_param->DataItem()->setDimensionIndizes();

  return;
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget *GuiQtToggle::myWidget(){
  return m_toggle;
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtToggle::destroy(){
  BUG(BugGui,"GuiQtToggle::destroy");
  if( m_toggle != 0 ){
    m_toggle->deleteLater();
    m_toggle = 0;
    return true;
  }
  BUG_EXIT("no widget");
  return false;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::getSize( int &w, int &h ){
  if( m_toggle == 0 ) {
    w = h = 0;
    return;
  }
  w = m_toggle->width();
  h = m_toggle->height();
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::enable(){
  setDisabled(false);
  if( m_toggle == 0 ) return;

  getAttributes();
  m_toggle->setEnabled( isEditable() );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::disable(){
  setDisabled(true);
  if( m_toggle == 0 ) return;

  getAttributes();
  m_toggle->setEnabled( false );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::update( UpdateReason reason ){
  BUG(BugGui,"GuiQtToggle::update");
  if( m_param == 0 || m_toggle == 0 ) return;
  updateWidgetProperty();
  bool changed = getAttributes();
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
    double value = 0.0;
    bool state = false;
    if( m_param->DataItem()->getValue( value ) ){
      if( value != 0 ){
	state = true;
      }
    }

    m_toggle->blockSignals( true );
    m_toggle->setChecked( state );
    m_toggle->blockSignals( false );

    if( useColorSet() ){
      changed = true;
    }
  }
  // --------------------------------------------------------------
  // Falls keine Attribute (exl. Updated) geaendert haben, haben
  // wir hier nicht mehr zu tun.
  // --------------------------------------------------------------
  if( !changed ){
    return;
  }
  // --------------------------------------------------------------
  // Nun wird Editable und TraversalOn neu gesetzt.
  // --------------------------------------------------------------

  m_toggle->setEnabled( isEditable() );

  // --------------------------------------------------------------
  // Als letztes werden die Farben neu gesetzt.
  // --------------------------------------------------------------
  setColors();
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtToggle::setScalefactor( Scale *scale ){
  return m_param->setScalefactor( scale );
}

/* --------------------------------------------------------------------------- */
/* CloneForFieldgroupTable --                                                  */
/* --------------------------------------------------------------------------- */

GuiQtDataField *GuiQtToggle::CloneForFieldgroupTable(){
  GuiQtToggle *txt = new GuiQtToggle( *this );
  return txt;
}

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::setColors(){
  QColor background, foreground;
  int dark_fac;
  getTheColor( background, foreground, dark_fac );

  // override qt default colors and make all possible sytles happy
  QPalette pal=  m_toggle->palette();
  pal.setColor(QPalette::Button,       background );
  if ( !(useColorSet() ||
      m_attr_mask & DATAcolor1 || m_attr_mask & DATAcolor2 || m_attr_mask & DATAcolor3 || m_attr_mask & DATAcolor4 ||
      m_attr_mask & DATAcolor5 || m_attr_mask & DATAcolor6 || m_attr_mask & DATAcolor7 || m_attr_mask & DATAcolor8)   ) {
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, GuiQtManager::buttonTextColor().darker(115) );
    pal.setColor(QPalette::Disabled, QPalette::Text, GuiQtManager::buttonTextColor().darker(115) );
  }
  pal.setColor(QPalette::Base,         background );
  pal.setColor(QPalette::Window,       background );
  pal.setColor(QPalette::WindowText,   foreground );
  pal.setColor(m_toggle->backgroundRole(),   background );
  pal.setColor(m_toggle->foregroundRole(), foreground );

  pal.setColor(QPalette::Disabled, m_toggle->backgroundRole(),   background.darker(115) );
  pal.setColor(QPalette::Disabled, QPalette::Base, background.darker(115) );
//   pal.setColor(QPalette::Disabled, QPalette::Window, background.darker(115) );

  // a very special case this QCheckBox
  m_toggle->setPalette(pal);
}

/* --------------------------------------------------------------------------- */
/* setAlarmColors --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::setAlarmColors(){
//   // ---------------------------------------------------------------------
//   // Dieses Attribute sorgt dafür, dass beim nächsten update() die Alarm-
//   // Color wieder verschwindet. Dieses Bit wird nur lokal in diesem Objekt
//   // gesetzt und landet nicht im Datapool.
//   // ---------------------------------------------------------------------
  setAttribute( DATAcolorAlarm );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtToggle::setInputValue(){
  BUG(BugGui,"GuiQtToggle::setInputValue");
  double value = 0.0;
  if( m_toggle->isChecked() ){
    value = 1.0;
  }
  DES_INFO("  " << m_param->DataItem()->getFullName(true)  << " = " << value << ";  // Toggle");
  if (AppData::Instance().PyLogMode()) {
    PYLOG_INFO(compose(PYLOG_SETVALUE, removeString2(m_param->DataItem()->getFullName(true), "[0]"), value));
    return true;
  }
  m_param->DataItem()->setValue( value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* FinalWork --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::FinalWork(){
  BUG(BugGui,"GuiQtToggle::FinalWork");
  if( isRunning() ){
    // Falls noch eine Function aktiv ist, sind keine Eingaben möglich. Diese
    // Erfahrung machen nur schnelle Finger.
      // strange error with xgettext here (Non-ASCII string)
      //printMessage( _("a function is aktiv."), msg_Warning );
    update( reason_Cancel );
    BUG_EXIT("a function is aktiv");
    return;
  }
  std::string error_msg;
  s_TargetsAreCleared = m_param->DataItem()->StreamTargetsAreCleared(error_msg);
  if( s_TargetsAreCleared ){ // Targets sind ok
    FinalWorkOk();
    return;
  }
  confirm(error_msg);
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::confirmYesButtonPressed(){
  FinalWorkOk();
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::confirmNoButtonPressed(){
  update( reason_Cancel );
  s_DialogIsAktive = false;
}

/* --------------------------------------------------------------------------- */
/* FinalWorkOk --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtToggle::FinalWorkOk(){
  doFinalWork();
  s_DialogIsAktive = false;
}

/* --------------------------------------------------------------------------- */
/* ValueChanged --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtToggle::valueChanged( bool state ){
  BUG(BugGui,"GuiQtToggle::ValueChanged");
  m_toggle->setFocus();
  FinalWork();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtToggle::serializeXML(std::ostream &os, bool recursive){
	os << "<intens:Toggle ";
	GuiDataField::serializeXML(os, recursive);
	os << "</intens:Toggle>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtToggle::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdate){
  return GuiQtDataField::serializeProtobuf(eles->add_data_fields(), onlyUpdate);
}
#endif

/* --------------------------------------------------------------------------- */
/* MyQCheckBox --                                                                */
/* --------------------------------------------------------------------------- */
GuiQtToggle::MyQCheckBox::MyQCheckBox( QWidget* parent, GuiQtElement *e )
  : QCheckBox( parent ), m_element( e ) {
  setContentsMargins(0,0,0,0);
  assert( e != 0 );
}

void GuiQtToggle::MyQCheckBox::enterEvent ( QEnterEvent *e )
{
  m_element->enterEvent( e );
  QCheckBox::enterEvent( e );
}

void GuiQtToggle::MyQCheckBox::leaveEvent ( QEvent *e )
{
  m_element->clearMessage();
  QCheckBox::leaveEvent( e );
}

// dies ist eine Kopie der orginal Methode mit der Ausnahme, dass
// bei der drawControl Methode als Argument ein PushButton ist
void GuiQtToggle::MyQCheckBox::paintEvent ( QPaintEvent *e ) {
#if QT_VERSION   >= 0x040300 // erst ab 4.4
  if (m_element->Type() == GuiElement::type_Toggle)
    QCheckBox::paintEvent(e);
  else {
    QStylePainter p(this);
    QStyleOptionButton opt;
    QCheckBox::initStyleOption(&opt);
    p.drawControl(QStyle::CE_RadioButton, opt);
    // Originalaufruf   p.drawControl(QStyle::CE_CheckBox, opt);
  }
#else
    QCheckBox::paintEvent(e);
#endif
}
