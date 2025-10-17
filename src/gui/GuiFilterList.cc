
#include <string>

#include "gui/GuiFilterList.h"
// #include "utils/Debugger.h"

std::string GuiFilterList::s_search_mask;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiFilterList::GuiFilterList( GuiFilterListListener &listener )
  : m_visibleCols( 0 )
// #ifdef HAVE_XBAE
//   , m_sortOption(NONE)
// #endif
  , m_listener(listener){

  m_listener.setMyList(this);
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/*-----------------------------------------------------------------------------*/
/* setLables --                                                                */
/* --------------------------------------------------------------------------- */
// void GuiMotifFilterList::setLabels(){
//   GuiFilterListListener::StringsVector labels;

//   if(m_listener.setLabels( labels ) ){
//     labels.insert( labels.begin(), "Index" );
//     GUI_FILTER_LIST_BASE_CLASS::setLabels( labels );
//   }
// }


/* --------------------------------------------------------------------------- */
/* fillList --                                                                 */
/* --------------------------------------------------------------------------- */
// int GuiMotifFilterList::fillList(int selectedRowIndex){
//   GuiFilterListListener::RowsVector rows;
//   GuiFilterListListener::StringsVector items;

//   getListItems(rows);
// #ifdef HAVE_XBAE
//   GuiFilterListListener::RowsVector::iterator rit;
//   int r;
//   for(rit=rows.begin(), r=0;rit!=rows.end();++rit, ++r){
//     //Zeilennummer als Index schreiben
//     std::ostringstream index;
//     index << r;
//     (*rit).insert( (*rit).begin(), index.str() );
//   }

//   // setzen der statische Variablen fuer die Sortierung
//   GuiFilterListListener::RowsVector::setSortParams( m_listener.getSortColumn(),
// 						    rows.isColumnTypeNumeric(m_listener.getSortColumn()-1) );
//   if (m_sortOption == UP) {
//     sort( rows.begin(), rows.end(),  GuiFilterListListener::RowsVector::less);
//   } else if (m_sortOption == DOWN) {
//     sort( rows.begin(), rows.end(),  GuiFilterListListener::RowsVector::greater );
//   }
// #endif
//   addRows(m_rowsConf.size());

//   GuiFilterListListener::RowsVector::iterator r_iter;
//   GuiFilterListListener::StringsVector::iterator i_iter;

//   int row;
//   int column;
//   std::ostringstream index;
//   XmString str;

// #if ( XRTTBL_VERSION > 2 )
//   for(r_iter=rows.begin(), row=0;r_iter!=rows.end();++r_iter, ++row){
//     //Zeilennummer als Index schreiben
//     std::ostringstream index;
//     index << row;
//     str = XmStringCreateSimple( const_cast<char *>(index.str().c_str()) );
//     XrtTblSetCell( myWidget(), row, 0, str, FALSE, FALSE );
//     XmStringFree( str );

//     //Zellen fuellen
//     for(i_iter=(*r_iter).begin(), column=1 ;i_iter!=(*r_iter).end();++i_iter, ++column){
//       str = XmStringCreateSimple(const_cast<char *>(i_iter->c_str()));
//       XrtTblSetCell( myWidget(), row, column, str, FALSE, FALSE );
//       XmStringFree( str );
//     }
//   }
// #elif HAVE_XBAE
//   for(r_iter=rows.begin(), row=0;r_iter!=rows.end();++r_iter, ++row){
//     // Spalte 0 Index schreiben
//     std::ostringstream index;
//     index << row;
//     XbaeMatrixSetCell( myWidget(), row, 0, const_cast<char *>(index.str().c_str()));
//     //Zellen fuellen
//     for(i_iter=(*r_iter).begin(), column=0 ;i_iter!=(*r_iter).end();++i_iter, ++column){
//       XbaeMatrixSetCell( myWidget(), row, column, const_cast<char *>(i_iter->c_str()));
//     }
//   }
//   // reselect old selected row
//   if (selectedRowIndex >= 0) {
//     std::vector<GuiFilterListListener::StringsVector>::iterator it = rows.begin();
//     int n=0;
//     std::ostringstream os;
//     os << selectedRowIndex;
//     std::string search_str( os.str() );
//     for(; it != rows.end(); ++n, ++it) {
//       if ((*it)[0] == search_str) {
// 	XbaeMatrixDeselectAll(myWidget());
// 	XbaeMatrixUnhighlightAll( myWidget());
// 	XbaeMatrixCancelEdit(myWidget(), FALSE);
// 	XbaeMatrixSelectRow( myWidget(), n);
// 	XbaeMatrixEditCell(myWidget(), n  , 0);
// 	break;
//       }
//     }
//   }
// #endif
//   doListConfig();
//   return rows.size();
// }

/* --------------------------------------------------------------------------- */
/* getListItems  --                                                            */
/* --------------------------------------------------------------------------- */
void GuiFilterList::getListItems( GuiFilterListListener::RowsVector &rows ){
  m_rowsConf.clear();
  m_listener.getListItems(m_rowsConf, rows);
}

/* --------------------------------------------------------------------------- */
/* getListConf --                                                              */
/* --------------------------------------------------------------------------- */
void  GuiFilterList::getListConf(){
  m_rowsConf.clear();
  m_colsConf.clear();

  m_listener.initVisibleRows( m_rowsConf );
  m_listener.getListColsConf( m_colsConf, m_visibleCols );
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */
// void  GuiMotifFilterList::setTitle(){
//   std::string title;
//   if( m_listener.setTitle( title ) ){
//     GUI_FILTER_LIST_BASE_CLASS::setTitle( title );
//     createTitle();
//   }
// }


/* --------------------------------------------------------------------------- */
/* rowActivate --                                                              */
/* --------------------------------------------------------------------------- */
// void GuiFilterList::rowActivate( int row ){
//   m_listener.activated( row == -1 ? getSelection() : row );
// }

/* --------------------------------------------------------------------------- */
/* sortTable --                                                                */
/* --------------------------------------------------------------------------- */
// void GuiMotifFilterList::sortTable( int column, SortOp sortOp ){
// #ifdef HAVE_XRT
//   if( sortOp == DOWN )
//     XrtTblSortTable( myWidget(), column, (int (*) (const void *, const void *))sortCompareDown);
//   else
//     XrtTblSortTable( myWidget(), column, (int (*) (const void *, const void *))sortCompareUp);
// #elif HAVE_XBAE
//   setSortOption(sortOp);
//   m_listener.setSortColumn(column);
//   rebuild();
// #endif
// }

/* --------------------------------------------------------------------------- */
/* selected --                                                                 */
/* --------------------------------------------------------------------------- */

// void GuiFilterList::selected( int row ){
//   m_listener.selected( row );
// }

/* --------------------------------------------------------------------------- */
/* unselected --                                                               */
/* --------------------------------------------------------------------------- */

// void GuiFilterList::unselected( int row ){
//   m_listener.unselected( row );
// }
