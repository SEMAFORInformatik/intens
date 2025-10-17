#include <QtCore/QStringList>


#include "gui/qt/GuiQtTableComboBoxItem.h"
#include "app/DataPoolIntens.h"

#include "utils/gettext.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtTableComboBoxItem::GuiQtTableComboBoxItem( GuiElement *parent )
  : GuiQtTableDataItem( parent )
  , m_set( 0 )
  , m_set_input( 0 )
  , m_set_output( 0 )
  , m_set_strings( 0 )
  , m_use_input( false )
  , m_noInvalidEntry( false )
  , m_pickList_timestamp( 0 ) {
}

GuiQtTableComboBoxItem::GuiQtTableComboBoxItem( const GuiQtTableComboBoxItem &item )
  : GuiQtTableDataItem( item )
  , m_set( 0 )
  , m_set_input( 0 )
  , m_set_output( 0 )
  , m_set_strings( 0 )
  , m_use_input( false )
  , m_noInvalidEntry( false ){
  if( item.m_set_input != 0 ){
    m_set         = DataPool::newDataReference( *item.m_set );
    m_set_input   = DataPool::newDataReference( *item.m_set_input );
    m_set_output  = DataPool::newDataReference( *item.m_set_output );
    m_set_strings = DataPool::newDataReference( *item.m_set_strings );
  }
}

