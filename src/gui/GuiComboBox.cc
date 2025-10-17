
#include <algorithm>

#include "utils/Debugger.h"

#include "utils/HTMLConverter.h"
#include "datapool/DataReference.h"
#include "datapool/DataVector.h"
#include "gui/GuiDataField.h"
#include "gui/GuiManager.h"
#include "gui/GuiElement.h"
#include "xfer/XferDataParameter.h"
#include "gui/GuiComboBox.h"
#include "app/DataPoolIntens.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiComboBox::GuiComboBox()
  : m_set_input( 0 )
  , m_set_output( 0 )
  , m_set_strings( 0 )
  , m_use_input( false )
  , m_reCreate( false )
  , m_position( -1 )
  , m_alarmColor(false)
  , m_pickList_timestamp( 0 ) {
}

GuiComboBox::GuiComboBox( GuiComboBox &combo )
  : m_set_input( 0 )
  , m_set_output( 0 )
  , m_set_strings( 0 )
  , m_use_input( false )
  , m_reCreate( false )
  , m_position( -1 )
  , m_alarmColor(false)
  , m_pickList_timestamp( 0 ) {
  if( combo.m_set_input != 0 ){
    m_dataset     = DataPool::newDataReference( *combo.m_dataset );
    m_set_input   = DataPool::newDataReference( *combo.m_set_input );
    m_set_output  = DataPool::newDataReference( *combo.m_set_output );
    m_set_strings = DataPool::newDataReference( *combo.m_set_strings );
  }
}

GuiComboBox::~GuiComboBox(){
  delete m_dataset;
  delete m_set_input;
  delete m_set_output;
  delete m_set_strings;
}

/* --------------------------------------------------------------------------- */
/* createPickList --                                                           */
/* --------------------------------------------------------------------------- */

