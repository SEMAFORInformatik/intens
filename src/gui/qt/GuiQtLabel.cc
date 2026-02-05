#include <sstream>
#include <qlabel.h>
#include <qfont.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qstringlist.h>
#include <QHBoxLayout>
#include <QVariant>
#include <QComboBox>

/* System headers */
#include "utils/Debugger.h"
#include "utils/HTMLConverter.h"
#include "app/UserAttr.h"

#include "gui/GuiManager.h"
#include "gui/UnitManager.h"
#include "gui/qt/GuiQtLabel.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtIconManager.h"

INIT_LOGGER();

/*=============================================================================*/
/* QVertLabel / Class                                                          */
/*=============================================================================*/
class QVertLabel : public QLabel
{
public:
  QVertLabel(GuiQtLabel* elem, QWidget *parent = 0)
	: QLabel(parent)
	, m_elem(elem) {
  }

  // makes layout manager happy
  QSize sizeHint() const {
    QSize s = QLabel::sizeHint();
    s.transpose();
    return s;
  }
  // makes layout manager happy
  QSize	minimumSizeHint() const {
    QSize s = QLabel::minimumSizeHint();
    s.transpose();
    return s;
  }
protected:
  void paintEvent(QPaintEvent *) {
    QSize s = sizeHint();
    QPainter painter(this);
    painter.setPen(Qt::black);
	if (m_elem->getRotation() == 180) {
	  painter.rotate(90);
	  painter.drawText(0,0, QLabel::text());
	} else {
	  painter.rotate(270);
	  painter.drawText(-s.height(),  s.width()-5, QLabel::text());
	}
  }
private:
  GuiQtLabel *m_elem;
};

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtLabel::GuiQtLabel( GuiElement *parent, std::string label )
  : GuiQtElement( parent )
  , m_labelwidget( 0 )
  , m_combobox( 0 )
  , m_label_pixmap( false )
  , m_indexed( false )
  , m_use_titlefont( false )
  , m_firstSeparatorPos( 0 )
  , m_orientation( orient_Default )
  , m_userattr(0)
{
  if (!label.empty())
    setLabel(label);
}

GuiQtLabel::~GuiQtLabel(){}

