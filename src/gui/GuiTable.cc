#include "gui/GuiComboBox.h"
#include "gui/GuiElement.h"
#include <string>

#include "gui/GuiFactory.h"
#include "gui/GuiTable.h"
#include "gui/GuiTableLine.h"
#include "gui/GuiTableItem.h"
#include "gui/GuiDataField.h"
#include "gui/GuiPopupMenu.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiSeparator.h"
#include "gui/GuiIndex.h"
#include "streamer/StreamManager.h"
#include "utils/Debugger.h"
#include "job/JobFunction.h"
#include "job/JobManager.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"
#include "utils/HTMLConverter.h"

#ifdef HAVE_QT
#include <QString>
#include <QColor>
#endif

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* Konstruktor, Destruktor                                                     */
/* --------------------------------------------------------------------------- */

GuiTable::GuiTable( GuiElement *parent ) :
    m_title_alignment( GuiElement::align_Center )
  , m_label_alignment( GuiElement::align_Left )
  , m_index_alignment( GuiElement::align_Center )

  , m_hline_placement( GuiElement::align_Default )
  , m_vline_placement( GuiElement::align_Default )
  , m_navigation( GuiElement::orient_Horizontal )
  , m_orientation( GuiElement::orient_Horizontal )
  , m_table_cols( 5 )
  , m_table_rows( 5 )
  , m_horzHeaderLines( 1 )
  , m_tooltip_line( 0 )
  , m_color_line( 0 )

  , m_row_range_from( 0 )
  , m_row_range_to( 0 )
  , m_row_header_hidden( false )
  , m_row_menu( 0 )
  , m_row_menu_title( 0 )

  , m_col_range_from( 0 )
  , m_col_range_to( 0 )
  , m_col_header_hidden( false )
  , m_col_menu( 0 )
  , m_col_menu_title( 0 )

  , m_hideIndexMenu( false )
  , m_table_matrix( false )
  , m_running_key( 0 )
  , m_cntRow( 0 )
  , m_cntCol( 0 )
  , m_row( 0 )
  , m_col( 0 )
  , m_emptyRow( 0 )
  , m_emptyColumn( 0 )
  , m_row_insert( this, true )
  , m_row_delete( this, true )
  , m_row_delete_selected( this, true, true )
  , m_row_duplicate( this, true )
  , m_row_clear( this, true )
  , m_row_pack( this, true )

  , m_col_insert( this, false )
  , m_col_delete( this, false )
  , m_col_delete_selected( this, false, true )
  , m_col_duplicate( this, false )
  , m_col_clear( this, false )
  , m_col_pack( this, false )
  , m_stream(0)
  , m_function( 0 )
  , m_auto_width(false)
  , m_argRowColumn(-1)
  , m_maxRowLen( 0 )
{
  // headless mode, minimize data
  double maxCurveLenFac = 0;
  if (AppData::Instance().HeadlessWebMode()) {
    const char *a = getenv("TABLE_MAX_ROW_LENGTH");
    if(a){
      m_maxRowLen = atoi(a);
    }
    if (m_maxRowLen == 0) { m_maxRowLen = 500; }
  }
}

GuiTable::GuiTable( const GuiTable& tbl) :
  m_title( tbl.m_title )
  , m_title_horizontal( tbl.m_title_horizontal )
  , m_title_vertical( tbl.m_title_vertical )
  , m_title_alignment( tbl.m_title_alignment )
  , m_label_alignment( tbl.m_label_alignment )
  , m_index_alignment( tbl.m_index_alignment )

  , m_hline_placement( tbl.m_hline_placement )
  , m_vline_placement( tbl.m_vline_placement )
  , m_navigation( tbl.m_navigation )
  , m_orientation( tbl.m_orientation )
  , m_table_cols( tbl.m_table_cols )
  , m_table_rows( tbl.m_table_rows )
  , m_horzHeaderLines( tbl.m_horzHeaderLines )
  , m_vertHeaderLines( tbl.m_vertHeaderLines )

  , m_row_range_from( tbl.m_row_range_from )
  , m_row_range_to( tbl.m_row_range_to )
  , m_row_header_hidden( tbl.m_row_header_hidden )
  , m_row_menu( tbl.m_row_menu )
  , m_row_menu_title( tbl.m_row_menu_title )

  , m_col_range_from( tbl.m_col_range_from )
  , m_col_range_to( tbl.m_col_range_to )
  , m_col_header_hidden( tbl.m_col_header_hidden )
  , m_col_menu( tbl.m_col_menu )
  , m_col_menu_title( tbl.m_col_menu_title )

  , m_hideIndexMenu( tbl.m_hideIndexMenu )
  , m_table_matrix( tbl.m_table_matrix )
  , m_cntRow( tbl.m_cntRow )
  , m_cntCol( tbl.m_cntCol )

  , m_row( tbl.m_row )
  , m_col( tbl.m_col )
  , m_emptyRow( tbl.m_emptyRow )
  , m_emptyColumn( tbl.m_emptyColumn )

  , m_tableaction_list( tbl.m_tableaction_list )
  , m_running_key( tbl.m_running_key )

  , m_row_insert( this, true )
  , m_row_delete( this, true )
  , m_row_delete_selected( this, true, true )
  , m_row_duplicate( this, true )
  , m_row_clear( this, true )
  , m_row_pack( this, true )

  , m_col_insert( this, false )
  , m_col_delete( this, false )
  , m_col_delete_selected( this, false, true )
  , m_col_duplicate( this, false )
  , m_col_clear( this, false )
  , m_col_pack( this, false )

  , m_hline_list( tbl.m_hline_list )
  , m_vline_list( tbl.m_vline_list )
  , m_table_list( tbl.m_table_list )
  , m_tooltip_line( tbl.m_tooltip_line )
  , m_color_line( tbl.m_color_line )
  , m_stream( tbl.m_stream )
  , m_function(tbl.m_function )
  , m_auto_width(tbl.m_auto_width)
  , m_argRowColumn(-1)
  , m_maxRowLen(tbl.m_maxRowLen)
{}

GuiTable::~GuiTable() {
}

/* --------------------------------------------------------------------------- */
/* newTableSize --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableSize *GuiTable::newTableSize(){
  return new GuiTableSize;
}

/* --------------------------------------------------------------------------- */
/* addHorizontalLine --                                                        */
/* --------------------------------------------------------------------------- */

GuiTableLine *GuiTable::addHorizontalLine( const std::string &label, GuiElement::Alignment align ){
  GuiTableLine *line = new GuiTableLine( this );
  line->setParent( getElement() );
  if( align == GuiElement::align_Default ){
    align = m_label_alignment;
  }
  line->setLabelAlignment( align );
  line->setLabel( label );
  m_hline_list.push_back( line );
  return line;
}

/* --------------------------------------------------------------------------- */
/* addVerticalLine --                                                          */
/* --------------------------------------------------------------------------- */

GuiTableLine *GuiTable::addVerticalLine( const std::string &label, GuiElement::Alignment align ){
 GuiTableLine *line = new GuiTableLine( this );
 line->setParent( getElement() );
  if( align == GuiElement::align_Default ){
     align = m_label_alignment;
   }
   line->setLabelAlignment( align );
   line->setLabel( label );
   m_vline_list.push_back( line );
   return line;
}

/* --------------------------------------------------------------------------- */
/* addTableLine --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableLine *GuiTable::addTableLine( const std::string &label, GuiElement::Alignment align ){
  GuiTableLine *line = new GuiTableLine( this );
  line->setParent( getElement() );
  if( align == GuiElement::align_Default ){
    align = m_label_alignment;
  }
  line->setLabelAlignment( align );
  line->setLabel( label );
  m_table_list.push_back( line );
  return line;
}

/* --------------------------------------------------------------------------- */
/* addTableLineToolTip --                                                      */
/* --------------------------------------------------------------------------- */

GuiTableLine *GuiTable::addTableLineToolTip(){
  m_tooltip_line = new GuiTableLine( this );
  m_tooltip_line->setParent( getElement() );
  return m_tooltip_line;
}

/* --------------------------------------------------------------------------- */
/* addTableLineColor --                                                        */
/* --------------------------------------------------------------------------- */

GuiTableLine *GuiTable::addTableLineColor(){
  m_color_line = new GuiTableLine( this );
  m_color_line->setParent( getElement() );
  return m_color_line;
}

/* --------------------------------------------------------------------------- */
/* mapTableLine --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiTable::mapTableLine(DataReference *dataref){
  GuiTableLineList list;
  list.insert(list.end(), m_table_list.begin(), m_table_list.end());
  list.insert(list.end(), m_vline_list.begin(), m_vline_list.end());
  list.insert(list.end(), m_hline_list.begin(), m_hline_list.end());
  list.push_back(m_tooltip_line);
  list.push_back(m_color_line);
  BUG_DEBUG("TableLine Map: "<<dataref->fullName(true));
  GuiTableLineList::iterator li;
  for( li = list.begin(); li != list.end(); ++li ){
    XferDataItem *xfer = (*li)->getTableItem(0)->getDataField()->DataItem();
    xfer->setDimensionIndizes();
    if (*xfer->Data() == *dataref){
      (*li)->setVisible(true);
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* unmapTableLine --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiTable::unmapTableLine(DataReference *dataref){
  GuiTableLineList list;
  list.insert(list.end(), m_table_list.begin(), m_table_list.end());
  list.insert(list.end(), m_vline_list.begin(), m_vline_list.end());
  list.insert(list.end(), m_hline_list.begin(), m_hline_list.end());
  list.push_back(m_tooltip_line);
  list.push_back(m_color_line);
  BUG_DEBUG("TableLine Unmap: "<<dataref->fullName(true));
  GuiTableLineList::iterator li;
  for( li = list.begin(); li != list.end(); ++li ){
    XferDataItem *xfer = (*li)->getTableItem(0)->getDataField()->DataItem();
    xfer->setDimensionIndizes();
    if (*xfer->Data() == *dataref){
      (*li)->setVisible(false);
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiTable::setTitle( const std::string &title, GuiElement::Alignment align ){
  m_title           = title;
  m_title_alignment = align;
}

/* --------------------------------------------------------------------------- */
/* setTitleHorizontal --                                                       */
/* --------------------------------------------------------------------------- */

void GuiTable::setTitleHorizontal( const std::string &title ){
  m_title_horizontal = title;
}

/* --------------------------------------------------------------------------- */
/* setTitleVertical --                                                         */
/* --------------------------------------------------------------------------- */

