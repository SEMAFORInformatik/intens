
#include "assert.h"
#include <stdlib.h>

#include "gui/GuiFilterListListener.h"
#include "gui/GuiFilterList.h"
#include "gui/GuiElement.h"

// statische member fuer die Sortierungsmethoden
int  GuiFilterListListener::RowsVector::s_sortColumn = 0;
bool GuiFilterListListener::RowsVector::s_sortNumeric = false;

int GuiFilterListListener::getSelection() {
  return m_list->getSelection();
}

void GuiFilterListListener::setMyList(GuiFilterList *xlist){
  assert( xlist != 0 );
  m_list=xlist;
}

void GuiFilterListListener::clearList(){
  if( m_list != 0 ) m_list->clear();
}

int GuiFilterListListener::rebuildList(){
  if( m_list == 0 ) return 0;
  return m_list->rebuild();
}

void GuiFilterListListener::grabListFocus () {
  if( m_list != 0 ) m_list->getElement()->grabFocus();
}

void GuiFilterListListener::enableList() {
  if( m_list != 0 ) m_list->getElement()->enable();
}

void GuiFilterListListener::disableList() {
  if( m_list != 0 ) m_list->getElement()->disable();
}

bool GuiFilterListListener::RowsVector::greater(  const StringsVector& svec1, const StringsVector& svec2 ) {
  if (s_sortNumeric)
    return atof(svec1[s_sortColumn].c_str()) > atof(svec2[s_sortColumn].c_str());
  else
    return svec1[s_sortColumn] > svec2[s_sortColumn];
}
bool GuiFilterListListener::RowsVector::less(  const StringsVector& svec1, const StringsVector& svec2 ) {
  if (s_sortNumeric)
    return atof(svec1[s_sortColumn].c_str()) < atof(svec2[s_sortColumn].c_str());
  else
    return svec1[s_sortColumn] < svec2[s_sortColumn];
}
