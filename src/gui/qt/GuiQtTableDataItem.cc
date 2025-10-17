#include "gui/qt/GuiQtTableComboBoxItem.h"
#include "gui/qt/GuiQwtSlider.h"

#include <QTableView>
#include "utils/Debugger.h"

#include "utils/gettext.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtTableDataItem.h"
#include "gui/qt/GuiQtTable.h"
#include "gui/UnitManager.h"
#include <QModelIndex>

#include "app/DataPoolIntens.h"

INIT_LOGGER();

GuiQtTableDataItem::PasteConfirmDialogMode GuiQtTableDataItem::s_disableConfirmDialog = Inactivated;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtTableDataItem::GuiQtTableDataItem( GuiElement *parent )
  : GuiTableItem( ), GuiQtDataField( parent )
  , m_itemtype( item_None )
  , m_length( 10 )
  , m_row( -1 )
  , m_col( -1 )
{
}

GuiQtTableDataItem::GuiQtTableDataItem( const GuiQtTableDataItem &item )
  : GuiTableItem( item ), GuiQtDataField( item )
  , m_itemtype( item.m_itemtype )
  , m_length( item.m_length )
  , m_value( item.m_value )
  , m_row( item.m_row )
  , m_col( item.m_col )
{
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* cloneTableDataItem --                                                       */
/* --------------------------------------------------------------------------- */

GuiQtTableDataItem *GuiQtTableDataItem::cloneTableDataItem(){
  GuiQtTableDataItem * item = new GuiQtTableDataItem( *this );
  static_cast<GuiElement*>(item)->setParent( getParent() );
  return item;
}

/* --------------------------------------------------------------------------- */
/* setDataItem --                                                              */
/* --------------------------------------------------------------------------- */
#include "gui/GuiIndex.h"

void GuiQtTableDataItem::setDataItem( XferDataItem *item ){
  BUG(BugGuiTable,"GuiQtTableDataItem::setDataItem");
  assert( m_param != 0 );

  XferDataItemIndex *index = 0;
  switch( item->getNumOfWildcards() ){
  case 0:
    m_itemtype = GuiTableItem::item_Value;
    break;
  case 1:
    m_itemtype = GuiTableItem::item_Array;
    index = item->getDataItemIndexWildcard( 1 );
    index->setIndexName( "@col@", "" );
    break;
  case 2:
    m_itemtype = GuiTableItem::item_Matrix;
    index = item->getDataItemIndexWildcard( 1 );
    index->setIndexName( "@row@", "" );
    index = item->getDataItemIndexWildcard( 2 );
    index->setIndexName( "@col@", "" );
    break;
  default:
    assert( index != 0 ); // unmoeglich
    break;
  }
  UserAttr *attr = static_cast<UserAttr*>( Attr() );
  setHelptext( attr->Helptext() );

  // Als letztes muessen die Indizes in der DataReference initialisiert werden.
  // Diese Arbeiten uebernimmt immer das DataItem.
  m_param->DataItem()->setDimensionIndizes();
  registerIndex();
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiQtTableDataItem::isDataItemUpdated( TransactionNumber trans ) {
  BUG_PARA( BugGuiTable, "isDataItemUpdated", "TransactionNUmber=" << trans );

  if( m_param->DataItem() ){
    return m_param->DataItem()->isDataItemUpdated( trans );
  }
  BUG_EXIT( "no parameter" );
  return false;
}

/* --------------------------------------------------------------------------- */
/* registerIndex --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::registerIndex(){
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
      index->registerIndexedElement( getParent() );
    }
  }
  m_param->registerIndex( getParent() );
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtTableDataItem::setLength( int len ){
  // set inverted flag
  setInverted((len < 0));

  if( true /*len != 0*/ ){  // becourse of hidden lines length can be 0
    m_length = abs(len);
    if ( m_param->DataItem()->getDataType() == DataDictionary::type_String ) {
      return true;
    }
    return m_param->setLength( len );
  }
  if ( m_param->DataItem()->getDataType() == DataDictionary::type_String ) {
    return true;
  }
  return m_param->setLength( m_length );
}