void GuiComboBox::createPickList(){
  setPickList_timestamp(DataPoolIntens::CurrentTransaction());
  if( m_set_input == 0 ){
    return;
  }

  // Hat das DataSet die Indexed Option
  // setzen wir auf dem DataSet im Level 0 den selben Wert
  // wie der Wert des letzten Indizes des DataItems
  // a.b.c[xx]  => dataset[xx].Input[*] dataset[xx].Output[*]
  if (datafield()->Attr()->DataSetIndexed()) {
    int to = 0;
    if (datafield()->DataItem()->getLastIndex())
      to = datafield()->DataItem()->getLastIndex()->getValue();

    m_set_input->SetDimensionIndizes(0,1,&to);
    m_set_output->SetDimensionIndizes(0,1,&to);
    m_set_strings->SetDimensionIndizes(0,1,&to);
  }

  // Zuerst ermitteln wir die Länge des Input-Arrays.
  int n = 0;
  while( m_set_input->GetItemValid( n ) ){
    n++;
  }
  if( n == 0 ){
    return;
  }

  // --------------------------------------------------------------------------
  // Nun kontrollieren wir, ob der Output in Ordnung ist. Wenn der Output-Array
  // leer oder älter als der Input ist, wird er neu generiert.
  // --------------------------------------------------------------------------
  TransactionNumber transout = m_set_output->getDataItemValueUpdated();
  TransactionNumber transin  = m_set_input->getDataItemValueUpdated();
  TransactionNumber transcur = GuiManager::Instance().LastGuiUpdate();
  std::string text;

  if( transin > transout || !m_set_output->GetItemValid( 0 ) ){
    int n = 0;
    while( m_set_input->GetItemValid( n )){
      m_set_input->GetValue( text, n );
      // nur wenn der Output invalid ist UND der Input einen Wert hat ( text == " " => Input ist Invalid )
      // wird der Output gleich dem Index gesetzt
      if ( !m_set_output->GetItemValid( n ) && text != " ")
	m_set_output->SetValue( n, n );
      n++;
    }
    m_set_output->SetItemInvalid( n );
  }

  // Nun scheint alles bereit generieren der Picklist.
  // -------------------------------------------------
  if( hasInvalidEntry() ){
    m_set_input->GetValue( text, n-1 );
    if( text != " " ){
      // Am Ende eines Sets soll immer ein leeres Feld zum Setzen
      // eines ungültigen Werts sein.
      m_set_input->SetValue( " " , n );
      m_set_output->SetValue( n, n );    // Timestamp von m_set_output muss erhöht werden (transin > transout oben)!
      m_set_output->SetItemInvalid( n ); // Falls m_set_output(n) schon invalid war, geschieht dies hier nicht
      n++;
    }
  }
  fill( n );

  // Wenn der Wert des items 'Strings' nicht grösser 0 ist, so verwenden wir bei
  // String-Items den Input als Output.
  // ---------------------------------------------------------------------------
  if( param()->DataItem()->Data()->getDataType() == DataDictionary::type_String ){
    m_use_input = true;
    int typ = 0;
    if( m_set_strings->GetValue( typ ) ){
      if( typ > 0 ){
	m_use_input = false;
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* hasInvalidEntry --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiComboBox::hasInvalidEntry() const{
  assert( m_dataset != 0 );
  UserAttr *attr = static_cast<UserAttr*>( m_dataset->getUserAttr() );
  if( attr != 0 ){
    return attr->hasInvalidEntry();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* installDataItem --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiComboBox::installDataItem( XferDataItem *dataitem ){
  bool rslt = datafield()->GuiDataField::installDataItem( dataitem );
  if( rslt )
    param()->setLength( 0 );
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* replaceDataItem --                                                          */
/* --------------------------------------------------------------------------- */

XferDataItem *GuiComboBox::replaceDataItem( XferDataItem *dataitem ){
  XferDataItem *old_item = datafield()->GuiDataField::replaceDataItem( dataitem );
  if( old_item != 0 ){
    param()->setLength( 0 );
  }
  return old_item;
}

/* --------------------------------------------------------------------------- */
/* lock --                                                                     */
/* --------------------------------------------------------------------------- */

void GuiComboBox::lock(){
  if( created() ){
    datafield()->getAttributes();
    if( !datafield()->isLockable() ){
      return;
    }
    datafield()->lockValue();
    getElement()->updateForms( GuiElement::reason_FieldInput );
  }
}

/* --------------------------------------------------------------------------- */
/* unlock --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiComboBox::unlock(){
  if( created() ){
    datafield()->getAttributes();
    if( !datafield()->isLockable() ) return;

    datafield()->unlockValue();
    getElement()->updateForms( GuiElement::reason_FieldInput );
  }
}

/* --------------------------------------------------------------------------- */
/* switchlock --                                                               */
/* --------------------------------------------------------------------------- */

void GuiComboBox::switchlock(){
  if( created() ){
    datafield()->getAttributes();
    if( !datafield()->isLockable() ){
      return;
    }
    if( datafield()->isLocked() ){
      datafield()->unlockValue();
    }
    else{
      datafield()->lockValue();
    }
    getElement()->updateForms( GuiElement::reason_FieldInput );
  }
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiComboBox::setScalefactor( Scale *scale ){
  return param()->setScalefactor( scale );
}

/* --------------------------------------------------------------------------- */
/* setSetName --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiComboBox::setSetName( const std::string &name ){
  assert( m_set_input == 0 );
  m_dataset = DataPoolIntens::Instance().getDataReference( name );
  if( m_dataset == 0 ){
    return false;
  }

  m_set_input = DataPoolIntens::Instance().getDataReference( m_dataset, "Input" );
  if( m_set_input == 0 ){
    return false;
  }

  m_set_output = DataPoolIntens::Instance().getDataReference( m_dataset, "Output" );
  if( m_set_output == 0 ){
    delete m_set_input;
    m_set_input = 0;
    return false;
  }

  m_set_strings = DataPoolIntens::Instance().getDataReference( m_dataset, "Strings" );
  assert( m_set_strings != 0 );
  return true;
}

/* --------------------------------------------------------------------------- */
/* convertInput --                                                             */
/* --------------------------------------------------------------------------- */

void GuiComboBox::convertInput( std::string &value ){
  BUG(BugGui,"GuiComboBox::convertInput");
  BUG_MSG( "position : " << m_position );
  if( m_position >= 0 ){
    DataReference *set = m_set_output;
    if( m_use_input ){
      set = m_set_input;
    }
    BUG_MSG( "get value : " << set->fullName( true ) );
    if( set->GetValue( value, m_position ) ){
      BUG_MSG( "value : " << value );
      return;
    }
    BUG_MSG( "no value found" );
  }
  value.erase();
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus GuiComboBox::checkFormat(){
  BUG(BugGuiTable,"GuiComboBox::checkFormat");
  std::string val;
  convertInput( val );
  return param()->checkFormat( val );
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiComboBox::setInputValue(){
  BUG_PARA(BugGui,"GuiComboBox::setValue","Position = " << m_position);
  std::string text;
  if( !(m_position >= 0) ){
    param()->clear();
    return true;
  }
  DataReference *set = m_set_output;
  if( m_use_input ){
    set = m_set_input;
  }
  if( !set->GetValue( text, m_position ) ){
    param()->clear();
    return true;
  }

  // Spezialfall: Invalid-Korrektur für
  // use_input und mit ungueltigen OutputItem
  std::string _text;
  if(m_use_input && !m_set_output->GetValue( _text, m_position ) ){
      param()->clear();
      return true;
  }

  if( param()->setFormattedValue( text ) ){
    return true;
  }
  BUG_EXIT("setValue failed");
  return false;
}

/* --------------------------------------------------------------------------- */
/* FinalWork --                                                                */
/* --------------------------------------------------------------------------- */

void GuiComboBox::FinalWork(){
  BUG(BugGui,"GuiComboBox::FinalWork");
  if( datafield()->isRunning() ){
    // Falls noch eine Function aktiv ist, sind keine Eingaben möglich. Diese
    // Erfahrung machen nur schnelle Finger.
      // strange error with xgettext here (Non-ASCII string)
      //element()->printMessage( _("a function is aktiv."), GuiElement::msg_Warning );
    getElement()->update( GuiElement::reason_Cancel );
    return;
  }

  switch( checkFormat() ){
  case XferParameter::status_Bad:
    getElement()->printMessage( _("Conversion error."), GuiElement::msg_Warning );
    getElement()->update( GuiElement::reason_Cancel );
    setAlarmColors();
    BUG_EXIT("checkFormat failed");
    return;

  case XferParameter::status_Unchanged:
    BUG_EXIT("checkFormat Unchanged");
    return;

  case XferParameter::status_Changed:
    std::string error_msg;
    datafield()->setTargetsCleared( param()->DataItem()->StreamTargetsAreCleared(error_msg) );
    if( datafield()->areTargetsCleared() ){ // Targets sind ok
      FinalWorkOk();
      return;
    }
    datafield()->confirm(error_msg);
    return;
  }
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void GuiComboBox::confirmYesButtonPressed(){
  BUG(BugGui,"GuiComboBox::confirmYesButtonPressed");
  FinalWorkOk();
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void GuiComboBox::confirmNoButtonPressed(){
  BUG(BugGui,"GuiComboBox::confirmNoButtonPressed");
  getElement()->update( GuiElement::reason_Cancel );
  datafield()->setDialogActive( false );
}

/* --------------------------------------------------------------------------- */
/* FinalWorkOk --                                                              */
/* --------------------------------------------------------------------------- */

void GuiComboBox::FinalWorkOk(){
  BUG(BugGui,"GuiComboBox::FinalWorkOk");
  datafield()->doFinalWork();
  datafield()->setDialogActive( false );
}

/* --------------------------------------------------------------------------- */
/* setAlarmColors --                                                           */
/* --------------------------------------------------------------------------- */

void GuiComboBox::setAlarmColors(){
  BUG(BugGui,"GuiXrtComboBox::setAlarmColors");
  // ---------------------------------------------------------------------
  // Dieses Attribute sorgt dafür, dass beim nächsten update() die Alarm-
  // Color wieder verschwindet. Dieses Bit wird nur lokal in diesem Objekt
  // gesetzt und landet nicht im Datapool.
  // ---------------------------------------------------------------------
  datafield()->setAttribute( DATAcolorAlarm );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiComboBox::serializeXML(std::ostream &os, bool recursive){
  os << " name=\"" << param()->getName() << "\"";

  XferDataItem *xfer = param()->DataItem();
  const XferDataItem::DataItemIndexList& indexlist = xfer->getDataItemIndexList();
  std::string fn = xfer->getFullName(indexlist);
  std::string disabled = fn;
  disabled.insert( disabled.rfind('['),"_disabled");

  std::string::size_type pos = 0;

  while( ( pos = fn.find( '.', pos ) ) != std::string::npos ){
    if( fn[pos-1] != ']' ){
      fn.insert( pos, "[0]" );
      pos += 8;
    }
    else{
      ++pos;
    }
  }

  os << " disabled=\""<<disabled<<"\"";
  os << " fullName=\"" << fn << "\"";
  os << " length=\"" << dynamic_cast<GuiDataField*>(this)->getLength() << "\"";
  os << ">" << std::endl;

  m_set_output->getFullName(fn,true);

  if((pos = fn.find("Output")) == std::string::npos )
    assert( false );

  fn.erase( pos-1 );

  if( fn[fn.size()-1] == ']' ){
    pos = fn.rfind('[');
    fn.erase(pos);
  }

  os << "<intens:Value";
  os << " fullName=\"" <<fn<<"\"";
  os << "/>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiComboBox::serializeJson( Json::Value& jsonObj,
                                 bool onlyUpdated ) {
  bool updated = datafield()->getAttributes();
  bool isValueInsideDataset(false);
  if( param()->DataItem()->isUpdated(GuiManager::Instance().LastWebUpdate(), true ) ||
      datafield()->isWebApiAttrChanged()) {
    updated = true;
  }

  if( m_set_input->isDataItemUpdated(DataReference::ValueUpdated, getPickList_timestamp()) ||
      m_set_output->isDataItemUpdated(DataReference::ValueUpdated, getPickList_timestamp()) ){
    rebuildPickList();
    updated = true;
  }

  if (onlyUpdated && !updated) {
    return false;
  }

  getElement()->writeJsonProperties(jsonObj);
  jsonObj["name"] = param()->getName();
  jsonObj["editable"] = datafield()->isEditable();
  jsonObj["updated"] = updated | datafield()->isUpdated();
  jsonObj["fullName"] =  param()->DataItem()->getFullName(true);
  jsonObj["length"] = datafield()->getLength();
  if (datafield()->getFunction())
    jsonObj["action"] = datafield()->getFunction()->Name();
  std::string fvalue;
  trim(fvalue);
  param()->getFormattedValue( fvalue );
  jsonObj["formatted_value"] = fvalue;

  // color
  std::string background, foreground;
  datafield()->getColorStrings(background, foreground);
  jsonObj["fgcolor"] = foreground;
  jsonObj["bgcolor"] = background;

  // helptext
  UserAttr *attr = param()->DataItem()->getUserAttr();
  assert( attr != 0 );
  std::string helptext = attr->Helptext();
  if (!helptext.empty() ) {
    jsonObj["helptext"] = helptext;
  }

  // dataset
  DataVector dVecOut;
  DataVector dVecIn;
  Json::Value inJsonAry = Json::Value(Json::arrayValue);
  Json::Value outJsonAry = Json::Value(Json::arrayValue);
  int id[1] = {-1};
  // output
  m_set_output->getDataVector(dVecOut, 1, id);
  for (DataVector::iterator it = dVecOut.begin();it != dVecOut.end(); ++it) {
    std::string s;
    (*it)->getValue(s);
///    trim(s);
    if (s.size() && s == fvalue && !m_use_input) isValueInsideDataset = true;
    outJsonAry.append(s);
  }

  // input
  m_set_input->getDataVector(dVecIn, 1, id);
  for (DataVector::iterator it = dVecIn.begin();it != dVecIn.end(); ++it) {
    std::string s;
    (*it)->getValue(s);
    if (s.size() && s == fvalue && m_use_input) isValueInsideDataset = true;
    inJsonAry.append(s);
  }

  // !!! add invalue value
  if (!isValueInsideDataset && fvalue.size() &&
      outJsonAry.size() && inJsonAry.size()) {
    Json::Value inJsonAry2 = Json::Value(Json::arrayValue);
    Json::Value outJsonAry2 = Json::Value(Json::arrayValue);
    outJsonAry2.append(fvalue);
    inJsonAry2.append(fvalue);
    for (int i=0; i < outJsonAry.size(); ++i)
      outJsonAry2.append(outJsonAry[i]);
    for (int i=0; i < outJsonAry.size(); ++i)
      inJsonAry2.append(inJsonAry[i]);
    inJsonAry2.swap(inJsonAry);
    outJsonAry2.swap(outJsonAry);

    // warning color?
    jsonObj["bgcolor"] = "#ff0000";  // red
  }

  // add dataset values
  jsonObj["dataset_input"] = inJsonAry;
  if (m_use_input) {
    jsonObj["dataset_output"] = inJsonAry;
    if (inJsonAry.size() > outJsonAry.size()) {
      jsonObj["dataset_output"].resize(outJsonAry.size());
    }
  } else {
    jsonObj["dataset_output"] = outJsonAry;
  }
  jsonObj["ds_input"] = (int) jsonObj["dataset_input"].size();
  jsonObj["ds_output"] = (int) jsonObj["dataset_output"].size();

  // dataset name
  std::string dsfn = m_set_output->fullName(true);
  std::string::size_type pos = dsfn.find("Output");
  if(pos == std::string::npos)
    assert( false );
  dsfn.erase( pos-1 );

  if( dsfn[dsfn.size()-1] == ']' ){
    pos = dsfn.rfind('[');
    dsfn.erase(pos);
  }
  jsonObj["dataset"] = dsfn;

  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiComboBox::serializeProtobuf(in_proto::ElementList* eles,
                                 bool onlyUpdated ) {
  bool updated = datafield()->getAttributes();
  bool isValueInsideDataset(false);
  if( param()->DataItem()->isUpdated(GuiManager::Instance().LastWebUpdate(), true ) ||
      datafield()->isWebApiAttrChanged()) {
    updated = true;
  }

  if( m_set_input->isDataItemUpdated(DataReference::ValueUpdated, getPickList_timestamp()) ||
      m_set_output->isDataItemUpdated(DataReference::ValueUpdated, getPickList_timestamp()) ){
    rebuildPickList();
    updated = true;
  }

  if (onlyUpdated && !updated) {
    return false;
  }

  auto element = eles->add_comboboxes();
  datafield()->serializeProtobuf(element->mutable_base());
  element->mutable_base()->mutable_base()->set_name(param()->getName());
  std::string fvalue;
  trim(fvalue);

  // dataset
  DataVector dVecOut;
  DataVector dVecIn;
  auto inJsonAry = std::vector<std::string>();
  auto outJsonAry = std::vector<std::string>();
  int id[1] = {-1};
  // output
  m_set_output->getDataVector(dVecOut, 1, id);
  for (DataVector::iterator it = dVecOut.begin();it != dVecOut.end(); ++it) {
    std::string s;
    (*it)->getValue(s);
///    trim(s);
    if (s.size() && s == fvalue && !m_use_input) isValueInsideDataset = true;
    outJsonAry.push_back(s);
  }

  // input
  m_set_input->getDataVector(dVecIn, 1, id);
  for (DataVector::iterator it = dVecIn.begin();it != dVecIn.end(); ++it) {
    std::string s;
    (*it)->getValue(s);
    if (s.size() && s == fvalue && m_use_input) isValueInsideDataset = true;
    inJsonAry.push_back(s);
  }

  // !!! add invalue value
  if (!isValueInsideDataset && fvalue.size() &&
      outJsonAry.size() && inJsonAry.size()) {
    auto inJsonAry2 = std::vector<std::string>();
    auto outJsonAry2 = std::vector<std::string>();
    outJsonAry2.push_back(fvalue);
    inJsonAry2.push_back(fvalue);
    for (int i=0; i < outJsonAry.size(); ++i)
      outJsonAry2.push_back(outJsonAry[i]);
    for (int i=0; i < outJsonAry.size(); ++i)
      inJsonAry2.push_back(inJsonAry[i]);
    inJsonAry2.swap(inJsonAry);
    outJsonAry2.swap(outJsonAry);

    // warning color?
    element->mutable_base()->set_bgcolor("#ff0000");
  }

  // add dataset values
  element->mutable_input_dataset()->Assign(inJsonAry.begin(), inJsonAry.end());
  if (m_use_input) {
    element->mutable_output_dataset()->Assign(inJsonAry.begin(), inJsonAry.end());
  } else {
    element->mutable_output_dataset()->Assign(outJsonAry.begin(), outJsonAry.end());
  }

  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */

void GuiComboBox::getVisibleDataPoolValues( GuiValueList& vmap ) {
  std::string::size_type posA, posE;
  std::string name, input, output;
  m_set_output->getFullName(name,true);
  std::string::size_type pos = 0;
  if(( pos = name.find( "Output" ) ) == std::string::npos ){
    assert( false );
  }
  name.erase( pos-1 );

  if( name[ name.size()-1 ] == ']' ){
    pos = name.rfind('[');
    name.erase( pos );
  }

  int n = 0;
  std::ostringstream os;
  os << "@set@";
  bool ri, ro;
  while( m_set_input->GetItemValid( n ) ){
    if( n > 0 ){
      os << ";";
    }
    ri = m_set_input->GetValue( input, n );
    ro = m_set_output->GetValue( output, n );
    os << (ri?input:"") << "," << (ro?output:" ");
    n++;
  }
  vmap.insert( GuiValueList::value_type(name, os.str() ) );
}
