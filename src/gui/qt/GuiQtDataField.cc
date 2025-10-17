
#include <limits>
#include <iomanip>
#include <qvalidator.h>
#include <qlineedit.h>
#include <qcolor.h>
#if defined Q_OS_WIN || defined Q_OS_CYGWIN || defined  Q_OS_UNIX
#include "gui/qt/KNumValidator.h"
#include "gui/qt/DateValidator.h"
#endif

#include "utils/Debugger.h"

#include "utils/gettext.h"
#include "utils/NumLim.h"
#include "gui/qt/GuiQtDataField.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/GuiIndex.h"
#include "job/JobManager.h"
#include "xfer/XferDataParameter.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "xfer/XferParameterComplex.h"
#include "xfer/XferParameterString.h"

INIT_LOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtDataField::GuiQtDataField( GuiElement *parent, const std::string name )
  : GuiQtElement( parent, name )
  , m_length( 0 ){
}

GuiQtDataField::GuiQtDataField( const GuiQtDataField &field )
  : GuiQtElement( field )
  , GuiDataField( field )
  , m_length( field.m_length ){
  registerIndex();
}

GuiQtDataField::~GuiQtDataField(){
  if (m_param) {
    unregisterIndex();
    delete m_param;
  }
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtDataField::acceptIndex( const std::string &name, int inx ){
  assert( m_param != 0 );
  assert( m_param -> DataItem() != 0 );
  if( !m_param->DataItem()->acceptIndex( name, inx ) ){
    BUG_DEBUG("data item '" << m_param->DataItem()->getFullName(true) <<
              "' does not accept index.");
    return false;
  }
  if( !m_param->acceptIndex( name, inx ) ){
    BUG_DEBUG("item '" << m_param->DataItem()->getFullName(true) <<
              "' does not accept index.");
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::setIndex( const std::string &name, int inx ){
  BUG_DEBUG( "setIndex " << name <<
             " =[" << inx << "] vn[" <<
             m_param->DataItem()->getFullName(true) <<"]");

  assert( m_param != 0 );
  assert( inx >= 0 );

  bool set = m_param->DataItem()->setIndex( name, inx );
  if( m_param->setIndex( name, inx ) ){
    set = true;
  }
  if( set ){
    update( reason_Always );
  }
}

/* --------------------------------------------------------------------------- */
/* getNamedColors --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtDataField::getNamedColors( const std::string &_background,
                                     const std::string &_foreground,
                                     QColor &background,
                                     QColor &foreground ){
  background = QColor( QString::fromStdString(_background) );
  foreground = QColor( QString::fromStdString(_foreground) );
  return ( background.isValid() && foreground.isValid() );
}

/* --------------------------------------------------------------------------- */
/* getColorString --                                                           */
/* --------------------------------------------------------------------------- */

std::string GuiQtDataField::getColorString( QColor &color ){
  if( !color.isValid() ){
    return "";
  }
  int r, g, b;
  color.getRgb( &r, &g, &b );
  std::stringstream str;
  str << "#" << std::setfill('0')
      << std::setw(2) << std::hex << r
      << std::setw(2) << std::hex << g
      << std::setw(2) << std::hex << b;
  return str.str();
}

/* --------------------------------------------------------------------------- */
/* getColorStrings --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::getColorStrings( std::string &background,
                                      std::string &foreground ){
  QColor bg, fg;
  int dark_fac;
  if( getTheColor( bg, fg, dark_fac ) ){
    background = getColorString( bg );
    foreground = getColorString( fg );
  }
}

/* --------------------------------------------------------------------------- */
/* setMyColors --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::setMyColors(){
  std::string resname( m_param->DataItem()->getFullName(false) + ".Background" );
  m_background = GuiQtManager::Instance().readResourceColor( resname );
  resname = m_param->DataItem()->getFullName(false) + ".Foreground";
  m_foreground = GuiQtManager::Instance().readResourceColor( resname );
  if( m_foreground != m_background ){
    return; // ok
  }

  resname = m_param->DataItem()->getName() + ".Background";
  m_background = GuiQtManager::Instance().readResourceColor( resname );
  resname = m_param->DataItem()->getName() + ".Foreground";
  m_foreground = GuiQtManager::Instance().readResourceColor( resname );
}

/* --------------------------------------------------------------------------- */
/* getMyColors --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtDataField::getMyColors( QColor &background, QColor &foreground ){
  if( m_foreground != m_background ){
    background = m_background;
    foreground = m_foreground;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getColors --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtDataField::getTheColor( QColor &background,
				  QColor &foreground,
				  int &dark_fac,
				  bool useColorSetOnly ){
  BUG_DEBUG( "getColors" );
  bool found = false;
  dark_fac = 115;
  getAttributes();

  if (getStandardColor(*m_param, background, foreground, useColorSetOnly))
    return true;
  if( !useColorSetOnly ){

    if( getMyColors( background, foreground ) ){
      BUG_DEBUG("Return: My Color");
      return true;
    }

    if( isLockable() ){
      // Ein Item mit Sperrmöglichkeit (lockable) hat immer
      // eine andere Farbe.
      if( !isEditable() ){
        background = GuiQtManager::readonlyLBackgroundColor();
        foreground = GuiQtManager::readonlyLForegroundColor();
        return true;
      }
      if( isOptional() ){
        background = GuiQtManager::optionalLBackgroundColor();
        foreground = GuiQtManager::optionalLForegroundColor();
        return true;
      }
      background = GuiQtManager::editableLBackgroundColor();
      foreground = GuiQtManager::editableLForegroundColor();
      return true;
    }

    if( !isEditable() ){
      if( isTypeLabel() ){
        background = GuiQtManager::fieldgroupBackgroundColor(); //backgroundColor();
        foreground = GuiQtManager::fieldgroupForegroundColor(); //foregroundColor();
        BUG_DEBUG("Return: Label Color");
      }
      else{
        background = GuiQtManager::readonlyBackgroundColor();
        foreground = GuiQtManager::readonlyForegroundColor();
        dark_fac = 100;
        BUG_DEBUG("Return: ReadOnly Color");
      }
      return false;
    }
    if( isOptional() ){
      background = GuiQtManager::optionalBackgroundColor();
      foreground = GuiQtManager::optionalForegroundColor();
      return true;
    }
    background = GuiQtManager::editableBackgroundColor();
    foreground = GuiQtManager::editableForegroundColor();
    if( foreground == background ){
      BUG_DEBUG("Background and Foreground are equal");
    }
    BUG_DEBUG("Return: Editable Color");
    return false;
  }
  BUG_DEBUG("Return: No Color");
  return false;
}

/* --------------------------------------------------------------------------- */
/* getStandardColor --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtDataField::getStandardColor(XferDataParameter& param, QColor& background, QColor& foreground, bool useColorSetOnly){
  // If individual colours have been set, these take priority over
  // the standard colours. This colour scheme complies with the hierarchy.
  DATAAttributeMask attr_mask = param.getAttributes( GuiManager::Instance().LastGuiUpdate() );
  if( ( attr_mask & DATAuseColorset ) != 0 ){
    ColorSet* colorset = DataPoolIntens::Instance().getColorSet(param.DataItem()->getUserAttr()->ColorsetName());
    std::string _foreground, _background;
    if( colorset &&
        GuiDataField::getColorsetColor(param, colorset,  _background, _foreground ) ){
      if( getNamedColors( _background, _foreground,
                          background, foreground ) ){ // a modifiable color found
        return true;
      }
    }
  }
  if( useColorSetOnly ){
    return false;
  }

  // Sobald ein Item gesperrt ist (locked), gibt es nur eine
  // Möglichkeit der Farbgebung.
  if( attr_mask & DATAisLocked ){
    background = GuiQtManager::lockedBackgroundColor();
    foreground = GuiQtManager::lockedForegroundColor();
    background = background.lighter(115);
    BUG_DEBUG("Return: Locked Color");
    return true;
  }
  // Alarm durch eine fehlerhafte Eingabe
  if( attr_mask & DATAcolorAlarm ){
    background = GuiQtManager::alarmBackgroundColor();
    foreground = GuiQtManager::alarmForegroundColor();
    BUG_DEBUG("Return: Alarm Color");
    return true;
  }

  bool isColorBit = param.DataItem()->getUserAttr()->IsColorBit();
  if (!isColorBit) {
    int color=0;
    if (attr_mask & DATAcolor1) color += 1;
    if (attr_mask & DATAcolor2) color += 2;
    if (attr_mask & DATAcolor3) color += 4;
    if (attr_mask & DATAcolor4) color += 8;
    if (attr_mask & DATAcolor5) color += 16;
    if (attr_mask & DATAcolor6) color += 32;
    if (attr_mask & DATAcolor7) color += 64;
    if (attr_mask & DATAcolor8) color += 128;
    BUG_DEBUG( "NO_COLOR_BIT get color number  1[" <<
               (attr_mask & DATAcolor1)<< "] " <<
               " 2["<<(attr_mask & DATAcolor2)<<"] " <<
               " 3["<<(attr_mask & DATAcolor3)<<"] " <<
               " 4["<<(attr_mask & DATAcolor4)<<"] " <<
               " 5["<<(attr_mask & DATAcolor5)<<"] " <<
               " 6["<<(attr_mask & DATAcolor6)<<"] " <<
               " 7["<<(attr_mask & DATAcolor7)<<"] " <<
               " 8["<<(attr_mask & DATAcolor8)<<"] " <<
               "=> color '" << color <<
               "' Varname '" << param.DataItem()->getFullName(true) <<
               "'");
    if( color > 0 ){
      background = GuiQtManager::colorBackgroundColor(color);
      foreground = GuiQtManager::colorForegroundColor(color);
      BUG_DEBUG("Return: Individual Background Color");
      return true;
    }
  }
  if( attr_mask & DATAcolor1 ){
    background = GuiQtManager::color1BackgroundColor();
    foreground = GuiQtManager::color1ForegroundColor();
    return true;
  }
  if( attr_mask & DATAcolor2 ){
    background = GuiQtManager::color2BackgroundColor();
    foreground = GuiQtManager::color2ForegroundColor();
    return true;
  }
  if( attr_mask & DATAcolor3 ){
    background = GuiQtManager::color3BackgroundColor();
    foreground = GuiQtManager::color3ForegroundColor();
    return true;
  }
  if( attr_mask & DATAcolor4 ){
    background = GuiQtManager::color4BackgroundColor();
    foreground = GuiQtManager::color4ForegroundColor();
    return true;
  }
  if( attr_mask & DATAcolor5 ){
    background = GuiQtManager::color5BackgroundColor();
    foreground = GuiQtManager::color5ForegroundColor();
    return true;
  }
  if( attr_mask & DATAcolor6 ){
    background = GuiQtManager::color6BackgroundColor();
    foreground = GuiQtManager::color6ForegroundColor();
    return true;
  }
  if( attr_mask & DATAcolor7 ){
    background = GuiQtManager::color7BackgroundColor();
    foreground = GuiQtManager::color7ForegroundColor();
    return true;
  }
  if( attr_mask & DATAcolor8 ){
    background = GuiQtManager::color8BackgroundColor();
    foreground = GuiQtManager::color8ForegroundColor();
    return true;
  }

  return false;
}

/* --------------------------------------------------------------------------- */
/* doFinalWork --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::doFinalWork(){
  BUG_DEBUG( "Begin doFinalWork()" );

  JobFunction *func = getFunction();
  GuiQtDataField::Trigger *trigger = 0;
  bool logMode = AppData::Instance().PyLogMode();
  std::string logFunction;

  if (logMode && func) {
    logFunction = compose(PYLOG_CALL_PROCESS, func->Name(), "Function");
    func = 0;
  }
  if( func != 0 ){
    BUG_DEBUG("Function " << func->Name() << " is available");
    // Zu diesem Zeitpunkt darf keine Funktion laufen, sonst haben wir
    // ein grosses Problem.
    if( !setRunningMode() ){
      BUG_DEBUG("End doFinalWork(): function is already running." );
      // assert( false );
      return;
    }

    trigger = new Trigger( this, func );
    // special case: we block multiple GuiUpdate
    // and do afterward a single GuiUpdate
    // used by paste function in a Table or List
    bool updateForms = func->getUpdateForms();
    if (!updateForms) {
      trigger->setUpdateForms( false );
      trigger->setSilentFunction();
    }

    // Der Wert vor der Eingabe (OLDVALUE) muss als erstes gespeichert werden
    // bevor der neue Wert in den Datapool geschrieben wird.
    trigger->setDataItem( m_param->DataItem() );
    trigger->startTransaction();

    if( !setInputValue() ){
      BUG_DEBUG("FATAL: setValue failed");
      printMessage( _("Conversion error."), msg_Warning );
    }

    trigger->startJob();
    // Hier geben wir die Kontrolle ab.
    // Wir werden aber mit prepareToStartJob() noch einmal aufgerufen,
    // bevor es dann richtig losgeht!
    // Nach Beendigung der Function gehts mit doEndOfWork() weiter.
    // Der Aufruf erfolgt durch den Trigger. Der Trigger
    // löscht sich am Ende selbständig.
  }
  else{
    BUG_DEBUG("No function available");
    if( !logMode && !s_TargetsAreCleared ){
      // Alle abhängigen Daten werden nun gelöscht.
      m_param->DataItem()->clearTargetStreams();
      s_TargetsAreCleared = true;
    }

    // Der Eingabewert wird nun in den Datapool geschrieben. Diese Aktion
    // sollte immer erfolgreich sein, sonst ist etwas oberfaul. Das Eingabe
    // muss zu diesem Zeitpunkt bereits geprüft worden sein.
    BUG_DEBUG("Eingabewert setzen");
    if( !setInputValue() ){
      BUG_DEBUG("FATAL: setValue failed");
      printMessage( _("Conversion error."), msg_Warning );

      if( !isEditable() ){
	protectField();
      }
      s_DialogIsAktive = false;
      EndOfWorkFinished();
      return;
    }

    if(!logFunction.empty()) {
      PYLOG_INFO(logFunction);
    }
    if(logMode) {
      PYLOG_INFO("");
    }

    // Wenn keine JobFunction auszuführen ist, so ist die Sache schnell
    // erledigt.
    if (GuiUpdate() && !AppData::Instance().HeadlessWebMode()){
      updateForms(logMode ? reason_Cancel : reason_FieldInput );
    }
    if(disabled()){
      protectField();
    }
    s_DialogIsAktive = false;
    EndOfWorkFinished();
    return;
  }

  BUG_DEBUG( "End doFinalWork()" );
}

/* --------------------------------------------------------------------------- */
/* EndOfWorkFinished --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::EndOfWorkFinished(){
  BUG_DEBUG( "EndOfWorkFinished");
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtDataField::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG_DEBUG( "hasChanged" );
  if (Attr()->IsDbTransient()) {
    BUG_DEBUG("item '"<<m_param->DataItem()->getFullName(true)<<"' is db transient.");
    return false;
  }
  if (m_param->DataItem()->isAncestorDbTransient()) {
    BUG_DEBUG("ancestor of item '"<<m_param->DataItem()->getFullName(true)<<"' is db transient.");
    return false;
  }

  if ( !xfer || (xfer && xfer->isAncestorOf( m_param->DataItem() )) ){
    if (m_param->DataItem()->isUpdated(trans)) {
      BUG_DEBUG("item '"<<m_param->DataItem()->getFullName(true)<<"' has Changed.");
      return true;
    }
  }
  BUG_DEBUG("item '"<<m_param->DataItem()->getFullName(true)<<"' noChanges.");
  return false;
}

/* --------------------------------------------------------------------------- */
/* setValidator --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::setValidator( QLineEdit *editor ){
  if( !editor )
    return;
  double min = m_param->DataItem()->getUserAttr()->getMin();
  double max = m_param->DataItem()->getUserAttr()->getMax();
  switch(m_param->DataItem()->getDataType() ) {
  case DataDictionary::type_Integer:
    {
      KIntValidator* validator = new KIntValidator( editor );
      if (min != std::numeric_limits<double>::min() ||
          max != std::numeric_limits<double>::max() ) {
        int imax = max == std::numeric_limits<double>::max() ? std::numeric_limits<int>::max() :
          floor(0.5+max);
        int imin = min == std::numeric_limits<double>::min() ? std::numeric_limits<int>::min() :
          floor(0.5+min);
       	validator->setRange(imin, imax);
      }
      editor->setValidator(  validator );
      break;
    }
  case DataDictionary::type_Real:
    {
      KDoubleValidator * validator;
      validator = new KDoubleValidator( editor, "", m_param->getScalefactor() );
      if (min != std::numeric_limits<double>::min() ||
          max != std::numeric_limits<double>::max() ) {
        validator->setRange(min, max);
      }
      editor->setValidator( validator );
    }

    break;
  case DataDictionary::type_Complex:
    {
      char radix_point = RealConverter::decimalPoint();
      char real_img_delimeter = radix_point == ',' ? ';'  : ',';
      std::string regexp( compose("\\({0,1}[1-9]+\\%1{0,1}[1-9]*\\%2[1-9]+\\%1{0,1}[1-9]*\\){0,1}",
                                  radix_point, real_img_delimeter) );
      QRegularExpression rx( QString::fromStdString(regexp) );
      editor->setValidator( new QRegularExpressionValidator( rx, editor) );
      break;
    }
  case DataDictionary::type_String:
    if (m_param->DataItem()->getUserAttr()->StringType() == UserAttr::string_kind_date) {
      editor->setValidator( new DateValidator( this, editor ) );
      break;
    } else if (m_param->DataItem()->getUserAttr()->Pattern().size()) {
      QRegularExpression rx( QString::fromStdString( m_param->DataItem()->getUserAttr()->Pattern() ) );
      editor->setValidator( new QRegularExpressionValidator( rx, editor) );
    }
  default:
    // kein validator
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtDataField::setLength( int len ){
  m_length=abs(len);
  return true;
}

/* --------------------------------------------------------------------------- */
/* getLength --                                                                */
/* --------------------------------------------------------------------------- */

int GuiQtDataField::getLength(){
  return m_length;
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::serializeXML(std::ostream &os, bool recursive){
  GuiDataField::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtDataField::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiDataField::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtDataField::serializeProtobuf(in_proto::DataField* field, bool onlyUpdated){
  return GuiDataField::serializeProtobuf(field, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::getVisibleDataPoolValues( GuiValueList& vmap ) {
  GuiDataField::getVisibleDataPoolValues( vmap );

  if (m_param) {
    std::string::size_type posA, posE;
    std::string s;
    m_param->getFormattedValue( s );
    posA = s.find_first_not_of(" ");
    if (posA == std::string::npos)  posA = 0;
    posE = s.find_last_not_of(" ");
    if (posE == std::string::npos)  posE = s.length();

    vmap.insert( GuiValueList::value_type(m_param->getFullName(), s.substr(posA, posE+1)) );
  }
}

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtDataField::updateWidgetProperty() {
  UserAttr *userAttr = m_param->DataItem()->getUserAttr();
  if (!(userAttr->Classname().empty() && userAttr->Classname(m_param->getFullName()).empty())) {
    if (Class() != userAttr->Classname(m_param->getFullName())) {
      BUG_INFO("UserAttr not found add cn: " << userAttr->Classname() << " to c:" << Class());
      setClass(userAttr->Classname(m_param->getFullName()));
    }
  }
  GuiQtElement::updateWidgetProperty();
}
