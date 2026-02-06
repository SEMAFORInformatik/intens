
#include <algorithm>
#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QStandardItem>
#include <QApplication>
#if QT_VERSION < 0x060000
#include <QDesktopWidget>
#endif
#include <QClipboard>
#include <QPainter>
#include <QSettings>

#include "utils/utils.h"
#include "utils/HTMLConverter.h"
#include "app/AppData.h"

#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtTable.h"
#include "gui/qt/GuiQtTableViewBase.h"
#include "gui/qt/GuiQtTableDataItem.h"
#include "gui/GuiTableLine.h"
#include "gui/GuiTableItem.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtLabel.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/gettext.h"
#include "app/DataPoolIntens.h"
#include "datapool/DataPoolDefinition.h"
#include "job/JobFunction.h"
#include "job/JobManager.h"

INIT_LOGGER();

#define XRTTBL_ALLCELLS  -998 /**< all cells and labels in a row or column */
#define XRTTBL_ALL       -997 /**< all cells (but not labels) in a row or column */

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtTable::GuiQtTable( GuiElement *parent, const std::string name )
  : GuiContainer( parent, GuiElement::type_Table )
  , GuiTable( parent)
  , GuiQtElement( parent, name)
  , m_model( 0 )
  , m_tablewidget( 0 )
  , m_frame( 0 )
//   , m_field_string( 0 )
//   , m_field_float( 0 )
//   , m_field_int( 0 )
//   , m_field_combotext( 0 )
//   , m_old_data_type( 0 )
//   , m_new_data_type( 0 )
  , m_cloneTable( 0 )
{
}

GuiQtTable::GuiQtTable( const GuiQtTable& tbl)
: GuiContainer( tbl )
, GuiTable( tbl)
, GuiQtElement( tbl )
, m_model( 0 )
, m_tablewidget( 0 )
, m_frame( 0 )
, m_cloneTable( &tbl )
{}

