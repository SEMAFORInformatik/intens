
#include <iostream>
#include <limits>

#include "utils/Debugger.h"

#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"
#include "gui/GuiDialog.h"
#include "gui/GuiDataField.h"
#include "gui/GuiIndex.h"
#include "gui/GuiScrolledText.h"
#include "gui/Timer.h"
#include "utils/gettext.h"
#include "utils/HTMLConverter.h"
#include "utils/StringUtils.h"
#include "utils/base64.h"
#include "utils/JsonUtils.h"
#include "xfer/XferDataParameter.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "xfer/XferParameterComplex.h"
#include "xfer/XferParameterString.h"
#include "app/DataPoolIntens.h"
#include "app/ColorSet.h"
#include "job/JobManager.h"

bool GuiDataField::s_DialogIsAktive = false;
bool GuiDataField::s_TargetsAreCleared = false;
const int GuiDataField::s_defaultFieldLength = 8;


INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiDataField::GuiDataField() :
  m_param( 0 )
  , m_paramInputStructFunc( 0 )
  , m_paramInputStructFocusFunc( 0 )
  , m_attr_mask( 0 )
  , m_colorset( 0 )
  , m_isInverted( false )
  , m_webApiAttrChanged(false)
  , m_disabled( false )
  , m_guiUpdate(true)
  , m_running_key( 0 )
{
   m_timer=GuiFactory::Instance()->createTimer( 0 );
   m_task = new  MyTimerTask(this);
   m_timer->addTask( m_task );
}

GuiDataField::GuiDataField( const GuiDataField &field )
  : m_param( 0 )
  , m_paramInputStructFunc( 0 )
  , m_paramInputStructFocusFunc( 0 )
  , m_attr_mask( 0 )
  , m_colorset( field.m_colorset )
  , m_isInverted( field.m_isInverted )
  , m_webApiAttrChanged(field.m_webApiAttrChanged)
  , m_running_key( 0 )
  , m_guiUpdate(field.m_guiUpdate)
  , m_disabled( field.m_disabled ){
  m_param = field.m_param->clone();
  m_timer=GuiFactory::Instance()->createTimer( 0 );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
}

