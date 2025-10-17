
#include <qwidget.h>
#include <qlabel.h>
#include <qsettings.h>
#include <qlayout.h>

#include "utils/utils.h"
#include "utils/gettext.h"

#include "gui/qt/GuiQtApplHeader.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQtManager.h"
#include "app/AppData.h"


/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtApplHeader::create(){
  BUG(BugGui,"GuiApplHeader::create");

  QWidget *pw = getParent()->getQtElement()->myWidget();
  assert(pw != 0);

  m_myWidget = new QFrame( pw );
  m_myWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  QHBoxLayout* hlayout = new QHBoxLayout();
  hlayout->setContentsMargins(0,0,0,0);

  // -- Left Icon --------------------------
  QLabel *l1 = new QLabel( "" );
  hlayout->addWidget( l1, 0, Qt::AlignLeft );

  QPixmap pix1;
  if( QtIconManager::Instance().getPixmap( AppData::Instance().LeftTitleIcon(), pix1 ) )
    l1->setPixmap( pix1 );
  else
    if( QtIconManager::Instance().getPixmap( "semafor", pix1 ) )
      l1->setPixmap( pix1 );

  // -- TITLE ------------------------------
  QWidget *titBox = new QWidget();
  hlayout->addWidget( titBox, 0, Qt::AlignCenter );
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(0,0,0,0);

  // get locale domain from description
  if (GuiQtManager::Settings()) {
    const char *domain=AppData::Instance().LocaleDomain().c_str();
    QString s=GuiQtManager::Settings()->value
      ( "Intens/apptitle.text", "INTENS").toString();
    QLabel *l2 = new QLabel( dgettext( domain, s.toStdString().c_str() ) );
    m_title = l2->text().toStdString();
    // set list font title
    QFont font =  l2->font();
    l2->setFont( QtMultiFontString::getQFont( "@appTitle@", font ) );
    layout->addWidget(l2, 0, Qt::AlignCenter);

    // -- sub TITTLE ------------------------------
    s=GuiQtManager::Settings()->value
      ( "Intens/appsubtitle.text", "").toString();
    if (s.size()) {
      QLabel *l3 = new QLabel( dgettext( domain, s.toStdString().c_str() ) );
      m_subtitle = l3->text().toStdString();
      // set list font subtitle
      font =  l3->font();
      l3->setFont( QtMultiFontString::getQFont( "@subTitle@", font ) );
      layout->addWidget(l3, 0, Qt::AlignCenter);
    }
  }
  titBox->setLayout(layout);

  // -- Right Icon -------------------------
  QLabel *l4 = new QLabel( "" );
  hlayout->addWidget( l4, 0, Qt::AlignRight );
  QPixmap pix2;
  if( QtIconManager::Instance().getPixmap( AppData::Instance().RightTitleIcon(), pix2 ) )
    l4->setPixmap( pix2 );
//   else
//     if( QtIconManager::Instance().getPixmap( "bombardier", pix2 ) )
//       l4->setPixmap( pix2 );

  m_myWidget->setLayout(hlayout);

  // evtl. doppelt aber hier gehört er hin
  if( !AppData::Instance().AppTitlebar() ){
    unmap();
  }
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtApplHeader::manage(){
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtApplHeader::unmanage(){
}

/* --------------------------------------------------------------------------- */
/* map --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtApplHeader::map(){
  if (m_myWidget)
    m_myWidget->show();
  GuiToggleListener::setToggleStatus( true );
}

/* --------------------------------------------------------------------------- */
/* unmap --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtApplHeader::unmap(){
  if (m_myWidget)
    m_myWidget->hide();
  GuiToggleListener::setToggleStatus( false );
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtApplHeader::getSize( int &w, int &h ){
  QSize  hs = m_myWidget->sizeHint();
  h = hs.height();
  w = hs.width();
  m_myWidget->resize(w, h);
}

/* --------------------------------------------------------------------------- */
/* ToggleStatusChanged --                                                      */
/* --------------------------------------------------------------------------- */
void GuiQtApplHeader::ToggleStatusChanged( bool pressed ){
  if( pressed )
    map();
  else
    unmap();
}

/* --------------------------------------------------------------------------- */
/* getElement --                                                               */
/* --------------------------------------------------------------------------- */
GuiElement *GuiQtApplHeader::getElement(){
  return this;
}

/* --------------------------------------------------------------------------- */
/* getExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */
Qt::Orientations GuiQtApplHeader::getExpandPolicy() {
  return m_myWidget->sizePolicy().expandingDirections();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtApplHeader::serializeXML(std::ostream &os, bool recursive){
  GuiApplHeader::serializeXML(os, recursive);
//   os << "<GuiApplHeader>" << endl;
//   os << "</GuiApplHeader>"<< endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJSON --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtApplHeader::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiApplHeader::serializeJson(jsonObj, onlyUpdated);
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtApplHeader::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiApplHeader::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtApplHeader::getDialogExpandPolicy() {
  return orient_Default;
}
