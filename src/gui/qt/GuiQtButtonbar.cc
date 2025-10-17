
/* qt headers */
#include <QHBoxLayout>
#include <QWidget>

/* System headers */
#include "utils/Debugger.h"

#include "gui/qt/GuiQtButtonbar.h"

INIT_LOGGER();

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtButtonbar::GuiQtButtonbar( GuiElement *parent )
  : GuiQtElement( parent )
  , m_buttonbar( 0 )
  , m_maxbuttons( 0 ){
}
GuiQtButtonbar::~GuiQtButtonbar(){
  delete m_buttonbar;
}


/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtButtonbar::create(){
  BUG_PARA(BugGui,"GuiQtButtonbar::create"
                 ,"Elements = " << m_elements.size());

  int nbuttons = (int)m_elements.size();

  if( nbuttons == 0 ){
    return;
  }

  // BESITZT IM MOMENT NOCH KEINEN PARENT -> EIGENE WINDOW
  m_buttonbar = new QWidget();
  m_buttonbar->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(5);

  // Buttons in Buttonbar einfuegen
  GuiElementList::iterator btn = m_elements.begin();
  for (int i=0; i<nbuttons; i++){
    (*btn) -> create();
    (*btn) -> manage();
    layout->addWidget( (*btn)->getQtElement()->myWidget(), 1 );
    btn++;
  }
  m_buttonbar->setLayout(layout);
}

/* --------------------------------------------------------------------------- */
/* setTabOrder --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtButtonbar::setTabOrder(){
  GuiElementList::iterator btn;
  for( btn = m_elements.begin(); btn != m_elements.end(); ++btn ){
    (*btn)->addTabGroup();
  }
}

/* --------------------------------------------------------------------------- */
/* unsetTabOrder --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtButtonbar::unsetTabOrder(){
  GuiElementList::iterator btn;
  for( btn = m_elements.begin(); btn != m_elements.end(); ++btn ){
    (*btn)->removeTabGroup();
  }
}

/* --------------------------------------------------------------------------- */
/* update  --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtButtonbar::update(UpdateReason r){
  GuiElementList::iterator btn;
  for( btn = m_elements.begin(); btn != m_elements.end(); ++btn ){
    (*btn)->update( r );
  }
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtButtonbar::manage(){
  if (m_buttonbar)  m_buttonbar -> show();
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtButtonbar::destroy(){
  BUG(BugGui,"GuiQtButtonbar::destroy");
  if( m_buttonbar != 0 ){
    m_buttonbar->deleteLater();
    m_buttonbar = 0;
    return true;
  }
  return true; //false;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtButtonbar::getSize( int &w, int &h ){
  w = h = 0;
  if (!m_buttonbar) return;
  h = m_buttonbar->height();
  w = m_buttonbar->width();
  QSize hs = m_buttonbar->sizeHint();
  BUG_DEBUG("getSize w: " << w << ", h:" << h << "  hintSize: " << hs.width() << ", " << hs.height());
  h = hs.height() < h ? h : hs.height();
  w = hs.width()  < w ? w : hs.width();
  w = std::min(w, 500); // max 500
  BUG_DEBUG("getSize w: " << w << ", h:" << h);
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtButtonbar::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG(BugGui,"GuiQtButtonbar::hasChanged");
  GuiElementList::iterator btn;
  for( btn = m_elements.begin(); btn != m_elements.end(); ++btn )
    if( (*btn)->hasChanged(trans, xfer, show) )
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* setMaxButtonsPerLine --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtButtonbar::setMaxButtonsPerLine( int buttons ){
  m_maxbuttons = buttons;
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* getButtonsPerLine --                                                        */
/* --------------------------------------------------------------------------- */

int GuiQtButtonbar::getButtonsPerLine( int &nbuttons ){
  if( nbuttons <= 0 ){
    return 0;
  }
  if( m_maxbuttons == 0 ){
    m_maxbuttons = nbuttons;
  }
  if( nbuttons < m_maxbuttons ){
    m_maxbuttons = nbuttons;
  }
  int lines = nbuttons / m_maxbuttons;
  int r = nbuttons % m_maxbuttons;
  if( r > 0 ){ lines++; }
  int b = nbuttons / lines;
  r = nbuttons % lines;
  if( r > 0 ){ b++; }
  nbuttons += - b;
  return b;
}

/* --------------------------------------------------------------------------- */
/* createButtonLine --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtButtonbar::createButtonLine( int buttons
                                     , GuiElementList::iterator &btn
                                     , int &width, int &height ){
  width  = 0;
  height = 0;

  for( int x=1; x<=buttons; x++ ){
    assert( btn != m_elements.end() );
    (*btn)->create();
    (*btn)->manage();
  }

}

QWidget *GuiQtButtonbar::myWidget(){
  return m_buttonbar;
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtButtonbar::getDialogExpandPolicy() {
  // soll sich im Dialog nicht mehr vergroessern
  return orient_Default;
}