void GuiTable::setTitleVertical( const std::string &title ){
  m_title_vertical = title;
}

/* --------------------------------------------------------------------------- */
/* setHorizontalPlacement --                                                   */
/* --------------------------------------------------------------------------- */

bool GuiTable::setHorizontalPlacement( GuiElement::Alignment align ){
  if( m_hline_placement != GuiElement::align_Default )
    return false;
  m_hline_placement = align;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setVerticalPlacement --                                                     */
/* --------------------------------------------------------------------------- */

bool GuiTable::setVerticalPlacement( GuiElement::Alignment align ){
  if( m_vline_placement != GuiElement::align_Default )
    return false;
  m_vline_placement = align;
  return true;
}


/* --------------------------------------------------------------------------- */
/* setTableOrientation --                                                      */
/* --------------------------------------------------------------------------- */

void GuiTable::setTableOrientation( GuiElement::Orientation orient ){
  m_orientation = orient;
}

/* --------------------------------------------------------------------------- */
/* setTableNavigation --                                                       */
/* --------------------------------------------------------------------------- */

void GuiTable::setTableNavigation( GuiElement::Orientation orient ){
  m_navigation  = orient;
}

/* --------------------------------------------------------------------------- */
/* setTableRowSize --                                                          */
/* --------------------------------------------------------------------------- */

void GuiTable::setTableRowSize( GuiTableSize *tsz ){
  m_table_rows     = tsz->Size();
  m_row_range_from = tsz->RangeFrom();
  m_row_range_to   = tsz->RangeTo();
  m_row_header_hidden = tsz->IsHeaderHidden();
}

/* --------------------------------------------------------------------------- */
/* newTableColSize --                                                          */
/* --------------------------------------------------------------------------- */

void GuiTable::setTableColSize( GuiTableSize *tsz ){
  m_table_cols     = tsz->Size();
  m_col_range_from = tsz->RangeFrom();
  m_col_range_to   = tsz->RangeTo();
  m_col_header_hidden = tsz->IsHeaderHidden();
}

/* --------------------------------------------------------------------------- */
/* setLabelAlignment --                                                        */
/* --------------------------------------------------------------------------- */

void GuiTable::setLabelAlignment( GuiElement::Alignment align ){
  m_label_alignment = align;
}


/* --------------------------------------------------------------------------- */
/* setTable --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiTable::setTable(){
  return m_table_list.empty();
}

// void GuiTable::addActions() {
//   GuiMotifTable::addActions();
// }

// void GuiTable::setMatrix() {
//   std::cout << "#################################+++[GuiTable::setMatrix]+++" << std::endl << std::flush;
// }



////////////////////////////////////////////////////////////////////////
///// PRIVATE AREA ////////// PRIVATE AREA ////////// PRIVATE AREA /////
////////////////////////////////////////////////////////////////////////


/* --------------------------------------------------------------------------- */
/* syncroniseTableItemLength --                                                */
/* --------------------------------------------------------------------------- */

static inline size_t count( const std::string& string, const char* search ){
  int count = 0;
  const char *tmp = string.c_str();
  while((tmp = strstr(tmp, search))){
    count++;
    tmp++;
  }
  return count;
}

void GuiTable::syncroniseTableItemLength(){
  BUG(BugGuiTable,"GuiTable::syncroniseTableItemLength");
  int        len, maxlen = 0;
  GuiTableLineList::iterator li;
  GuiTableLineList::iterator hl;
  GuiTableLineList::iterator vl;

  /* same length for all vertical items */
  for( li = m_vline_list.begin(); li != m_vline_list.end(); ++li ){
    (*li)->syncroniseTableItemLength();
  }

  // horizontal Lines
  for( hl = m_hline_list.begin(); hl != m_hline_list.end(); ++hl ){
    m_vertHeaderLines.push_back( 1+count((*hl)->Label(),"\\n") );
  }

  /* horizontal Lines or Matrix in Table */
  if( m_table_matrix || getOrientation() == GuiElement::orient_Horizontal ){
    for( vl = m_vline_list.begin(); vl != m_vline_list.end(); ++vl ){
      int c =count((*vl)->Label(),"\\n");
      if (c>=m_horzHeaderLines)
        m_horzHeaderLines=c+1;
    }
    for( li = m_table_list.begin(); li != m_table_list.end(); ++li ){
      len = (*li)->getMaxLength();
      maxlen = len > maxlen ? len : maxlen;

       // multi column header labels
      m_vertHeaderLines.push_back( 1+count((*li)->Label(),"\\n") );
    }
    for( hl = m_hline_list.begin(); hl != m_hline_list.end(); ++hl ){
       len = (*hl)->getMaxLength();
       maxlen = len > maxlen ? len : maxlen;

       // multi column header labels
       m_vertHeaderLines.push_back( 1+count((*hl)->Label(),"\\n") );
    }
    BUG_MSG("Orientation horizontal: maxlen = " << maxlen);
    for( hl = m_hline_list.begin(); hl != m_hline_list.end(); ++hl ){
      (*hl)->setMaxLength( maxlen );
    }
    for( li = m_table_list.begin(); li != m_table_list.end(); ++li ){
      (*li)->setMaxLength( maxlen );
    }
  }
  else{ /* vertical Lines in Table */
    int n = 0;
    for( li = m_table_list.begin(); li != m_table_list.end(); ++li ){
      maxlen = (*li)->getMaxLength();

      // multi row header labels
      int c =count((*li)->Label(),"\\n");
      if (c>=m_horzHeaderLines)
        m_horzHeaderLines=c+1;

      for( hl = m_hline_list.begin(); hl != m_hline_list.end(); ++hl ){
        len = (*hl)->getLength( n );
        maxlen = len > maxlen ? len : maxlen;
      }
      BUG_MSG("Orientation vertical: maxlen = " << maxlen);
      (*li)->setMaxLength( maxlen );
      for( hl = m_hline_list.begin(); hl != m_hline_list.end(); ++hl ){
        (*hl)->setLength( maxlen, n );
      }
    }

  }
}

/* --------------------------------------------------------------------------- */
/* getTableItem --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiTable::getTableItem( int row, int col ){
  if( row < 0 || col < 0 ){
    return 0;
  }

  if( row < (int)m_hline_list.size() ){
    // Das gesuchte Item befindet sich in einer horizontalen Linie
    return m_hline_list[row]->getTableItem( col - (int)m_vline_list.size() );
  }

  if( col < (int)m_vline_list.size() ) {
    // Das gesuchte Item befindet sich in einer vertikalen Linie
    return m_vline_list[col]->getTableItem( row - (int)m_hline_list.size() );
  }

  row = row - (int)m_hline_list.size();
  col = col - (int)m_vline_list.size();

  if( getOrientation() == GuiElement::orient_Horizontal ){
    if( m_table_matrix ){
      assert( m_table_list.size() == 1 );
      return m_table_list[0]->getTableItem( row, col );
    }
    if( row < m_table_list.size() ){
      return m_table_list[row]->getTableItem( col );
    }
  }
  else{
    if( m_table_matrix ){
      assert( m_table_list.size() == 1 );
      return m_table_list[0]->getTableItem( col, row );
    }
    if( col < m_table_list.size() ){
      return m_table_list[col]->getTableItem( row );
    }
  }

  return 0;
}

/* --------------------------------------------------------------------------- */
/* getFilledTableItem --                                                       */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiTable::getFilledTableItem(int row, int col, int numRows, int numCols){
  int row_diff(0), col_diff(0);
  if( row < 0 || col < 0 ){
    return 0;
  }
  if (numRows == -1 || numCols == -1) {
  getTableSize( numRows, numCols );
  }

  if (m_hline_list.size()) {
    if (getHorzLinePlacement() == GuiElement::align_Top) {
      if( row < (int)m_hline_list.size() ){
	// Das gesuchte Item befindet sich in einer horizontalen Linie
	int c = getVertLinePlacement() == GuiElement::align_Left ?
	  col - (int)m_vline_list.size() : col;
	if (c >= 0 && c < (numCols - m_vline_list.size()))
	  return m_hline_list[row]->getTableItem( c );
      }
      row_diff = (int)m_hline_list.size();
    } else {
      if( ///row < numRows &&
	 row >= (numRows - m_hline_list.size())){
	// Das gesuchte Item befindet sich in einer horizontalen Linie
	int r = row - numRows + m_hline_list.size();
	int c = getVertLinePlacement() == GuiElement::align_Left ?
	  col - m_vline_list.size() : col;

	if (r >= 0 && r < m_hline_list.size() &&
	    c >= 0  && c < (numCols - m_vline_list.size()))
	  return m_hline_list[r]->getTableItem( c );
      }
    }
  }

  if (m_vline_list.size()) {
    if (getVertLinePlacement() == GuiElement::align_Left) {
      if( col < (int)m_vline_list.size() ) {
	// Das gesuchte Item befindet sich in einer vertikalen Linie
	int r = getHorzLinePlacement() == GuiElement::align_Top ?
	  row - (int)m_hline_list.size() : row;
	if (r >= 0 && r < (numRows - m_hline_list.size()))
	  return m_vline_list[col]->getTableItem( r );
      }
      col_diff = (int)m_vline_list.size();
    } else {
      if( col >= (numCols - m_vline_list.size())){
	// Das gesuchte Item befindet sich in einer horizontalen Linie
	int r = getHorzLinePlacement() == GuiElement::align_Top ?
	  row -  m_hline_list.size() : row;
	int c =  col - (numCols - m_vline_list.size());
	if (c >= 0 && c < m_vline_list.size() &&
	    r >= 0  && r < (numRows - m_hline_list.size()))
	  return m_vline_list[c]->getTableItem( r );
      }
    }
  }

  row = row - row_diff; //(int)m_hline_list.size();
  col = col - col_diff; //(int)m_vline_list.size();

  if( getOrientation() == GuiElement::orient_Horizontal ){
    if( m_table_matrix ){
      assert( m_table_list.size() == 1 );
      return m_table_list[0]->getTableItem( row, col );
    }
    if( row < m_table_list.size() ){
      return m_table_list[row]->getTableItem( col );
    }
  }
  else{
    if( m_table_matrix ){
      assert( m_table_list.size() == 1 );
      return m_table_list[0]->getTableItem( col, row );
    }
    if( col < m_table_list.size() ){
      return m_table_list[col]->getTableItem( row );
    }
  }

  return 0;
}

/* --------------------------------------------------------------------------- */
/* getTableItemToolTip --                                                      */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiTable::getTableItemToolTip( int idx ){
  if( idx < 0 || m_table_matrix || !m_tooltip_line){
    return 0;
  }

  if( getOrientation() == GuiElement::orient_Horizontal ){
    int col = idx  - (int)m_vline_list.size();
    if (col >= 0)
      return m_tooltip_line->getTableItem( col );
  }
  else{
    int row = idx  - (int)m_hline_list.size();
    if (row >= 0)
      return m_tooltip_line->getTableItem( row );
  }

  return 0;
}

/* --------------------------------------------------------------------------- */
/* getTableItemColor --                                                        */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiTable::getTableItemColor( int idx ){
  if( idx < 0 || m_table_matrix || !m_color_line){
    return 0;
  }

  if( getOrientation() == GuiElement::orient_Horizontal ){
    int col = idx  - (int)m_vline_list.size();
    if (col >= 0)
      return m_color_line->getTableItem( col );
  }
  else{
    int row = idx  - (int)m_hline_list.size();
    if (row >= 0)
      return m_color_line->getTableItem( row );
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getTableHorizontalSize --                                                   */
/* --------------------------------------------------------------------------- */

void GuiTable::getTableHorizontalSize(int &numRows, int &numCols, SizeType size_type){
  int rows, cols;
  GuiTableLineList::iterator li;
  numRows = 0;
  numCols = 0;

  if (size_type == size_init) {
    numRows = getTableRows() + (int)getHLineList().size();
    numCols = getTableCols() + (int)getVLineList().size();
    return;
  }

  // Anzahl horizontale Felder
  for( li = getVLineList().begin(); li != getVLineList().end(); ++li ){
    if (size_type != size_default || !(*li)->isVisible())
      continue;
    (*li)->LineSize( rows, cols );
    numCols = std::max(cols, numCols);
  }
  for( li = getTableList().begin(); li != getTableList().end(); ++li ){
    if (size_type != size_default || !(*li)->isVisible())
      continue;
    (*li)->LineSize( rows, cols );
    numCols = std::max(cols, numCols);
  }

  if(getTableCols() > numCols ){
    numCols = getTableCols();
  }
  numRows = getSizeTableList(size_type);

  // is table size fixed?
  if ( isTableHorizontalFixed() )
    numCols = getTableCols();

  numRows += getSizeHLineList(size_type);
  numCols += getSizeVLineList(size_type);
  BUG_DEBUG("Horizontal TableSize rows: " << numRows << ", cols: " << numCols
            << ", Name: " << getElement()->getName() << ", size_type: " << size_type);
}

/* --------------------------------------------------------------------------- */
/* getTableVerticalSize --                                                     */
/* --------------------------------------------------------------------------- */

void GuiTable::getTableVerticalSize(int &numRows, int &numCols, SizeType size_type){
  GuiTableLineList::iterator li;
  int rows, cols;
  numRows = 0;
  numCols = 0;
  rows = 0;
  cols = 0;

  if (size_type == size_init) {
    numRows = getTableRows() + (int)getHLineList().size();
    numCols = getTableCols() + (int)getVLineList().size();
    return;
  }

 // Anzahl vertikale Felder
  for( li = getVLineList().begin(); li != getVLineList().end(); ++li ){
    if (size_type != size_default || !(*li)->isVisible())
      continue;
    (*li)->LineSize( cols, rows );
    numRows = rows > numRows ? rows : numRows;
  }
  for( li = getTableList().begin(); li != getTableList().end(); ++li ){
    if (size_type != size_default || !(*li)->isVisible())
      continue;
    (*li)->LineSize( cols, rows );
    numRows = rows > numRows ? rows : numRows;
  }

  if( getTableRows() > numRows ){
    numRows = getTableRows();
  }
  numCols = getSizeTableList(size_type);

  // is table size fixed?
  if ( isTableVerticalFixed() )
    numRows = getTableRows();

  numRows += getSizeHLineList(size_type);
  numCols += getSizeVLineList(size_type);
  BUG_DEBUG("Vertical TableSize rows: " << numRows << ", cols: " << numCols
            << ", Name: " << getElement()->getName() << ", size_type: " << size_type);
}

/* --------------------------------------------------------------------------- */
/* isTableVerticalFixed --                                                     */
/* --------------------------------------------------------------------------- */
bool GuiTable::isTableVerticalFixed() {
  if ((!getTableMatrix() && getOrientation() == GuiElement::orient_Horizontal) ||
      getTableRowRangeTo() &&
      (getTableRowRangeTo() - getTableRowRangeFrom() + 1) <= getTableRows()) {
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isTableHorizontalFixed --                                                   */
/* --------------------------------------------------------------------------- */
bool GuiTable::isTableHorizontalFixed() {
  if ((!getTableMatrix() && getOrientation() == GuiElement::orient_Vertical) ||
      getTableColRangeTo() &&
      (getTableColRangeTo() - getTableColRangeFrom() + 1) <= getTableCols()) {
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getTableMatrixSize --                                                       */
/* --------------------------------------------------------------------------- */

void GuiTable::getTableMatrixSize( int &numRows, int &numCols, SizeType size_type){
  GuiTableLineList::iterator li;
  int rows, cols;
  numRows = 0;
  numCols = 0;

  if (size_type == size_init) {
    numRows = getTableRows() + (int)getHLineList().size();
    numCols = getTableCols() + (int)getVLineList().size();
    return;
  }

  // Anzahl vertikale Felder
  for( li = getVLineList().begin(); li != getVLineList().end(); ++li ){
    if (size_type != size_default || !(*li)->isVisible())
      continue;
    (*li)->LineSize( cols, rows );
    assert( cols == 1 );
    numRows = std::max(rows, numRows);
  }

  // Anzahl horizontale Felder
  for( li = getHLineList().begin(); li != getHLineList().end(); ++li ){
    if (size_type != size_default || !(*li)->isVisible())
      continue;
    (*li)->LineSize( rows, cols );
    assert( rows == 1 );
    numCols = cols > numCols ? cols : numCols;
  }

  assert( getTableList().size() == 1 );
  li = getTableList().begin();
  if (getOrientation() == GuiElement::orient_Horizontal)
    (*li)->LineSize( rows, cols );
  else
    (*li)->LineSize( cols, rows );

  numRows = std::max(rows, numRows);
  numCols = std::max(cols, numCols);

  if( getTableRows() > numRows ){
    numRows = getTableRows();
  }
  if( getTableCols() > numCols ){
    numCols = getTableCols();
  }
  // is table size fixed?
  if ( isTableVerticalFixed() )
    numRows = getTableRows();
  if ( isTableHorizontalFixed() )
    numCols = getTableCols();

  numRows += getSizeHLineList(size_type);
  numCols += getSizeVLineList(size_type);
  BUG_DEBUG("Matrix TableSize rows: " << numRows << ", cols: " << numCols
            << ", Name: " << getElement()->getName() << ", size_type: " << size_type);
}

/* --------------------------------------------------------------------------- */
/* getTableSize --                                                             */
/* --------------------------------------------------------------------------- */

void GuiTable::getTableSize( int &numRows, int &numCols, SizeType size_type){
  if( getTableMatrix() ){
    getTableMatrixSize(numRows, numCols, size_type);
  }
  else{
    if( getOrientation() == GuiElement::orient_Vertical ){
      getTableVerticalSize(numRows, numCols, size_type);
    }
    else{
      getTableHorizontalSize(numRows, numCols, size_type);
    }
  }
}


/* --------------------------------------------------------------------------- */
/* editData --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiTable::editData( bool use_row, TableActionFunctor &func,
			 bool forAll, const std::set<int>& list ){
  BUG(BugGuiTable,"GuiTable::editData");

  // webapi
  int num;
  if (AppData::Instance().HeadlessWebMode() &&
      (num = popArgRowColumnNumber()) >= 0) {
    setCurrentRow(use_row ? num : 0);
    setCurrentColumn(use_row ? 0 : num);
    BUG_DEBUG("editData use_row: " << use_row << ", num: " << num);
  }

  // Zu diesem Zeitpunkt darf keine Funktion laufen, sonst haben wir
  // ein grosses Problem.
  if( !setRunningMode() ){ assert( false ); }

  m_tableaction_list.clear();
  getElement()->grabFocus(); // event reason

  DataPoolIntens::Instance().BeginDataPoolTransaction( getElement()->transactionOwner() );
  GuiManager::Instance().initErrorBox();

  if (forAll) {
    // clear all
    //  LEFT/RIGHT
    for(int r = m_hline_list.size(); r < getRowCount(); ++r) {
      setCurrentRow(r);
      getTableActionVertical(func, "", "@col@", r - m_hline_list.size());
    }
    //  TOP/BOTTOM
    for(int c = m_vline_list.size(); c < getColumnCount(); ++c) {
      setCurrentColumn(c);
      getTableActionHorizontal(func, "", "@col@", c - m_vline_list.size());
    }
  }
  else if(use_row) {
    // Horizontale Linien: LEFT/RIGHT
    if(list.size() > 0) {
      // reverse iterate because i.E. delete shifts the following rows
      std::set<int>::reverse_iterator it = list.rbegin();
      for (;it != list.rend(); ++it) {
        setCurrentRow( *it );
        getTableActionVertical( func, "", "@col@", m_row - m_hline_list.size() );
      }
    } else
      // default (one row)
      getTableActionVertical( func, "", "@col@", m_row - m_hline_list.size() );
  }
  else{
    // Vertikale Linien: TOP/BOTTOM
    if(list.size() > 0) {
      // reverse iterate because i.E. delete shifts the following rows
      std::set<int>::reverse_iterator it = list.rbegin();
      for (;it != list.rend(); ++it) {
        setCurrentColumn( *it );
        getTableActionHorizontal( func, "", "@col@", m_col - m_vline_list.size() );
      }
    } else
      // default (one column)
      getTableActionHorizontal( func, "", "@col@", m_col - m_vline_list.size() );
  }

  std::string rowName;
  std::string colName;

  if( (getOrientation() == GuiElement::orient_Horizontal && use_row) ||
      (getOrientation() == GuiElement::orient_Vertical  && !use_row)  ){
    rowName = "@col@";
    colName = "@row@";
  }
  else{
    rowName = "@row@";
    colName = "@col@";
  }

  if( use_row ){
    // Horizontal
    if (forAll) {
      // clear all
      for(int r = m_hline_list.size(); r < getRowCount(); ++r) {
        setCurrentRow(r);
        getTableActionWithRow(func, rowName, colName, r - m_hline_list.size());
      }
    }
    else if (list.size() > 0) {
      // reverse iterate because i.E. delete shifts the following rows
      std::set<int>::reverse_iterator it = list.rbegin();
      for (;it != list.rend(); ++it) {
        setCurrentRow( *it );
        getTableActionWithRow( func, rowName, colName, m_row - m_hline_list.size() );
      }
    } else
      // default
      getTableActionWithRow( func, rowName, colName, m_row - m_hline_list.size() );
  }
  else{
    // Vertikal
    if (forAll) {
      // clear all
      for(int c = m_vline_list.size(); c < getColumnCount(); ++c) {
        setCurrentColumn(c);
        getTableActionWithColumn(func, rowName, colName, c - m_vline_list.size());
      }
    }
    else if (list.size() > 0) {
      // reverse iterate because i.E. delete shifts the following columns
      std::set<int>::reverse_iterator it = list.rbegin();
      for (;it != list.rend(); ++it) {
        setCurrentColumn( *it );
        getTableActionWithColumn( func, rowName, colName, m_col - m_vline_list.size() );
      }
    } else
      // default
      getTableActionWithColumn( func, rowName, colName, m_col - m_vline_list.size() );
  }

  // Zuerst werden alle Datapool-Aktionen durchgeführt. Erst nach diesen
  // Arbeiten werden die vorhandenen Functions aufgerufen.
  TableActionList::iterator _tableaction_iter = m_tableaction_list.begin();  // forAll
  for( m_tableaction_iter = m_tableaction_list.begin();
       m_tableaction_iter != m_tableaction_list.end();
       ++m_tableaction_iter ){
    TableActionFunctor &f = *((*m_tableaction_iter).first);
    GuiDataField &data = *((*m_tableaction_iter).second);
    f( data );
    if (forAll && data.getFunction())
      _tableaction_iter = m_tableaction_iter;
  }

  // Nun beginnt der letzte Teil: die Functions. Bei jeder Function
  // verlassen wir das sichere Terain und geben die Kontrolle ab.
  GuiManager::Instance().initErrorBox();

  // forAll ist gesetzt => Aufruf nur der letzten Funktion
  // sonst werden alle Funktion ausgeführt
  m_tableaction_iter = forAll ? _tableaction_iter : m_tableaction_list.begin();
  nextTableAction( false );
}

/* --------------------------------------------------------------------------- */
/* getTableActionHorizontal --                                                 */
/* --------------------------------------------------------------------------- */

void GuiTable::getTableActionHorizontal( TableActionFunctor &func
                                         , const std::string &rowName, const std::string &colName
                                         , int inx ){
  int col = m_col - (int)m_vline_list.size();
  if( col < 0 ) return;
  if( m_hline_list.size() == 0 ) return;

  GuiTableItem     *item = 0;

  for( int row = 0; row < (int)m_hline_list.size(); row++ ){
    item = m_hline_list[row]->getTableItem( col );
    if( item != 0 ){
      if( item->ItemType() == GuiTableItem::item_Value ||
          item->ItemType() == GuiTableItem::item_Array ||
          item->ItemType() == GuiTableItem::item_Matrix ){
        TableActionFunctor *f = func.clone( rowName, colName, inx );
        m_tableaction_list.push_back( TableActionList::value_type( f, item->getDataField() ) );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getTableActionVertical --                                                   */
/* --------------------------------------------------------------------------- */

void GuiTable::getTableActionVertical( TableActionFunctor &func
                                       , const std::string &rowName, const std::string &colName
                                       , int inx ){
  int row = m_row - (int)m_hline_list.size();
  if( row < 0 ) return;
  if( m_vline_list.size() == 0 ) return;

  GuiTableItem     *item = 0;

  for( int col = 0; col < (int)m_vline_list.size(); col++ ){
    item = m_vline_list[col]->getTableItem( row );
    if( item != 0 ){
      if( item->ItemType() == GuiTableItem::item_Value ||
          item->ItemType() == GuiTableItem::item_Array ||
          item->ItemType() == GuiTableItem::item_Matrix ){
        TableActionFunctor *f = func.clone( rowName, colName, inx );
        m_tableaction_list.push_back( TableActionList::value_type( f, item->getDataField() ) );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getTableActionWithRow --                                                    */
/* --------------------------------------------------------------------------- */

void GuiTable::getTableActionWithRow( TableActionFunctor &func
				    , const std::string &rowName, const std::string &colName
				    , int inx ){
  BUG( BugGuiTable, "GuiTable::getTableActionWithRow");

  int columns = (int)m_vline_list.size() + (int)m_table_list.size();
  GuiTableItem     *item = 0;

  for( int col = (int)m_vline_list.size(); col < columns; col++ ){
    if( (item = getTableItem( m_row, col )) != 0 ){
      if( item->ItemType() == GuiTableItem::item_Value ||
          item->ItemType() == GuiTableItem::item_Array ||
	  item->ItemType() == GuiTableItem::item_Matrix ){
	TableActionFunctor *f = func.clone( rowName, colName, inx );
	m_tableaction_list.push_back( TableActionList::value_type( f, item->getDataField() ) );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getTableActionWithColumn --                                                 */
/* --------------------------------------------------------------------------- */

void GuiTable::getTableActionWithColumn( TableActionFunctor &func
				       , const std::string &rowName, const std::string &colName
				       , int inx ){
  BUG( BugGuiTable, "GuiTable::getTableActionWithColumn");

  int rows = (int)m_hline_list.size() + (int)m_table_list.size();
  GuiTableItem     *item = 0;

  for( int row = (int)m_hline_list.size(); row < rows; row++ ){
    if( (item = getTableItem( row, m_col )) != 0 ){
      if( item->ItemType() == GuiTableItem::item_Value ||
          item->ItemType() == GuiTableItem::item_Array ||
	  item->ItemType() == GuiTableItem::item_Matrix ){
	TableActionFunctor *f = func.clone( rowName, colName, inx );
	m_tableaction_list.push_back( TableActionList::value_type( f, item->getDataField() ) );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* finalTableAction --                                                         */
/* --------------------------------------------------------------------------- */

void GuiTable::finalTableAction( bool ok ){
  // Alle Functor-Kopien müssen wieder gelöscht werden.
  for( m_tableaction_iter = m_tableaction_list.begin();
       m_tableaction_iter != m_tableaction_list.end();
       ++m_tableaction_iter ){
    delete (*m_tableaction_iter).first;
    (*m_tableaction_iter).first  = 0;
    (*m_tableaction_iter).second = 0;
  }
  m_tableaction_list.clear();

  if( ok ){
    DataPoolIntens::Instance().CommitDataPoolTransaction( getElement()->transactionOwner() );
    getElement()->updateForms( GuiElement::reason_FieldInput );
    getElement()->waitCursor( false, this );
  }
  else{
    DataPoolIntens::Instance().RollbackDataPoolTransaction( getElement()->transactionOwner() );
    getElement()->updateForms( GuiElement::reason_Cancel );
    GuiManager::Instance().showErrorBox( getElement() );
  }
  unsetRunningMode();
}

/* --------------------------------------------------------------------------- */
/* setRunningMode --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiTable::setRunningMode(){
  m_running_key = JobManager::Instance().setRunningMode( m_running_key );
  return m_running_key != 0;
}

/* --------------------------------------------------------------------------- */
/* unsetRunningMode --                                                         */
/* --------------------------------------------------------------------------- */

void GuiTable::unsetRunningMode(){
  if( m_running_key != 0 ){
    JobManager::Instance().unsetRunningMode( m_running_key );
    m_running_key = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* nextTableAction --                                                          */
/* --------------------------------------------------------------------------- */

void GuiTable::nextTableAction( bool error ){
  if( error ){
    finalTableAction( false );
    return;
  }

  if( m_tableaction_iter == m_tableaction_list.end() ){
    finalTableAction( true );
    return;
  }
  GuiDataField *field = (*m_tableaction_iter).second;
  JobElement::CallReason reason = (*m_tableaction_iter).first->getReason();
  m_tableaction_iter++;

  JobFunction *func = field->getFunction();
  if( func == 0 ){
    nextTableAction( false );
    return;
  }

  // Nun wird der Trigger (JobStarter) initialisiert.
  Trigger *trigger = new Trigger( this, func );
  trigger->setReason( reason );
  //trigger->setGuiElement( this );

  trigger->startJob();
  // Hier geben wir die Kontrolle ab. Nach Beendigung der Function gehts mit
  // nextTableAction() weiter. Der Aufruf erfolgt durch den Trigger. Der Trigger
  // löscht sich am Ende selbständig.
}

/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                       */
/* --------------------------------------------------------------------------- */

void GuiTable::Trigger::backFromJobStarter( JobAction::JobResult rslt  ){
  m_table->nextTableAction( rslt != JobAction::job_Ok );
}

/* --------------------------------------------------------------------------- */
/* insertData --                                                               */
/* --------------------------------------------------------------------------- */

void GuiTable::insertData( bool use_row ){
  InsertFunctor functor;
  editData( use_row, functor );
}

/* --------------------------------------------------------------------------- */
/* deleteData --                                                               */
/* --------------------------------------------------------------------------- */

void GuiTable::deleteData( bool use_row ){
  DeleteFunctor functor;
  editData( use_row, functor );
}

/* --------------------------------------------------------------------------- */
/* duplicateData --                                                            */
/* --------------------------------------------------------------------------- */

void GuiTable::duplicateData( bool use_row ){
  DuplicateFunctor functor;
  editData( use_row, functor );
}

/* --------------------------------------------------------------------------- */
/* clearData --                                                                */
/* --------------------------------------------------------------------------- */

void GuiTable::clearData( bool use_row ){
  ClearFunctor functor;
  editData( use_row, functor );
}

/* --------------------------------------------------------------------------- */
/* clearAllData --                                                             */
/* --------------------------------------------------------------------------- */

void GuiTable::clearAllData(){
  ClearFunctor functor;
  editData( getOrientation() == GuiElement::orient_Vertical, functor, true );
}

/* --------------------------------------------------------------------------- */
/* packData --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiTable::packData( bool use_row ){
  PackFunctor functor;
  editData( use_row, functor );
}

/* --------------------------------------------------------------------------- */
/* getRowMenu --                                                               */
/* --------------------------------------------------------------------------- */

void GuiTable::getRowMenu() {
  GuiMenuButton *button = 0;
  std::ostringstream title;
  title << _("Row ");
  if (!AppData::Instance().HeadlessWebMode()) {
    title << (m_row + getTableRowRangeFrom() - m_hline_list.size());
  }
  if (!m_row_menu) {
    m_row_menu = GuiFactory::Instance()->createPopupMenu( getElement() );
    m_row_menu->setTitle( _("RowMenu") );
    m_row_menu->resetMenuPost();
    m_row_menu_title = GuiFactory::Instance()->createMenuButton( m_row_menu->getElement(), NULL);
    m_row_menu->attach( m_row_menu_title->getElement() );
    m_row_menu_title->setLabel( title.str() );
  } else {
    m_row_menu_title->setLabel( title.str() );
    return;
  }

  m_row_menu->attach( GuiFactory::Instance()->createSeparator(m_row_menu->getElement())->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_row_menu->getElement(), &m_row_insert );
  button->setLabel( _("Insert empty row") );
  m_row_menu->attach( button->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_row_menu->getElement(), &m_row_delete );
  button->setLabel( _("Delete row") );
  m_row_menu->attach( button->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_row_menu->getElement(), &m_row_duplicate );
  button->setLabel( _("Duplicate row") );
  m_row_menu->attach( button->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_row_menu->getElement(), &m_row_clear );
  button->setLabel( _("Clear row") );
  m_row_menu->attach( button->getElement() );

  m_row_menu->attach( GuiFactory::Instance()->createSeparator(m_row_menu->getElement())->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_row_menu->getElement(), &m_row_pack );
  button->setLabel( _("Pack all rows") );
  m_row_menu->attach( button->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_row_menu->getElement(), &m_row_delete_selected );
  button->setLabel( _("Delete selected rows") );
  m_row_menu->attach( button->getElement() );

  m_row_menu->getElement()->create();
}

/* --------------------------------------------------------------------------- */
/* getColumnMenu --                                                            */
/* --------------------------------------------------------------------------- */

void GuiTable::getColumnMenu() {
  GuiMenuButton *button = 0;
  std::ostringstream title;
  title << _("Column ");
  if (!AppData::Instance().HeadlessWebMode()) {
    title << (m_col + getTableColRangeFrom() - m_vline_list.size());
  }
  if (!m_col_menu) {
    m_col_menu = GuiFactory::Instance()->createPopupMenu( getElement() );
    m_col_menu->setTitle( _("ColumnMenu") );
    m_col_menu->resetMenuPost();
    m_col_menu_title = GuiFactory::Instance()->createMenuButton(m_col_menu->getElement(), NULL);
    m_col_menu->attach( m_col_menu_title->getElement()  );
    m_col_menu_title->setLabel( title.str() );
  } else {
    m_col_menu_title->setLabel( title.str() );
    return;
  }

  m_col_menu->attach( GuiFactory::Instance()->createSeparator(m_col_menu->getElement())->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_col_menu->getElement(), &m_col_insert );
  button->setLabel( _("Insert empty column") );
  m_col_menu->attach( button->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_col_menu->getElement(), &m_col_delete );
  button->setLabel( _("Delete column") );
  m_col_menu->attach( button->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_col_menu->getElement(), &m_col_duplicate );
  button->setLabel( _("Duplicate column") );
  m_col_menu->attach( button->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_col_menu->getElement(), &m_col_clear );
  button->setLabel( _("Clear column") );
  m_col_menu->attach( button->getElement() );

  m_col_menu->attach( GuiFactory::Instance()->createSeparator(m_col_menu->getElement())->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_col_menu->getElement(), &m_col_pack );
  button->setLabel( _("Pack all columns") );
  m_col_menu->attach( button->getElement() );

  button = GuiFactory::Instance()->createMenuButton( m_col_menu->getElement(), &m_col_delete_selected );
  button->setLabel( _("Delete selected column") );
  m_col_menu->attach( button->getElement() );

  m_col_menu->getElement()->create();
}

/* --------------------------------------------------------------------------- */
/* createPopupMenu --                                                          */
/* --------------------------------------------------------------------------- */

GuiPopupMenu* GuiTable::PopupMenu(int x, int y, GuiElement::Orientation orientation){
  if( (getTableMatrix() || getOrientation() == GuiElement::orient_Horizontal ) &&
      orientation == GuiElement::orient_Horizontal ){
    const std::string s("_COL");
    getColumnMenu();
    return m_col_menu;
  }
  if( (getTableMatrix() || getOrientation() == GuiElement::orient_Vertical ) &&
      orientation == GuiElement::orient_Vertical ){
    std::string s("_ROW");
    getRowMenu();
    return m_row_menu;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getSizeVLineList --                                                         */
/* --------------------------------------------------------------------------- */

int GuiTable::getSizeVLineList(SizeType size_type) {
  if (size_type != size_visible)
    return m_vline_list.size();
  int num(0);
  GuiTableLineList::iterator li;
  for( li = getVLineList().begin(); li != getVLineList().end(); ++li ){
    if ((*li)->isVisible()){
      ++num;
    }
  }
  return num;
}

/* --------------------------------------------------------------------------- */
/* getSizeHLineList --                                                         */
/* --------------------------------------------------------------------------- */

int GuiTable::getSizeHLineList(SizeType size_type) {
  if (size_type != size_visible)
    return m_hline_list.size();
  int num(0);
  GuiTableLineList::iterator li;
  for( li = getHLineList().begin(); li != getHLineList().end(); ++li ){
    if ((*li)->isVisible()){
      ++num;
    }
  }
  return num;
}

/* --------------------------------------------------------------------------- */
/* getSizeTableList --                                                         */
/* --------------------------------------------------------------------------- */

int GuiTable::getSizeTableList(SizeType size_type) {
  if (size_type != size_visible)
    return m_table_list.size();
  int num(0);
  GuiTableLineList::iterator li;
  for( li = m_table_list.begin(); li != m_table_list.end(); ++li ){
    if ((*li)->isVisible()){
      ++num;
    }
  }
  return num;
}

/* --------------------------------------------------------------------------- */
/* getStream --                                                          */
/* --------------------------------------------------------------------------- */

Stream* GuiTable::getStream() {
  if (m_stream)
    return m_stream;
  XferDataItem *dataitem;
  m_stream = StreamManager::Instance().newStream();
  m_stream->setDelimiter( GuiFactory::Instance()->getDelimiter() );

  // alle vertikalen Titelzeile DataItems suchen
  std::vector<XferDataItem* > _vertVec;
  GuiTableLineList::iterator li;
  /*
  for( li = getVLineList().begin(); li != getVLineList().end(); ++li ){
    dataitem = (*li)->getDataItem(0);
    if (!dataitem) continue;

    if( dataitem->getNumOfWildcards() == 1 ){
      _vertVec.push_back( dataitem );
    }
  }

  // alle horizontalen Titelzeile
  std::vector<XferDataItem* > _horzVec;
  for( li = getHLineList().begin(); li != getHLineList().end(); ++li ){
    dataitem = (*li)->getDataItem(0);
    if (!dataitem) continue;
    if( dataitem->getNumOfWildcards() == 1 ){
      _horzVec.push_back( dataitem );
	// m_stream->beginGroup();
	// StreamParameter *s = m_stream->addParameter( dataitem, 0, -1, 0, false, false, false );
	// m_stream->endGroup();
	// m_stream->addToken( "\n", 1, false);
    }
  }


  // vertikale Ausrichtung :: zuerst horz Titel
  if (getOrientation() == GuiElement::orient_Vertical) {
    std::vector<XferDataItem* >::iterator it;
    for( it = _horzVec.begin(); it != _horzVec.end(); ++it ){
      m_stream->beginGroup();
      StreamParameter *s = m_stream->addParameter( (*it), 0, -1, 0, false, false, false );
      m_stream->endGroup();
    }
    m_stream->beginGroup();
  }

  // vertikale Ausrichtung :: vertikale Titelzeile
  if (getOrientation() == GuiElement::orient_Vertical) {
    std::vector<XferDataItem* >::iterator it;
    for( it = _vertVec.begin(); it != _vertVec.end(); ++it ){
      StreamParameter *s = m_stream->addParameter( (*it), 0, -1, 0, false, false, false );
    }
  }
*/
  // vertikale Ausrichtung
  if (getOrientation() == GuiElement::orient_Vertical) {
	m_stream->beginGroup();
  }
  int i=0;
  for( li = getTableList().begin(); li != getTableList().end(); ++li, ++i ){
    dataitem = (*li)->getDataItem(0);
    if (!dataitem) continue;
    if( dataitem->getNumOfWildcards() == 1 ){
      if (getOrientation() == GuiElement::orient_Horizontal)
	m_stream->beginGroup();
      StreamParameter *s = m_stream->addParameter( dataitem, 0, -1,
					     (*li)->getScalefactor(i),
					     false, false, false );
      if (getOrientation() == GuiElement::orient_Horizontal) {
	m_stream->endGroup();
	m_stream->addToken( "\n", 1, false);
      }
    }
  }

  // vertikale Ausrichtung
  if (getOrientation() == GuiElement::orient_Vertical) {
    m_stream->addToken( "\n", 1, false);
    m_stream->endGroup();
  }

  return m_stream;
}

void GuiTable::DeleteListener::ButtonPressed() {
  if (m_allSelected) {
    m_table->deleteSelectedData(m_use_row);
  } else {
    m_table->deleteData(m_use_row);
  }
}

/* --------------------------------------------------------------------------- */
/* pushArgRowColumnNumber --                                                   */
/* --------------------------------------------------------------------------- */
void GuiTable::pushArgRowColumnNumber(int rowColumn) {
  BUG_DEBUG("Push row_column: " << rowColumn);
  if (rowColumn >= 0)
    m_argRowColumn = rowColumn;
}

/* --------------------------------------------------------------------------- */
/* popArgRowColumnNumber --                                                    */
/* --------------------------------------------------------------------------- */
int GuiTable::popArgRowColumnNumber(){
  int ret = m_argRowColumn;
  BUG_DEBUG("got Pop row_column: " << ret);
  m_argRowColumn = -1;
  return ret;
}

/* --------------------------------------------------------------------------- */
/* pushArgSelected --                                                          */
/* --------------------------------------------------------------------------- */
void GuiTable::pushArgSelected(const Json::Value& jsonAry) {
  BUG_DEBUG("Push Selected: " << ch_semafor_intens::JsonUtils::value2string(jsonAry));
  ch_semafor_intens::JsonUtils::value2vectorInt(jsonAry,  m_argSelected);
}

/* --------------------------------------------------------------------------- */
/* popArgSelected --                                                          */
/* --------------------------------------------------------------------------- */
bool GuiTable::popArgSelected(int& x0, int& y0, int& x1, int& y1) {
  x0 = -1, y0 = -1, x1 = -1, y1 = -1;
  BUG_DEBUG("POP Selected: " << m_argSelected.size());
  if (m_argSelected.size()) {
    if (m_argSelected.size() > 0) x0 = m_argSelected[0];
    if (m_argSelected.size() > 1) y0 = m_argSelected[1];
    if (m_argSelected.size() > 2) x1 = m_argSelected[2];
    if (m_argSelected.size() > 3) y1 = m_argSelected[3];
    m_argSelected.clear();
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* pushArgClipboard --                                                         */
/* --------------------------------------------------------------------------- */
void GuiTable::pushArgClipboardText(const std::string data) {
  BUG_DEBUG("Push clipboard: " << data);
  m_clipboardText = data;
}

/* --------------------------------------------------------------------------- */
/* popArgClipboard --                                                          */
/* --------------------------------------------------------------------------- */
std::string GuiTable::popArgClipboardText() {
  std::string ret( m_clipboardText);
  BUG_DEBUG("got Pop clipboard: " << m_clipboardText);
  m_clipboardText.clear();
  return ret;
}

/* --------------------------------------------------------------------------- */
/* setFunction --                                                              */
/* --------------------------------------------------------------------------- */
void GuiTable::setFunction( JobFunction *func ) {
  assert( func != 0 );
  m_function = func;
  m_function->setUsed();
}

//---------------------------------------------------------------------------
// serializeXML
//---------------------------------------------------------------------------

void GuiTable::serializeXML(std::ostream &os, bool recursive ){
  os << "<intens:Table";
  os << " lines=\""<<getTableList().size()<<"\"";
  os << ">" << std::endl;

  GuiTableLineList::iterator li;
  for( li = getTableList().begin(); li != getTableList().end(); ++li ){
    (*li)->serializeXML(os, recursive);
  }

  os << "</intens:Table>"<<std::endl;
}

//---------------------------------------------------------------------------
// getHeaderLabels
//---------------------------------------------------------------------------

void GuiTable::getHeaderLabels(Json::Value& vertLblAry, Json::Value& horzLblAry, int& maxVertLen) {
  std::vector<std::string> vertLblVec;
  std::vector<std::string> horzLblVec;
  std::vector<std::string>::iterator it;
  maxVertLen = 0;
  getHeaderLabels(vertLblVec, horzLblVec);
  for(it = vertLblVec.begin(); it != vertLblVec.end(); ++it) {
    vertLblAry.append(*it);
    maxVertLen = std::max(maxVertLen, (int) (*it).size());
  }
  for(it = horzLblVec.begin(); it != horzLblVec.end(); ++it) {
    horzLblAry.append(*it);
  }
  BUG_DEBUG("Name[" << getElement()->getName()<<"] horz["
	    << ch_semafor_intens::JsonUtils::value2string(horzLblAry) << "] vert["
	    << ch_semafor_intens::JsonUtils::value2string(vertLblAry)<<"]");
}

//---------------------------------------------------------------------------
// getHeaderLabels
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// getHeaderLabels
//---------------------------------------------------------------------------

void GuiTable::getHeaderLabels(std::vector<std::string>& vertLblVec,
			       std::vector<std::string>& horzLblVec, int& maxVertLen) {
  std::vector<std::string>::iterator it;
  maxVertLen = 0;
  getHeaderLabels(vertLblVec, horzLblVec);
  for(it = vertLblVec.begin(); it != vertLblVec.end(); ++it) {
    maxVertLen = std::max(maxVertLen, (int) (*it).size());
  }
}

//---------------------------------------------------------------------------
// getHeaderLabels
//---------------------------------------------------------------------------

void GuiTable::getHeaderLabels(std::vector<std::string>& vertLblAry,
			       std::vector<std::string>& horzLblAry) {
  int numRows=0, numCols=0;
  vertLblAry.clear();
  horzLblAry.clear();
  getTableSize( numRows, numCols );
  if (m_emptyRow) ++numRows;
  if (m_emptyColumn) ++numCols;

  // get header labels
  if( getTableMatrix() ){
    // Matrix :: labels Horz
    int v;
    for (v = 0; v < (int)getVLineList().size(); ++v) {
      std::string str(getVLineList()[v]->Label());
      replaceAll(str, "\n", "\\n");
      HTMLConverter::convertFromHTML(str);
      removeFontString(str);
      horzLblAry.push_back(str);
    }
    int voff=getTableColRangeFrom();
    for(; v<numCols; ++v){
      std::stringstream os;
      os << (v-getVLineList().size()+voff);
      horzLblAry.push_back(os.str());
    }

    // Matrix :: labels Vert
    for (int h = 0; h < (int)getHLineList().size(); ++h) {
      std::string str(getHLineList()[h]->Label());
      replaceAll(str, "\n", "\\n");
      HTMLConverter::convertFromHTML(str);
      removeFontString(str);
      vertLblAry.push_back(str);
    }
    int hoff=getTableRowRangeFrom();
    for(int i = 0; i < (numRows - (int)getHLineList().size()); ++i){
      std::stringstream os;
      os << (i+hoff);
      vertLblAry.push_back(os.str());
    }
  } else {
    if( getOrientation() == GuiElement::orient_Vertical ){
      // orient Vert :: labels Horz
      for (int v = 0; v < (int)getVLineList().size(); ++v) {
        std::string str(getVLineList()[v]->Label());
        replaceAll(str, "\n", "\\n");
        HTMLConverter::convertFromHTML(str);
        removeFontString(str);
        horzLblAry.push_back(str);
      }
      for(GuiTableLineList::iterator li = getTableList().begin();
          li != getTableList().end(); ++li ){
        if ((*li)->getTableItem(0)->getDataField()->getLength() == 0) continue;
        std::string str((*li)->Label());
        replaceAll(str, "\n", "\\n");
        HTMLConverter::convertFromHTML(str);
        removeFontString(str);
        horzLblAry.push_back(str);
      }

      // orient Vert :: labels Vert
      for (int h = 0; h < (int)getHLineList().size(); ++h) {
        std::string str(getHLineList()[h]->Label());
        replaceAll(str, "\n", "\\n");
        HTMLConverter::convertFromHTML(str);
        removeFontString(str);
        vertLblAry.push_back(str);
      }
      int off=getTableRowRangeFrom();
      for( int i = 0; i < (numRows - (int)getHLineList().size()); ++i){
        std::stringstream os;
        os << (i+off);
        vertLblAry.push_back(os.str());
      }
    } else {
      // orient Horz :: labels Horz
      for (int v = 0; v < (int)getVLineList().size(); ++v) {
        std::string str(getVLineList()[v]->Label());
        replaceAll(str, "\n", "\\n");
        HTMLConverter::convertFromHTML(str);
        removeFontString(str);
        horzLblAry.push_back(str);
      }
      int off=getTableColRangeFrom();
      for( int i = 0; i < (numCols - (int)getVLineList().size()); ++i){
        std::stringstream os;
        os << (i+off);
        horzLblAry.push_back(os.str());
      }
      // orient Horz :: labels Vert
      for (int h = 0; h < (int)getHLineList().size(); ++h) {
        std::string str(getHLineList()[h]->Label());
        replaceAll(str, "\n", "\\n");
        HTMLConverter::convertFromHTML(str);
        removeFontString(str);
        vertLblAry.push_back(str);
      }
      for(GuiTableLineList::iterator li = getTableList().begin();
          li != getTableList().end(); ++li){
        if ((*li)->getTableItem(0)->getDataField()->getLength() == 0) continue;
        std::string str((*li)->Label());
        // replaceAll(str, "\n", "\\n");
        // HTMLConverter::convertFromHTML(str);
        removeFontString(str);
        vertLblAry.push_back(str);
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiTable::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  bool updated = isDataItemUpdated(GuiManager::Instance().LastWebUpdate());
  if (onlyUpdated && !updated) {
    return false;
  }
  Json::Value jsonVertLblAry = Json::Value(Json::arrayValue);
  Json::Value jsonHorzLblAry = Json::Value(Json::arrayValue);
  int maxVertLblLength;
  int maxFrozenColumnLength(0);
  int cntHiddenCols(0);
  Json::Value jsonHorzLenAry = Json::Value(Json::arrayValue);
  Json::Value jsonHorzDefaultAry = Json::Value(Json::arrayValue);
  Json::Value jsonVertDefaultAry = Json::Value(Json::arrayValue);
  GuiTableLineList::iterator lit;

  // popup base menu
  Json::Value jsonElem = Json::Value(Json::objectValue);
  GuiPopupMenu* pm = getElement()->getPopupMenu();
  pm->serializeJson(jsonElem, onlyUpdated);
  jsonObj["popup_base_menu"] = jsonElem;

  // popup header menu
  getHeaderLabels(jsonVertLblAry, jsonHorzLblAry, maxVertLblLength);
  if (isEditable() && !isIndexMenuHidden()) {
    if (m_table_matrix || getOrientation() == GuiElement::orient_Vertical) {
      Json::Value jsonElem = Json::Value(Json::objectValue);
      GuiPopupMenu* pm = GuiTable::PopupMenu(0, 0, GuiElement::orient_Vertical);
      pm->serializeJson(jsonElem, onlyUpdated);
      jsonObj["popup_header_menu"] = jsonElem;
    }

    if (m_table_matrix || getOrientation() == GuiElement::orient_Horizontal) {
      Json::Value jsonElem = Json::Value(Json::objectValue);
      GuiPopupMenu* pm = GuiTable::PopupMenu(0, 0, GuiElement::orient_Horizontal);
      pm->serializeJson(jsonElem, onlyUpdated);
      jsonObj["popup_header_menu_col"] = jsonElem;
    }
  }

  // horizontal length left header
  for( lit = getVLineList().begin(); lit != getVLineList().end(); ++lit ){
    int n=0;
    jsonHorzLenAry.append(hasAutoWidth() ? -1 :
                          (*lit)->getTableItem( n )->getDataField()->getLength());
  }
  // horizontal length data
  if (getOrientation() == GuiElement::orient_Horizontal) {
    int maxlen = -1;
    if (!hasAutoWidth()) {
      for( lit = getTableList().begin(); lit != getTableList().end(); ++lit ) {
        maxlen = std::max(maxlen,(*lit)->getTableItem( 0 )->getDataField()->getLength());
        if ((*lit)->getTableItem(0)->getDataField()->getLength() == 0) ++cntHiddenCols;
      }
    }
    for(int i=0; i < m_cntCol; ++i) {
      jsonHorzLenAry.append(maxlen);
    }
  } else {
    for( lit = getTableList().begin(); lit != getTableList().end(); ++lit ){
      int n=0;
      int len = (*lit)->getTableItem( n )->getDataField()->getLength();
      if (len)
        jsonHorzLenAry.append(hasAutoWidth() ? -1 : len);
      if (getTableMatrix()) {
        int numRows=0, numCols=0;
        getTableMatrixSize( numRows, numCols );
        while (--numCols > (int)getVLineList().size()) {
          jsonHorzLenAry.append(hasAutoWidth() ? -1 :
                                (*lit)->getTableItem( n )->getDataField()->getLength());
        }
      }
      if ((*lit)->getTableItem(0)->getDataField()->getLength() == 0) ++cntHiddenCols;
    }
  }
  // vertical length top header
  for( lit = getVLineList().begin(); lit != getVLineList().end(); ++lit ){
    maxFrozenColumnLength = std::max(maxFrozenColumnLength,
                                     (*lit)->getTableItem(0)->getDataField()->getLength());
  }

  getElement()->writeJsonProperties(jsonObj);
  jsonObj["title"] = m_title;
  jsonObj["title_alignment"] = GuiElement::StringAlignmentType(m_title_alignment);
  jsonObj["title_horizontal"] = m_title_horizontal;
  jsonObj["title_vertical"] = m_title_vertical;
  if (getHLineList().size() > 0)
    jsonObj["placement_horizontal"] = GuiElement::StringAlignmentType(m_hline_placement);
  if (getVLineList().size() > 0)
    jsonObj["placement_vertical"] = GuiElement::StringAlignmentType(m_vline_placement);

  // row header
  if (getOrientation() == GuiElement::orient_Vertical && jsonVertLblAry.size() > m_maxRowLen) {
    jsonVertLblAry.resize(m_maxRowLen);
  }
  jsonObj["row_header"] = m_row_header_hidden ? Json::Value(Json::arrayValue) : jsonVertLblAry;
  jsonObj["row_header_hidden"] = m_row_header_hidden;
  jsonObj["row_header_max_length"] = maxVertLblLength;
  jsonObj["row_header_length"] = maxFrozenColumnLength;
  jsonObj["row_offset"] = getTableRowRangeFrom();

  // column header
  if (getOrientation() == GuiElement::orient_Horizontal && jsonHorzLblAry.size() > m_maxRowLen) {
    jsonHorzLblAry.resize(m_maxRowLen);
  }
  jsonObj["column_header"] = m_col_header_hidden ? Json::Value(Json::arrayValue) : jsonHorzLblAry;
  jsonObj["column_header_hidden"] = m_col_header_hidden;
  jsonObj["column_length"] = jsonHorzLenAry;
  jsonObj["column_default_data"] = jsonHorzDefaultAry;
  jsonObj["column_offset"] = getTableColRangeFrom();

  jsonObj["orientation"] = (m_orientation == GuiElement::orient_Vertical && !m_table_matrix)? "vertical" : "horizontal";
  jsonObj["isMatrix"] = m_table_matrix;
  jsonObj["navigation"] = m_navigation == GuiElement::orient_Vertical ? "vertical" : "horizontal";
  int numRows, numCols, numRowsVis, numColsVis;
  getTableSize( numRows, numCols );
  getTableSize( numRowsVis, numColsVis, size_init );
  if (m_emptyRow) ++numRows;
  if (m_emptyColumn) ++numCols;
  jsonObj["num_rows"] = numRows - (getOrientation() == GuiElement::orient_Horizontal ? cntHiddenCols : 0);
  jsonObj["num_columns"] = numCols - (getOrientation() != GuiElement::orient_Horizontal ? cntHiddenCols : 0);
  jsonObj["num_rows_visible"] = numRowsVis - (getOrientation() == GuiElement::orient_Horizontal ? cntHiddenCols : 0);
  jsonObj["num_columns_visible"] = numColsVis - (getOrientation() != GuiElement::orient_Horizontal ? cntHiddenCols : 0);
  jsonObj["num_frozen_rows"] = (int) getHLineList().size();
  jsonObj["num_frozen_columns"] = (int) getVLineList().size();
  // is table size fixed?
  if ( isTableVerticalFixed() )
    jsonObj["num_rows_fixed"] = getTableRows();
  if ( isTableHorizontalFixed() )
    jsonObj["num_columns_fixed"] = getTableCols();

  //  jsonObj["updated"] = isDataItemUpdated(GuiManager::Instance().LastUpdate());

  Json::Value jsonAry = Json::Value(Json::arrayValue);
  GuiTableLineList::iterator li;

  // matrix
  Json::Value rowAry = Json::Value(Json::arrayValue);
  matrixSerializeJson(rowAry, numRows, numCols, false, 0, m_maxRowLen);
  jsonObj["matrix"] = rowAry;
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiTable::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  bool updated = isDataItemUpdated(GuiManager::Instance().LastWebUpdate());
  if (onlyUpdated && !updated) {
    return false;
  }
  auto vertLblVec = std::vector<std::string>();
  auto horzLblVec = std::vector<std::string>();
  int maxVertLblLength;
  int maxFrozenColumnLength(0);
  int cntHiddenCols(0);
  auto horzLenVec = std::vector<int>();
  GuiTableLineList::iterator lit;
  auto element = eles->add_tables();

  // popup base menu
  GuiPopupMenu* pm = getElement()->getPopupMenu();
  pm->serializeProtobuf(eles, element->mutable_popup_base_menu(), onlyUpdated);

  // popup header menu
  getHeaderLabels(vertLblVec, horzLblVec, maxVertLblLength);
  if (isEditable() && !isIndexMenuHidden()) {
    if (m_table_matrix || getOrientation() == GuiElement::orient_Vertical) {
      GuiPopupMenu* pm = GuiTable::PopupMenu(0, 0, GuiElement::orient_Vertical);
      pm->serializeProtobuf(eles, element->mutable_popup_header_menu(), onlyUpdated);
    }

    if (m_table_matrix || getOrientation() == GuiElement::orient_Horizontal) {
      GuiPopupMenu* pm = GuiTable::PopupMenu(0, 0, GuiElement::orient_Horizontal);
      pm->serializeProtobuf(eles, element->mutable_popup_header_menu_col(), onlyUpdated);
    }
  }

  // horizontal length left header
  for( lit = getVLineList().begin(); lit != getVLineList().end(); ++lit ){
    int n=0;
    horzLenVec.push_back(hasAutoWidth() ? -1 :
                          (*lit)->getTableItem( n )->getDataField()->getLength());
  }
  // horizontal length data
  if (getOrientation() == GuiElement::orient_Horizontal) {
    int maxlen = -1;
    if (!hasAutoWidth()) {
      for( lit = getTableList().begin(); lit != getTableList().end(); ++lit ) {
        maxlen = std::max(maxlen,(*lit)->getTableItem( 0 )->getDataField()->getLength());
        if ((*lit)->getTableItem(0)->getDataField()->getLength() == 0) ++cntHiddenCols;
      }
    }
    for(int i=0; i < m_cntCol; ++i) {
      horzLenVec.push_back(maxlen);
    }
  } else {
    for( lit = getTableList().begin(); lit != getTableList().end(); ++lit ){
      int n=0;
      int len = (*lit)->getTableItem( n )->getDataField()->getLength();
      if (len)
        horzLenVec.push_back(hasAutoWidth() ? -1 : len);
      if (getTableMatrix()) {
        int numRows=0, numCols=0;
        getTableMatrixSize( numRows, numCols );
        while (--numCols > (int)getVLineList().size()) {
          horzLenVec.push_back(hasAutoWidth() ? -1 :
                                (*lit)->getTableItem( n )->getDataField()->getLength());
        }
      }
      if ((*lit)->getTableItem(0)->getDataField()->getLength() == 0) ++cntHiddenCols;
    }
  }
  // vertical length top header
  for( lit = getVLineList().begin(); lit != getVLineList().end(); ++lit ){
    maxFrozenColumnLength = std::max(maxFrozenColumnLength,
                                     (*lit)->getTableItem(0)->getDataField()->getLength());
  }

  element->set_allocated_base(getElement()->writeProtobufProperties());

  element->set_title(m_title);
  element->set_title_horizontal(m_title_horizontal);
  element->set_title_vertical(m_title_vertical);
  element->set_title_alignment(GuiElement::ProtoAlignmentType(m_title_alignment));
  if (getHLineList().size() > 0)
    element->set_placement_horizontal(GuiElement::ProtoAlignmentType(m_hline_placement));
  if (getVLineList().size() > 0)
    element->set_placement_vertical(GuiElement::ProtoAlignmentType(m_vline_placement));

  // row header
  element->mutable_row_header()->Assign(vertLblVec.begin(), vertLblVec.end());
  element->set_row_header_hidden(m_row_header_hidden);
  element->set_row_header_max_length(maxVertLblLength);
  element->set_row_header_length(maxFrozenColumnLength);
  element->set_row_offset(getTableRowRangeFrom());

  // column header
  element->mutable_column_header()->Assign(horzLblVec.begin(), horzLblVec.end());
  element->set_column_header_hidden(m_col_header_hidden);
  element->mutable_column_length()->Assign(horzLenVec.begin(), horzLenVec.end());
  element->set_column_offset(getTableColRangeFrom());

  element->set_orientation(m_orientation == GuiElement::orient_Vertical && !m_table_matrix ?
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Vertical :
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Horizontal);
  element->set_is_matrix(m_table_matrix);
  element->set_navigation(m_navigation == GuiElement::orient_Vertical ?
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Vertical :
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Horizontal);
  int numRows, numCols, numRowsVis, numColsVis;
  getTableSize( numRows, numCols );
  getTableSize( numRowsVis, numColsVis, size_init );
  if (m_emptyRow) ++numRows;
  if (m_emptyColumn) ++numCols;
  element->set_num_rows(numRows - (getOrientation() == GuiElement::orient_Horizontal ? cntHiddenCols : 0));
  element->set_num_columns(numCols - (getOrientation() != GuiElement::orient_Horizontal ? cntHiddenCols : 0));
  element->set_num_rows_visible(numRowsVis - (getOrientation() == GuiElement::orient_Horizontal ? cntHiddenCols : 0));
  element->set_num_columns_visible(numColsVis - (getOrientation() != GuiElement::orient_Horizontal ? cntHiddenCols : 0));
  element->set_num_frozen_rows(getHLineList().size());
  element->set_num_frozen_columns(getVLineList().size());
  // is table size fixed?
  if ( isTableVerticalFixed() )
    element->set_num_rows_fixed(getTableRows());
  if ( isTableHorizontalFixed() )
    element->set_num_columns_fixed(getTableCols());

  // matrix
  matrixSerializeProtobuf(eles, element->mutable_matrix(), numRows, numCols, false, 0, m_maxRowLen);
  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */
std::string GuiTable::variantMethod(const std::string& method,
                                     const Json::Value& jsonArgs, JobEngine *eng) {
  Json::Value jsonElem = Json::Value(Json::objectValue);
  if (lower(method) == "menubuttonaction") {
    if (jsonArgs.isMember("id") && jsonArgs["id"].isInt()) {
      GuiButtonListener* btnLsnr = GuiButtonListener::getButtonListenerByMenuButtonId(jsonArgs["id"].asInt());
      if (btnLsnr) {
        if (jsonArgs.isMember("row_column") && jsonArgs["row_column"].isInt()) {
          pushArgRowColumnNumber(jsonArgs["row_column"].asInt());
        }
        if (jsonArgs.isMember("selected") && jsonArgs["selected"].isArray()) {
          pushArgSelected(jsonArgs["selected"]);
        }
        if (jsonArgs.isMember("clipboard") && jsonArgs["clipboard"].isString()) {
          pushArgClipboardText(jsonArgs["clipboard"].asString());
        }
        btnLsnr->ButtonPressed();
        jsonElem["status"] = "OK";
      } else {
        jsonElem["message"] = "Wrong Parameter";
        jsonElem["status"] = "Error";
      }
    }
  } else if (lower(method) == "gettabledata") {
    Json::Value::Members members = jsonArgs.getMemberNames();
    if (jsonArgs.isMember("range") &&
        jsonArgs["range"].isArray() &&
        jsonArgs["range"].size() == 2) {
      int numRows, numCols;
      getTableSize( numRows, numCols );
      // matrix
      if (m_emptyRow) ++numRows;
      if (m_emptyColumn) ++numCols;
      Json::Value rowAry = Json::Value(Json::arrayValue);
      matrixSerializeJson(rowAry, numRows, numCols, false,
                          jsonArgs["range"].get((Json::ArrayIndex) 0, 0).asInt(),
                          jsonArgs["range"].get((Json::ArrayIndex) 1, numRows).asInt());
      jsonElem["from"] = (int) jsonArgs["range"].get((Json::ArrayIndex)  0, 0).asInt();
      jsonElem["to"] = (int) jsonArgs["range"].get((Json::ArrayIndex) 1, numRows).asInt();
      jsonElem["matrix"] = rowAry;
      jsonElem["status"] = "OK";
      BUG_DEBUG("getTableData Args: " << ch_semafor_intens::JsonUtils::value2string(jsonArgs));

      // row column header
      int maxVertLblLength;
      Json::Value jsonVertLblAry = Json::Value(Json::arrayValue);
      Json::Value jsonHorzLblAry = Json::Value(Json::arrayValue);
      Json::Value jsonLblAry = Json::Value(Json::arrayValue);
      getHeaderLabels(jsonVertLblAry, jsonHorzLblAry, maxVertLblLength);
      int from = jsonElem["from"].asInt();
      if (getOrientation() == GuiElement::orient_Horizontal) {
        for (int i=from; i < std::min((int)jsonHorzLblAry.size(), from + m_maxRowLen); ++i) {
          jsonLblAry.append(jsonHorzLblAry[i]);
        }
        jsonElem["column_header"] = m_col_header_hidden ? Json::Value(Json::arrayValue) : jsonLblAry;
      } else {
        for (int i=from;i < std::min((int)jsonVertLblAry.size(), from + m_maxRowLen);++i) {
          jsonLblAry.append(jsonVertLblAry[i]);
        }
        jsonElem["row_header"] = m_row_header_hidden ? Json::Value(Json::arrayValue) : jsonLblAry;
      }
    }
  }

  if (!jsonElem.isNull()) {
    BUG_INFO("variantMethod Method[" << method << "], Args["
              << ch_semafor_intens::JsonUtils::value2string(jsonArgs) << "]  Return: "
              << ch_semafor_intens::JsonUtils::value2string(jsonElem));
    return ch_semafor_intens::JsonUtils::value2string(jsonElem);
  }
  return getElement()->defaultVariantMethod(method, jsonArgs, eng);
}

/* --------------------------------------------------------------------------- */
/* matrixSerializeJson --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiTable::matrixSerializeJson(Json::Value& rowAry, int numRows, int numCols, bool onlyUpdated, int from, int to) {
  int rId, cId;
  if( m_table_matrix || getOrientation() == GuiElement::orient_Horizontal ){
    for (int r=0; r < numRows; ++r) {
      GuiTableItem *itemRow = getFilledTableItem(r, 0, numRows, numCols);
      if (itemRow && !itemRow->getDataField()->getLength())
        continue;  // no length, hidden, continue
      Json::Value columnAry = Json::Value(Json::arrayValue);
      // for (int c=0; c < numCols; ++c) {
      int numCols2 = std::min(numCols, to);
      for (int c=from; c < numCols2; ++c) {
        Json::Value jsonData = Json::Value(Json::objectValue);
        GuiTableItem *item = getFilledTableItem(r, c, numRows, numCols);
        if (item && item->getElement()) {
          item->getElement()->serializeJson(jsonData, false);
          if (!jsonData.isMember("fgcolor")) {
            jsonData["fgcolor"] = "black";
          }
          // Alternate Color
          if (!jsonData.isMember("bgcolor")) {
            jsonData["bgcolor"] = "white";
          }
          std::string background(jsonData["bgcolor"].asString());
          alternateColor(background, r);
          jsonData["bgcolor"] = background;
          if (!isEditable()) {  // override editable
            jsonData["editable"] = false;
          }
        }
        removeExtentedData(jsonData);
        columnAry.append(jsonData);
      }
      rowAry.append(columnAry);
    }
  } else  {
    for (int c=0; c < numCols; ++c) {
      GuiTableItem *itemCol = getFilledTableItem(0, c, numRows, numCols);
      if (itemCol && !itemCol->getDataField()->getLength())
        continue;  // no length, hidden, continue
      Json::Value columnAry = Json::Value(Json::arrayValue);
      // for (int r=0; r < numRows; ++r) {
      int numRows2 = std::min(numRows, to);
      for (int r=from; r < numRows2; ++r) {
        Json::Value jsonData = Json::Value(Json::objectValue);
        GuiTableItem *item = getFilledTableItem(r, c, numRows, numCols);
        if (item && item->getElement()) {
          item->getElement()->serializeJson(jsonData, false);
          // Alternate Color
          if (jsonData.isMember("bgcolor")) {
            std::string background(jsonData["bgcolor"].asString());
            alternateColor(background, c);
            jsonData["bgcolor"] = background;
          }
          if (!isEditable()) {  // override editable
            jsonData["editable"] = false;
          }
        }
        removeExtentedData(jsonData);
        columnAry.append(jsonData);
      }
      rowAry.append(columnAry);
    }
  }
  return true;
}

#if HAVE_PROTOBUF
bool GuiTable::matrixSerializeProtobuf(in_proto::ElementList* eles,
                             google::protobuf::RepeatedPtrField<in_proto::Table::MatrixEntry>* matrix,
                             int numRows, int numCols, bool onlyUpdated, int from, int to) {
  int rId, cId;
  for (int r=0; r < numRows; ++r) {
    GuiTableItem *itemRow = getFilledTableItem(r, 0, numRows, numCols);
    if (itemRow && !itemRow->getDataField()->getLength())
      continue;  // no length, hidden, continue
    // for (int c=0; c < numCols; ++c) {
    auto rowsAry = matrix->Add();
    int numCols2 = std::min(numCols, to);
    for (int c=from; c < numCols2; ++c) {
      auto entry = rowsAry->add_a();
      GuiTableItem *item = getFilledTableItem(r, c, numRows, numCols);
      if (item && item->getElement()) {
        entry->set_allocated_element(dynamic_cast<GuiTableItem*>(item)->serializeProtobuf());
        entry->set_type(item->getElement()->ProtobufType());

        // entry->mutable_element()->mutable_base()->set_editable(isEditable());
        auto background = entry->element().base().bgcolor();
        if (background.empty()) background = "#fff";
        if (!isEditable()) {  // override editable
          entry->mutable_element()->mutable_base()->set_editable(false);
        }

        alternateColor(background, c);
        entry->set_bgcolor(background);
      }
    }
  }
  return true;
}
#endif

//----------------------------------------------------
// alternateColor
//----------------------------------------------------
void GuiTable::alternateColor(std::string& backgroundStr, int row) {
  QColor background(QString::fromStdString(backgroundStr));
  int ra = (row % 4) < 2 ? 0 : row % 4 -2;
  int rb = row % 2;
  background = background.darker(100+5*rb);
  if (ra) {
    int blue = background.blue()+10*ra;
    if (blue < 256)
      background.setBlue( blue );
  }
  backgroundStr = background.name().toStdString();
}

//----------------------------------------------------
// removeExtentedData
//----------------------------------------------------
void GuiTable::removeExtentedData(Json::Value& jsonData) {
  jsonData.removeMember("id");
  jsonData.removeMember("length");
  jsonData.removeMember("enabled");
  if (jsonData["type"].asString() != "TableToggleDataItem")  {
    jsonData.removeMember("type");
  }
  if (!jsonData["editable"].asBool()) {
    jsonData.removeMember("alignment");
    jsonData.removeMember("unit");
    jsonData.removeMember("typeLabel");
    jsonData.removeMember("name");
    jsonData.removeMember("label");
    jsonData.removeMember("fullName");
    jsonData.removeMember("helptext");
    jsonData.removeMember("precision");
    jsonData.removeMember("scale");
  }
}
