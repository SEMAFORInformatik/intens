
#include <qmenubar.h>
#include <qsettings.h>
#include <assert.h>

#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtManager.h"

#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtSeparator::GuiQtSeparator( GuiElement *parent )
  : GuiQtElement( parent )
  , m_sepwidget( 0 )
  , m_orientation( orient_Horizontal ){
}

GuiQtSeparator::GuiQtSeparator( const GuiQtSeparator &separator )
  : GuiQtElement( separator )
  , m_sepwidget( 0 )
  , m_orientation( separator.m_orientation ){
}

GuiQtSeparator::~GuiQtSeparator(){
}


/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtSeparator::create(){
   BUG(BugGui,"GuiQtSeparator::create");
   QMenu* menudata = 0;

   if ( getParent() )
     menudata = dynamic_cast<QMenu*>( getParent()->getQtElement()->myWidget() );

   if (menudata) {
     bool visible(true);
     if (GuiQtManager::Settings()) {
       visible = GuiQtManager::Settings()->value("Intens/menu.separator", true).toBool();
       if (!visible)
	 setHiddenFlag();
     }
     // menu item
     if (visible)
       menudata->addSeparator();
   } else {
     // separator line inside fieldgroup
     m_sepwidget = new QWidget();
     m_sepwidget->setAutoFillBackground(true);
     QPalette  pal = m_sepwidget->palette();
     pal.setColor( m_sepwidget->backgroundRole (), QColor("black") );
     m_sepwidget->setPalette( pal );
   }

   setOrientation( m_orientation );
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtSeparator::destroy(){
  if( m_sepwidget ){
    m_sepwidget->deleteLater();
    m_sepwidget = 0;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtSeparator::getSize( int &w, int &h ){
  if (m_orientation == orient_Vertical) {
	w = 2;
	h =0;
  } else {
	w = 0;
	h = 2;
  }
 BUG_DEBUG("getSize: " << myWidget() << "  ["<<w << ", " << h << "]");
}

QWidget* GuiQtSeparator::myWidget() {
  return m_sepwidget;
}

/* --------------------------------------------------------------------------- */
/* setOrientation --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtSeparator::setOrientation( GuiElement::Orientation orient ){
  m_orientation = orient;

if( m_sepwidget == 0 ) return;

  switch( m_orientation ){
  case orient_Default:
  case orient_Horizontal:
    m_sepwidget->setMinimumHeight(2);
    m_sepwidget->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    m_sepwidget->setObjectName( "GuiHorizontalSeparator" );
    break;
  case orient_Vertical:
    m_sepwidget->setMinimumWidth(2);
    m_sepwidget->setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_sepwidget->setObjectName( "GuiVerticalSeparator" );
    break;
  case orient_Both:
    m_sepwidget->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    m_sepwidget->setObjectName( "GuiSeparator" );
    break;
  }
}


/* --------------------------------------------------------------------------- */
/* serialize --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtSeparator::serializeXML( std::ostream &os, bool recursive) {
  GuiSeparator::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtSeparator::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return GuiSeparator::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtSeparator::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiSeparator::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getContainerExpandPolicy --                                                 */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtSeparator::getContainerExpandPolicy() {
  return GuiElement::orient_Default;
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtSeparator::getDialogExpandPolicy() {
  return GuiElement::orient_Default;
}