/* --------------------------------------------------------------------------- */
/* setPrecision --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtTableDataItem::setPrecision( int prec ){
  return m_param->setPrecision( prec );
}

/* --------------------------------------------------------------------------- */
/* setThousandSep --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtTableDataItem::setThousandSep(){
  return m_param->setThousandSep();
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtTableDataItem::setScalefactor( Scale *scale ){
  return m_param->setScalefactor( scale );
}

/* --------------------------------------------------------------------------- */
/* getScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

Scale* GuiQtTableDataItem::getScalefactor(){
  return m_param->getScalefactor();
}

/* --------------------------------------------------------------------------- */
/* ItemType --                                                                 */
/* --------------------------------------------------------------------------- */

GuiTableItem::TableItemType GuiQtTableDataItem::ItemType(){
  return m_itemtype;
}


/* --------------------------------------------------------------------------- */
/* setFieldDataAttributes --                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::setFieldDataAttributes( int row, int col ){
  BUG_PARA(BugGuiTable,"GuiTableDataItem::setFieldDataAttributes",row << "," << col);
  setFieldColor( row, col );
}

/* --------------------------------------------------------------------------- */
/* setFieldColor --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::setFieldColor( int row, int col ){
  QColor background, foreground;
  int dark_fac;
  getTheColor( background, foreground, dark_fac );
  // nop
}

/* --------------------------------------------------------------------------- */
/* getTableItem --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiQtTableDataItem::getTableItem(){
  BUG(BugGuiTable,"GuiQtTableDataItem::getTableItem");
  checkAttributes( 0, 0 );
  return this;
}

/* --------------------------------------------------------------------------- */
/* getTableItem --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiQtTableDataItem::getTableItem( int col ){
  BUG_PARA(BugGuiTable,"GuiQtTableDataItem::getTableItem","col=" << col);
  if( acceptIndex( "@col@", col ) ){
    setTableIndex( "@col@", col );
    checkAttributes( 0, col );
    return this;
  }
  BUG_EXIT("NULL");
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getTableItem --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiQtTableDataItem::getTableItem( int row, int col ){
  BUG_PARA(BugGuiTable,"GuiQtTableDataItem::getTableItem","row=" << row << ", col=" << col);
  if( acceptIndex( "@row@", row ) && acceptIndex( "@col@", col ) ){
    setTableIndex( "@row@", row );
    setTableIndex( "@col@", col );
    checkAttributes( row, col );
    return this;
  }
  BUG_EXIT("NULL");
  return 0;
}

/* --------------------------------------------------------------------------- */
/* setTableIndex --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::setTableIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtTableDataItem::setTableIndex",name << " =[" << inx << "]");

  // Diese Funktion entspricht setIndex() im GuiDataField, jedoch ohne update().
  // Wir brauchen das für den internen Gebrauch.
  assert( m_param != 0 );
  m_param->DataItem()->setIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* getTableItemSize --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::getTableItemSize( int &rows, int &cols ) {
  rows = 1;
  cols = 1;

  if( !(m_itemtype == GuiTableItem::item_Array ||
        m_itemtype == GuiTableItem::item_Matrix ) ){
    return;
  }

  XferDataItemIndex *index1 = m_param->DataItem()->getDataItemIndexWildcard( 1 );
  assert( index1 != 0 );

  int num = index1->getDimensionSize( m_param->Data() );

  if( m_itemtype == GuiTableItem::item_Array ){
    cols = num;
    return;
  }

  XferDataItemIndex *index2 = m_param->DataItem()->getDataItemIndexWildcard( 2 );
  assert( index2 != 0 );
  rows = num;
  for( int i=0; i < num; i++ ) {
    index1->setIndex( m_param->Data(), i );
    int tmp = index2->getDimensionSize( m_param->Data() );
    if ( tmp > cols ) cols = tmp;
  }
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus GuiQtTableDataItem::checkFormat(){
  BUG(BugGuiTable,"GuiQtTableDataItem::checkFormat");
  // Buttom sollen immer geändert haben, damit die Function aufgerufen wird
  if( getDataField()->Attr()->IsButton())  return XferParameter::status_Changed;

  // look for invalid endline(s)
  if (m_value.find('\n') != std::string::npos) {
    m_value = QString::fromStdString(m_value).trimmed().toStdString();
    // return status_Bad if newline is inside string
    if ( m_value.find('\n') != std::string::npos)
      return XferParameter::status_Bad;
  }

  return m_param->checkFormat( m_value );
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtTableDataItem::setInputValue(){
  BUG_PARA(BugGuiTable,"GuiQtTableDataItem::setInputValue"
	  ,m_param->Data()->fullName( true ) << " = '" << m_value << "'");

  if( m_param->isEmpty( m_value ) ){
    m_param->clear();
    return true;
  }
  if( m_param->setFormattedValue( m_value ) ){
    return true;
  }
  BUG_EXIT("setValue failed");
  return false;
}

bool GuiQtTableDataItem::isPasteConfirmDialogMode(PasteConfirmDialogMode mode) {
  return (s_disableConfirmDialog == mode);
}

void GuiQtTableDataItem::setPasteConfirmDialogMode(PasteConfirmDialogMode mode) {
  s_disableConfirmDialog = mode;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::update( UpdateReason reason ){
   BUG(BugGuiTable,"GuiQtTableDataItem::update");
   if( reason == reason_Cancel || reason == reason_Always ){
     ResetLastWebUpdated();  // reason_Always for webtens
     if( getParent()->Type() == type_Table ) {
       GuiQtTable* table = dynamic_cast<GuiQtTable*>(getParent());
       table->updateItem(m_row, m_col, this);
     }
   }
   if (reason == reason_Unit){
     updateScale(m_param);
   }
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtTableDataItem::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG(BugGui, "GuiQtTableDataItem::hasChanged");
  if ( !xfer || (xfer && xfer->isAncestorOf( m_param->DataItem() ) ) ) {
    if (m_param->DataItem()->isDataItemUpdated(trans)) {
      BUG_EXIT("item '"<<m_param->DataItem()->getFullName(true)<<"' has Changed.");
      return true;
    }
  }
  BUG_EXIT("item '"<<m_param->DataItem()->getFullName(true)<<"' noChanges.");
  return false;
}

/* --------------------------------------------------------------------------- */
/* FinalWork --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::FinalWork(){
  BUG(BugGuiTable,"GuiQtTableDataItem::FinalWork");
  GuiQtTableDataItem *item = 0;

  if( getDataField()->isRunning() ){
    BUG_EXIT("a function is running. do nothing");
    return;
  }

  switch( checkFormat() ){
  case XferParameter::status_Bad:
    printMessage( _("Conversion error."), msg_Warning );
    setAttribute( DATAcolorAlarm );
    update( reason_Cancel );
    BUG_EXIT("checkFormat failed");
    return;

  case XferParameter::status_Unchanged:
    clearMessage();
    BUG_EXIT("checkFormat Unchanged");
    return;

  case XferParameter::status_Changed:
    clearMessage();
    item = cloneTableDataItem();
    std::string error_msg;
    s_TargetsAreCleared = m_param->DataItem()->StreamTargetsAreCleared(error_msg);
    if( isPasteConfirmDialogMode(Disabled)  || s_TargetsAreCleared ){ // Targets sind ok
      item->FinalWorkOk();
      return;
    }
    item->confirm(error_msg);
    return;
  }
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::confirmYesButtonPressed(){
  BUG(BugGuiTable,"GuiQtTableDataItem::confirmYesButtonPressed");
  FinalWorkOk();
  if ( isPasteConfirmDialogMode(Activated) ) setPasteConfirmDialogMode(Disabled);
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::confirmNoButtonPressed(){
   BUG(BugGuiTable,"GuiQtTableDataItem::confirmNoButtonPressed");
   update( reason_Cancel );
   s_DialogIsAktive = false;
   // set mode to inactivated to abort paste operation
   if ( isPasteConfirmDialogMode(Activated) ) setPasteConfirmDialogMode(Inactivated);
//   delete this;
}

/* --------------------------------------------------------------------------- */
/* FinalWorkOk --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::FinalWorkOk(){
  BUG(BugGuiTable,"GuiQtTableDataItem::FinalWorkOk");
  doFinalWork();
  s_DialogIsAktive = false;
}

/* --------------------------------------------------------------------------- */
/* EndOfWorkFinished --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::EndOfWorkFinished(){
  BUG(BugGuiTable,"GuiQtTableDataItem::EndOfWorkFinished");
  // delete this;
}

/* --------------------------------------------------------------------------- */
/* checkAttributes --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::checkAttributes( int row, int col ){
  if( m_row != row || m_col != col ){
//     m_row = row;
//     m_col = col;
    getAttributes();
  }
}

/* --------------------------------------------------------------------------- */
/* CellValue --                                                                */
/* --------------------------------------------------------------------------- */