GuiDataField::~GuiDataField(){
  if (m_timer) delete m_timer;
  if (m_task) delete m_task;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* installDataItem --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiDataField::installDataItem( XferDataItem *dataitem ){
  assert( dataitem != 0 );
  assert( m_param == 0 );
  DataReference *data = dataitem->Data();
  assert( data != 0 );

  switch( data->getDataType() ){
  case DataDictionary::type_Integer:
    m_param = new XferParameterInteger( 0, 0, 0 );
    break;

  case DataDictionary::type_Real:
    m_param = new XferParameterReal( 0, 0, -1 , 0, false );
    break;

  case DataDictionary::type_Complex:
    switch( dataitem->getParamType() ){
    case XferDataItem::param_Default:
      m_param = new XferParameterComplex( 0, 0, -1, 0, false );
      break;
    case XferDataItem::param_ComplexReal:
    case XferDataItem::param_ComplexImag:
    case XferDataItem::param_ComplexAbs:
    case XferDataItem::param_ComplexArg:
      m_param = new XferParameterReal( 0, 0, -1 , 0, false );
      break;
    default:
      assert( false );
    }
    break;

  case DataDictionary::type_String:
    m_param = new XferParameterString( 0, 0, '\0' );
    break;
  case DataDictionary::type_CharData:
    m_param = new XferParameterString();
    break;

  default:
    return false;
  }
  m_param->setDataItem( dataitem );
  registerIndex();
  return true;
}

/* --------------------------------------------------------------------------- */
/* replaceDataItem --                                                          */
/* --------------------------------------------------------------------------- */

XferDataItem *GuiDataField::replaceDataItem( XferDataItem *dataitem ){
  assert( dataitem != 0 );
  assert( m_param != 0 );

  XferDataItem *old_dataitem = m_param->DataItem();
  assert( old_dataitem != 0 );
  m_param->resetDataItem();
  delete m_param;
  m_param = 0;

  if( !installDataItem( dataitem ) ){
    assert( false );
  }
  return old_dataitem;
}

/* --------------------------------------------------------------------------- */
/* newGuiDataItemIndex --                                                      */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *GuiDataField::newDataItemIndex( int level ){
  return m_param->DataItem()->newDataItemIndex( level );
}

/* --------------------------------------------------------------------------- */
/* newGuiDataItemIndex --                                                      */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *GuiDataField::newDataItemIndex(){
  return m_param->DataItem()->newDataItemIndex();
}

/* --------------------------------------------------------------------------- */
/* getNumOfWildcards --                                                        */
/* --------------------------------------------------------------------------- */

int GuiDataField::getNumOfWildcards(){
  return m_param->DataItem()->getNumOfWildcards();
}

/* --------------------------------------------------------------------------- */
/* getDataItemIndexWildcard --                                                 */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *GuiDataField::getDataItemIndexWildcard( int num ){
  return m_param->DataItem()->getDataItemIndexWildcard( num );
}

/* --------------------------------------------------------------------------- */
/* FieldIsEditable --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiDataField::FieldIsEditable(){
  return Attr()->IsEditable();
}

/* --------------------------------------------------------------------------- */
/* FieldIsLockable --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiDataField::FieldIsLockable(){
  return Attr()->IsLockable();
}

/* --------------------------------------------------------------------------- */
/* FieldIsLocked --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiDataField::FieldIsLocked(){
  return m_param->Data()->GetItemLocked_PreInx();
}

/* --------------------------------------------------------------------------- */
/* registerIndex --                                                            */
/* --------------------------------------------------------------------------- */

void GuiDataField::registerIndex(){
  assert( m_param != 0 );
  XferDataItem *dataitem = m_param->DataItem();
  XferDataItemIndex *itemindex = 0;
  int i = 0;
  GuiElement *element = 0;
  GuiIndex *index = 0;

  while( (itemindex=dataitem->getDataItemIndexIndexed(++i)) != 0 ){
    element = GuiElement::findElement( itemindex->IndexName() );
    // Lokale GuiIndex Elemente in einer Fieldgroup sind im GuiManager nicht
    // registiert und muessen nicht beachtet werden. Die GuiFieldgroup ueber-
    // nimmt diese Arbeit.
    if( element != 0 ){
      assert( element->Type() == GuiElement::type_Index );
      index = element->getGuiIndex();
      index->registerIndexedElement( this->getElement() );
    }
  }
  m_param->registerIndex( this->getElement() );
}

/* --------------------------------------------------------------------------- */
/* unregisterIndex --                                                          */
/* --------------------------------------------------------------------------- */

void GuiDataField::unregisterIndex(){
  assert( m_param != 0 );
  XferDataItem *dataitem = m_param->DataItem();
  XferDataItemIndex *itemindex = 0;
  int i = 0;
  GuiElement *element = 0;
  GuiIndex *index = 0;

  while( (itemindex=dataitem->getDataItemIndexIndexed(++i)) != 0 ){
    element = GuiElement::findElement( itemindex->IndexName() );
    // Lokale GuiIndex Elemente in einer Fieldgroup sind im GuiManager nicht
    // registiert und muessen nicht beachtet werden. Die GuiFieldgroup ueber-
    // nimmt diese Arbeit.
    if( element != 0 ){
      assert( element->Type() == GuiElement::type_Index );
      index = element->getGuiIndex();
      index->unregisterIndexedElement( this->getElement() );
    }
  }
  m_param->unregisterIndex( this->getElement() );
}

/* --------------------------------------------------------------------------- */
/* confirm --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiDataField::confirm(std::string msg){
  // Wir lassen dem X-Server eine Möglichkeit noch schnell etwas zu
  // erledigen, damit es keine Kollision gibt.
  m_task->setType(MyTimerTask::task_ConfirmationDialog);
  m_task->setMessage(msg);
  m_timer->start();
}

/* --------------------------------------------------------------------------- */
/* callConfirmationDialog --                                                   */
/* --------------------------------------------------------------------------- */

void GuiDataField::callConfirmationDialog(const std::string& msg){
  s_DialogIsAktive = true;
  // Was soll das ?? das führt zu einem fürchterlichen Absturz.
  // !!GuiManager::Instance().getDialog()->setParentDialog( getElement()->getDialog() );
  BUG(BugGuiFld,"consistent check failed on data item named '"+m_param->getFullName()+"'");
  std::string eMsg = _("The results are not consistent anymore.\nDo you want to delete them?");
  if (msg.size())
    eMsg += std::string("\n\n") + msg;
  GuiFactory::Instance()->showDialogConfirmation
    ( this->getElement()
      , _("OK")
      , eMsg
      , this );
}

/* --------------------------------------------------------------------------- */
/* doInsertData --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiDataField::doInsertData( const std::string &row, const std::string &col, int inx ){
  BUG_PARA(BugGuiFld,"GuiFieldgroup::doInsertData","Index = " << inx);
  InsertFunctor func;
  m_param->DataItem()->doEditData( func, row, col, inx );
  return true;
}

/* --------------------------------------------------------------------------- */
/* doDeleteData --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiDataField::doDeleteData( const std::string &row, const std::string &col, int inx ){
  DeleteFunctor func;
  assert( m_param != 0 );
  assert( m_param->DataItem() != 0 );
  m_param->DataItem()->doEditData( func, row, col, inx );
  return true;
}

/* --------------------------------------------------------------------------- */
/* doDuplicateData --                                                          */
/* --------------------------------------------------------------------------- */
bool GuiDataField::doDuplicateData( const std::string &row, const std::string &col, int inx ){
  DuplicateFunctor func;
  m_param->DataItem()->doEditData( func, row, col, inx );
  return true;
}

/* --------------------------------------------------------------------------- */
/* doClearData --                                                              */
/* --------------------------------------------------------------------------- */
bool GuiDataField::doClearData( const std::string &row, const std::string &col, int inx ){
  ClearFunctor func;
  m_param->DataItem()->doEditData( func, row, col, inx );
  return true;
}

/* --------------------------------------------------------------------------- */
/* doPackData --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiDataField::doPackData( const std::string &row, const std::string &col, int inx ){
  PackFunctor func;
  m_param->DataItem()->doEditData( func, row, col, inx );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setRunningMode --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiDataField::setRunningMode(){
  m_running_key = JobManager::Instance().setRunningMode( m_running_key );
  return m_running_key != 0;
}

/* --------------------------------------------------------------------------- */
/* unsetRunningMode --                                                         */
/* --------------------------------------------------------------------------- */

void GuiDataField::unsetRunningMode(){
  if( m_running_key != 0 ){
    JobManager::Instance().unsetRunningMode( m_running_key );
    m_running_key = 0;
  }
}
/* --------------------------------------------------------------------------- */
/* isRunning --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiDataField::isRunning(){
  return  m_running_key != 0 ? true : JobManager::Instance().isRunning();
}

/* --------------------------------------------------------------------------- */
/* protectField --                                                             */
/* --------------------------------------------------------------------------- */

void GuiDataField::protectField(){
  m_task->setType(MyTimerTask::task_ProtectAction);
  m_timer->start();
}

/* --------------------------------------------------------------------------- */
/* getAttributes --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiDataField::getAttributes(){
  resetUpdated();

  DATAAttributeMask mask_alt = m_attr_mask;
  m_attr_mask = m_param->getAttributes( GuiManager::Instance().LastGuiUpdate() );

  // stylesheet
  bool changed;
  std::string sh = m_param->getStylesheet( GuiManager::Instance().LastGuiUpdate(), changed);
  if (sh.size() == 0)
    sh = Attr()->Stylesheet();
  if (changed || m_stylesheet != sh) {
    m_stylesheet = sh;
    return true;
  }

  return mask_alt != (( m_attr_mask | DATAisGuiUpdated ) ^ DATAisGuiUpdated);
}

/* --------------------------------------------------------------------------- */
/* getAttribute --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiDataField::getAttribute( DATAAttributeMask mask ){
  return ( m_attr_mask & mask ) != 0;
}

/* --------------------------------------------------------------------------- */
/* setAttribute --                                                             */
/* --------------------------------------------------------------------------- */

void GuiDataField::setAttribute( DATAAttributeMask mask ){
  m_attr_mask = m_attr_mask | mask;
}

/* --------------------------------------------------------------------------- */
/* resetAttribute --                                                           */
/* --------------------------------------------------------------------------- */

void GuiDataField::resetAttribute( DATAAttributeMask mask ){
  m_attr_mask = ( m_attr_mask | mask ) ^ mask;
}

/* --------------------------------------------------------------------------- */
/* setUpdated --                                                               */
/* --------------------------------------------------------------------------- */

void GuiDataField::setUpdated() {
  setAttribute( DATAisGuiUpdated );
  getElement()->ResetLastWebUpdated();  // reason_Always for webtens
}

/* --------------------------------------------------------------------------- */
/* resetUpdated --                                                             */
/* --------------------------------------------------------------------------- */

void GuiDataField::resetUpdated() {
  resetAttribute( DATAisGuiUpdated );
}

/* --------------------------------------------------------------------------- */
/* isUpdated --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiDataField::isUpdated() const {
  if (m_colorset &&
      m_colorset->isDataItemUpdated(GuiManager::Instance().LastGuiUpdate())) {
    return true;
  }
  return ( m_attr_mask & DATAisGuiUpdated ) != 0;
}

/* --------------------------------------------------------------------------- */
/* isOptional --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiDataField::isOptional() const {
  return ( m_attr_mask & DATAoptional ) != 0;
}

/* --------------------------------------------------------------------------- */
/* isLockable --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiDataField::isLockable() const {
  return ( m_attr_mask & DATAlockable ) != 0;
}

/* --------------------------------------------------------------------------- */
/* isLocked --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiDataField::isLocked() const {
  return ( m_attr_mask & DATAisLocked ) != 0;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiDataField::isValid() const {
  return ( m_attr_mask & DATAisValid ) != 0;
}

/* --------------------------------------------------------------------------- */
/* lockValue --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiDataField::lockValue() {
  //setAttribute( DATAisLocked );
  return m_param->Data()->SetItemLocked_PreInx( true );
}

/* --------------------------------------------------------------------------- */
/* unlockValue --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiDataField::unlockValue() {
  //resetAttribute( DATAisLocked );
  return m_param->Data()->SetItemLocked_PreInx( false );
}

/* --------------------------------------------------------------------------- */
/* isEditable --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiDataField::isEditable() const {
  if( m_disabled ){
    return false;
  }
  if( isLocked() ){
    return false;
  }
  // both are set (imposible, no => structure variable)
  // => collection of all UserAttributes inside Tree
  if( m_attr_mask & DATAIsEditable && m_attr_mask & DATAIsReadOnly )
    return false;
  if( m_attr_mask & DATAeditable ){
    return ( m_attr_mask & DATAIsReadOnly ) == 0;
  }
  return ( m_attr_mask & DATAIsEditable ) != 0;
}

/* --------------------------------------------------------------------------- */
/* isTypeLabel --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiDataField::isTypeLabel() const {
  return ( m_attr_mask & DATAtypeLabel ) != 0;
}

/* --------------------------------------------------------------------------- */
/* useColorSet --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiDataField::useColorSet() {
  if( ( m_attr_mask & DATAuseColorset ) == 0 ){
    return false;
  }
  if( m_colorset == 0 ){
    m_colorset = DataPoolIntens::Instance().getColorSet( Attr()->ColorsetName() );
  }
  return m_colorset != 0;
}

/* --------------------------------------------------------------------------- */
/* colorBitIsSet --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiDataField::colorBitIsSet() {
  return m_attr_mask & DATAcolorAll;
}

/* --------------------------------------------------------------------------- */
/* getColorsetColor --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiDataField::getColorsetColor( std::string &background, std::string &foreground ){
  if( !useColorSet() )
    return false;
  return getColorsetColor(*m_param, m_colorset, background, foreground);
}

/* --------------------------------------------------------------------------- */
/* getColorsetColor --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiDataField::getColorsetColor(XferDataParameter& param, ColorSet* colorset, std::string &background, std::string &foreground ){
  ColorSetItem *color = 0;
  if (!colorset)
    return false;
  if( param.DataItem()->getDataType() == DataDictionary::type_String ){
    std::string s;
    if( !param.DataItem()->getValue( s ) ){
      color = colorset->getColorItemInvalid();
    }
    else{
      color = colorset->getColorItem( s );
    }
  }
  else{
    double d;
    if( !param.DataItem()->getValue( d ) ){
      color = colorset->getColorItemInvalid();
    }
    else{
      color = colorset->getColorItem( d );
      // nothing found => try to get interpolated color
      if (!color) {
        if (colorset->getInterpolatedColors( d, background, foreground ))
          return true;
      }
    }
  }
  if( !color )
    return false;
  background = color->background();
  foreground = color->foreground();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */
void GuiDataField::getVisibleDataPoolValues( GuiValueList& vmap ) {
  if (m_param) {
    std::string::size_type posA, posE;
    std::string s;
    m_param->getFormattedValue( s );
    posA = s.find_first_not_of(" ");
    if (posA == std::string::npos)  posA = 0;
    posE = s.find_last_not_of(" ");
    if (posE == std::string::npos)  posE = s.length();
    getAttributes();
    std::string flag("false");

    //if( !isEditable(){
    if( FieldIsEditable() ){
      flag="true";
    }
    std::string value = s.substr(posA, posE+1)+"@"+flag;
    vmap.insert( GuiValueList::value_type(m_param->getFullName(), value ) );
  }
}

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */
void GuiDataField::setDisabled(bool disabled) {
  if (  m_disabled != disabled)
    setWebApiAttrChanged(true);
  m_disabled = disabled;
}


/* --------------------------------------------------------------------------- */
/* getFunction --                                                              */
/* --------------------------------------------------------------------------- */
JobFunction *GuiDataField::getFunction(){
  JobFunction* func = Attr()->getFunction();

  // no function found, looking for an parent (base) function
  if (!func && AppData::Instance().InputStructFunc()) {
    // delete old paramInputStructFunc
    if (m_paramInputStructFunc) {
      delete m_paramInputStructFunc;
      m_paramInputStructFunc = 0;
    }

    m_paramInputStructFunc =  m_param->clone();
    do {
      m_paramInputStructFunc->Data()->ConvertToParentReference();
      func = m_paramInputStructFunc->getUserAttr()->getFunction();
    } while(!func && m_paramInputStructFunc->DataItem()->getNumberOfLevels() > 1);
    if (!func) {
      delete m_paramInputStructFunc;
      m_paramInputStructFunc = 0;
    }
  }

  return func;
}

/* --------------------------------------------------------------------------- */
/* getFocusFunction --                                                         */
/* --------------------------------------------------------------------------- */
JobFunction *GuiDataField::getFocusFunction(){
  JobFunction* func = Attr()->getFocusFunction();

  // no function found, looking for an parent (base) function
  if (!func && AppData::Instance().InputStructFunc()) {
    // delete old paramInputStructFunc
    if (m_paramInputStructFocusFunc) {
      delete m_paramInputStructFocusFunc;
      m_paramInputStructFocusFunc = 0;
    }

    m_paramInputStructFocusFunc =  m_param->clone();
    do {
      m_paramInputStructFocusFunc->Data()->ConvertToParentReference();
      func = m_paramInputStructFocusFunc->getUserAttr()->getFocusFunction();
    } while(!func && m_paramInputStructFocusFunc->DataItem()->getNumberOfLevels() > 1);
    if (!func) {
      delete m_paramInputStructFocusFunc;
      m_paramInputStructFocusFunc = 0;
    }
  }

  return func;
}

/* --------------------------------------------------------------------------- */
/* startFocusFunction --                                                       */
/* --------------------------------------------------------------------------- */
bool GuiDataField::startFocusFunction(bool focus) {
  if(getFocusFunction() == 0) return false;
  JobStarter *starter = new Trigger(this, getFocusFunction());
  starter->setReason(focus ? JobElement::cll_FocusIn : JobElement::cll_FocusOut);
  starter->setDataItem( m_param->DataItem() );
  starter->startJob();
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiDataField::serializeXML(std::ostream &os, bool recursive){
  assert( m_param != 0 );

  XferDataItem *item = m_param->DataItem();
  assert( item != 0 );
  UserAttr *attr = item->getUserAttr();
  assert( attr != 0 );
  os << " name=\"" << item->getName() << "\"";
  os << " id=\"" << getElement()->getElementId() << "\"";

  const XferDataItem::DataItemIndexList& indexlist = item->getDataItemIndexList();

  std::string fn = item->getFullName(indexlist);

  std::string funcname, focusfuncname;
  JobFunction *func = getFunction();
  if( func != 0 ){
    funcname = func->Name();
  }
  func = getFocusFunction();
  if( func != 0 ){
    focusfuncname = func->Name();
  }

  std::string disabled = fn;
  if (disabled.rfind('.') != std::string::npos)
    disabled.insert( disabled.rfind('.'),".disabled");
  else
    disabled.insert( 0,"disabled.");
  os << " disabled=\""<<disabled<<"\"";

  os << " action=\"" << funcname << "\"";
  os << " action_focus=\"" << focusfuncname << "\"";
  os << " fullName=\"" << fn << "\"";
  std::string label = attr->Label( item->Data() );
  if( !label.empty() ) {
    HTMLConverter::convert2HTML(label);
    os << " label=\"" << label << "\"";
  }
  std::string unit = attr->Unit();
  if( !unit.empty() ) {
    HTMLConverter::convert2HTML(unit);
    os << " unit=\"" << unit << "\"";
  }
  os << " length=\"" << getLength() << "\"";
  std::string helptext = attr->Helptext();
  if( !helptext.empty() ) {
    HTMLConverter::convert2HTML(helptext);
    os << " helptext=\"" << helptext << "\"";
  }
  switch( item->getDataType() ){
  case DataDictionary::type_Integer:{
    os << " type=\"integer\">" << std::endl;
    if( item->isValid() ){
      std::string s;
      m_param->getFormattedValue( s );
      os << "<intens:Value>" << s << "</intens:Value>" << std::endl;
    }

    break;
  }
  case DataDictionary::type_Real:{
    os << " type=\"double\">" << std::endl;
    if( item->isValid() ){
      std::string s;
      m_param->getFormattedValue( s );
      os << "<intens:Value>" << s << "</intens:Value>" << std::endl;
    }
    break;
  }
  case DataDictionary::type_String:{
    os << " type=\"string\">" << std::endl;
    if( item->isValid() ){
      std::string s;
      m_param->getFormattedValue( s );
      HTMLConverter::convert2HTML(s);
      os << "<intens:Value>" << s << "</intens:Value>" << std::endl;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    os << " type=\"complex\">" << std::endl;
    if( item->isValid() ){
      dComplex dC;
      getValue(dC);
      os << "<intens:Value>" << dC << "</intens:Value>" << std::endl;
    }
    break;
  }
  default:
    os << ">" << std::endl;
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiDataField::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  assert( m_param != 0 );
  XferDataItem *item = m_param->DataItem();
  assert( item != 0 );
  bool updated = getAttributes();
  if (item->isUpdated( GuiManager::Instance().LastWebUpdate(), true /* Gui Update */ ) ||
      isWebApiAttrChanged()) {
    updated = true;
  }
  if (onlyUpdated && !updated && !getElement()->getAttributeChangedFlag()) {
    return false;
  }
  getElement()->resetAttributeChangedFlag();

  UserAttr *attr = item->getUserAttr();
  assert( attr != 0 );
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["name"] = item->getName();
  jsonObj["typeLabel"] = isTypeLabel();
  jsonObj["editable"] = isEditable();
  if (getElement()->Type() == GuiElement::type_ScrolledText) {
    jsonObj["length"] = dynamic_cast<GuiScrolledText*>(getElement())->getOptionLength();
    jsonObj["precision"] = dynamic_cast<GuiScrolledText*>(getElement())->getOptionLines();
  } else {
    jsonObj["length"] = getLength();
    jsonObj["precision"] = getPrecision();
  }
  jsonObj["scale"] = (m_param->getScalefactor() && !std::isnan( m_param->getScalefactor()->getValue())) ? m_param->getScalefactor()->getValue() : 1.;
  if (getElement()->Type() == GuiElement::type_Pixmap) {
    BUG_INFO(">> CData, Mimetype Id["<<jsonObj["id"].asString()<<"] name["<<jsonObj["name"].asString()<<"] vn["<<m_param->DataItem()->getFullName(true)<<"]");
  }
  if (getElement()->Type() == GuiElement::type_Toggle ||
      getElement()->Type() == GuiElement::type_RadioButton) {
    double value = 0.0;
    bool state = false;
    if( m_param->DataItem()->getValue( value ) ){
      if( value != 0 ){
        state = true;
      }
    }
    jsonObj["checked"] = state;
    if (getElement()->Type() == GuiElement::type_Toggle) {
      jsonObj["checkedChar"] = state ? "☒" : "☐";
    } else {
      jsonObj["checkedChar"] = state ? "◉" : "○";
    }
  }
  // range (for slider)
  if (getElement()->Type() == GuiElement::type_Slider &&
      attr->getMin() != std::numeric_limits<double>::min() ||
      attr->getMax() != std::numeric_limits<double>::max()) {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    jsonElem["min"] = attr->getMin();
    jsonElem["max"] = attr->getMax();
    jsonElem["step"] = attr->getStep();
    jsonObj["range"] = jsonElem;
  }

  jsonObj["fullName"] = m_param->getFullName();

  const XferDataItem::DataItemIndexList& indexlist = item->getDataItemIndexList();

  JobFunction *func = getFunction();
  if( func != 0 ){
    jsonObj["action"] = func->Name();
  }
  func = getFocusFunction();
  if( func != 0 ){
    jsonObj["action_focus"] = func->Name();
  }

  std::string label = attr->Label( item->Data() );
  // special case button
  if (getElement()->Type() == GuiElement::type_FieldButton) {
    if (!jsonObj.isMember("icon") &&
       attr->Label(m_param->Data()) == m_param->Data()->nodeName()) {
      m_param->getFormattedValue( label );
      BUG_INFO("nodeName: " << m_param->Data()->nodeName() << ", gotLabel:" << label);
      if (label.empty()) jsonObj["visible"] = false;
    }
  }
  if (!label.empty() ) {
    jsonObj["label"] = label;
  }
  std::string unit = attr->Unit();
  if (!unit.empty() ) {
    jsonObj["unit"] = unit;
  }
  std::string helptext = attr->Helptext();
  if (!helptext.empty() ) {
    jsonObj["helptext"] = helptext;
  }
  std::string placeholder = attr->Placeholder();
  if (!placeholder.empty() ) {
    jsonObj["placeholder"] = placeholder;
  }

  // set data type attributes
  getElement()->writeDataTypeJsonProperties(jsonObj, item, m_param);

  // extent helptext
  if (item->getDataType() == DataDictionary::type_String) {
    std::string value(jsonObj["value"].asString());
    if (jsonObj.isMember("length") && value.size() > jsonObj["length"].asInt()) {
      jsonObj["helptext"] = (helptext.empty() ? "" : helptext + ": ") + value;
    }
  }
  // color
  std::string bgcolor, fgcolor;
  getColorStrings( bgcolor, fgcolor );
  if( !bgcolor.empty() ){
    jsonObj["bgcolor"] = bgcolor;
  }
  if( !fgcolor.empty() ){
    jsonObj["fgcolor"] = fgcolor;
  }

  setWebApiAttrChanged(false);  // reset WebApi Update
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiDataField::serializeProtobuf(in_proto::DataField* field, bool onlyUpdated){
  assert( m_param != 0 );
  XferDataItem *item = m_param->DataItem();
  assert( item != 0 );
  bool updated = getAttributes();
  if (item->isUpdated( GuiManager::Instance().LastWebUpdate(), true /* Gui Update */ ) ||
      isWebApiAttrChanged()) {
    updated = true;
  }
  if (onlyUpdated && !updated && !getElement()->getAttributeChangedFlag()) {
    return false;
  }
  getElement()->resetAttributeChangedFlag();

  UserAttr *attr = item->getUserAttr();
  assert( attr != 0 );
  field->set_allocated_base(getElement()->writeProtobufProperties());
  field->mutable_base()->set_name(item->getName());
  field->set_type_label(isTypeLabel());
  field->set_editable(isEditable());
  if (getElement()->Type() == GuiElement::type_ScrolledText) {
    field->set_length(dynamic_cast<GuiScrolledText*>(getElement())->getOptionLength());
    field->set_precision(dynamic_cast<GuiScrolledText*>(getElement())->getOptionLines());
  } else {
    field->set_length(getLength());
    field->set_precision(getPrecision());
  }
  field->set_scale((m_param->getScalefactor() && !std::isnan( m_param->getScalefactor()->getValue())) ? m_param->getScalefactor()->getValue() : 1.);
  if (getElement()->Type() == GuiElement::type_Toggle ||
      getElement()->Type() == GuiElement::type_RadioButton) {
    double value = 0.0;
    bool state = false;
    if( m_param->DataItem()->getValue( value ) ){
      if( value != 0 ){
        state = true;
      }
    }
    field->set_checked(state);
  }
  // range (for slider)
  if (getElement()->Type() == GuiElement::type_Slider &&
      attr->getMin() != std::numeric_limits<double>::min() ||
      attr->getMax() != std::numeric_limits<double>::max()) {
    auto range = field->mutable_range();
    range->set_min(attr->getMin());
    range->set_max(attr->getMax());
    range->set_step(attr->getStep());
  }

  field->set_full_name(m_param->getFullName());
  field->set_visible(true);

  const XferDataItem::DataItemIndexList& indexlist = item->getDataItemIndexList();

  JobFunction *func = getFunction();
  if( func != 0 ){
    field->set_action(func->Name());
  }

  std::string label = attr->Label( item->Data() );
 //  // special case button
  if (getElement()->Type() == GuiElement::type_FieldButton) {
    if (field->icon().length() == 0 &&
      	attr->Label(m_param->Data()) == m_param->Data()->nodeName()) {
      m_param->getFormattedValue( label );
      BUG_INFO("nodeName: " << m_param->Data()->nodeName() << ", gotLabel:" << label);
      if (label.empty()) field->set_visible(false);
    }
  }
  if (!label.empty() ) {
    field->set_label(label);
  }
  std::string helptext = attr->Helptext();
  if (!helptext.empty() ) {
    field->set_helptext(helptext);
  }
  std::string placeholder = attr->Placeholder();
  if (!placeholder.empty() ) {
    field->set_placeholder(placeholder);
  }

  field->set_allocated_value(getElement()->writeDataTypeProtobufProperties(item, m_param));
  // set data type attributes


 //  // extent helptext
  if (item->getDataType() == DataDictionary::type_String) {
    field->set_helptext((helptext.empty() ? "" : helptext + ": ") + field->value().string_value());
  }
 //  // color
  std::string bgcolor, fgcolor;
  getColorStrings( bgcolor, fgcolor );
  if( !bgcolor.empty() ){
    field->set_bgcolor(bgcolor);
  }
  if( !fgcolor.empty() ){
    field->set_fgcolor(fgcolor);
  }

  setWebApiAttrChanged(false);  // reset WebApi Update
  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* prepareToStartJob --                                                        */
/* --------------------------------------------------------------------------- */

void GuiDataField::prepareToStartJob( Trigger *trigger ){
  BUG_DEBUG( "Begin prepareToStartJob" );

  // Die Function wurde bereits gestartet

  // Nun wird der Trigger (JobStarter) initialisiert.
  // Nur Focus Reasons werden nicht überschrieben
  if (trigger->getReason() != JobElement::cll_FocusIn &&
      trigger->getReason() != JobElement::cll_FocusOut)
    trigger->setReason( JobElement::cll_Input );
  if (m_paramInputStructFunc) {
    trigger->setBase( m_paramInputStructFunc->DataItem() );
  }

  if( !s_TargetsAreCleared && !Attr()->IsButton()){
    // Alle abhängigen Daten werden nun gelöscht.
    m_param->DataItem()->clearTargetStreams();
    s_TargetsAreCleared = true;
  }

  BUG_DEBUG( "End prepareToStartJob" );
}

/* --------------------------------------------------------------------------- */
/* doEndOfWork --                                                              */
/* --------------------------------------------------------------------------- */

void GuiDataField::doEndOfWork( bool error, bool updateforms ){
  BUG_DEBUG( "Begin doEndOfWork" );

  // Diese Funktion wird nur benötigt, wenn eine Function an der Variablen
  // angehängt ist (ein sogenannter trigger).
  if( error ){
    getElement()->updateForms( GuiElement::reason_Cancel );
  }
  else{
    if (updateforms)
      getElement()->updateForms( GuiElement::reason_FieldInput );
  }

  if(disabled()){
    protectField();
  }
  unsetRunningMode();
  s_DialogIsAktive = false;
  EndOfWorkFinished();

  if( error ){
    GuiManager::Instance().showErrorBox( getElement() );
  }
  BUG_DEBUG( "End doEndOfWork" );
}

/* --------------------------------------------------------------------------- */
/* MyTimerTask::setType --                                                     */
/* --------------------------------------------------------------------------- */
void GuiDataField::MyTimerTask::setType(TaskType type) {
  m_type = type;
}
/* --------------------------------------------------------------------------- */
/* MyTimerTask::setMessage --                                                  */
/* --------------------------------------------------------------------------- */
void GuiDataField::MyTimerTask::setMessage(const std::string& msg) {
  m_message = msg;
}


/* --------------------------------------------------------------------------- */
/* MyTimerTask::tick --                                                        */
/* --------------------------------------------------------------------------- */

void GuiDataField::MyTimerTask::tick() {
  switch(m_type) {
  case task_ConfirmationDialog:
    m_datafield->callConfirmationDialog(m_message);
    m_message.clear();
    break;
  case task_ProtectAction:
    m_datafield->protect();
    break;
  case task_None:
    assert(false);
  }
}

/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                       */
/* --------------------------------------------------------------------------- */

void GuiDataField::Trigger::backFromJobStarter( JobAction::JobResult rslt  ){
  m_datafield->doEndOfWork( rslt != JobAction::job_Ok, m_updateForms);
}