GuiQtTable::~GuiQtTable(){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */
GuiElement* GuiQtTable::clone() {
  GuiElement* baseElem = findElement( getName() );
  if (baseElem == this->getElement()) {
    getPopupMenu();
    getRowMenu();
    getColumnMenu();
    m_clonedTable.push_back( new GuiQtTable( *this ) );
  } else
    return baseElem->clone();
  return m_clonedTable.back();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtTable::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  cList.insert(cList.begin(), m_clonedTable.begin(), m_clonedTable.end());
}

/* --------------------------------------------------------------------------- */
/*getPopupMenu --                                                              */
/* --------------------------------------------------------------------------- */
GuiPopupMenu* GuiQtTable::getPopupMenu() {
  return m_tablewidget ? m_tablewidget->getMenu() : NULL;
}

/* --------------------------------------------------------------------------- */
/* setFieldLength --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtTable::setFieldLength ( GuiTableItem *item, int row, int col, GuiTableLine* line ) {
  long nRows = m_model->rowCount();
  long nCols = m_model->columnCount();
  char fontChar = (item->getDataField()->getDataType() == DataDictionary::type_String ||
		   item->getDataField()->getDataType() == DataDictionary::type_CharData) ? 'W' : '0';
#if QT_VERSION > QT_VERSION_CHECK(5,11,0)
  int charlen = m_tablewidget->fontMetrics().horizontalAdvance( fontChar );
#else
  int charlen = m_tablewidget->fontMetrics().width( fontChar );
#endif

  // set visible flag
  if (col == XRTTBL_ALLCELLS) {
    m_tablewidget->setRowHidden( row, !line->isVisible() );
  }
  if (row == XRTTBL_ALLCELLS) {
    m_tablewidget->setColumnHidden( col, !line->isVisible() );
    if (m_tablewidget->m_horzFrozenTableView)
      m_tablewidget->m_horzFrozenTableView->setColumnHidden( col, !line->isVisible() );
    if (m_tablewidget->m_vertFrozenTableView)
      m_tablewidget->m_vertFrozenTableView->setColumnHidden( col, !line->isVisible() );
    if (m_tablewidget->m_vertHorzFrozenTableView)
      m_tablewidget->m_vertHorzFrozenTableView->setColumnHidden( col, !line->isVisible() );
  }

  if (!item){
    return false;
  }

  // set width array
  if (row == XRTTBL_ALL && col >= 0 && col < nCols) {
    if (!line->isVisible() ||
        item->getDataField()->getLength() == 0) {
      m_tablewidget->setColumnWidth( col, 0 );
      m_tablewidget->setColumnHidden( col, true );
    } else {
      m_tablewidget->setColumnWidth( col, GuiQtTableViewBase::COLUMN_MARGIN_WIDTH + charlen*(0.5+item->getDataField()->getLength()) );
      m_tablewidget->setColumnHidden( col, false );
    }
    return false;
  }
  else {
    m_tablewidget->setColumnWidth( col, item->getDataField()->getLength() ?
                                   GuiQtTableViewBase::COLUMN_MARGIN_WIDTH + charlen*(0.5+item->getDataField()->getLength()) : 0);
    m_tablewidget->setColumnHidden( col, item->getDataField()->getLength() == 0 || !line->isVisible());
  }

  // set width matrix
  if ( (col == XRTTBL_ALLCELLS && row == XRTTBL_ALL) ||
       (col == XRTTBL_ALL && row == XRTTBL_ALL) ) {
    int maxlen=0;
    GuiTableLineList::iterator li;
    int c=(int)getVLineList().size();
    GuiTableItem *itm;
    for( li = getTableList().begin(); li != getTableList().end(); ++li ){
      if ( (itm = (*li)->getTableItem(c)) ){
        maxlen = std::max(maxlen, itm->getDataField()->getLength());
      }
    }
    for (; c<nCols; ++c){
      if (maxlen == 0) {
        m_tablewidget->setColumnWidth( c, 0 );
        m_tablewidget->setColumnHidden( col, true );
      } else {
        m_tablewidget->setColumnWidth( c, GuiQtTableViewBase::COLUMN_MARGIN_WIDTH + charlen*(0.5+maxlen) );
        m_tablewidget->setColumnHidden( col, false );
      }
    }
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setFieldTableAttributes --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setFieldTableAttributes(){
  BUG_DEBUG("GuiTable::setFieldTableAttributes");
  GuiTableItem *itm;
  int row = 0, col = 0;
  GuiTableLineList::iterator li;
  int lines = 0;

  // fixed area
  for( li = getVLineList().begin(); li != getVLineList().end(); ++li ){
    row = XRTTBL_ALLCELLS;
    col = li - getVLineList().begin();
    if( (itm=(*li)->getTableItem( 0 )) != 0 ){
      setFieldLength( itm, row, col, (*li) );
    }
  }
  for( li = getHLineList().begin(); li != getHLineList().end(); ++li ){
    row = li -getHLineList().begin();
    col = XRTTBL_ALLCELLS;
    if( (itm=(*li)->getTableItem( 0 )) != 0 ){
      setFieldLength( itm, row, col, (*li) );
    }
  }

  // content
  for( li = getTableList().begin(); li != getTableList().end(); ++li ){
    BUG_DEBUG( "Table Line " << (lines+1) );
    int n = 0;
    bool endofline = false;
    while( !endofline ){
      if( (itm=(*li)->getTableItem( n )) != 0 ){
        switch( itm->ItemType() ){
        case GuiTableItem::item_Matrix:
          BUG_DEBUG("Matrix");
          row = XRTTBL_ALLCELLS;
          col = XRTTBL_ALLCELLS;
          itm->setFieldDataAttributes( row, col );
          if (setFieldLength( itm, XRTTBL_ALL, XRTTBL_ALL, (*li) ))
            return; // all processed
          endofline = true;
          break;

        case GuiTableItem::item_Array:
          BUG_DEBUG("Array");
          if( getOrientation() == orient_Vertical ){
            row = XRTTBL_ALLCELLS;
            col = lines + (int)getVLineList().size();
          }
          else{
            row = lines + (int)getHLineList().size();
            col = XRTTBL_ALLCELLS;
          }
          itm->setFieldDataAttributes( row, col );
          setFieldLength( itm, row, col, (*li) );
          endofline = true;
          break;

        case GuiTableItem::item_Value:
          BUG_DEBUG("Value");
          if( getOrientation() == orient_Vertical ){
            row = n + (int)getHLineList().size();
            col = lines + (int)getVLineList().size();
          }
          else{
            row = lines + (int)getHLineList().size();
            col = n + (int)getVLineList().size();
          }
          itm->setFieldDataAttributes( row, col );
          break;

        case GuiTableItem::item_None:
          BUG_DEBUG("None");
          break;

        case GuiTableItem::item_Label:
          BUG_DEBUG("Label");
          if( getOrientation() == orient_Vertical ){
            row = n + (int)getHLineList().size();
            col = lines + (int)getVLineList().size();
          }
          else{
            row = lines + (int)getHLineList().size();
            col = n + (int)getVLineList().size();
          }
          itm->setFieldLabelAttributes( row, col, row, col );
          break;

        default:
          assert( false );
          break;
        }
      }
      else{
        endofline = true;
      }
      n++;
    }
    lines++;
  }
}

/* --------------------------------------------------------------------------- */
/* setFieldHorizontalAttributes --                                             */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setFieldHorizontalAttributes(){
  BUG_DEBUG("GuiTable::setFieldHorizontalAttributes");
  GuiTableItem *itm;
  int row = 0, col = 0;

  GuiTableLineList::iterator li;
  for( li = getHLineList().begin(); li != getHLineList().end(); ++li ){
    int n = 0;
    bool endofline = false;
    while( !endofline ){
      if( (itm=(*li)->getTableItem( n )) != 0 ){
        switch( itm->ItemType() ){
        case GuiTableItem::item_Matrix:
          assert( false );

        case GuiTableItem::item_Array:
          itm->setFieldDataAttributes( row, col );
          endofline = true;
          break;

        case GuiTableItem::item_Value:
          col = n + (int)getVLineList().size();
          itm->setFieldDataAttributes( row, col );
          break;

        case GuiTableItem::item_Label:
          col = n + (int)getVLineList().size();
          itm->setFieldLabelAttributes( row, col, row, col );
          break;

        default:
          break;
        }
      }
      else{
        endofline = true;
      }
      n++;
    }
    row++;
  }
}

/* --------------------------------------------------------------------------- */
/* setFieldVerticalAttributes --                                               */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setFieldVerticalAttributes(){
  BUG_DEBUG("GuiTable::setFieldVerticalAttributes");
  GuiTableItem *itm;
  int row = 0, col = 0;

  GuiTableLineList::iterator li;
  for( li = getVLineList().begin(); li != getVLineList().end(); ++li ){
    BUG_DEBUG( "Vertical Line " << (col+1) );
    int n = 0;
    bool endofline = false;
    while( !endofline ){
      if( (itm=(*li)->getTableItem( n )) != 0 ){
        switch( itm->ItemType() ){
        case GuiTableItem::item_Matrix:
          assert( false );

        case GuiTableItem::item_Array:
	  BUG_DEBUG( "Array");
          itm->setFieldDataAttributes( row, col );
	  if (setFieldLength( itm, XRTTBL_ALL, col, (*li) ))
	    return; // all processed
          endofline = true;
          break;

        case GuiTableItem::item_Value:
          BUG_DEBUG( "Value");
          row = n + (int)getHLineList().size();
          itm->setFieldDataAttributes( row, col );
          break;

        case GuiTableItem::item_Label:
          BUG_DEBUG("Label");
          row = n + (int)getHLineList().size();
          itm->setFieldLabelAttributes( row, col, row, col );
          break;

        default:
          break;
        }
      }
      else{
        endofline = true;
      }
      n++;
    }
    col++;
  }
}

/* --------------------------------------------------------------------------- */
/* setFieldAttributes --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setFieldAttributes(){
  BUG_DEBUG("GuiTable::setFieldAttributes");
  syncroniseTableItemLength();

  setFieldTableAttributes();
  setFieldHorizontalAttributes();
  setFieldVerticalAttributes();

}
/* --------------------------------------------------------------------------- */
/* setFieldPicklists --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setFieldPicklists(){
  BUG_DEBUG("GuiTable::setFieldPicklists" );
  setFieldTablePicklists();
  setFieldHorizontalPicklists();
  setFieldVerticalPicklists();
}

/* --------------------------------------------------------------------------- */
/* setFieldTablePicklists --                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setFieldTablePicklists(){
    BUG_DEBUG("GuiTable::setFieldTablePicklists");
  GuiTableItem *itm;
  int row = 0, col = 0;
  GuiTableLineList::iterator li;
  int lines = 0;

  for( li = getTableList().begin(); li != getTableList().end(); ++li ){
    BUG_DEBUG("Table Line " << (lines+1) );
    int n = 0;
    bool endofline = false;
    while( !endofline ){
      if( (itm=(*li)->getTableItem( n )) != 0 ){
        switch( itm->ItemType() ){
        case GuiTableItem::item_Matrix:
	    BUG_DEBUG("Matrix");
          itm->setFieldPicklist( row, col );
          endofline = true;
	  break;

        case GuiTableItem::item_Array:
	    BUG_DEBUG("Array");
          if( getOrientation() == orient_Vertical ){
	    row = XRTTBL_ALLCELLS;
            col = lines + (int)getVLineList().size();
          }
          else{
            row = lines + (int)getHLineList().size();
	    col = XRTTBL_ALLCELLS;
          }
          itm->setFieldPicklist( row, col );
          endofline = true;
          break;

        case GuiTableItem::item_Value:
	    BUG_DEBUG("Value");
          if( getOrientation() == orient_Vertical ){
            row = n + (int)getHLineList().size();
            col = lines + (int)getVLineList().size();
          }
          else{
            row = lines + (int)getHLineList().size();
            col = n + (int)getVLineList().size();
          }
          itm->setFieldPicklist( row, col );
          break;

        default:
          break;
        }
      }
      else{
        endofline = true;
      }
      n++;
    }
    lines++;
  }
}

/* --------------------------------------------------------------------------- */
/* setFieldVerticalPicklists --                                                */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setFieldVerticalPicklists(){
  BUG_DEBUG("GuiTable::setFieldVerticalPicklists");
  GuiTableItem *itm;
  int row = 0, col = 0;

  GuiTableLineList::iterator li;
  for( li = getHLineList().begin(); li != getHLineList().end(); ++li ){
    std::string str(getHLineList()[row]->Label());
    std::string ttp(getHLineList()[row]->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(row, Qt::Vertical, QtMultiFontString::getQString(str) );
    m_model->verticalHeaderItem(row)->setToolTip(QString::fromStdString(ttp));

    int n = 0;
    bool endofline = false;
    while( !endofline ){
      if( (itm=(*li)->getTableItem( n )) != 0 ){
        switch( itm->ItemType() ){
        case GuiTableItem::item_Array:
          itm->setFieldPicklist( row, col );
          endofline = true;
          break;

        case GuiTableItem::item_Value:
          col = n + (int)getVLineList().size();
          itm->setFieldPicklist( row, col );
          break;

        default:
          break;
        }
      }
      else{
        endofline = true;
      }
      n++;
    }
    row++;
  }

  // special case numerical labels matrix
  int off=getTableRowRangeFrom();
  if (getTableMatrix())
    for(int v=0; v<getTableList().size(); ++v )
      m_model->setHeaderData(v+getHLineList().size(), Qt::Vertical, QString::number(v+off+1-1) );

  // set vertical numerical labels
  off -= getHLineList().size();
  int v = (getOrientation() == orient_Horizontal) ? getTableList().size() : 0;
  for(v+=getHLineList().size(); v<m_model->rowCount(); ++v ){
    m_model->setHeaderData(v, Qt::Vertical, QString::number(v+off+1-1) );
  }
}

/* --------------------------------------------------------------------------- */
/* setFieldHorizontalPicklists --                                              */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setFieldHorizontalPicklists(){
  BUG(BugGuiTable,"GuiTable::setFieldHorizontalPicklists");
  GuiTableItem *itm;
  int row = 0, col = 0;

  GuiTableLineList::iterator li;
  for( li = getVLineList().begin(); li != getVLineList().end(); ++li ){
    std::string str(getVLineList()[col]->Label());
    std::string ttp(getVLineList()[col]->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(col, Qt::Horizontal, QtMultiFontString::getQString(str));
    m_model->horizontalHeaderItem(col)->setToolTip(QString::fromStdString(ttp));
    BUG_MSG("Vertical Line " << (col+1) );
    int n = 0;
    bool endofline = false;
    while( !endofline ){
      if( (itm=(*li)->getTableItem( n )) != 0 ){
        switch( itm->ItemType() ){
        case GuiTableItem::item_Array:
	  BUG_MSG("Array");
          itm->setFieldPicklist( row, col );
          endofline = true;
          break;

        case GuiTableItem::item_Value:
          BUG_MSG("Value");
          row = n + (int)getHLineList().size();
          itm->setFieldPicklist( row, col );
          break;

        default:
          break;
        }
      }
      else{
        endofline = true;
      }
      n++;
    }
    col++;
  }

  // special case numerical labels matrix
  int off=getTableColRangeFrom();
  if (getTableMatrix())
    for(int v=0; v<getTableList().size(); ++v ) {
      m_model->setHeaderData(v+getVLineList().size(), Qt::Horizontal, QString::number(v+off+1-1) );
    }

  // set horizontal numerical labels
  off -= getVLineList().size();
  int v = (getOrientation() == orient_Vertical) ? getTableList().size() : 0;
  for(v+=getVLineList().size()+getTableList().size(); v<m_model->columnCount(); ++v ){
    m_model->setHeaderData(v, Qt::Horizontal, QString::number(v+off+1-1) );
  }
}

/* --------------------------------------------------------------------------- */
/* createTableHorizontal --                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtTable::createTableHorizontal(){
  BUG(BugGuiTable,"GuiTable::createTableHorizontal");
  int numRows=0, numCols=0;

  getTableHorizontalSize( numRows, numCols );
  BUG_DEBUG(  "createTableHorizontal++["
		 <<numRows<<","<<numCols<<"]");

  setTableRows( (int)getTableList().size() );

  m_model->insertColumns(0, numCols);
  m_model->insertRows(0, numRows);

  // set vertical labels
  GuiTableLineList::iterator li;
  int h = 0;
  for ( h = 0; h < (int)getHLineList().size(); ++h) {
    std::string str(getHLineList()[h]->Label());
    std::string ttp(getHLineList()[h]->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(h, Qt::Vertical, QtMultiFontString::getQString(str));
    m_model->verticalHeaderItem(h)->setToolTip(QString::fromStdString(ttp));
  }
  for( li = getTableList().begin(); li != getTableList().end(); ++li, ++h ){
    std::string str((*li)->Label());
    std::string ttp((*li)->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(h, Qt::Vertical, QtMultiFontString::getQString(str));
    m_model->verticalHeaderItem(h)->setToolTip(QString::fromStdString(ttp));
  }

  // set horizontal labels
  int v;
  for ( v = 0; v < (int)getVLineList().size(); ++v) {
    std::string str(getVLineList()[v]->Label());
    std::string ttp(getVLineList()[v]->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(v, Qt::Horizontal, QtMultiFontString::getQString(str));
    m_model->horizontalHeaderItem(v)->setToolTip(QString::fromStdString(ttp));
  }
  int off=getTableColRangeFrom();
  for( int i = 0; i < numCols; ++i, ++v ){
    m_model->setHeaderData(v, Qt::Horizontal, i+off );
  }
}

/* --------------------------------------------------------------------------- */
/* createTableVertical --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQtTable::createTableVertical(){
  BUG_DEBUG( "createTableVertical");
  BUG(BugGuiTable,"GuiTable::createTableVertical");
  int numRows=0, numCols=0;

  getTableVerticalSize( numRows, numCols );

  setTableCols( (int)getTableList().size() );

  m_model->insertColumns(0, numCols);
  m_model->insertRows(0, numRows);

  // set horizontal labels
  GuiTableLineList::iterator li;
  int v = 0;
  for ( v = 0; v < (int)getVLineList().size(); ++v) {
    std::string str(getVLineList()[v]->Label());
    std::string ttp(getVLineList()[v]->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(v, Qt::Horizontal, QtMultiFontString::getQString(str));
    m_model->horizontalHeaderItem(v)->setToolTip(QString::fromStdString(ttp));
  }
  for( li = getTableList().begin(); li != getTableList().end(); ++li, ++v ){
    std::string str((*li)->Label());
    std::string ttp((*li)->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(v, Qt::Horizontal, QtMultiFontString::getQString(str));
    m_model->horizontalHeaderItem(v)->setToolTip(QString::fromStdString(ttp));
  }

  // set vertical labels
  int h = 0;
  for ( h = 0; h < (int)getHLineList().size(); ++h) {
    std::string str(getHLineList()[h]->Label());
    std::string ttp(getHLineList()[h]->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(h, Qt::Vertical, QtMultiFontString::getQString(str));
    m_model->verticalHeaderItem(h)->setToolTip(QString::fromStdString(ttp));
  }
  int off=getTableRowRangeFrom();
  for( int i = 0; i < m_model->rowCount(); ++i, ++h ){
    m_model->setHeaderData(h, Qt::Vertical, QString::number(i+off) );
  }
}

/* --------------------------------------------------------------------------- */
/* createTableMatrix --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtTable::createTableMatrix(){
  BUG_DEBUG( " createTableMatrix");
  int numRows=0, numCols=0;

  getTableMatrixSize( numRows, numCols );

  BUG_DEBUG("numRows: " << numRows << "  numCols: " << numCols );
  m_model->insertColumns(0, numCols);
  m_model->insertRows(0, numRows);

  // set horizontal labels
  GuiTableLineList::iterator li;
  int v = 0;
  for ( v = 0; v < (int)getVLineList().size(); ++v) {
    std::string str(getVLineList()[v]->Label());
    std::string ttp(getVLineList()[v]->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(v, Qt::Horizontal, QtMultiFontString::getQString(str));
    m_model->horizontalHeaderItem(v)->setToolTip(QString::fromStdString(ttp));
  }
  int voff=getTableColRangeFrom();
  for( ; v<m_model->columnCount(); ++v ){
    m_model->setHeaderData(v, Qt::Horizontal, QString::number(v-getVLineList().size()+voff) );
  }

  // set vertical labels
  int h = 0;
  for ( h = 0; h < (int)getHLineList().size(); ++h) {
    std::string str(getHLineList()[h]->Label());
    std::string ttp(getHLineList()[h]->Tooltip());
    HTMLConverter::convertFromHTML(str);
    m_model->setHeaderData(h, Qt::Vertical, QtMultiFontString::getQString(str));
    m_model->verticalHeaderItem(h)->setToolTip(QString::fromStdString(ttp));
  }
  int hoff=getTableRowRangeFrom();
  for( int i = 0; i < m_model->rowCount(); ++i, ++h ){
    m_model->setHeaderData(h, Qt::Vertical, QString::number(i+hoff) );
  }
}

/* --------------------------------------------------------------------------- */
/* addTitles --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtTable::addTitles(){
  std::string title;

  // set table title if exists
  if( getTitle().size() ){
    title = getTitle();
    if( !getTitleHorizontal().empty() && getHorzLinePlacement() == align_Top ){
      title += '\n';
      title += '\n';
      title += getTitleHorizontal();
      getTitleHorizontal().erase();
    }

    Qt::AlignmentFlag align = getQtAlignment( getTitleAlignment() );
    QLabel* titlewidget = new QLabel(QtMultiFontString::getQString(title), m_frame);
    QFont font =  titlewidget->font();
    titlewidget->setFont( QtMultiFontString::getQFont( "@title@", font ) );
    dynamic_cast<QGridLayout*>(m_frame->layout())->addWidget( titlewidget, 0, 1, align|Qt::AlignTop );
    titlewidget->setObjectName("GuiTableTitle");

    QLabel* titlewidget2 = new QLabel( QtMultiFontString::getQString( getTitleVertical() ).split(QRegularExpression("\\s"), Qt::SkipEmptyParts).join("\n"), m_frame);
    titlewidget2->setFont( QtMultiFontString::getQFont( "@title@", font ) );
    dynamic_cast<QGridLayout*>(m_frame->layout())->addWidget( titlewidget2, 1, 0, Qt::AlignLeft );
  }

  // set other fonts
  QFont font = m_tablewidget->horizontalHeader()->font();
  m_tablewidget->setHeaderFont( QtMultiFontString::getQFont( "@label@", font ) );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtTable::setIndex",name << " =[" << inx << "]");

  GuiTableLineList::iterator iter;
  for( iter = getTableList().begin(); iter != getTableList().end(); ++iter )
    (*iter)->setIndex( name, inx );
  for( iter = getVLineList().begin(); iter != getVLineList().end(); ++iter )
    (*iter)->setIndex( name, inx );
  for( iter = getHLineList().begin(); iter != getHLineList().end(); ++iter )
    (*iter)->setIndex( name, inx );

  if(myWidget()) {// if created!!!
    update( reason_Always );
  }

  // process clones
  std::vector<GuiQtTable*>::iterator it =  m_clonedTable.begin();
  for (; it != m_clonedTable.end(); ++it)
    (*it)->setIndex(name, inx);
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtTable::acceptIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtTable::acceptIndex",name << " =[" << inx << "]");

  // if not created => return true
  if(myWidget() == NULL)
    return true;

  GuiTableLineList::iterator iter;
  for( iter = getTableList().begin(); iter != getTableList().end(); ++iter )
    if ( !(*iter)->acceptIndex( name, inx ) )
      return false;
  for( iter = getVLineList().begin(); iter != getVLineList().end(); ++iter )
    if ( !(*iter)->acceptIndex( name, inx ) )
      return false;
  for( iter = getHLineList().begin(); iter != getHLineList().end(); ++iter )
    if ( !(*iter)->acceptIndex( name, inx ) )
      return false;

  return true;
}

QWidget* GuiQtTable::myWidget() {
  return m_frame;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTable::create( ){
  BUG_DEBUG("GuiQtTable::create");

  // new frame with layout
  assert( m_frame == 0);
  m_frame = new QFrame();
  QFont font =  m_frame->font();
  QGridLayout *vboxlayout = new QGridLayout();
  vboxlayout->setContentsMargins(0,0,0,0);
  vboxlayout->setSpacing( 0*3 );
  m_frame->setLayout( vboxlayout );

  m_model = new QtStandardItemModel(this);

  if (!m_tablewidget)
    m_tablewidget = new GuiQtTableViewBase(m_model,
					   NULL,this,
					   new QtHeaderView(this, Qt::Horizontal),
					   new QtHeaderView(this, Qt::Vertical));

  if (!isEnabled())
    disable();

  // are headers invisible?
  if (isTableRowHeaderHidden())
    m_tablewidget->verticalHeader()->hide();
  if (isTableColHeaderHidden())
    m_tablewidget->horizontalHeader()->hide();

  m_tablewidget->setFont( QtMultiFontString::getQFont( "@list@", font ) );
  m_tablewidget->setItemDelegate(new SpreadSheetDelegate(this));
  vboxlayout->addWidget( m_tablewidget, 1, 1 );
  connect( m_tablewidget->itemDelegate(), SIGNAL(commitData(QWidget*)),this, SLOT(slotCommitData(QWidget*)));
//   QObject::connect( m_model, SIGNAL( itemChanged(QStandardItem*) ),
//  		    this, SLOT( slotItemChanged(QStandardItem*) ) );

  BUG_DEBUG("rows: " << getTableRows() << "  cols: " << getTableCols() );

  assert( getTableList().size() > 0 );
  int _contentWidthSB = 0, _contentHeightSB = 0;
  m_frame->setSizePolicy( QSizePolicy( isTableHorizontalFixed() ? QSizePolicy::Fixed : QSizePolicy::MinimumExpanding,
                                       isTableVerticalFixed() ? QSizePolicy::Fixed : QSizePolicy::MinimumExpanding) );
  if( getTableMatrix() ){
    createTableMatrix();
    // fixed vertical table?
    if (!isTableVerticalFixed()) {
      m_tablewidget->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
      _contentWidthSB = m_tablewidget->verticalScrollBar()->sizeHint().width();
    }
    // fixed horizontal table?
    if (!isTableHorizontalFixed()) {
      m_tablewidget->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
      _contentHeightSB = m_tablewidget->horizontalScrollBar()->sizeHint().height();
    }
  }
  else{
    if( getOrientation() == orient_Vertical ){
      createTableVertical();
      // fixed vertical table?
      if (!isTableVerticalFixed()) {
        m_tablewidget->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
        _contentWidthSB = m_tablewidget->verticalScrollBar()->sizeHint().width();
      }
    }
    else{
      createTableHorizontal();
      // fixed horizontal table?
      if (!isTableHorizontalFixed()) {
        m_tablewidget->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
        _contentHeightSB = m_tablewidget->horizontalScrollBar()->sizeHint().height();
      }
    }
  }
  addTitles();
  setFieldAttributes();

  setWidgetSize(size_init);
  m_tablewidget->postInit();
  setDebugTooltip();

  m_tablewidget->setObjectName(QString::fromStdString(getName()));
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTable::manage(){
  BUG(BugGui,"GuiTable::manage");
  if( myWidget() != 0 )
    myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTable::enable(){
  // enable cloned and parent
  if (m_tablewidget)
    m_tablewidget->setEditTriggers(QAbstractItemView::CurrentChanged);
  GuiElement::enable();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtTable::disable(){
  // disable cloned and parent
  if (m_tablewidget) {
    m_tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  }
  GuiElement::disable();
}

/* --------------------------------------------------------------------------- */
/* isEditable --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtTable::isEditable(){
	return isEnabled();
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTable::update( UpdateReason reason ){
  BUG_PARA( BugGuiTable, "update", "reason=" << reason );

  if( m_tablewidget == 0 )
    return;
  int vertOldWidth = m_tablewidget->verticalHeader()->size().width();
  int numRows=0, numCols=0;

  switch( reason ){
  case reason_Process:
  case reason_FieldInput:
    if( !isDataItemUpdated( GuiManager::Instance().LastGuiUpdate() ) ){
      return;
    }
  case reason_Cycle:
  case reason_Always:
  case reason_Cancel:
    ResetLastWebUpdated();  // reason_Always for webtens
    break;

  case reason_Unit:
    updateUnitFields();
    break;
  default:
    BUG_DEBUG("Unhandled Update Reason");
    break;
  }

  getTableSize( numRows, numCols );
  setRowCount(numRows);
  setColumnCount(numCols);

  BUG_DEBUG("numRows: " << numRows << "  numCols: " << numCols );
  setValues();
  setFieldPicklists();
  setFieldAttributes();

  // damit es auch angezeigt wird
  setWidgetSize(size_visible);
  m_tablewidget->repaint();
  m_tablewidget->updateFrozenParts();

  // Korrektur der Breite?
  if (vertOldWidth != m_tablewidget->verticalHeader()->size().width()) {
    m_tablewidget->setMinimumWidth(m_tablewidget->size().width() + (m_tablewidget->verticalHeader()->size().width() - vertOldWidth));
  }
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiTable::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG( BugGuiTable, "hasChanged" );

  GuiTableLineList::iterator iter;
  for( iter = getTableList().begin(); iter != getTableList().end(); ++iter )
    if ( (*iter)->hasChanged(trans, xfer, show) )
      return true;
  for( iter = getVLineList().begin(); iter != getVLineList().end(); ++iter )
    if ( (*iter)->hasChanged(trans, xfer, show) )
      return true;
  for( iter = getHLineList().begin(); iter != getHLineList().end(); ++iter )
    if ( (*iter)->hasChanged(trans, xfer, show) )
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiTable::isDataItemUpdated( TransactionNumber trans ) {
  BUG_PARA( BugGuiTable, "isDataItemUpdated", "TransactionNUmber=" << trans );

  GuiTableLineList::iterator iter;
  for( iter = getTableList().begin(); iter != getTableList().end(); ++iter ){
    if ( (*iter)->isDataItemUpdated(trans) ){
      return true;
    }
  }
  for( iter = getVLineList().begin(); iter != getVLineList().end(); ++iter ){
    if ( (*iter)->isDataItemUpdated(trans) ){
      return true;
    }
  }
  for( iter = getHLineList().begin(); iter != getHLineList().end(); ++iter ){
    if ( (*iter)->isDataItemUpdated(trans) ){
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------
// serializeXML
//---------------------------------------------------------------------------

void GuiQtTable::serializeXML(std::ostream &os, bool recursive ){
  GuiTable::serializeXML(os, recursive);
#if 0
  os << "<intens:Table";
  os << " lines=\""<<getTableList().size()<<"\"";
  os << ">" << std::endl;

  GuiTableLineList::iterator li;
  for( li = getTableList().begin(); li != getTableList().end(); ++li ){
    (*li)->serializeXML(os, recursive);
  }

  os << "</intens:Table>"<<std::endl;
#endif
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtTable::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return GuiTable::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtTable::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiTable::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* updateItem --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtTable::updateItem( int row, int col, GuiTableItem* item ) {
  BUG_DEBUG(  "updateItem row["<<row<<"] col["<<col<<"]");
  if (!item) return;

  if ( col<0 || row <0) {
    int colStart(col), colEnd(col), rowStart(row), rowEnd(row);
    if (col == XRTTBL_ALLCELLS || col == XRTTBL_ALL) {
      colStart = 0;
      colEnd  = getTableCols()-1;
    }
    if (row == XRTTBL_ALLCELLS || row == XRTTBL_ALL) {
      rowStart = 0;
      rowEnd  = getTableRows()-1;
    }

    for (int r = rowStart; r <= rowEnd; ++r)
      for (int c = colStart; c <= colEnd; ++c) {
        if (m_model)
          m_model->setData(m_model->index(r, c, QModelIndex()),
                           QString::fromStdString(item->CellValue()), Qt::DisplayRole );
      }
  } else
    if (m_model)
      m_model->setData(m_model->index(row, col, QModelIndex()),
                       QString::fromStdString(item->CellValue()), Qt::DisplayRole );
}

/* --------------------------------------------------------------------------- */
/* deleteSelectedData --                                                       */
/* --------------------------------------------------------------------------- */
void GuiQtTable::deleteSelectedData( bool use_row) {
  std::set<int> vec;

  QModelIndexList selIdxs = m_tablewidget->selectedIndexes();  // column sorted
  if (selIdxs.size() == 0)
    return;

  // get indexes of selected rows or columns
  for (long i = 0; i < selIdxs.size(); ++i) {
    vec.insert( use_row ? selIdxs.at(i).row() : selIdxs.at(i).column() );
  }

  // iterate over selected rows or column
  DeleteFunctor functor;
  editData( use_row, functor, false, vec);

  m_tablewidget->clearSelection();
}

/* --------------------------------------------------------------------------- */
/* clearSelection --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQtTable::clearSelection() {
  BUG(BugGui,"GuiQtList::clearSelection");
  if (m_tablewidget)
	m_tablewidget->clearSelection();
}

/* --------------------------------------------------------------------------- */
/* getSelectedItems --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiQtTable::getSelectedItems(std::vector<int>& row_idxs, std::vector<int>& column_idxs) {
  row_idxs.clear();
  column_idxs.clear();
  std::vector<int> res_row_idxs, res_column_idxs;

  if ( !m_tablewidget ) {
    // find in cloned lists
    for (std::vector<GuiQtTable*>::iterator it =  m_clonedTable.begin();
		 it != m_clonedTable.end(); ++it)
      if ( (*it)->getSelectedItems( row_idxs, column_idxs)  && row_idxs.size() > 0)
		return true;
    return false;
  }
  // get all selected cells
  QModelIndexList idxList = m_tablewidget->selectedIndexes();
  for (int i = 0; i < idxList.size(); ++i) {
	row_idxs.push_back( idxList.at(i).row() );
	column_idxs.push_back( idxList.at(i).column() );
  }

  // if complete row is selected, return only row indexes
  std::vector<int>::iterator rit = row_idxs.begin();
  for (; rit != row_idxs.end(); ++rit) {
	if ( std::count(row_idxs.begin(), row_idxs.end(), (*rit)) == m_model->columnCount()) {
	  if ( std::count(res_row_idxs.begin(), res_row_idxs.end(), (*rit)) == 0) {
		res_row_idxs.push_back(*rit);
	  }
	}
  }

  // if complete column is selected, return only column indexes
  std::vector<int>::iterator cit = column_idxs.begin();
  for (; cit != column_idxs.end(); ++cit) {
	if ( std::count(column_idxs.begin(), column_idxs.end(), (*cit)) == m_model->rowCount()) {
	  if ( std::count(res_column_idxs.begin(), res_column_idxs.end(), (*cit)) == 0) {
		res_column_idxs.push_back(*cit);
	  }
	}
  }

  // swap results
  if (res_row_idxs.size() > 0 || res_column_idxs.size() > 0) {
	row_idxs.swap(res_row_idxs);
	column_idxs.swap(res_column_idxs);
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* selectRow --                                                                */
/* --------------------------------------------------------------------------- */
bool GuiQtTable::selectRow( int row, bool recursive ) {

  // gui update
  update(reason_Process); //Always);
  m_tablewidget->raise();  // sehr wichtig!!!

  // clones selektieren
  if (recursive)
    for (std::vector<GuiQtTable*>::iterator it =  m_clonedTable.begin();
		 it != m_clonedTable.end(); ++it)
      (*it)->selectRow( row, false );

  if( row >=0 && row < m_tablewidget->model()->rowCount() ) {
    if (m_tablewidget->model()->index(row, 0).isValid()) {
	  m_tablewidget->blocked_selectRow( row );
	  m_tablewidget->scrollTo( m_tablewidget->model()->index(row, 0), QAbstractItemView::EnsureVisible);
      return true;
    }
  }
  else
    if (m_tablewidget)
      m_tablewidget->clearSelection();

  return true;
}

/* --------------------------------------------------------------------------- */
/* selectRows --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtTable::selectRows( std::vector<int> rows, bool recursive ) {
  // clones selektieren
  if (recursive)
    for (std::vector<GuiQtTable*>::iterator it =  m_clonedTable.begin();
		 it != m_clonedTable.end(); ++it)
      (*it)->selectRows( rows, false );

  if ( !m_tablewidget ) {
	return false;
  }
  // maybe sorted => get list index for sorting
  QWidget *focusWidget = QApplication::focusWidget();
  m_tablewidget->setFocus();
  std::vector<int>::iterator it = rows.begin();
  m_tablewidget->clearSelection();
  for(; it != rows.end(); ++it)
	m_tablewidget->blocked_selectRow( *it );

  //  m_tablewidget->blocked_selectRows( rows );
  focusWidget->setFocus();
  m_tablewidget->update();
  return true;
}

/* --------------------------------------------------------------------------- */
/* setValues --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setValues() {
  int numRows(getRowCount()), numCols(getColumnCount());
  bool lastRow(false), lastCol(false);
  int rowh =  m_tablewidget->sizeHintForRow(1);

  if ( m_model->columnCount() != numCols) {
    m_model->setColumnCount( numCols );
  }
  if ( m_model->rowCount() != numRows) {
    m_model->setRowCount( numRows );
  }
  BUG_DEBUG( "setValues  row["<<getTableRows()<<"]  col["<<getTableCols()<<"]");
  int row = 0;

  //  long t(numRows), b(0), l(numCols), r(0) ;
  long t(-1), b(-1), l(-1), r(-1) ;
  bool editable = false;
  for (int row = 0; row < numRows; ++row) {
    QList<QStandardItem*> items;
#if 0
    if (row == m_model->rowCount()) {
      m_model->insertRows(row, 1, QModelIndex());
      m_tablewidget->setRowHeight(row,rowh);
    }
#endif
    for (int col = 0; col < numCols; ++col) {
      // get TableItem and set value of QTableItem
      GuiTableItem *item = getTableItem( row, col );
      if (item) {
        QString str=QString::fromStdString(item->CellValue()).trimmed();
        QString strTP;
        GuiTableItem *itemTP = getTableItemToolTip( (getOrientation() == GuiElement::orient_Vertical ? row : col) );
        if (itemTP)
          strTP=QString::fromStdString(itemTP->CellValue()).trimmed();
        if (!lastCol && (col+1)==numCols && str.size()) lastCol = true;
        if (!lastRow && (row+1)==numRows && str.size()) lastRow = true;
        QModelIndex idx=m_model->index(row, col);
        if (str != m_model->data(idx, Qt::DisplayRole).toString()) {
          m_model->setData(idx, str, Qt::DisplayRole );
          m_model->setData(idx, strTP, Qt::ToolTipRole );
          if (str.size() > item->getDataField()->getLength() && strTP.size() == 0) {
            m_model->setData(idx, str, Qt::ToolTipRole );
          }
          if (t<0 || t < row) t = row;
          if (l<0 || l < col) l = col;
          if (b<0 || b > row) b = row;
          if (r<0 || r > col) r = col;
        }
        // editable flag
        if (!editable) editable = item->isEditable();
      }

      // if color line exists => we use this colors
      GuiTableItem *itemColor = getTableItemColor( (getOrientation() == GuiElement::orient_Vertical ? row : col) );
      if (itemColor) {
        // set color
        QColor background, foreground;
        if (itemColor){
          int dark_fac;
          dynamic_cast<GuiQtTableDataItem*>(itemColor)->getTheColor(background,
                                                                    foreground,
                                                                    dark_fac,
                                                                    true);
        }
        QModelIndex idx=m_model->index(row, col);
        if (background.isValid() && m_model->data( idx, Qt::BackgroundRole) !=  background) {
          m_model->setData(idx, background, Qt::BackgroundRole);
          m_model->setData(idx, foreground, Qt::ForegroundRole);
        }
      }
    }
  }
  if (t >=0 && l >=0 && b >=0 && r)
    m_model->signalDataChanged(m_model->index(t,l), m_model->index(b+1,r+1));

  // insert for addional empty row or column
  resetEmptyRowColumnFlag();
  if (getTableMatrix()) {
    if (!isTableHorizontalFixed() && lastCol && editable) {
      m_model->insertColumn(numCols, QModelIndex());
      setEmptyColumnFlag();
    }
    if (!isTableVerticalFixed() && lastRow && editable) {
      m_model->insertRow(numRows, QModelIndex());
      setEmptyRowFlag();
    }
  } else {
    if (!isTableVerticalFixed() &&
        lastRow && getOrientation() == GuiElement::orient_Vertical) {
      if (editable) {
        m_model->insertRow(numRows, QModelIndex());
        setEmptyRowFlag();
      }
    } else
      if (!isTableHorizontalFixed() &&
          lastCol && getOrientation() == GuiElement::orient_Horizontal) {
        if (editable) {
          m_model->insertColumn(numCols, QModelIndex());
          setEmptyColumnFlag();
        }
      }
  }

  // FROZEN Column or Rows: insert for addional empty row or column
  if( getVertLinePlacement() == GuiElement::align_Right ) {
    if (getVLineList().size())
      m_model->insertColumn(numCols, QModelIndex());
  }
  if( getHorzLinePlacement() == GuiElement::align_Bottom ) {
    for (int row=0; row < getHLineList().size(); ++row) {
      m_model->insertRow(numRows+row, QModelIndex());
    }
  }

}

/* --------------------------------------------------------------------------- */
/* setWidgetSize --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtTable::setWidgetSize(SizeType size_type) {
  ///  SizeType size_type = SizeType::size_visible;

  int numRows, numCols, numRowsDefault, numColsDefault;
  getTableSize( numRows, numCols, size_type);
  getTableSize( numRowsDefault, numColsDefault, size_init);
  if (size_type == size_init   ){
    setRowCount(numRows);
    setColumnCount(numCols);
  }

  // adjust
  if (numCols > getTableCols()+getVLineList().size())   // size correction
    numCols = getTableCols()+getVLineList().size();
  if (numRows > getTableRows()+getHLineList().size())   // size correction
    numRows = getTableRows()+getHLineList().size();
  m_tablewidget->verticalHeader()->adjustSize();
  m_tablewidget->horizontalHeader()->adjustSize();

  // section row height
  QFont f =  m_tablewidget->font();
  QFont fontList =  QtMultiFontString::getQFont( "@list@", f );
  QFont fontLabel =  QtMultiFontString::getQFont( "@label@", f );
  int incH = fontLabel.pointSize() >= fontList.pointSize() ? (fontLabel.pointSize()+1 - fontList.pointSize()) : 0;
  int rowh =  m_tablewidget->sizeHintForRow(1) + incH;
  m_tablewidget->setSectionHeight( rowh );

  // row height (frozen header)
  m_tablewidget->setAllRowHeight(-1, getHorizontalHeaderLines()*rowh);

  // content row height
  int cnt_lines = getVLineList().size() > 0 ? getHorizontalHeaderLines()-1 : 0;
  int cnt_multi_lines = getHorizontalHeaderLines()-1;
  for (int r=0 ;r < numRows; ++r) {
    int l = r < getVerticalHeaderLines().size() ? getVerticalHeaderLines()[r] : 1;
    cnt_lines += l;
    cnt_multi_lines += l-1;
    m_tablewidget->setAllRowHeight(r,l*rowh);
  }

  // scrollbar dimensions
  int _contentWidthSB(0), _contentHeightSB(0);
  if(!isTableVerticalFixed() &&
     (getTableMatrix() || getOrientation() == orient_Vertical)){
    _contentWidthSB = m_tablewidget->verticalScrollBar()->sizeHint().width();
  }
  if(!isTableHorizontalFixed() &&
     (getTableMatrix() || getOrientation() == orient_Horizontal)){
    _contentHeightSB = m_tablewidget->horizontalScrollBar()->sizeHint().height();
  }

  int _contentWidth = 2; // extra space
  if (!isTableRowHeaderHidden())
    _contentWidth +=m_tablewidget->verticalHeader()->width();
    for (int c=0 ;c < numColsDefault; ++c){
      _contentWidth += m_tablewidget->columnWidth ( c ) ;
    }

  // little trick and unfortunately not possible otherwise
  // Problem: VerticalHeader needs more space after GuiUpdates
  // because additional lines are added
  // Solution: We request more space and set the StretchLastSection attribute
  if(m_frame->sizePolicy().horizontalPolicy() == QSizePolicy::Fixed &&
     !isTableHorizontalFixed() && !isTableVerticalFixed()) {
    int lastId = m_tablewidget->horizontalHeader()->count()-1;
    // set stretch option to last section
    if (!m_tablewidget->horizontalHeader()->isSectionHidden(lastId) &&
        m_tablewidget->horizontalHeader()->sectionSize(lastId) > 0
        ) {
      m_tablewidget->horizontalHeader()->setStretchLastSection(numCols != numColsDefault); // true
    } else {
      // set stretch option to last "Visible" section
      int lastVisibleId = m_tablewidget->horizontalHeader()->count()-1;
      while ( lastVisibleId >= 0 && m_tablewidget->horizontalHeader()->sectionSize(lastVisibleId) == 0 )
        --lastVisibleId;
      if (lastVisibleId >= 0)
#if QT_VERSION < 0x050000
        m_tablewidget->horizontalHeader()->setResizeMode(lastVisibleId, QHeaderView::Stretch);
#else
      m_tablewidget->horizontalHeader()->setSectionResizeMode(lastVisibleId, QHeaderView::Stretch);
#endif
    }
// #if QT_VERSION > QT_VERSION_CHECK(5,11,0)
//     _contentWidth += m_tablewidget->fontMetrics().horizontalAdvance( "0" );
// #else
//     _contentWidth += m_tablewidget->fontMetrics().width( "0" );
// #endif
    if (getOrientation() == GuiElement::orient_Horizontal) {
      m_tablewidget->setMaximumWidth(  _contentWidth+_contentWidthSB );
    }
  }

  int _contentHeight = cnt_lines*rowh;
  if (!isTableColHeaderHidden()) {
    _contentHeight += m_tablewidget->horizontalHeader()->height();
  }
  dynamic_cast<GuiQtTableViewBase*>(m_tablewidget)->setSizeHint( _contentWidth+_contentWidthSB, _contentHeight+_contentHeightSB);
  dynamic_cast<GuiQtTableViewBase*>(m_tablewidget)->setScrollBarHint( _contentWidthSB, _contentHeightSB);
  // unfortunately we have to make an estimate at this point
  // whether the table will be too large and no longer fit on the screen
  // (knowledge of other GuiElements in containers is not available)
  // If this is the case, the ExpandPolicy is added in this direction
#if QT_VERSION < 0x060000
  QRect drect = QApplication::desktop()->availableGeometry(myWidget());
#else
  QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#endif
  if ( drect.width() < (_contentWidth+_contentWidthSB+50) ) {
    m_frame->setSizePolicy( QSizePolicy::MinimumExpanding,
			    m_frame->sizePolicy().verticalPolicy() );
  }
  if ( drect.height() < (_contentHeight+_contentHeightSB+50) ) {
    m_frame->setSizePolicy( m_frame->sizePolicy().horizontalPolicy(),
			    QSizePolicy::MinimumExpanding );
  }
  // set size
  if (!getTableMatrix()) {
    if (getOrientation() == GuiElement::orient_Horizontal) {
      m_tablewidget->setMinimumHeight( _contentHeight+_contentHeightSB );
      m_tablewidget->setMaximumHeight( _contentHeight+_contentHeightSB );
      BUG_DEBUG("FIX HEIGHT: " <<_contentHeight+_contentHeightSB);
    } else {
      m_tablewidget->setMinimumWidth( _contentWidth+_contentWidthSB );
      m_tablewidget->setMaximumWidth( _contentWidth+_contentWidthSB );
      BUG_DEBUG("FIX WIDTH: " <<_contentWidth+_contentWidthSB);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* updateUnitFields --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtTable::updateUnitFields(){
  int numRows(getRowCount()), numCols(getColumnCount());
  for (int row=0; row < numRows; ++row){
    for (int col=0; col < numCols; ++col){
      GuiQtTableDataItem *item = dynamic_cast<GuiQtTableDataItem*>(getTableItem( row, col ));
      if (item)
        item->update(reason_Unit);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* slotItemChanged --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtTable::slotItemChanged(QStandardItem * item) {
  GuiTableItem *tbl_item = getTableItem( item->row(), item->column() );
  if (tbl_item) {
    tbl_item->ValidateCell( item->data(Qt::DisplayRole).toString().toStdString());
  }
}

/* --------------------------------------------------------------------------- */
/* slotCommitData  --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtTable::slotCommitData(QWidget* widget) {
  // seems to be a qt bug, so WE clear the selection
  ///  m_tablewidget->clearSelection();
}

/* --------------------------------------------------------------------------- */
/* rowSelect --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtTable::rowSelect(const QModelIndex& index) {
  if( getFunction() == 0 ) return;
  QVariant var = index.sibling(index.row(),index.column()).data();
  if (var.isValid()) {
    JobStarter *starter = new GuiTableTrigger( getFunction() );
    starter->setReason( JobElement::cll_Select );
    starter->setIndex( index.row() );
    starter->startJob();
  }
}

/* --------------------------------------------------------------------------- */
/* rowUnselect --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtTable::rowUnselect(const QModelIndex& index) {
  if( getFunction() == 0 ) return;
  QVariant var = index.sibling(index.row(),index.column()).data();
  if (var.isValid()) {
    JobStarter *starter = new GuiTableTrigger( getFunction() );
    starter->setReason( JobElement::cll_Unselect );
    starter->setIndex( index.row() );
    starter->startJob();
  }
}

/* --------------------------------------------------------------------------- */
/* getContainerExpandPolicy --                                                 */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtTable::getContainerExpandPolicy() {
  if( myWidget() )
    return getGuiOrientation(myWidget()->sizePolicy().expandingDirections());
  return GuiElement::orient_Default;
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtTable::getDialogExpandPolicy() {
  // Table soll sich im Dialog nicht mehr vergroessern
  // if( myWidget() )
  //   return myWidget()->sizePolicy().expandingDirections();
  return orient_Default;
}

/* --------------------------------------------------------------------------- */
/* getExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

Qt::Orientations GuiQtTable::getExpandPolicy() {
  if( myWidget() )
    return myWidget()->sizePolicy().expandingDirections();
  return Qt::Orientations();
}

/* --------------------------------------------------------------------------- */
/* createPopupMenu --                                                          */
/* --------------------------------------------------------------------------- */

GuiPopupMenu* GuiQtTable::PopupMenu(int x, int y, GuiElement::Orientation orientation){
  setCurrentRow(  m_tablewidget->rowAt(y) );
  setCurrentColumn( m_tablewidget->columnAt(x) );
  return GuiTable::PopupMenu(x, y, orientation);
}