GuiQtTableComboBoxItem::~GuiQtTableComboBoxItem(){
  delete m_set;
  delete m_set_input;
  delete m_set_output;
  delete m_set_strings;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* cloneComboBoxItem --                                                        */
/* --------------------------------------------------------------------------- */

GuiQtTableComboBoxItem *GuiQtTableComboBoxItem::cloneComboBoxItem(){
  GuiQtTableComboBoxItem * item = new GuiQtTableComboBoxItem( *this );
  item->GuiElement::setParent( getParent() );
  return item;
}

/* --------------------------------------------------------------------------- */
/* setSetName --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtTableComboBoxItem::setSetName( const std::string &name ){
  assert( m_set_input == 0 );
  m_set = DataPoolIntens::Instance().getDataReference( name );
  if( m_set == 0 ){
    return false;
  }

  m_set_input = DataPoolIntens::Instance().getDataReference( m_set, "Input" );
  if( m_set_input == 0 ){
    delete m_set;
    m_set = 0;
    return false;
  }

  m_set_output = DataPoolIntens::Instance().getDataReference( m_set, "Output" );
    if( m_set_output == 0 ){
    delete m_set;
    delete m_set_input;
    m_set = 0;
    m_set_input = 0;
    return false;
  }

  m_set_strings = DataPoolIntens::Instance().getDataReference( m_set, "Strings" );
  assert( m_set_strings != 0 );
  return true;
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiQtTableComboBoxItem::isDataItemUpdated( TransactionNumber trans ){
  if( m_set->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
    return true;
  }
  if( m_set_input->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
    return true;
  }
  if( m_set_output->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
    return true;
  }
  if( m_set_strings->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
    return true;
  }
  return GuiQtTableDataItem::isDataItemUpdated( trans );
}

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtTableComboBoxItem::setColors(){
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* setFieldPicklist --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtTableComboBoxItem::setFieldPicklist( int row, int col ){

  deletePickList();

  // use same size as the Data Object
  DataContainer::SizeList dimsize;
  m_set->GetAllDataDimensionSize( dimsize );
  int to = dimsize.size() ? dimsize[0] : 0;
  for (int idx=0; idx < to; ++idx) {
    QStringList _picklist;
    m_set_input->SetDimensionIndizes(0,1,&idx);
    m_set_output->SetDimensionIndizes(0,1,&idx);
    m_set_strings->SetDimensionIndizes(0,1,&idx);

    createPickList();
  }
}

/* --------------------------------------------------------------------------- */
/* getCellItems --                                                              */
/* --------------------------------------------------------------------------- */

QStringList GuiQtTableComboBoxItem::getCellItems() {
  int idx = 0;
  int numWC = DataItem()->getNumOfWildcards();
  if (Attr()->DataSetIndexed() && numWC >= 1) {
    XferDataItemIndex *index = DataItem()->getDataItemIndexWildcard(numWC);
    idx = index->getValue();
  }
  return (idx < m_picklist.size()) ? m_picklist[idx] :
    (m_picklist.size()==1 && !(Attr()->DataSetIndexed())) ? m_picklist[0] : QStringList();
}

/* --------------------------------------------------------------------------- */
/* getTrimmedCellItems --                                                      */
/* --------------------------------------------------------------------------- */
void GuiQtTableComboBoxItem::getTrimmedCellItems(QStringList& slist) {
  if (slist.contains(""))
    slist.removeAt(slist.indexOf(""));
  if (slist.contains(" "))
    slist.removeAt(slist.indexOf(" "));
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* createPickList --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtTableComboBoxItem::createPickList(){
  setPickList_timestamp(DataPoolIntens::CurrentTransaction());
  if( m_set_input == 0 ){
    return;
  }
  // Zuerst ermitteln wir die Länge des Input-Arrays.
  int n = 0;
  while( m_set_input->GetItemValid( n ) ){
    n++;
  }
  if( n == 0 ){
    m_picklist.push_back( QStringList() );
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
    while( m_set_input->GetItemValid( n ) ){
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
  if( !m_noInvalidEntry ){
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
  if( m_param->DataItem()->Data()->getDataType() == DataDictionary::type_String ){
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
/* fill --                                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtTableComboBoxItem::fill( int n ){
  std::string text;

  QStringList _picklist;
  for( int i=0; i<n; i++ ){
    m_set_input->GetValue( text, i );
    _picklist << QString::fromStdString(text);
  }
  if (!Attr()->DataSetIndexed())
    m_picklist.clear();
  m_picklist.push_back(_picklist);
}

/* --------------------------------------------------------------------------- */
/* deletePickList --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtTableComboBoxItem::deletePickList(){
  if( m_picklist.empty()){
     return;
   }
   m_picklist.clear();
}

/* --------------------------------------------------------------------------- */
/* convertInput --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtTableComboBoxItem::convertInput( std::string &value ){
  BUG_DEBUG(  "GuiQtTableComboBoxItem::convertInput '" << value << "'" );
  int n = 0;
  std::string set_value;
  while( m_set_input->GetValue( set_value, n ) ){
    BUG_DEBUG(  "'" << value << "' = '" << set_value << "'" );
    if( value == set_value ){
      DataReference *set = m_set_output;
      if( m_use_input ){
	set = m_set_input;
      }
      set->GetValue( value, n );
      BUG_ERROR(  "value '" << value << "'" );
      return true;
    }
    n++;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* convertOutput --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtTableComboBoxItem::convertOutput( std::string &value ){
  BUG_DEBUG(  "GuiQtTableComboBoxItem::convertOutput '" << value << "'" );
  int n = 0;
  std::string set_value;
  DataReference *set = m_set_output;
  if( m_use_input ){
    set = m_set_input;
  }
  while( set->GetValue( set_value, n ) ){
    BUG_DEBUG(  "'" << value << "' = '" << set_value << "'" );
    if( value == set_value ){
      m_set_input->GetValue( value, n );
      BUG_ERROR(  "value '" << value << "'" );
      return true;
    }
    n++;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus GuiQtTableComboBoxItem::checkFormat(){
  BUG_DEBUG(  "GuiQtTableComboBoxItem::checkFormat" );

  // set index
  int idx = 0;
  int numWC = DataItem()->getNumOfWildcards();
  if (numWC >= 1) {
    XferDataItemIndex *index = DataItem()->getDataItemIndexWildcard(numWC);
    idx = index->getValue();
  }
  idx = (idx < m_picklist.size()) ? idx : 0;
  m_set_input->SetDimensionIndizes(0,1,&idx);
  m_set_output->SetDimensionIndizes(0,1,&idx);
  m_set_strings->SetDimensionIndizes(0,1,&idx);

  // do conversion (ignore if selection list is empty)
  QStringList sl = getCellItems();
  getTrimmedCellItems( sl );
  if( sl.size() && !m_param->isEmpty( m_value ) ){
    if( !convertInput( m_value ) ){
      BUG_ERROR(  "value '" << m_value << "' not found in set" );
      return XferParameter::status_Bad;
    }
  }
  return m_param->checkFormat( m_value );
}

/* --------------------------------------------------------------------------- */
/* CellValue --                                                                */
/* --------------------------------------------------------------------------- */

std::string GuiQtTableComboBoxItem::CellValue(){
  // set index
  int idx = 0;
  int numWC = DataItem()->getNumOfWildcards();
  if (numWC >= 1) {
    XferDataItemIndex *index = DataItem()->getDataItemIndexWildcard(numWC);
    idx = index->getValue();
  }
  idx = (idx < m_picklist.size()) ? idx : 0;
  m_set_input->SetDimensionIndizes(0,1,&idx);
  m_set_output->SetDimensionIndizes(0,1,&idx);
  m_set_strings->SetDimensionIndizes(0,1,&idx);

  std::string text;
  // get value, considering a possible scale factor
  Scale* scale = m_param->getScalefactor();
  if (scale && scale->getValue() != 1) {
    m_param->getFormattedValue( text );

    // trim white space
    text = QString::fromStdString(text).trimmed().toStdString();
  } else
    getValue( text );

  convertOutput( text );
  return text;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
#include "datapool/DataVector.h"

bool GuiQtTableComboBoxItem::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  bool updated = getAttributes();
  bool isValueInsideDataset(false);
  bool isValid(false);
  if( DataItem()->isUpdated(GuiManager::Instance().LastWebUpdate(), true ) ) {
    updated = true;
  }

  if( m_set_input->isDataItemUpdated(DataReference::ValueUpdated, getPickList_timestamp()) ||
      m_set_output->isDataItemUpdated(DataReference::ValueUpdated, getPickList_timestamp()) ){
    setPickList_timestamp(DataPoolIntens::CurrentTransaction());
    updated = true;
  }

  if (onlyUpdated && !updated) {
    return false;
  }

  // base data
  bool ret = GuiQtDataField::serializeJson(jsonObj, onlyUpdated);

  // dataset
  int numWC = DataItem()->getNumOfWildcards();
  if (Attr()->DataSetIndexed() && numWC >= 1) {
    XferDataItemIndex *index = DataItem()->getDataItemIndexWildcard(numWC);
    int idx = index->getValue();
    m_set_input->SetDimensionIndizes(0,1,&idx);
    m_set_output->SetDimensionIndizes(0,1,&idx);
    m_set_strings->SetDimensionIndizes(0,1,&idx);
    if (idx >= m_picklist.size()) return ret;  // return
  }

  std::string fvalue;
  m_param->getFormattedValue(fvalue);
  trim(fvalue);

  // dataset
  std::string dsfn = m_set_output->fullName(true);
  DataVector dVecOut;
  DataVector dVecIn;
  Json::Value inJsonAry = Json::Value(Json::arrayValue);
  Json::Value outJsonAry = Json::Value(Json::arrayValue);
  int id[1] = {-1};
  // output
  int n = 0;
  std::string s;
  while( m_set_output->GetValue( s, n++ ) ){
    outJsonAry.append(s);
    trim(s);
    if (!isValueInsideDataset && s.size() && s == fvalue) isValueInsideDataset = true;
  }

  // input
  n = 0;
  while( m_set_input->GetValue( s, n++ ) ){
    inJsonAry.append(s);
    trim(s);
    if (!isValid && s.size()) isValid= true;
  }

  // !!! add invalue value
  if (isValid &&
      !isValueInsideDataset && fvalue.size() &&
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
  if (isValid && outJsonAry.size() > 0 && inJsonAry.size() > 0) {
    jsonObj["dataset_output"] = outJsonAry;
    jsonObj["dataset_input"] = inJsonAry;

    jsonObj["ds_output"] = (int) outJsonAry.size();
    jsonObj["ds_input"] = (int) inJsonAry.size();
  }

  std::string::size_type pos = dsfn.find("Output");
  if(pos == std::string::npos)
    assert( false );
  dsfn.erase( pos-1 );

  if( dsfn[dsfn.size()-1] == ']' ){
    pos = dsfn.rfind('[');
    dsfn.erase(pos);
  }
  jsonObj["dataset"] = dsfn;

  return ret;
}


#if HAVE_PROTOBUF
in_proto::ComboBox* GuiQtTableComboBoxItem::serializeProtobuf(){
  bool updated = getAttributes();
  bool isValueInsideDataset(false);
  bool isValid(false);

  auto element = new in_proto::ComboBox;

  // base data
  bool ret = GuiQtDataField::serializeProtobuf(element->mutable_base(), false);

  // dataset
  int numWC = DataItem()->getNumOfWildcards();
  if (Attr()->DataSetIndexed() && numWC >= 1) {
    XferDataItemIndex *index = DataItem()->getDataItemIndexWildcard(numWC);
    int idx = index->getValue();
    m_set_input->SetDimensionIndizes(0,1,&idx);
    m_set_output->SetDimensionIndizes(0,1,&idx);
    m_set_strings->SetDimensionIndizes(0,1,&idx);
    if (idx >= m_picklist.size()) return element;  // return
  }

  std::string fvalue;
  m_param->getFormattedValue(fvalue);
  trim(fvalue);

  // dataset
  std::string dsfn = m_set_output->fullName(true);
  DataVector dVecOut;
  DataVector dVecIn;
  int id[1] = {-1};
  // output
  int n = 0;
  std::string s;
  while( m_set_output->GetValue( s, n++ ) ){
    element->add_output_dataset(s);
    trim(s);
    if (!isValueInsideDataset && s.size() && s == fvalue) isValueInsideDataset = true;
  }

  // input
  n = 0;
  while( m_set_input->GetValue( s, n++ ) ){
    element->add_input_dataset(s);
    trim(s);
    if (!isValid && s.size()) isValid= true;
  }

  // !!! add invalue value
  if (isValid &&
      !isValueInsideDataset && fvalue.size() &&
      element->output_dataset_size() && element->input_dataset_size()) {

    // warning color?
    element->mutable_base()->set_bgcolor("#ff0000");  // red
  }

  return element;
}
#endif
