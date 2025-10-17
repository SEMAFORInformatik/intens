
#include <qstatusbar.h>
#include <qmainwindow.h>

#include "utils/Debugger.h"

#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtMessageLine.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/gettext.h"

INIT_LOGGER();

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtMessageLine::create() {
  BUG(BugGui,"GuiQtMessageLine::create");
  QWidget *pw = getParent()->getQtElement()->myWidget();

  m_statusbarwidget = new QStatusBar();
  m_statusbarwidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  QObject::connect( m_statusbarwidget, SIGNAL(messageChanged(const QString&)), this, SLOT(slotMessageChanged(const QString&)) );
  if (getParent()->getQtElement()->getDialogExpandPolicy() == 0)
    m_statusbarwidget->setSizeGripEnabled( false );

  // set font
  QFont font = m_statusbarwidget->font();
  m_statusbarwidget->setFont( QtMultiFontString::getQFont( "@messageLine@", font ) );
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtMessageLine::destroy(){
  if( m_statusbarwidget != 0 ){
    m_statusbarwidget->deleteLater();
    m_statusbarwidget = 0;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtMessageLine::getSize( int &w, int &h ){
  w = h = 0;
  if (!m_statusbarwidget) return;
  h = m_statusbarwidget->height();
  w = m_statusbarwidget->width();
  QSize hs = m_statusbarwidget->sizeHint();
  BUG_DEBUG("getSize w: " << w << ", h:" << h << "  hintSize: " << hs.width() << ", " << hs.height());
  h = hs.height() < h ? h : hs.height();
  w = hs.width()  < w ? w : hs.width();
  w = std::min(w, 500); // max 500
  BUG_DEBUG("getSize w: " << w << ", h:" << h);
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtMessageLine::update( UpdateReason reason ){
  if( reason == reason_Cycle ){
    ResetLastWebUpdated();  // reason_Always for webtens
    clearMessage();
  }
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtMessageLine::myWidget() {
  return m_statusbarwidget;
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtMessageLine::manage() {
  if ( m_statusbarwidget->isHidden() )
    m_statusbarwidget->show();
}

/* --------------------------------------------------------------------------- */
/* printMessage --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtMessageLine::printMessage( const std::string &msg, MessageType type, time_t delay ){
  if ( m_statusbarwidget == 0 ) return;
  QPalette  pal = m_statusbarwidget->palette();

  switch(type){

  case msg_Error:
    pal.setColor( QPalette::WindowText, GuiQtManager::alarmForegroundColor() );
    pal.setColor( QPalette::Window, GuiQtManager::alarmBackgroundColor() );
    break;

  case msg_Warning:
    pal.setColor( QPalette::WindowText, GuiQtManager::warnForegroundColor() );
    pal.setColor( QPalette::Window, GuiQtManager::warnBackgroundColor() );
    break;

  case msg_Information:
    pal.setColor( QPalette::WindowText, GuiQtManager::infoForegroundColor() );
    pal.setColor( QPalette::Window, GuiQtManager::infoBackgroundColor() );
    break;

  case msg_Help:
    pal.setColor( QPalette::WindowText, GuiQtManager::helpForegroundColor() );
    pal.setColor( QPalette::Window, GuiQtManager::helpBackgroundColor() );
    pal.setColor( m_statusbarwidget->foregroundRole(),      GuiQtManager::helpForegroundColor() );
    pal.setColor( m_statusbarwidget->backgroundRole(),      GuiQtManager::helpBackgroundColor() );
    break;
  }
  m_statusbarwidget->setPalette(pal);

  // set status message
  //  m_statusbarwidget->clearMessage();
  int duration = delay > 0 ? delay*1000 : AppData::Instance().ToolTipDuration();
  m_statusbarwidget->showMessage( QString::fromStdString(msg), duration );
  m_statusbarwidget->update();

  // set tooltip (like a status history)
  QString tmsg( m_statusbarwidget->toolTip().size() ? m_statusbarwidget->toolTip() + "\n" + m_statusbarwidget->currentMessage() :  m_statusbarwidget->currentMessage());
  QStringList tmsgList(tmsg.split('\n'));
  if (tmsgList.size() > 10)  tmsgList.pop_front();
#if QT_VERSION > 0x050200
  m_statusbarwidget->setToolTipDuration(AppData::Instance().ToolTipDuration());
#endif
  m_statusbarwidget->setToolTip(tmsgList.join("\n"));

  // set font
  QFont font = m_statusbarwidget->font();
  m_statusbarwidget->setFont( QtMultiFontString::getQFont( "@messageLine@", font ) );
}

/* --------------------------------------------------------------------------- */
/* clearMessage --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtMessageLine::clearMessage(){
  if ( m_statusbarwidget ) {
    m_statusbarwidget->clearMessage();
  }
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtMessageLine::serializeXML(std::ostream &os, bool recursive){
  GuiMessageLine::serializeXML(os, recursive);
}


/* --------------------------------------------------------------------------- */
/* serializeJSON --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtMessageLine::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiMessageLine::serializeJson(jsonObj, onlyUpdated);
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtMessageLine::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiMessageLine::serializeProtobuf(eles, onlyUpdated);
}
#endif


/* --------------------------------------------------------------------------- */
/* slotMessageChanged --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtMessageLine::slotMessageChanged(const QString &message) {
  if (message.length() == 0) {
    QPalette  pal = m_statusbarwidget->palette();
    //     m_statusbarwidget->setPaletteForegroundColor( GuiQtManager::foregroundColor() );
//     pal.setColor( QPalette::Window, GuiQtManager::backgroundColor() );
    m_statusbarwidget->setPalette( pal );
  }
}