std::string GuiQtTableDataItem::CellValue() {
  std::string text;
  m_param->getFormattedValue( text );
  if( text.empty() )
    text = std::string( m_length, ' ' );
  return text;
}

/* --------------------------------------------------------------------------- */
/* CellColor --                                                                */
/* --------------------------------------------------------------------------- */

// void GuiQtTableDataItem::CellColor(QColor& foreground, QColor& background) {
//   std::string text;
//   m_param->getFormattedValue( text );

//   ColorSet *colorset = DataPoolIntens::Instance().getColorSet( m_param->DataItem()->getUserAttr()->ColorsetName() );
//   if (!colorset) return false;
//   ColorSetItem *colorsetItem = colorset->getColorItem( value );
//   if (colorsetItem) {
//     foreground = QColor(QString::fromStdString(colorsetItem->foreground()));
//     background = QColor(QString::fromStdString(colorsetItem->background()));
//   }
// }

/* --------------------------------------------------------------------------- */
/* ValidateCell --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtTableDataItem::ValidateCell(const std::string &value ){
  BUG(BugGuiTable,"GuiQtTableDataItem::ValidateCell");
  std::string formatedtext;
  m_param->getFormattedValue( formatedtext );
  if( QString::fromStdString(formatedtext).trimmed() == QString::fromStdString(value).trimmed()){
    // Keine Aenderungen im Textfield. Der Grund des LosingFocus ist
    // nicht von Bedeutung.
    clearMessage();

    if(disabled()){
      protectField();
    }
    BUG_EXIT("Text not modified");
    // Buttom sollen immer geändert haben, damit die Function aufgerufen wird
    if( !getDataField()->Attr()->IsButton() )
      return;
  }
  m_value = value;
  FinalWork();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtTableDataItem::serializeXML(std::ostream &os, bool recursive){
  os << "<GuiTableDataItem";
  os << " row=\""<<m_row<<"\"";
  os << " col=\""<<m_col<<"\"";
  os << " length=\""<<m_length<<"\"";
  os << " value=\""<<m_value<<"\"";
//   os << ">";
//   os << m_value;
  os << ">" << std::endl;

  GuiQtDataField::serializeXML(os, recursive);


  os << "</GuiTableDataItem>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                             */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
in_proto::ComboBox* GuiQtTableDataItem::serializeProtobuf(){

  auto box = dynamic_cast<GuiQtTableComboBoxItem*>(this);
  if (box) {
    return box->serializeProtobuf();
  }
  auto ele = new in_proto::ComboBox;
  GuiQtDataField::serializeProtobuf(ele->mutable_base(), false);
  return ele;
}
#endif
