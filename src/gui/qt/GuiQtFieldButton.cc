
#include <qpushbutton.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qstring.h>
#include <qapplication.h>
#include <QToolButton>
#include <QColorDialog>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMenu>
#include <QEnterEvent>

#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtFieldButton.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtIconManager.h"
#include "datapool/DataPoolDefinition.h"
#include "app/UserAttr.h"
#include "app/DataPoolIntens.h"
#include "job/JobFunction.h"
#include "job/JobManager.h"
#include "utils/utils.h"
#include "utils/StringUtils.h"
#include "utils/gettext.h"

INIT_LOGGER();

class MyQtFieldButton : public QPushButton {
public:
  MyQtFieldButton(GuiQtFieldButton* e, const QString &text, QWidget* parent)
    : QPushButton(text, parent), m_elem(e)
	, m_orientation(Qt::Horizontal)
	, m_mirrored(false)
  {
    setAutoDefault( false );
  }

  void enterEvent ( QEnterEvent *e )  {
    m_elem->enterEvent( e);
    QPushButton::enterEvent( e );
  }
  void leaveEvent ( QEvent *e )  {
    m_elem->clearMessage();
    QPushButton::leaveEvent( e );
  }
  virtual void focusInEvent ( QFocusEvent *e )  {
    setDefault( true );
    QPushButton::focusInEvent( e);
  }
  virtual void focusOutEvent ( QFocusEvent *e )  {
    setDefault( false );
    QPushButton::focusOutEvent( e);
  }

  void setOrientation(Qt::Orientation orientation)
  {
	m_orientation = orientation;
	switch (orientation)
	{
		case Qt::Horizontal:
			setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
			m_mirrored = m_elem->getRotation() == 180 ? true : false;
			break;

		case Qt::Vertical:
			setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
			m_mirrored = m_elem->getRotation() == 180 ? false : true;
			break;
	}
	updateGeometry();
	update();
  }

protected:
  Qt::Orientation orientation() const {
	return m_orientation;
  }

  bool mirrored() const {
	return m_mirrored;
  }

  void setMirrored(bool mirrored) {
	m_mirrored = mirrored;
  }

  QSize sizeHint() const {
	QSize size = QPushButton::sizeHint();
	if (m_orientation == Qt::Vertical)
	  size.transpose();
	return size;
  }

  QSize minimumSizeHint() const {
	QSize size = QPushButton::minimumSizeHint();
	/*if (m_orientation == Qt::Vertical)
	  size.transpose();*/
	return size;
  }

  QStyleOptionButton getStyleOption() const {
	QStyleOptionButton opt;
	opt.initFrom(this);
	if (m_orientation == Qt::Vertical)
	{
		QSize size = opt.rect.size();
		size.transpose();
		opt.rect.setSize(size);
	}
	opt.features = QStyleOptionButton::None;
	if (isFlat())
		opt.features |= QStyleOptionButton::Flat;
	if (menu())
		opt.features |= QStyleOptionButton::HasMenu;
	if (autoDefault() || isDefault())
		opt.features |= QStyleOptionButton::AutoDefaultButton;
	if (isDefault())
		opt.features |= QStyleOptionButton::DefaultButton;
	if (isDown() || (menu() && menu()->isVisible()))
		opt.state |= QStyle::State_Sunken;
	if (isChecked())
		opt.state |= QStyle::State_On;
	if (!isFlat() && !isDown())
		opt.state |= QStyle::State_Raised;
	opt.text = text();
	opt.icon = icon();
	opt.iconSize = iconSize();
	return opt;
  }
  void paintEvent(QPaintEvent *event) {
	Q_UNUSED(event);
	QStylePainter p(this);

	switch (m_orientation)
	{
		case Qt::Horizontal:
			if (m_mirrored)
			{
				p.rotate(180);
				p.translate(-width()+1, -height());
			}
			break;

		case Qt::Vertical:
			if (m_mirrored)
			{
				p.rotate(-90);
				p.translate(-height()+1, 0);
			}
			else
			{
				p.rotate(90);
				p.translate(0, -width());
			}
			break;
	}

	p.drawControl(QStyle::CE_PushButton, getStyleOption());
  }

private:
  GuiQtFieldButton *m_elem;
  bool              m_mirrored;
  Qt::Orientation   m_orientation;
};

class MyQtToolButton : public QToolButton {
public:
  MyQtToolButton(GuiQtFieldButton* e)
    : QToolButton(), m_elem(e) {
  }

