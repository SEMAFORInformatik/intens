
#include <vector>
#include <iostream>

#include "gui/GuiRuler.h"

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* setTitleSize --                                                             */
/* --------------------------------------------------------------------------- */

void GuiRuler::setTitleSize( int sz ){
  m_title.m_size = sz;
}

/* --------------------------------------------------------------------------- */
/* setSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiRuler::setSize( int pos, int sz ){
  if( pos >= (int)m_items.size() ){
    m_items.resize( pos + 1);
  }
  if( m_items[pos].m_size < sz ){
    m_items[pos].m_size = sz;
  }
}

/* --------------------------------------------------------------------------- */
/* setTitleSpacing --                                                          */
/* --------------------------------------------------------------------------- */

void GuiRuler::setTitleSpacing( int sz ){
  m_title.m_spacing = sz;
}

/* --------------------------------------------------------------------------- */
/* setSpacing --                                                               */
/* --------------------------------------------------------------------------- */

void GuiRuler::setSpacing( int pos, int sz ){
  if( pos >= (int)m_items.size() ){
    m_items.resize( pos + 1);
  }
  m_items[pos].m_spacing = sz;
}

/* --------------------------------------------------------------------------- */
/* setSpacing --                                                               */
/* --------------------------------------------------------------------------- */

void GuiRuler::setSpacing( int sz ){
  GuiRulerItemList::iterator ri;
  for( ri = m_items.begin(); ri != m_items.end(); ++ ri ){
    if( (*ri).m_spacing < sz ){
      (*ri).m_spacing = sz;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* TitleSize --                                                                */
/* --------------------------------------------------------------------------- */

int GuiRuler::TitleSize(){
  return m_title.m_size;
}

/* --------------------------------------------------------------------------- */
/* Size --                                                                     */
/* --------------------------------------------------------------------------- */

int GuiRuler::Size( int pos ){
  if( pos >= (int)m_items.size() ){
    return 0;
  }
  return m_items[pos].m_size;
}

/* --------------------------------------------------------------------------- */
/* TitleSpacing --                                                             */
/* --------------------------------------------------------------------------- */

int GuiRuler::TitleSpacing(){
  return m_title.m_size > 0 ? m_title.m_spacing : 0;
}

/* --------------------------------------------------------------------------- */
/* Spacing --                                                                  */
/* --------------------------------------------------------------------------- */

int GuiRuler::Spacing( int pos ){
  if( pos >= (int)m_items.size() ){
    return 0;
  }
  return m_items[pos].m_spacing;
}

/* --------------------------------------------------------------------------- */
/* alignFields --                                                              */
/* --------------------------------------------------------------------------- */

void GuiRuler::alignFields( int anz ){
  int sz = 0;
  int sp = 0;
  int i = 0;
  GuiRulerItemList::iterator ri = m_items.begin();
  while( ri != m_items.end() && i < anz ){
    sz = sz > (*ri).m_size ? sz : (*ri).m_size;
    sp = sp > (*ri).m_spacing ? sp : (*ri).m_spacing;
    ++ri;
    i++;
  }
  i = 0;
  ri = m_items.begin();
  while( ri != m_items.end() && i < anz ){
    (*ri).m_size = sz;
    (*ri).m_spacing = sp;
    ++ri;
    i++;
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiRuler::print(){
  std::cout << "GuiRuler::print" << std::endl;
  std::cout << " - Title: "
       << "size = " << m_title.m_size << ", spacing = " << m_title.m_spacing << std::endl;

  for( int i = 0; i < (int)m_items.size(); i++ ){
    std::cout << " - Entry " << i << ": "
         << "size = " << m_items[i].m_size
         << ", spacing = " << m_items[i].m_spacing << std::endl;
  }
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/
