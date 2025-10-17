
/* qt headers */
#include <qpushbutton.h>
#include <qstring.h>

/* System headers */
#include "utils/Debugger.h"

#include "gui/qt/GuiQtButton.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiEventData.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtButton::GuiQtButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event )
  : GuiQtElement( parent ), GuiButton( listener, event )
    , m_buttonwidget( 0 )
{
#if defined(GUI_STANDALONE_TEST)
  m_helptext = "GuiQtButton";
#endif
}

GuiQtButton::~GuiQtButton(){
  delete m_buttonwidget;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* addTabGroup --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtButton::addTabGroup(){
}

/* --------------------------------------------------------------------------- */
/* removeTabGroup --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtButton::removeTabGroup(){
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtButton::create( ){
  BUG(BugGui,"GuiQtButton::create");
//   assert( m_buttonwidget == 0 );
  m_buttonwidget = new QPushButton( getParent()->getQtElement()->myWidget() );
  m_buttonwidget->setAutoDefault( false );

#if 0
  if( m_label_pixmap && !m_label.empty() ){
    QtIconManager &iconMgr = QtIconManager::Instance();
//     Pixel background;

    QPixmap pixmap;
    if( iconMgr.getPixmap( m_label, pixmap ) ){
      std::string nameX = m_label + "_arm";
      if( iconMgr.getPixmap( nameX, pixmap ) ){
      }
      nameX = m_label + "_insensitive";
      if( iconMgr.getPixmap( nameX, pixmap ) ){
      }
    }
    else{
      m_label_pixmap = false;
    }
  }
#else
      m_label_pixmap = false;
#endif
  if( !m_label_pixmap ){
    QString qs = QString::fromStdString(m_label);
    m_buttonwidget -> setText( qs );
  }
  //  XtOverrideTranslations
  //  ( m_convertwidget->xtWidget()
  //  , XtParseTranslationTable( "<Key>Return: ArmAndActivate()")
  //  );

  // set button font
  QFont font =  m_buttonwidget->font();
  m_buttonwidget->setFont( QtMultiFontString::getQFont( "@button@", font ) );
  int h =  (int)floor(0.5 + (1.8*QFontInfo(QtMultiFontString::getQFont( "@button@", font )).pixelSize()));
  m_buttonwidget->setMinimumHeight( h );
  m_buttonwidget->setMaximumHeight( h );
  m_buttonwidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed) );

  if( m_listener != 0 ){
    QObject::connect( m_buttonwidget, SIGNAL(clicked()), this, SLOT( activate() ) );
    // sollte gutes GUI ABER selber machen (Focus setzen nachdem Element angeklickt wurde)
    QObject::connect( m_buttonwidget, SIGNAL(pressed()), m_buttonwidget, SLOT( setFocus() )  );

    m_buttonwidget -> setEnabled( m_listener->sensitive() );
  }
  else{
    m_buttonwidget -> setEnabled( false );
  }
}


/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtButton::activate(){
  GuiButton::Activate();
}
/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtButton::destroy(){
  BUG(BugGui,"GuiQtButton::destroy");
  m_buttonwidget->deleteLater();
  m_buttonwidget = 0;
  return true;
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtButton::manage(){
  if ( m_buttonwidget->isHidden() )
    m_buttonwidget->show();
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtButton::enable(){
  if( m_buttonwidget != 0 && m_listener != 0 ){
    m_buttonwidget -> setEnabled( true );
  }
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtButton::disable(){
  if( m_buttonwidget != 0 ){
    if( m_buttonwidget->isActiveWindow() )
       m_buttonwidget->setEnabled( false );
  }
}

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtButton::serializeXML(std::ostream &os, bool recursive){

  GuiButton::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtButton::serializeJson(Json::Value& jsonObj, bool onlyUpdated){

  return GuiButton::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtButton::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiButton::serializeProtobuf(eles, onlyUpdated);
}
#endif

QWidget *GuiQtButton::myWidget(){
  return m_buttonwidget;
}