  void enterEvent ( QEnterEvent *e )  {
    m_elem->enterEvent( e);
    QToolButton::enterEvent( e );
  }
  void leaveEvent ( QEvent *e )  {
    m_elem->clearMessage();
    QToolButton::leaveEvent( e );
  }
private:
  GuiQtFieldButton *m_elem;
};

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtFieldButton::GuiQtFieldButton( GuiElement *parent )
    : GuiQtDataField( parent )
    , m_buttonwidget( 0 )
    , m_label_pixmap( GuiElement::button_TEXT )
    , m_width_element( -1 )
    , m_height_element( -1 )
	, m_orientation( orient_Default )
    , m_length(0) {
  setAlignment( align_Stretch );
}

GuiQtFieldButton::GuiQtFieldButton( GuiQtFieldButton &button )
    : GuiQtDataField( button )
    , m_buttonwidget( 0 )
    , m_label( button.m_label )
    , m_pixmap_name( button.m_pixmap_name )
    , m_label_pixmap( button.m_label_pixmap )
    , m_width_element ( button.m_width_element  )
    , m_height_element( button.m_height_element )
	, m_orientation( button.m_orientation )
    , m_length( button.m_length ) {
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* addTabGroup --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::addTabGroup(){
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::create( ){
  BUG(BugGui,"GuiQtFieldButton::create");
  assert( m_buttonwidget == 0 );

  getAttributes();
  bool editable = isEditable() && (Attr()->getFunction() || Attr()->IsColorPicker());

  DataReference *data = m_param->DataItem()->Data();
  UserAttr *user_attr = m_param->getUserAttr();
  if (m_label_pixmap != GuiElement::button_TEXT) {
    m_buttonwidget = new MyQtToolButton(this);
    if (m_label_pixmap == GuiElement::button_TEXT_PIXMAP) {
      dynamic_cast<QToolButton*>(m_buttonwidget)->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
      if(m_label.empty()){
        m_label = QtMultiFontString::getQString(user_attr->Label(data)).toStdString();
      }
    }
  } else {
    QString label = QtMultiFontString::getQString( user_attr->Label( data ) );
    if ( Attr()->IsColorPicker() ) {
      label = label.trimmed();
    }
    m_buttonwidget = new MyQtFieldButton( this, label, getParent()->getQtElement()->myWidget() );
    dynamic_cast<MyQtFieldButton*>(m_buttonwidget)
      ->setOrientation(m_orientation!=orient_Vertical ? Qt::Horizontal: Qt::Vertical);
  }
  if (user_attr->Label(m_param->Data()) == m_param->Data()->nodeName())
    m_buttonwidget->setText( QString( m_param->Data()->nodeName().size(),' ') );
  else if (!m_label.empty())
    m_buttonwidget->setText(QString::fromStdString(m_label));
  m_buttonwidget->setEnabled( editable );

  // set Pixmap
  if (m_label_pixmap != GuiElement::button_TEXT) {
    QPixmap pm;

    QtIconManager::Instance().getPixmap(m_pixmap_name, pm);
    if (!pm.isNull() &&
        m_width_element > 0 && m_height_element > 0 ) {
      pm = pm.scaled(m_width_element, m_height_element);
    }
    m_buttonwidget->setIcon(QIcon(pm));
    if (m_label_pixmap == GuiElement::button_TEXT_PIXMAP) {
      m_buttonwidget->setText(QString::fromStdString(m_label));
    }
    m_buttonwidget->setIconSize(pm.size());
  }

  updateWidgetProperty();

  // set button font
  QFont font =  m_buttonwidget->font();
  m_buttonwidget->setFont( QtMultiFontString::getQFont( "@button@", font ) );

  // set button size
  setButtonSize();

  // set color
  setColors();

  if(Attr()->getFunction() || Attr()->IsColorPicker()) {
    QObject::connect( m_buttonwidget, SIGNAL(clicked()), this, SLOT( slot_activate()) );
  }
  QObject::connect( m_buttonwidget, SIGNAL(pressed()), this, SLOT( slot_pressed() )  );

  // Als letztes muessen die Indizes in der DataReference initialisiert werden.
  // Diese Arbeiten uebernimmt immer das DataItem.
  m_param->DataItem()->setDimensionIndizes();
  BUG_EXIT("DataReference " << m_param->DataItem()->Data() << ": "
            << m_param->DataItem()->Data()->fullName( true ));

  // color picker mode (muss hier am Ende gemacht werden)
  if (Attr()->IsColorPicker()) {
    int strlen = m_buttonwidget->fontMetrics().horizontalAdvance( m_buttonwidget->text() );
    QSize si(m_buttonwidget->size().width()*0.9-strlen, m_buttonwidget->size().height()*0.65);
    m_buttonwidget->setIconSize( si );
  }
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::getSize( int &w, int &h ){
  int height = 0;
  int width  = 0;
  if( m_buttonwidget != 0 ){
    width = m_buttonwidget -> width();
    height = m_buttonwidget -> height();
  }
  w=width;
  h=height;
}

/* --------------------------------------------------------------------------- */
/* stretchWidth --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::stretchWidth( int width ){
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::enable(){
  setDisabled(false);
  getAttributes();
  if ( m_buttonwidget)
    m_buttonwidget -> setEnabled( isEditable() );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::disable(){
  setDisabled(true);
  getAttributes();
  if ( m_buttonwidget)
    m_buttonwidget -> setDisabled( true );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* lock --                                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::lock(){
  BUG(BugGui,"GuiQtFieldButton::lock");
  if( m_buttonwidget == 0 ) return;

  getAttributes();
  if( !isLockable() ) return;

  //lockValue();
  updateForms( reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* unlock --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::unlock(){
  BUG(BugGui,"GuiQtFieldButton::unlock");
  if( m_buttonwidget == 0 ) return;

  getAttributes();
  if( !isLockable() ) return;

  //unlockValue();
  updateForms( reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* switchlock --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::switchlock(){
  BUG(BugGui,"GuiQtFieldButton::switchlock");
  if( m_buttonwidget == 0 ) return;

  getAttributes();
  if( !isLockable() ) return;

  if( isLocked() ){
    //unlockValue();
  }
  else{
    //lockValue();
  }
  updateForms( reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::update( UpdateReason reason ){
  BUG(BugGui,"GuiQtFieldButton::update");
  if( m_param == 0 || m_buttonwidget == 0 ) return;

  updateWidgetProperty();
  bool changed = getAttributes();

  switch( reason ){
  case reason_FieldInput:
  case reason_Process:
    break;

  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:
    setUpdated();
    break;
  default:
    BUG_MSG("Unhandled Update Reason");
    setUpdated();
    break;
  }

  if( isUpdated() ){
    if( useColorSet() ){
      changed = true;
    }
  }

  // evtl. den Wert als Label setzen
  UserAttr *attr =  m_param -> getUserAttr();
  if (attr->Label(m_param->Data()) == m_param->Data()->nodeName()) {
    std::string s;
    m_param->getFormattedValue( s );
    if (!m_label.empty()) s = m_label;
    QPushButton *btn = dynamic_cast<QPushButton*>(m_buttonwidget);
    if(m_label_pixmap == GuiElement::button_PIXMAP) {
      dynamic_cast<QToolButton*>(m_buttonwidget)->
        setToolButtonStyle(s.empty() ? Qt::ToolButtonIconOnly : Qt::ToolButtonFollowStyle);
    }
    if (!s.empty()) {
      if (btn) btn->setFlat(false);
      m_buttonwidget->setEnabled(true);
      m_buttonwidget->blockSignals(false);
      m_buttonwidget->setFocusPolicy(Qt::StrongFocus);
      m_buttonwidget->setText( QString::fromStdString(s) );
      m_buttonwidget->setObjectName( QString::fromStdString(s) );

	  // set button size
	  setButtonSize();
    } else if(m_label_pixmap == GuiElement::button_TEXT) {
      if (btn) btn->setFlat(true);
      m_buttonwidget->setEnabled(false);
      m_buttonwidget->blockSignals(true);
      m_buttonwidget->setFocusPolicy(Qt::NoFocus);
      m_buttonwidget->setText( QString::fromStdString(s) );
      m_buttonwidget->setObjectName("empty");
      // set button height to 0
      m_buttonwidget->setMinimumHeight( 0 );
      m_buttonwidget->setMaximumHeight( 0 );
    }
    else
      setButtonSize();
    changed = true;
  }

  // color picker mode
  if (Attr()->IsColorPicker()) {
	int strlen = m_buttonwidget->fontMetrics().horizontalAdvance( m_buttonwidget->text() );
	QPixmap pm(m_buttonwidget->iconSize().width(), m_buttonwidget->iconSize().height());
	std::string s;
	m_param->getFormattedValue( s );
	pm.fill(QColor(QString::fromStdString(s)));
 	if (!pm.isNull() && s.size()) {
	  m_buttonwidget->setIcon(QIcon(pm));
	}
  }


  // --------------------------------------------------------------
  // Falls keine Attribute (exl. Updated) geaendert haben, haben
  // wir hier nicht mehr zu tun.
  // --------------------------------------------------------------
  if( !changed ){
    return;
  }

  // --------------------------------------------------------------
  // Nun wird Editable neu gesetzt.
  // --------------------------------------------------------------
  if( Attr()->getFunction() || Attr()->IsColorPicker()) {
    m_buttonwidget -> setEnabled( isEditable() );
  }

  // --------------------------------------------------------------
  // Als letztes werden die Farben neu gesetzt.
  // --------------------------------------------------------------
  setColors();
}

/* --------------------------------------------------------------------------- */
/* setLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldButton::setLabel( const std::string &name ){
  m_label = name;
  m_label_pixmap = m_pixmap_name.empty() ? GuiElement::button_TEXT : GuiElement::button_TEXT_PIXMAP;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setPixmap --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldButton::setPixmap( const std::string &name, bool withLabel ){
  m_pixmap_name = name;
  m_label_pixmap = m_label.empty() ? GuiElement::button_PIXMAP : GuiElement::button_TEXT_PIXMAP;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setElementSize --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::setElementSize( int width, int height ){
  m_width_element = width;
  m_height_element = height;
}

/* --------------------------------------------------------------------------- */
/* CloneForFieldgroupTable --                                                  */
/* --------------------------------------------------------------------------- */

GuiQtDataField *GuiQtFieldButton::CloneForFieldgroupTable(){
  GuiQtFieldButton *button = new GuiQtFieldButton( *this );
  return button;
}

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::setColors(){
  BUG(BugGui,"GuiQtFieldButton::setColors");
  if (!m_buttonwidget) return;

  QColor background, foreground;
  int dark_fac;
  bool setColor = getTheColor( background, foreground, dark_fac );

  // if (!qApp->styleSheet().contains("QPushButton")) {
    if ( useColorSet() ||
         m_attr_mask & DATAcolor1 || m_attr_mask & DATAcolor2 || m_attr_mask & DATAcolor3 || m_attr_mask & DATAcolor4 ||
         m_attr_mask & DATAcolor5 || m_attr_mask & DATAcolor6 || m_attr_mask & DATAcolor7 || m_attr_mask & DATAcolor8   ) {
      QPalette pal=  m_buttonwidget->palette();
      pal.setColor(QPalette::Button,          background );
      pal.setColor(QPalette::ButtonText,      foreground );
      pal.setColor(m_buttonwidget->backgroundRole(), background );
      pal.setColor(m_buttonwidget->foregroundRole(), foreground );
      pal.setColor(QPalette::Disabled, QPalette::Button, background.darker(115) );
      pal.setColor(QPalette::Disabled, m_buttonwidget->backgroundRole(), background.darker(115) );
      pal.setColor(QPalette::Disabled, QPalette::Text, GuiQtManager::buttonTextColor().darker(115) );
      m_buttonwidget->setPalette(pal);
    } else {
      getAttributes();
      bool editable = isEditable() && (Attr()->getFunction() || Attr()->IsColorPicker());
      if (disabled() || !editable) {
        QPalette pal=  QApplication::palette();
        pal.setColor(QPalette::Button, GuiQtManager::backgroundColor().darker(115) );
        pal.setColor(m_buttonwidget->backgroundRole(), GuiQtManager::backgroundColor().darker(115) );
        pal.setColor(QPalette::Disabled, m_buttonwidget->foregroundRole(), GuiQtManager::buttonTextColor() );
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, GuiQtManager::buttonTextColor() );
        pal.setColor(QPalette::Disabled, QPalette::Text, GuiQtManager::buttonTextColor() );
        m_buttonwidget->setPalette(pal);
      } else {
        QPalette pal=  QApplication::palette();
        m_buttonwidget->setPalette(pal);
      }
    }
  // }

  // check if we need to set style sheet to update a previously set style sheet
  // TODO: If we have an application style sheet that sets QPushButton palette things,
  //       it would be better to just set an empty style sheet for setColor=false so that the
  //       application style sheets colors would be used
  //       But if we don't have one, we should use the palette colors...
  //       For the moment, we set the palette colors in the resource file even if you use style sheets!
  if ( !setColor ) {
    QString styleSheet = m_buttonwidget->styleSheet();
    if ( !styleSheet.isEmpty() ) {
      setColor = true;
    }
  }

  std::string css;
  if ( setColor ) {
    css = compose("QPushButton { background-color:%1; color: %2; }",
		  background.name().toStdString(),
		  foreground.name().toStdString());
    css += compose("\nQPushButton:disabled { background-color:%1; color: %2; }",
		   background.darker(dark_fac).name().toStdString(),
		   foreground.darker(dark_fac).name().toStdString());
  }
  const std::string& attrCss = GuiDataField::getStylesheet();
  if (css.size() || attrCss.size()) {
    QString sheet(QString::fromStdString(css + "\n" + attrCss));
    if(m_buttonwidget->styleSheet() != sheet) {
      m_buttonwidget->setStyleSheet(sheet);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* setButtonSize --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::setButtonSize(){
  int h = getButtonHeight();
  int w = getButtonWidth();

  // swap?
  if (m_orientation==orient_Vertical)
   	std::swap(w, h);

  // set widget size
  if (m_label_pixmap == GuiElement::button_TEXT) {
    if (m_orientation == orient_Vertical) {
      m_buttonwidget->setMinimumWidth( w );
      m_buttonwidget->setMaximumWidth( w );
      if (getLength() != GuiDataField::defaultFieldLength()){
        m_buttonwidget->setMinimumHeight( h );
        m_buttonwidget->setMaximumHeight( h );
      }
    }else{
      m_buttonwidget->setMinimumHeight( h );
      m_buttonwidget->setMaximumHeight( h );
      if (getLength() != GuiDataField::defaultFieldLength()){
        m_buttonwidget->setMinimumWidth( w );
        m_buttonwidget->setMaximumWidth( w );
      }
    }
  } else {
    QSize iconsize = m_buttonwidget->iconSize();
    QSize iconsh = m_buttonwidget->sizeHint();
    // fehlerhaftes Qt? :: bei gentoo 4.5.3 funktioniert es problemlos
    if (iconsh.width() < iconsize.width() || iconsh.height() < iconsize.height()) {
      iconsh = iconsize;
    }
    m_buttonwidget->setMinimumSize( iconsh );
    m_buttonwidget->setMaximumSize( iconsh );
  }
}
/* --------------------------------------------------------------------------- */
/* getButtonHeight --                                                          */
/* --------------------------------------------------------------------------- */

int GuiQtFieldButton::getButtonHeight(){
  // if height is set
  if ( m_height_element > 0 )
    return m_height_element;
  // else calculate
  double fac = 1.8;
  std::string::size_type pos=-1;
  while ( (pos=m_buttonwidget->text().toStdString().find('\n', pos+1)) != std::string::npos) {
    fac += 1.4;
  }
  QFont font =  m_buttonwidget->font();
  int h =  (int)floor(0.5 + (fac*QFontInfo(QtMultiFontString::getQFont( "@button@", font )).pixelSize()));
  return h;
}

/* --------------------------------------------------------------------------- */
/* getButtonWidth --                                                           */
/* --------------------------------------------------------------------------- */

int GuiQtFieldButton::getButtonWidth(){
  QString qstr((getLength() != GuiDataField::defaultFieldLength()
				? std::max((int)m_buttonwidget->text().size(), getLength()) : m_buttonwidget->text().size()), 'X');
  int w = 10 + m_buttonwidget->fontMetrics().horizontalAdvance( qstr );

  // set multiline length
  if (m_label_pixmap == GuiElement::button_TEXT) {
	std::string label = m_param->getUserAttr()->Label( m_param->DataItem()->Data() );
	int labelsize = label.size();

	std::string s;
	m_param->getFormattedValue( s );
	if (s.size()) label = s;

	// if multiline => get longest line
	std::vector<std::string> lines = split(label, "\\n");
	if (lines.size() > 1) { // multiLine
	  labelsize = 0;
	  for( std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
		if (labelsize < (*it).size())
		  labelsize = (*it).size();
	  }
	}

	// set Width
	int charlen = m_buttonwidget->fontMetrics().horizontalAdvance( 'X' );
	int len = getLength() > labelsize ? getLength() : labelsize;
	if (lines.size() > 1)
	  return len*charlen;
  }

  return w;
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldButton::setInputValue(){
  // Ein Button schreibt nicht direkt in den Datapool. Ohne Function ist
  // der Button sinnlos.
  return true;
}

/* --------------------------------------------------------------------------- */
/* slot_activate --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::slot_activate(){
  BUG(BugGui,"GuiQtFieldButton::slot_activate");

  // color picker mode: open selection color dialog
  if (Attr()->IsColorPicker()) {
	std::string s;
	m_param->getFormattedValue( s );

	QColorDialog dlg(myWidget());
	if (s.size())
	  dlg.setCurrentColor(QColor(QString::fromStdString(s)));
	if (dlg.exec() == QDialog::Accepted) {
	  QColor color = dlg.selectedColor();
	  m_param->setFormattedValue( color.name().toStdString() );
	}
  }

  if (isRunning()) {
    BUG_EXIT("funktion is running already");
    return;
  }

  clearMessage();
  doFinalWork();
}

/* --------------------------------------------------------------------------- */
/* slot_pressed --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::slot_pressed(){
  BUG(BugGui,"GuiQtFieldButton::slot_pressed");
  m_buttonwidget->setFocus();
  GuiQtManager::Instance().setWaitingWidget( true, m_buttonwidget );
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldButton::setLength( int len ){
  m_length = abs(len);
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtFieldButton::serializeXML(std::ostream &os, bool recursive){

  GuiQtDataField::serializeXML(os, recursive);
  return;

  os << "<intens:Button";
  XferDataItem *xfer = m_param->DataItem();
  DataReference *data = xfer->Data();
  UserAttr *attr = static_cast<UserAttr*>(data->getUserAttr());
  std::string label;
  if( !m_label.empty() )
    label = QtMultiFontString::getQString( m_label ).toStdString();
  else{
    label = QtMultiFontString::getQString( attr->Label( data ) ).toStdString();
  }

  const XferDataItem::DataItemIndexList& indexlist = xfer->getDataItemIndexList();
  std::string disabled = xfer->getFullName( indexlist );
  if (disabled.rfind('.') != std::string::npos)
    disabled.insert( disabled.rfind('.'),".disabled");
  else
    disabled.insert( 0,"disabled.");
  os << " disabled=\""<<disabled<<"\"";
  if( m_label_pixmap != GuiElement::button_TEXT){
    std::string::size_type pos;
    pos = label.rfind('/');
    if( pos != std::string::npos ){
      label = label.substr( ++pos );
    }
    os << " pixmap=\"true\"";
  }
  os << " label=\"" << label << "\"";



  JobFunction *func = attr->getFunction();
  if( func )
    os << " action=\"" << func->Name() << "\"";
  os << "/>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiQtFieldButton::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  jsonObj["pixmap"] = m_label_pixmap != GuiElement::button_TEXT;
  if (m_label_pixmap != GuiElement::button_TEXT) {
    IconManager::ICON_TYPE icon_type;
    std::string icon, file, content;
    if (!IconManager::Instance().getDataUrlFileContent(icon_type, m_pixmap_name, content)) {
      std::cerr << "icon '"<<icon<<"' not found.\n";
    } else {
      jsonObj["icon"] = content;
    }
  }
  return GuiQtDataField::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtFieldButton::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  auto element = eles->add_buttonfields();
  element->set_pixmap(m_label_pixmap != GuiElement::button_TEXT);
  if (m_label_pixmap != GuiElement::button_TEXT) {
    IconManager::ICON_TYPE icon_type;
    std::string icon, file, content;
    if (onlyUpdated || !IconManager::Instance().getDataUrlFileContent(icon_type, m_pixmap_name, content)) {
      element->set_icon("unchanged");
    } else {
      element->set_icon(content);
      element->mutable_field()->set_icon(content);
    }
  }
  return GuiQtDataField::serializeProtobuf(element->mutable_field(), onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getContainerExpandPolicy --                                                 */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtFieldButton::getContainerExpandPolicy() {
  return  GuiElement::orient_Default;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldButton::acceptIndex( const std::string &name, int inx ){
  BUG(BugGuiFld,"GuiQtFieldButton::acceptIndex");
  assert( m_param != 0 );
  assert( m_param -> DataItem() != 0 );
  if( !m_param->DataItem()->acceptIndex( name, inx ) ){
    return false;
  }
  if( !m_param->acceptIndex( name, inx ) ){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtFieldButton::setIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtFieldButton::setIndex",name << " =[" << inx << "]");
  assert( m_param != 0 );
  assert( inx >= 0 );

  m_param->DataItem()->setIndex( name, inx );
  update( reason_Always );
}

/* --------------------------------------------------------------------------- */
/* sgetExpandPolicy --                                                         */
/* --------------------------------------------------------------------------- */

Qt::Orientations GuiQtFieldButton::getExpandPolicy() {
  return Qt::Orientations();
}