GuiQtLabel::GuiQtLabel( const GuiQtLabel &label )
  : GuiQtElement( label )
  , m_labelwidget( 0 )
  , m_combobox( 0 )
  , m_label( label.m_label )
  , m_label_pixmap( label.m_label_pixmap )
  , m_indexed( label.m_indexed )
  , m_use_titlefont( label.m_use_titlefont )
  , m_firstSeparatorPos( 0 )
  , m_orientation( label.m_orientation )
  , m_userattr(label.m_userattr)
  , m_userattr_fullname(label.m_userattr_fullname)
{
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtLabel::create( ){
  BUG(BugGui,"GuiLabel::create");
  QBoxLayout* _layout(0);

  QWidget *parent = getParent()->getQtElement()->myWidget();

  // handle different types
  std::string extractPreString, extractPostString;
  Json::Value valueObject = UnitManager::extractJsonObject(m_label,
                                                           extractPreString,
                                                           extractPostString);
  if (AppData::Instance().hasUnitManagerFeature() &&
      !valueObject.isNull()) {
    // pre, post string
    bool use_layout(!extractPreString.empty() || !extractPostString.empty());
    if (use_layout){
      m_labelwidget = new QWidget(); // parent );
      if (m_orientation == orient_Vertical)
        _layout = new QVBoxLayout;
      else
        _layout = new QHBoxLayout;
      _layout->setContentsMargins(0,0,0,0);
      _layout->setSpacing(0);
      m_labelwidget->setLayout( _layout );
    }

    // is a combobox
    int curIdx(0);
    std::string value(valueObject["value"].asString());
    void *pvoid;
    std::istringstream (valueObject["ptr_userattr"].asString()) >> pvoid;
    m_userattr = static_cast<UserAttr*>(pvoid);
    m_userattr_fullname = valueObject["userattr_fullName"].asString();

    m_combobox = new QComboBox();
    QFont font =  m_combobox->font();
    m_combobox->setMinimumContentsLength(value.size());
    int h =  4 + QFontMetrics(font).height();
    m_combobox->setMinimumSize( m_combobox->minimumSizeHint().width(), h );
    m_combobox->setMaximumSize( m_combobox->minimumSizeHint().width(), h );
    for (int i=0; i < valueObject["input"].size(); ++i) {
      if (valueObject["input"][i].isString()) {
        m_combobox->addItem(QString::fromStdString(valueObject["input"][i].asString()));
        if (valueObject["input"][i].asString() == value) curIdx = i;
      }
    }
    m_combobox->setCurrentIndex( curIdx );
    m_combobox->setFont( QtMultiFontString::getQFont( "@combobox@", font ) );
    connect(m_combobox,SIGNAL(currentTextChanged(const QString&)), this, SLOT(comboxBoxActivate(const QString&)));
    if (use_layout){
      if (!extractPreString.empty()){
        QLabel *l = m_orientation == orient_Vertical? new QVertLabel(this) : new QLabel();
        l->setText(QString::fromStdString(extractPreString));
        l->setAlignment( Qt::AlignBottom );
        _layout->addWidget(l);
      }
      _layout->addWidget(m_combobox);
      if (!extractPostString.empty()){
        QLabel *l = m_orientation == orient_Vertical? new QVertLabel(this) : new QLabel();
        l->setText(QString::fromStdString(extractPostString));
        l->setAlignment( Qt::AlignBottom );
        _layout->addWidget(l);
      }
    }else{
      m_labelwidget = m_combobox;
    }
  } else if( isMultiFont() ){
    // is multi font label
    m_labelwidget = new QWidget(); // parent );
    if (m_orientation == orient_Vertical)
      _layout = new QVBoxLayout;
    else
      _layout = new QHBoxLayout;
    _layout->setContentsMargins(0,0,0,0);
    _layout->setSpacing(0);
    m_labelwidget->setLayout( _layout );
    setLabelList();

    for(int i=0; i<m_labelList.size(); ++i){
      QLabel *l = m_orientation == orient_Vertical? new QVertLabel(this) : new QLabel();
      l->setAlignment( Qt::AlignBottom );

      _layout->addWidget( l, Qt::AlignRight );
      m_QLabelVec.push_back( l );
    }
    _layout->addStretch(1000);

    setLabelString( m_label );
    if (m_orientation == orient_Vertical) {
      m_labelwidget->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum) );
    } else {
      m_labelwidget->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed) );
    }
  } else if (m_orientation == orient_Vertical) {
    // is vertical label
    m_labelwidget = new QVertLabel(this);
    m_labelwidget->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum) );
  } else {
    // is horizontal label
    m_labelwidget = new QLabel();
    m_labelwidget->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed) );
    QLabel *wlabel = dynamic_cast<QLabel*>(m_labelwidget);
    if (wlabel) wlabel->setOpenExternalLinks(true);
  }
  if( m_label_pixmap && !m_label.empty()){
    QPixmap icon;
    if( QtIconManager::Instance().getPixmap( m_label, icon ) ){
      QLabel *wlabel = dynamic_cast<QLabel*>(m_labelwidget);
      if (wlabel) wlabel->setPixmap( icon );
    }
    else {
      setLabelString( m_label );
    }
  }
  else {
    setLabelString( m_label );
  }

  updateWidgetProperty();
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtLabel::destroy(){
  if( m_labelwidget != 0 ){
    m_labelwidget->deleteLater();
    m_labelwidget = 0;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtLabel::update( UpdateReason reason ){
  if (AppData::Instance().hasUnitManagerFeature() &&
      m_combobox && reason == reason_Unit) {
    std::string value(UnitManager::extractValue(m_label));
    std::string valueAttr(m_userattr->Unit(false));
    if (value != valueAttr) {
      value = valueAttr;
    }
    if (!value.empty()){
      std::string s(UnitManager::Instance().getComboBoxData(value, m_userattr));
      if (m_label == s) {
        return;  // no changes
      }
      m_label = s;
      Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(s);
      for (int i=valueObject["input"].size(); i < m_combobox->count(); ++i) {
        m_combobox->removeItem(i);
      }
      int curIdx(m_combobox->currentIndex());
      for (int i=0; i < valueObject["input"].size(); ++i) {
        if (valueObject["input"][i].isString()) {
          if (i < m_combobox->count()){
            m_combobox->setItemText(i, QString::fromStdString(valueObject["input"][i].asString()));
          }else{
            m_combobox->addItem(QString::fromStdString(valueObject["input"][i].asString()));
          }
        }
        if (valueObject["input"][i].asString() == value) curIdx = i;
      }
      if (curIdx != m_combobox->currentIndex()){
        m_combobox->setCurrentIndex( curIdx );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtLabel::myWidget() {
  return m_labelwidget;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtLabel::acceptIndex( const std::string &name, int inx ){
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtLabel::setIndex( const std::string &name, int inx ){
  if( !m_indexed ){
    return;
  }
  std::ostringstream text;
  text << inx;
  setLabelString( text.str() );
}

/* --------------------------------------------------------------------------- */
/* setLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQtLabel::setLabel( const std::string &label ){
  BUG(BugGui,"GuiLabel::setLabel(label)");
  m_label = label;
  m_label_pixmap = false;
  setLabelString( label );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setPixmap --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtLabel::setPixmap( const std::string &name, bool withLabel ){
  BUG(BugGui,"GuiLabel::setPixmap(label)");
  m_label = name;
  m_label_pixmap = true;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtLabel::setTitle( const std::string &label ){
  BUG(BugGui,"GuiLabel::setTitle(label)");
  m_label = label;
  m_use_titlefont = true;
  setLabelString( label );
}

/* --------------------------------------------------------------------------- */
/* setSimpleLabel --                                                           */
/* --------------------------------------------------------------------------- */

std::string GuiQtLabel::getSimpleLabel(){
  return QtMultiFontString::getQString( m_label ).toStdString();
}

/* --------------------------------------------------------------------------- */
/* setLabelString --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtLabel::setLabelString( const std::string &label ){
  BUG(BugGui,"GuiLabel::setLabelString(label)");
  if( m_labelwidget == 0 && m_QLabelVec.size()==0){
    return;
  }
  QFont font = m_labelwidget->font();
  m_labelwidget->setFont( QtMultiFontString::getQFont( "@label@", font ) );

  if( isMultiFont() ){
    std::vector<QLabel *>::iterator IqLabel = m_QLabelVec.begin();
    QStringList::Iterator Ilabel = m_labelList.begin();
    QFont font;
    for( ; IqLabel != m_QLabelVec.end(); ++IqLabel ){
      if(Ilabel!=m_labelList.end()){
        (*IqLabel)->setText( QtMultiFontString::getQString( (*Ilabel).toStdString() ) );
        font = m_labelwidget->font();
        (*IqLabel)->setFont( QtMultiFontString::getQFont( (*Ilabel).toStdString(), font ) );
        if( isMultiLine( (*Ilabel) ) )
          (*IqLabel)->setAlignment( getQtAlignment(getAlignment() ) );
        Ilabel++;
      }
    }
    return;
  }

  if( m_labelwidget == 0 ) {
    return;
  }
  QLabel *wlabel = dynamic_cast<QLabel*>(m_labelwidget);
  if (wlabel) wlabel->setText( QtMultiFontString::getQString( label ));

  if( isMultiLine(QString::fromStdString(label)) )
    if (wlabel) wlabel->setAlignment( getQtAlignment(getAlignment() ) );

  if( m_indexed ){
    m_labelwidget->setFont( QtMultiFontString::getQFont( "@index@", font ) );
    return;
  }
  if( m_use_titlefont ){
    m_labelwidget->setFont( QtMultiFontString::getQFont( "@title@", font ) );
    return;
  }
  return;
}

/* --------------------------------------------------------------------------- */
/* isMultiFont --                                                              */
/* --------------------------------------------------------------------------- */
bool GuiQtLabel::isMultiFont(){
  QString atSign("\\@");
  if( (m_firstSeparatorPos = QString::fromStdString(m_label).indexOf(atSign) ) > -1 )
    return true;
  return false;
}

/* -----------------------------------------------------an---------------------- */
/* isMultiLine --                                                              */
/* --------------------------------------------------------------------------- */
bool GuiQtLabel::isMultiLine( const QString &s ){
  QString newlineSign("\\\\n");
  if( s.indexOf(newlineSign) > -1 ){
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* comboxBoxActivate --                                                        */
/* --------------------------------------------------------------------------- */
void GuiQtLabel::comboxBoxActivate(const QString& str){
  QComboBox *combobox = qobject_cast<QComboBox *>(sender());
  BUG_DEBUG("GuiQtLabel::comboxBoxActivate str: " << str.toStdString());
  UnitManager::Instance().update_unit(m_userattr, str.toStdString(), m_userattr_fullname);
}

/* --------------------------------------------------------------------------- */
/* setLabelList --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiQtLabel::setLabelList(){
  QStringList labelSplit = QString::fromStdString(m_label).split("@");
  QStringList::Iterator I = labelSplit.begin();

  if( m_firstSeparatorPos > 0 ){
    m_labelList << *I;
    ++I;
  }
  QString tmpLabel("");
  for ( ;I != labelSplit.end(); ++I ) {
    if ((*I).isEmpty()) continue;
    tmpLabel = "@" + (*I) + "@";
    if ( ++I == labelSplit.end() )
      break;
    tmpLabel += (*I);
    m_labelList << tmpLabel;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* serialize --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtLabel::serializeXML( std::ostream &os, bool recursive ){
  return GuiLabel::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtLabel::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiLabel::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtLabel::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiLabel::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtLabel::getContainerExpandPolicy() {
  return  GuiElement::orient_Default;
}
