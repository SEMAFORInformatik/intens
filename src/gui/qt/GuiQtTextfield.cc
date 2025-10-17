
#include <algorithm>
#include <sstream>
#include <qcursor.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qapplication.h>
#include <qdialog.h>
#include <QMouseEvent>
#include <QLabel>
#include <QStackedWidget>
#include <QComboBox>
#include <QCalendarWidget>
#include <QListView>

#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "utils/JsonUtils.h"
#include "utils/JsonException.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtTextfield.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/KNumValidator.h"
#include "gui/qt/ArrowKeyLineEdit.h"
#include "gui/qt/QtIconManager.h"
#include "gui/GuiLabel.h"
#include "gui/UnitManager.h"
#include "job/JobFunction.h"
#include "app/DataPoolIntens.h"
#include "app/DataSet.h"
#include "xfer/XferParameterReal.h"
#include <jsoncpp/json/json.h>
#ifdef Q_OS_LINUX
#include <linux/version.h>
#endif

INIT_LOGGER();

bool GuiQtTextfield::m_arrowKeyAllowed = true;
int  GuiQtTextfield::m_oldCursorPos = 0;

#ifdef __USE_GUITIMETABLE__
/*=============================================================================*/
/* intern MyQCalMonthLookup class for popup a month calendar dialog            */
/*=============================================================================*/
const unsigned int nHorOffset = 14;
const unsigned int s_dCaptionHeight = 20;
class MyQCalMonthLookup : public QCalMonthLookup {
public:
  MyQCalMonthLookup(QDialog* parent) : QCalMonthLookup(parent), m_dialog(parent) {
    QFont font =  QWidget::font();
    font.setPointSize( 8 );
    setFont( font );
    resize(180, 140);
  }
private:
  virtual void keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape ) {
      QDate d;
      setDate(d);
    }
    QCalMonthLookup::keyPressEvent(e);
  }
  virtual void mousePressEvent(QMouseEvent* thisEvent) {
    lastClick = QTime::currentTime();
    QCalMonthLookup::mousePressEvent( thisEvent );
  }
  virtual void mouseDoubleClickEvent(QMouseEvent* thisEvent) {
    QTime currTime = QTime::currentTime();
    if (lastClick.msecsTo(currTime) > 300) {
      QCalMonthLookup::mouseDoubleClickEvent( thisEvent );
      return;
    }
    float x_dist = 1.0f * (this->width() - 2 * nHorOffset) / 7;
    float y_dist = 1.0f * (this->height() - 3 * s_dCaptionHeight) / 6;
    int mX = (int)(thisEvent->pos().x() - nHorOffset);
    int mY = (int)(thisEvent->pos().y() - 2.0f * s_dCaptionHeight);

    // sake of consistence, col starts at 1, row at 0, see paintEvent for details
    unsigned int col = (unsigned int)(1.0f + 1.0f * mX/*thisEvent->pos().x()*/ / x_dist);
    unsigned int row = (unsigned int)(1.0f * mY/*(thisEvent->pos().y() - 2 * s_dCaptionHeight)*/ / y_dist);

    if (mY < 0 || mX < 0 || col < 1 || col > 7 || row < 0 || row > 5) {
      return;
    }
    m_dialog->close();
  }
  QDialog *m_dialog;
  QTime lastClick;
};
#endif


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtTextfield::GuiQtTextfield( GuiElement *parent )
  : GuiQtDataField( parent )
    , m_length( 8 )
    , m_initialLength( true )
    , m_textwidget( 0 )
    , m_lineEdit( 0 )
    , m_dateEdit( 0 )
    , m_combobox( 0 )
    , m_widgetStack( 0 )
    , m_relCursorPos( 0 )
    , m_textChanged( false )
    , m_invertLabelAlignment( false ) {
}
GuiQtTextfield::GuiQtTextfield( const GuiQtTextfield &textfield )
  : GuiQtDataField( textfield )
    , m_length( textfield.m_length )
    , m_initialLength( textfield.m_initialLength )
    , m_textwidget( 0 )
    , m_lineEdit( 0 )
    , m_dateEdit( 0 )
    , m_combobox( 0 )
    , m_widgetStack( 0 )
    , m_relCursorPos( 0 )
    , m_textChanged( false )
    , m_invertLabelAlignment( textfield. m_invertLabelAlignment ) {
}

QWidget* GuiQtTextfield::myWidget() {
  return m_widgetStack ? m_widgetStack : m_textwidget;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* installDataItem --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiQtTextfield::installDataItem( XferDataItem *dataitem ){
  if( GuiQtDataField::installDataItem( dataitem ) ){
    if( dataitem->getDataType() != DataDictionary::type_String &&
	dataitem->getDataType() != DataDictionary::type_CharData ){
      m_param->setLength( m_length );
    }
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* replaceDataItem --                                                          */
/* --------------------------------------------------------------------------- */

XferDataItem *GuiQtTextfield::replaceDataItem( XferDataItem *dataitem ){
  XferDataItem *old_item = GuiQtDataField::replaceDataItem( dataitem );
  if( old_item != 0 ){
    if( dataitem->getDataType() != DataDictionary::type_String &&
	dataitem->getDataType() != DataDictionary::type_CharData ){
      m_param->setLength( m_length );
    }
  }
  return old_item;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::create() {
  BUG_DEBUG("GuiQtTextfield::create(" << m_param->DataItem()->getFullName(false) << ")");
  assert( m_textwidget == 0 );

  QWidget *parent = getParent()->getQtElement()->myWidget();

  // create stacked widget
  m_widgetStack = new QStackedWidget(parent);
  parent = m_widgetStack;

  assert( parent != 0 );
  getAttributes();  // vergiss ja nicht diese Funktion aufzurufen!!!
  if (m_param->DataItem()->getUserAttr()->StringType() == UserAttr::string_kind_date ||
      m_param->DataItem()->getUserAttr()->StringType() == UserAttr::string_kind_time ||
      m_param->DataItem()->getUserAttr()->StringType() == UserAttr::string_kind_datetime ) {
    m_dateEdit = createDateWidget();
    m_widgetStack->addWidget( m_dateEdit );
    m_textwidget = m_dateEdit;
  }
  else {
    if( isTypeLabel() ) {
      m_textwidget = new QLabel();
      m_textwidget->setFocusPolicy( Qt::NoFocus );
      m_widgetStack->addWidget( m_textwidget );
    }
    else {
      m_lineEdit = new ArrowKeyLineEdit( parent, this );
      m_textwidget = m_lineEdit;
      // placeholder text
      std::string phStr = m_param->DataItem()->getUserAttr()->Placeholder();
      if (phStr.size()) {
        m_lineEdit->setPlaceholderText(QString::fromStdString(phStr));
      }
      m_widgetStack->addWidget( m_lineEdit );
      if (m_param->DataItem()->getUserAttr()->StringType() == UserAttr::string_kind_password)
        m_lineEdit -> setEchoMode(QLineEdit::Password);
    }
  }
  m_widgetStack->setCurrentIndex( 0 );

  updateWidgetProperty();

  // set font
  QFont default_font = m_textwidget->font();
  BUG_DEBUG("*** Get Font: " << QtMultiFontString::printFont(default_font) << " ***");

  bool available;
  m_css_name = m_param->DataItem()->getFullName(false);
  QFont font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font, &available );
  if( !available ){
    BUG_DEBUG("Font for " << m_css_name << " is NOT available");
    m_css_name = m_param->DataItem()->getName();
    font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font, &available );
    if( !available ){
      BUG_DEBUG("Font for " << m_css_name << " is NOT available");
      m_css_name = isTypeLabel() ? "label" : "text";
      font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font );
    }
  }

  BUG_DEBUG("*** Set Font: " << QtMultiFontString::printFont(font) << " ***");
  m_textwidget->setFont(font);

  // for stylesheet settings
  m_textwidget->setObjectName( "GuiTextfield" );

  // unit feature: set scale from unit manager
  if (AppData::Instance().UnitManagerFeature()) {
    UnitManager::Unit* unit;
    unit = UnitManager::Instance().getUnitData(m_param->DataItem()->getUserAttr()->Unit(false));
    if (unit) {
      setScalefactor(new Scale(unit->factor, (unit->use_divide ? '/' : '*'), unit->shift));
    }
  }

  bool editable = isEditable();

  if (m_lineEdit) {
    m_lineEdit->setReadOnly( !isEditable() );
    if ( getDataType() != DataDictionary::type_String &&
         getDataType() != DataDictionary::type_CharData )
      m_lineEdit->setMaxLength( m_length );
    if (AppData::Instance().GuiWithRangeCheck()) {
      setValidator( m_lineEdit );
    }

    // set Alignment
    if ( getDataType() == DataDictionary::type_String ||
         getDataType() == DataDictionary::type_CharData )
      m_lineEdit->setAlignment( m_invertLabelAlignment ? Qt::AlignRight : Qt::AlignLeft );
    else
      m_lineEdit->setAlignment( m_invertLabelAlignment ? Qt::AlignLeft : Qt::AlignRight );
  } else {
    if (!isTypeLabel()) // TypeLabel nicht disablen, da er sonst Schatten erhält
      m_textwidget->setEnabled( isEditable() );
  }

  // groesse setzen
  int charlen = m_textwidget->fontMetrics().horizontalAdvance( '0' );
  int delta = floor(0.5+1.5*m_textwidget->fontMetrics().horizontalAdvance( '0' ));
  int h = 5 + QFontMetrics(font).height();
  if (m_lineEdit || (isTypeLabel() && !m_initialLength))  {
    m_textwidget->setMinimumSize( delta+charlen*m_length, h );
    m_widgetStack->setMinimumSize( delta+charlen*m_length, h );

    m_textwidget->setMaximumSize( delta+charlen*m_length, h );
    m_widgetStack->setMaximumSize( delta+charlen*m_length, h );
  } else {
    //    delta += charlen; // increase DateEdits
    MyQDateEdit *de = dynamic_cast<MyQDateEdit*>(m_dateEdit);
    if (de) {
      m_length =std::max(m_length, (int) de->displayFormat().size());
      delta += 8; // increase DateEdits
      de->setMinimumWidth( delta+charlen*m_length );
      de->setMaximumWidth( delta+charlen*m_length );
      h+=2;
      m_textwidget->setMinimumHeight( h );
      m_textwidget->setMaximumHeight( h );
    }
    MyQTimeEdit *te = dynamic_cast<MyQTimeEdit*>(m_dateEdit);
    if (te) {
      m_length =std::max(m_length, (int) te->displayFormat().size());
      delta += 8; // increase DateEdits
      te->setMinimumWidth( delta+charlen*m_length );
      te->setMaximumWidth( delta+charlen*m_length );
      h+=2;
      m_textwidget->setMinimumHeight( h );
      m_textwidget->setMaximumHeight( h );
    }
    MyQDateTimeEdit *dte = dynamic_cast<MyQDateTimeEdit*>(m_dateEdit);
    if (dte) {
      m_length =std::max(m_length, (int) dte->displayFormat().size());
      delta += 8; // increase DateEdits
      dte->setMinimumWidth( delta+charlen*m_length );
      dte->setMaximumWidth( delta+charlen*m_length );
      h+=2;
      m_textwidget->setMinimumHeight( h );
      m_textwidget->setMaximumHeight( h );
    }
  }

  m_textwidget->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
  m_widgetStack->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );

  if( isLockable() ){
    editable = true;
  }

  if (!editable)
    m_textwidget->setFocusPolicy( Qt::NoFocus );

  if (m_lineEdit) {
    connect( m_lineEdit, SIGNAL(editingFinished()), this, SLOT(lostFocus()) );
    connect( m_lineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(textEdited(const QString&)) );
  } else {
    UserAttr::STRINGtype dtype = m_param->DataItem()->getUserAttr()->StringType();
    switch(dtype) {
    case UserAttr::string_kind_date:
       connect( m_dateEdit, SIGNAL(editingFinished()),
  	       this, SLOT(dateChanged()) );
       connect( static_cast<MyQDateEdit*>(m_dateEdit)->lineEdit(),
  	       SIGNAL(selectionChanged()), this, SLOT(dateSelectionChanged()));
#if QT_VERSION >= 0x040400 // erst ab 4.4
       connect( static_cast<MyQDateEdit*>(m_dateEdit)->calendarWidget(),
		SIGNAL(clicked(const QDate&)), this, SLOT(dateSelected(const QDate&)));
       static_cast<MyQDateEdit*>(m_dateEdit)->calendarWidget()->setSelectedDate( QDate::currentDate() );
#endif
      break;
    case UserAttr::string_kind_time:
      connect( m_dateEdit, SIGNAL(editingFinished()),
	       this, SLOT(timeChanged()) );
      break;
    case UserAttr::string_kind_datetime:
      connect( m_dateEdit, SIGNAL(editingFinished()),
	       this, SLOT(datetimeChanged()) );
      break;
    default:
      break;
    }
  }

  setMyColors();
  setColors();

  // Als letztes muessen die Indizes in der DataReference initialisiert werden.
  // Diese Arbeiten uebernimmt immer das DataItem.
  m_param->DataItem()->setDimensionIndizes();

  BUG_DEBUG("End GuiQtTextfield::create");
  return;
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtTextfield::destroy(){
  BUG(BugGui,"GuiQtTextfield::destroy");
  if( m_textwidget != 0 ){
    m_textwidget->deleteLater();
    m_textwidget = 0;
    return true;
  }
  BUG_EXIT("no widget");
  return false;
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::enable(){
  setDisabled(false);
  if( m_textwidget == 0 ) return;

  getAttributes();
  if( !isTypeLabel() ) {
    if (m_lineEdit) {
      m_lineEdit->setReadOnly( !isEditable() );
    } else m_textwidget->setEnabled( isEditable() );
  }
  if (m_combobox) m_combobox->setEnabled( true );
  setColors();
  m_textwidget->setFocusPolicy( Qt::StrongFocus );
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::disable(){
  setDisabled(true);
  if( m_textwidget == 0 ) return;

  getAttributes();
  if( !isTypeLabel() )
    if (m_lineEdit) {
      m_lineEdit->setReadOnly( true );
    }
  if (m_combobox) m_combobox->setEnabled( false );
  setColors();
  m_textwidget->setFocusPolicy( Qt::NoFocus );
  if (m_dateEdit) {
    MyQDateEdit *de = dynamic_cast<MyQDateEdit*>(m_dateEdit);
    if (de) de->setReadOnly( true );
    MyQDateTimeEdit *dte = dynamic_cast<MyQDateTimeEdit*>(m_dateEdit);
    if (dte) dte->setReadOnly( true );
    MyQTimeEdit *te = dynamic_cast<MyQTimeEdit*>(m_dateEdit);
    if (te) te->setReadOnly( true );
  }
}

/* --------------------------------------------------------------------------- */
/* protect --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::protect(){
  BUG(BugGui,"GuiQtTextfield::protect");
  assert(disabled());
  if (m_lineEdit) {
    m_lineEdit->setReadOnly( true );
  }
  if (m_combobox) m_combobox->setEnabled( false );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::update( UpdateReason reason ){
  BUG(BugGui,"GuiQtTextfield::update");
  if( m_param == 0 || m_textwidget == 0 )
    return;

  updateWidgetProperty();
  bool changed = getAttributes();
  switch( reason ){
  case reason_Unit:
    if (updateScale(m_param))
      setUpdated();
    break;
  case reason_FieldInput:
  case reason_Process:
    break;

  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:
    setUpdated();
    changed = true;  // to make sure the color is set
    break;
  default:
    BUG_MSG("Unhandled Update Reason");
    setUpdated();
    break;
  }

  // --------------------------------------------------------------
  // Zuerst wird nur der Wert geprueft.
  // --------------------------------------------------------------
  if( isUpdated() ){
    std::string text;
    bool nullText(false);

    // special case (type Label and DataSet is used)
    if( isTypeLabel() && Attr()->DataSetName().size()) {
      getValue( text );
      if ( text.size() ) {
        std::string input;
        if(DataItem()->getDataSetInputValue(input, text)) {
          text = input;
        }
      }
    } else
      m_param->getFormattedValue( text );
    BUG_MSG("Value of " << m_param->getName() << " is '" << text << "'");

    // json-value ==object==> [ComboBox]
    Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(text);
    if (!valueObject.isNull()) {
      if (valueObject["value"].isString())
        text = valueObject["value"].asString();
      else {
        std::ostringstream os;
        if (valueObject["value"].isInt() )
          os << valueObject["value"].asInt();
        else if (valueObject["value"].isDouble() )
          os << valueObject["value"].asDouble();
        else if (valueObject["value"].isNull())
          nullText= true;
        else {
          std::cerr << "Textfield '"<< m_param->getName()
                    <<"' ERROR: could not handle this jsonObject '"<<text<<"'\n";
          return;
        }
        text = os.str();
      }

      // create
      if (!m_combobox) {
        m_combobox = new MyQComboBox(this);
        QFont font =  m_combobox->font();
        m_combobox->setFont( QtMultiFontString::getQFont( "@combobox@", font ) );
        connect(m_combobox,SIGNAL(activated(const QString&)), this, SLOT(comboxBoxActivate(const QString&)));
      } else m_combobox->clear();

      // add combox items
      int curIdx = 0;
      bool iconAdded(false);
      for (int i=0; i < std::max(valueObject["input"].size(),
                                 valueObject["inputPixmap"].size()); ++i) {
        std::ostringstream os;
        // input label
        if (valueObject["input"][i].isString())
          os << valueObject["input"][i].asString();
        else if (valueObject["input"][i].isInt())
          os << valueObject["input"][i].asInt();
        else if (valueObject["input"][i].isDouble())
          os << valueObject["input"][i].asDouble();
        else if (valueObject["input"][i].isNull())
          os << "";

        // input pixmap and label
        if (valueObject["inputPixmap"][i].isString()) {
          QPixmap icon;
          if( QtIconManager::Instance().getPixmap(valueObject["inputPixmap"][i].asString(), icon ) ){
            m_combobox->addItem(icon, QString::fromStdString(os.str()));
            if (!iconAdded) {
              m_combobox->setIconSize(icon.size());
              iconAdded = true;
            }
          }
        }
        // input only label
        else {
          m_combobox->addItem(QString::fromStdString(os.str()));
        }

        // output, get current index
        if (valueObject["output"][i].isString()) {
          if (valueObject["output"][i].asString() == text)
            curIdx=i;
        } else if (valueObject["output"][i].isInt()) {
          if (valueObject["output"][i].asInt() == QString::fromStdString(text).toInt())
            curIdx=i;
        } else if (valueObject["output"][i].isDouble()) {
          if (valueObject["output"][i].asDouble() == QString::fromStdString(text).toDouble())
            curIdx=i;
        } else if (valueObject["output"][i].isNull()) {
          if (nullText)
            curIdx=i;
        }
      }

      // set current index
      m_combobox->setCurrentIndex( curIdx );
      m_widgetStack->addWidget( m_combobox );
      m_widgetStack->setCurrentWidget( m_combobox );
    } else {

      // set new value to simple widget [QLineEdit, QDate*, QLabel]
      if (m_lineEdit) {
        setCursor(m_lineEdit, text,  m_relCursorPos);
        if ( (getDataType() == DataDictionary::type_String ||
              getDataType() == DataDictionary::type_CharData ) &&
             m_length < text.size())
          m_lineEdit->setCursorPosition(0);
      } else {
        if( isTypeLabel() ) {
          dynamic_cast<QLabel*>(m_textwidget)->setText( QString::fromStdString(text).trimmed() );
          if ( m_initialLength ) {
            int charlen = m_textwidget->fontMetrics().horizontalAdvance( '0' );
            int delta = floor(0.5+1.5*m_textwidget->fontMetrics().horizontalAdvance( '0' ));
            m_textwidget->setMaximumWidth( delta+charlen*text.size() );
            m_widgetStack->setMaximumWidth( delta+charlen*text.size() );
          }
        } else
          if (m_dateEdit)
            setDate(QString::fromStdString(text).trimmed().toStdString(), Qt::ISODate);
      }

      m_widgetStack->setCurrentIndex( 0 );
    }

    m_textChanged = false;

    if( useColorSet() ){
      changed = true;
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
  // Nun wird Editable und TraversalOn neu gesetzt.
  // --------------------------------------------------------------
  if (m_lineEdit) {
    if ( m_lineEdit->isReadOnly() == isEditable() ) {
      m_lineEdit->setReadOnly( !isEditable() );
    }
  } else
    if ( !isTypeLabel() && m_textwidget->isEnabled() != isEditable() ) // TypeLabel nicht disablen, da er sonst Schatten erhält
      m_textwidget->setEnabled( isEditable() );

  if (isEditable())
    m_textwidget->setFocusPolicy( Qt::StrongFocus );
  else
    m_textwidget->setFocusPolicy( Qt::NoFocus );
  if (m_combobox) m_combobox->setEnabled( isEditable() );

  // --------------------------------------------------------------
  // Als letztes werden die Farben neu gesetzt.
  // --------------------------------------------------------------
  setColors();

  // set font
  QFont font =  m_textwidget->font();
  BUG_DEBUG("*** Get Current Font: " << QtMultiFontString::printFont(font) << " ***");
  QFont new_font = QtMultiFontString::getQFont( "@" + m_css_name + "@", font );
  if( font != new_font ){
    BUG_DEBUG("*** Set New Font: " << QtMultiFontString::printFont(new_font) << " ***");
    m_textwidget->setFont( new_font );
  }
}

/* --------------------------------------------------------------------------- */
/* setCursor --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::setCursor(QLineEdit* lineEdit, std::string& text, int old_relCursorPos) {
  int diff = 0;

  // hat nicht den Focus => das geht schnell
  if (!lineEdit->hasFocus()) {
    if (lineEdit->maxLength() < text.size())
      lineEdit->setMaxLength( text.size() );
    lineEdit->setText( QString::fromStdString(text).trimmed() );
    return;
  }

  // Ist ein ungueltiger Wert => alte Einstellungen speichern und raus (und auf naechsten gueltigen Wert warten)
  // !!! sollte nie passieren, aber passiert in Verbindung mit Socketverbindungen
  QString decPt( KIntValidator::decimalSymbol() );
  if (QString::fromStdString(text).trimmed().length()==0) {
    m_arrowKeyAllowed = false;
    m_oldCursorPos = old_relCursorPos; //diff;
    lineEdit->setText( "" ); // leer
    return;
  } else {
    if (!m_arrowKeyAllowed) {
      // mal wieder gueltig => alte Werte holen
      m_arrowKeyAllowed = true;
      diff =  m_oldCursorPos;
    } else {
      int cur_pos =  lineEdit->cursorPosition();
      int ppos =   lineEdit->text().indexOf(decPt);
      if (ppos<0)
	ppos= lineEdit->text().length();
      diff = cur_pos - ppos;
    }
  }

  // den Cursor an die richtige Stelle setzen
  bool selecAll=false;
  if (lineEdit->text().size() && lineEdit->selectedText() ==  lineEdit->text()) selecAll=true;
  if (lineEdit->maxLength() < text.size())
    lineEdit->setMaxLength( text.size() );
  lineEdit->setText( QString::fromStdString(text).trimmed() );

  int pposNew =   lineEdit->text().indexOf(decPt);
  if (pposNew<0) // imagine decPt after the end
    pposNew = lineEdit->text().length();
  pposNew += diff;
  if (pposNew<0) {
    pposNew=0;
    m_arrowKeyAllowed = false;
  }
  if (pposNew > lineEdit->text().length()) {
    pposNew = lineEdit->text().length();
    m_arrowKeyAllowed = false;
  }
  if (m_arrowKeyAllowed)
    lineEdit->setCursorPosition(pposNew);
  else {
    m_arrowKeyAllowed = false;
    m_oldCursorPos = old_relCursorPos; //diff;
//    std::cerr << "!!!!!!!!!!!!! GuiQtTextfield internal error (Cursor)!!!!!!!!!!!!!!!!!\n"<<std::flush;
//     printMessage( _("GuiQtTextfield internal error."),
// 		  GuiElement::msg_Warning );
  }
  if (selecAll) lineEdit->selectAll();
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtTextfield::setLength( int len ){
  if (m_length != len)
    m_initialLength = false;
  if( true /*len != 0*/ ){ // becourse of hidden lines length can be 0
    m_length = abs(len);
  }
  if (len < 0)
    m_invertLabelAlignment = true;

  if( getDataType() == DataDictionary::type_String ||
      getDataType() == DataDictionary::type_CharData ){
    return true;
  }
  if( len != 0 ){
    return m_param->setLength( len );
  }
  return m_param->setLength( m_length );
}

/* --------------------------------------------------------------------------- */
/* setPrecision --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtTextfield::setPrecision( int prec ){
  return m_param->setPrecision( prec );
}

/* --------------------------------------------------------------------------- */
/* getPrecision --                                                             */
/* --------------------------------------------------------------------------- */

int GuiQtTextfield::getPrecision(){
  if (getDataType() == DataDictionary::type_Real)
    return m_param->getPrecision();
  return -1;
}

/* --------------------------------------------------------------------------- */
/* setThousandSep --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtTextfield::setThousandSep(){
  return m_param->setThousandSep();
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtTextfield::setScalefactor( Scale *scale ){
  UnitManager::Unit* unit = UnitManager::Instance().getUnitData(m_param->DataItem()->getUserAttr()->Unit(false));
  double factor = unit ? unit->factor : 1.;
  BUG_DEBUG("Varname["<<m_param->DataItem()->getFullName(true)<<"] Unit["
            <<m_param->DataItem()->getUserAttr()->Unit()<<"] Factor["
            << factor << "]");
  return m_param->setScalefactor( scale );
}

/* --------------------------------------------------------------------------- */
/* getScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

Scale* GuiQtTextfield::getScalefactor(){
  return m_param->getScalefactor();
}

/* --------------------------------------------------------------------------- */
/* CloneForFieldgroupTable --                                                  */
/* --------------------------------------------------------------------------- */

GuiQtDataField *GuiQtTextfield::CloneForFieldgroupTable(){
  GuiQtTextfield *txt = new GuiQtTextfield( *this );
  return txt;
}

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::setColors(){
  BUG_DEBUG("Begin setColors");

  QColor background, foreground;
  int dark_fac;
  bool setColor = getTheColor( background, foreground, dark_fac );
  bool setColorForce(false);
  bool appQssContains = GuiQtManager::stylesheetContains("QLineEdit");
  BUG_DEBUG("- Background=" << background.darker(100).name().toStdString() <<
            "  Foreground=" << foreground.darker(100).name().toStdString() <<
            "  setColor=" << setColor);

  if ( !setColor ) {
    QString styleSheet = m_textwidget->styleSheet();
    if ( !styleSheet.isEmpty() ) {
      BUG_DEBUG("- Text Style: " << styleSheet.toStdString());
      setColorForce = true;
    }
    else{
      BUG_DEBUG("- Text Style: empty");
    }
  }
  ///  setColor = true;
  // if (!qApp->styleSheet().contains("QLineEdit")) {
  QPalette pal=  QApplication::palette();
  // override qt default colors
  if( isTypeLabel() ) {
    pal.setColor( QPalette::WindowText,
                  foreground );
    pal.setColor( QPalette::Window,
                  background );
  }
  else {
    // for combobox
    pal.setColor(QPalette::ButtonText, foreground);
    pal.setColor(QPalette::Button, background);

    pal.setColor( QPalette::Base,
                  background );
    pal.setColor( QPalette::Text,
                  foreground );
    pal.setColor( m_textwidget->backgroundRole(),
                  background );
    pal.setColor( m_textwidget->foregroundRole(),
                  foreground );
    pal.setColor( QPalette::Disabled,
                  m_textwidget->backgroundRole(),
                  background.darker(115) );
    pal.setColor( QPalette::Disabled,
                  QPalette::Base,
                  background.darker(115) );
  }
  if (!appQssContains) {
    m_textwidget->setPalette(pal);
    m_widgetStack->setPalette(pal);
  }

  // check if we need to set style sheet to update a previously set style sheet
  // TODO: If we have an application style sheet that sets QLineEdit palette things,
  //       it would be better to just set an empty style sheet for setColor=false so that the
  //       application style sheets colors would be used
  //       But if we don't have one, we should use the palette colors...
  //       For the moment, we set the palette colors in the resource file even if you use style sheets!

  std::string css;
  if ( setColor || setColorForce) {
    BUG_DEBUG("- set color");
    std::string fontcss( QtMultiFontString::getCss( m_css_name ) );
    css = compose("QLineEdit { background:%1; color: %2; %3}",
                  background.name().toStdString(),
                  foreground.name().toStdString(),
                  fontcss);
    css += compose("\nQLineEdit:read-only { background-color:%1; color: %2;  %3}",
                   background.darker(dark_fac).name().toStdString(),
                   foreground.darker(dark_fac).name().toStdString(),
                   fontcss);
    if (m_combobox) {
      css += compose("\nQComboBox { background:%1; color: %2;  %3}",
                     background.name().toStdString(),
                     foreground.name().toStdString(),
                     fontcss);
      css += compose("\nQComboBox:read-only { background-color:%1; color: %2;  %3}",
                     background.darker(dark_fac).name().toStdString(),
                     foreground.darker(dark_fac).name().toStdString(),
                     fontcss);
    }
    BUG_DEBUG("- set Stylesheet(" << css << ")");
  }
  const std::string& attrCss = GuiDataField::getStylesheet();
  if( attrCss.size() ){
    BUG_DEBUG("- add Stylesheet(" << attrCss << ")");
  }

  if (css.size() || attrCss.size()) {
    QString sheet(QString::fromStdString(((appQssContains && !setColor) ? "" : css) +
                                         "\n" + attrCss));
    if(m_textwidget->styleSheet() != sheet) {
      m_textwidget->setStyleSheet( sheet );
    }
    if(m_widgetStack->styleSheet() != sheet) {
      m_widgetStack->setStyleSheet( sheet );
    }
  }
  BUG_DEBUG("End setColors");
}

/* --------------------------------------------------------------------------- */
/* setAlarmColors --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::setAlarmColors(){
  // ---------------------------------------------------------------------
  // Dieses Attribute sorgt dafuer, dass beim naechsten update() die Alarm-
  // Color wieder verschwindet. Dieses Bit wird nur lokal in diesem Objekt
  // gesetzt und landet nicht im Datapool.
  // ---------------------------------------------------------------------
  setAttribute( DATAcolorAlarm );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* checkFormatPrivate --                                                       */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus  GuiQtTextfield::checkFormatPrivate( std::string newValue ){
  BUG(BugGuiFld,"GuiQtTextfield::checkFormat");

  // look for invalid endline(s)
  if (newValue.find('\n') != std::string::npos) {
    std::string newValueTrimmed = QString::fromStdString(newValue).trimmed().toStdString();
    // return status_Bad if newline is inside string
    if ( newValueTrimmed == newValue || newValueTrimmed.find('\n') != std::string::npos)
      return XferParameter::status_Bad;
  }
  if (m_lineEdit) {
    return m_param->checkFormat( newValue );
  } else {
    return m_param->checkFormat( newValue );
  }
  return XferParameter::status_Bad;
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtTextfield::setInputValue(){
  BUG_DEBUG("Begin setInputValue()");
  std::string text;
  int wsId = m_widgetStack ? m_widgetStack->currentIndex() : 0;
  if (wsId > 0) {
    // json object input field
    if (dynamic_cast<QComboBox*>(m_widgetStack->currentWidget())) {
      text = getCurrentJsonObjectComboBox();
    }
    else
      std::cerr << "should never happen: unknow widget type:"<<std::endl;
  } else {
    // normal input field
    if (m_lineEdit) {
      text = m_lineEdit->text().toStdString();
      // remove leading or trailing endlines
      text = QString::fromStdString(text).trimmed().toStdString();
    } else {
      if( isTypeLabel() )
        dynamic_cast<QLabel*>(m_textwidget)->setText( QString::fromStdString(text) );
      else
        getDate( text, Qt::ISODate );
    }
  }
  BUG_DEBUG("Text = '" << text << "'");

  std::string x;
  m_param->getFormattedValue( x );

  if( m_param->isEmpty( text ) ){
    m_param->clear();
    BUG_DEBUG("End setInputValue(): clear");
    return true;
  }
  if( m_param->setFormattedValue( text ) ){
    BUG_DEBUG("End setInputValue(): value '" << text << "'");
    return true;
  }
  BUG_DEBUG("End setInputValue(): setFormattedValue failed");
  return false;
}

/* --------------------------------------------------------------------------- */
/* FinalWork --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtTextfield::FinalWork( const std::string newValue ){
  BUG_DEBUG("Begin FinalWork()");

  if( isRunning() ){
    // Falls noch eine Function aktiv ist, sind keine Eingaben moeglich. Diese
    // Erfahrung machen nur schnelle Finger.
    // strange error with xgettext here?
    // printMessage( _("a function is active."), msg_Warning );
     update( reason_Cancel );
     if(disabled()){
       protectField();
     }
     BUG_DEBUG("End FinalWork(): a function is active");
     return;
  }

  switch( checkFormatPrivate( newValue ) ){
  case XferParameter::status_Bad:
     printMessage( _("Conversion error."), msg_Warning );
     update( reason_Cancel );
     setAlarmColors();
     if(disabled()){
       protectField();
     }
     BUG_DEBUG("End FinalWork(): checkFormat failed");
     return;

  case XferParameter::status_Unchanged:
     clearMessage();
     update( reason_Cancel );
     if(disabled()){
       protectField();
     }
     BUG_DEBUG("End FinalWork(): checkFormat unchanged");
     return;

   case XferParameter::status_Changed:
     clearMessage();
     std::string error_msg;
     s_TargetsAreCleared = m_param->DataItem()->StreamTargetsAreCleared(error_msg);
     if( s_TargetsAreCleared ){ // Targets sind ok
       FinalWorkOk();
       BUG_DEBUG("End FinalWork(): changed - targets ok");
       return;
     }
     confirm(error_msg);
     BUG_DEBUG("End FinalWork(): changed");
     return;
  }
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::confirmYesButtonPressed(){
  FinalWorkOk();
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::confirmNoButtonPressed(){
  update( reason_Cancel );
  s_DialogIsAktive = false;
}

/* --------------------------------------------------------------------------- */
/* FinalWorkOk --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::FinalWorkOk(){
  BUG(BugGuiFld,"GuiQtTextfield::FinalWorkOk");
  doFinalWork();
  s_DialogIsAktive = false;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::serializeXML( std::ostream &os, bool recursive ){
  os << "<intens:Textfield";
  JobFunction *func = Attr()->getFunction();
  GuiQtDataField::serializeXML(os, recursive);
  os << "</intens:Textfield>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtTextfield::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  bool updated = GuiQtDataField::serializeJson(jsonObj, onlyUpdated);
  if (m_lineEdit && updated) {
    jsonObj["gui_value"] = m_lineEdit->text().toStdString();
  }
  return updated;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtTextfield::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_data_fields();
  bool updated = GuiQtDataField::serializeProtobuf(element, onlyUpdated);
  return updated;
}
#endif

/* --------------------------------------------------------------------------- */
/* Qt slot setText --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::textEdited( const QString& str ){
  BUG_DEBUG("textEdited");
  m_textChanged = true;
}

/* --------------------------------------------------------------------------- */
/* Qt slot lostFocus --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::lostFocus() {
  BUG_DEBUG("Begin lostFocus()");
  std::string formattedtext;
  std::string text;

  m_arrowKeyAllowed = true; // arrow key wieder erlaubt
  m_relCursorPos = 0; // auch wieder auf 0 gesetzt
  m_param->getFormattedValue( formattedtext );
  if (m_lineEdit){
    text = m_lineEdit->text().toStdString();
  }
  else{
    getDate( text, Qt::ISODate );
  }

  if( !m_textChanged || formattedtext == text){
     // Keine Aenderungen im Textfield. Der Grund des LosingFocus ist
     // nicht von Bedeutung.
    clearMessage();
    if(disabled()){
      protectField();
    }
    BUG_DEBUG("Text not modified");
  }
  else{
    BUG_DEBUG("Text = '" << text << "'");
    FinalWork( text );
  }
  BUG_DEBUG("End lostFocus()");
}

/* --------------------------------------------------------------------------- */
/* Qt slot dateSelectionChanged --                                             */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::dateSelectionChanged() {
  QLineEdit *le = static_cast<MyQDateEdit*>(m_dateEdit)->lineEdit();
  QChar deli = static_cast<MyQDateEdit*>(m_dateEdit)->getDelimeter();
  if (le && le->hasSelectedText()) {
    QString s = le->selectedText();
    if ( s.count(deli) == 1 ||
	 (s.count(deli) == 2 && s != le->text()) ) {
      int pos = s.indexOf(deli);
      le->setSelection(le->selectionStart(), pos);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getCurrentJsonObjectComboBox --                                             */
/* --------------------------------------------------------------------------- */

std::string GuiQtTextfield::getCurrentJsonObjectComboBox() {
  if (!m_combobox)
    return "";

  std::string formatedtext;
  m_param->getFormattedValue( formatedtext );
  Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(formatedtext);
  if (valueObject.isNull()) {
    return "";
  }

  if ( valueObject.isMember("inputPixmap") ) {
    // combobox text may not be unique
    valueObject["value"] = valueObject["output"][m_combobox->currentIndex()];

  } else if ( valueObject.isMember("input") && valueObject["input"].isArray() &&
              valueObject.isMember("output") && valueObject["output"].isArray() ) {
    // why not always 'if' code (above)?
    std::string text = m_combobox->currentText().toStdString();
    for(unsigned int i=0; i<valueObject["input"].size(); ++i) {
      if ( valueObject["input"][i] == text ) {
        valueObject["value"] = valueObject["output"][i];
        break;
      }
    }
  }
  return ch_semafor_intens::JsonUtils::value2string(valueObject);
}

void GuiQtTextfield::comboxBoxActivate(const QString& str){
  QComboBox *combobox = qobject_cast<QComboBox *>(sender());

  BUG_PARA(BugGuiFld,"GuiQtTextfield::comboxBoxActivate",this);
  std::string formatedtext;
  std::string text = getCurrentJsonObjectComboBox();
  m_param->getFormattedValue( formatedtext );

  if( formatedtext == text){
    // Keine Aenderungen im Textfield. Der Grund des LosingFocus ist
    // nicht von Bedeutung.
    clearMessage();

    if (disabled()){
      protectField();
    }
    BUG_EXIT("Text not modified");
    return;
  }

  FinalWork( text );
}

/* --------------------------------------------------------------------------- */
/* Qt slot dateSelectied --                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtTextfield::dateSelected(const QDate& date) {
  m_textChanged = true;
  QString df = static_cast<MyQDateEdit*>(m_dateEdit)->getDisplayFormat();
  static_cast<MyQDateEdit*>(m_dateEdit)->lineEdit()->setText(date.toString(df));
  FinalWork(date.toString(df).toStdString());  // fix for Event Bug
}

/* --------------------------------------------------------------------------- */
/* Qt slot dateChanged --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQtTextfield::dateChanged() {
  BUG_PARA(BugGuiFld,"GuiQtTextfield::dateChanged",this);
  QDate newdate = static_cast<QDateEdit*>(m_dateEdit)->date();
  // other format if inside  edit mode
  if (!newdate.isValid() && m_dateEdit->hasFocus())
    static_cast<MyQDateEdit*>(m_dateEdit)->setEditMode();
  m_textChanged = true;
  if (m_dateEdit->hasFocus()
#if QT_VERSION >= 0x040400 // erst ab 4.4
      || static_cast<MyQDateEdit*>(m_dateEdit)->calendarWidget()->isVisible()
#endif
      ) {
    return;
  }

  std::string formatedtext;
  std::string text;
  m_param->getFormattedValue( formatedtext );
  if (newdate.isValid() && !newdate.toString(Qt::ISODate).isNull())
    text = newdate.toString(Qt::ISODate).toStdString();
  if( newdate.isValid() && (formatedtext == text) ){
     // Keine Aenderungen im Textfield. Der Grund des LosingFocus ist
     // nicht von Bedeutung.
    if(disabled()){
      protectField();
    }
    BUG_EXIT("Text not modified");
    return;
  }

  FinalWork( text );
}

void GuiQtTextfield::timeChanged() {

  BUG_PARA(BugGuiFld,"GuiQtTextfield::timeChanged",this);
  QTime newdate = static_cast<QTimeEdit*>(m_dateEdit)->time();
  m_textChanged = true;
  if (m_dateEdit->hasFocus()) {
    return;
  }

  std::string formatedtext;
  std::string text;
  m_param->getFormattedValue( formatedtext );
  if (newdate.isValid() && !newdate.toString(Qt::ISODate).isNull())
    text = newdate.toString(Qt::ISODate).toStdString();
  if( newdate.isValid() && (formatedtext == text) ){
     // Keine Aenderungen im Textfield. Der Grund des LosingFocus ist
     // nicht von Bedeutung.
    if(disabled()){
      protectField();
    }
    BUG_EXIT("Text not modified");
    return;
  }

  FinalWork( text );
}

void GuiQtTextfield::datetimeChanged() {
  BUG_PARA(BugGuiFld,"GuiQtTextfield::datetimeChanged",this);
  QDateTime  newdate = static_cast<QDateTimeEdit*>(m_dateEdit)->dateTime();
  QDateTimeEdit *datetimeEdit = static_cast<QDateTimeEdit*>(m_dateEdit);
    m_textChanged = true;
    if (datetimeEdit->hasFocus()) {
      if ( !(datetimeEdit->hasFocus()) ) {
	return;
    }
  }

  std::string formatedtext;
  std::string text;
  m_param->getFormattedValue( formatedtext );
  if (newdate.isValid() && !newdate.toString(Qt::ISODate).isNull())
    text = newdate.toString(Qt::ISODate).toStdString();
  if( newdate.isValid() && (formatedtext == text) ){
    // Keine Aenderungen im Textfield. Der Grund des LosingFocus ist
    // nicht von Bedeutung.
    if(disabled()){
      protectField();
    }
    BUG_EXIT("Text not modified");
    return;
  }

  FinalWork( text );
}


/* --------------------------------------------------------------------------- */
/* create a dateEdit Widget                                                    */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtTextfield::createDateWidget() {
  QWidget *parent = getParent()->getQtElement()->myWidget();
  assert( parent != 0 );
  UserAttr::STRINGtype dtype = m_param->DataItem()->getUserAttr()->StringType();
  switch(dtype) {
  case UserAttr::string_kind_date:
      return new MyQDateEdit( parent, this );
  case UserAttr::string_kind_time:
    {
      QTimeEdit *te = new MyQTimeEdit( parent, this );
      te->setDisplayFormat("hh:mm"); // QTimeEdit::HourSection|QTimeEdit::MinuteSection);
      return te;
      break;
    }
  case UserAttr::string_kind_datetime:
    return new GuiQtTextfield::MyQDateTimeEdit( parent, this );
  default:
    break;
  }
  return NULL;
}

/* --------------------------------------------------------------------------- */
/* set date (and or time)                                                      */
/* --------------------------------------------------------------------------- */
void GuiQtTextfield::setDate(std::string str, Qt::DateFormat format) {
  assert(m_dateEdit);
  UserAttr::STRINGtype dtype = m_param->DataItem()->getUserAttr()->StringType();
  switch(dtype) {
  case UserAttr::string_kind_date:
    {
      QDate date;
      date = QDate::fromString( QString::fromStdString(str), format );
      if (str.size() && date.isValid()) {
	static_cast<QDateEdit*>(m_dateEdit)->setDate(date);
      }
      // ein update des Datumsfeldes geht leider nur so => spaeter ist das (hide/show) nicht mehr noetig
      m_dateEdit->hide();
      static_cast<MyQDateEdit*>(m_dateEdit)->setValid( date.isValid() );
      m_dateEdit->show();
      return;
    }
  case UserAttr::string_kind_time:
    {
      QTime time;
      MyQTimeEdit *te = dynamic_cast<MyQTimeEdit*>(m_dateEdit);
      if (str.size() == 0) {
        te->setTime(QTime(0, 0));  // invalid time is 00:00
      } else {
        time = QTime::fromString( QString::fromStdString(str), Qt::ISODate );
        te->setTime(time);
      }
      te->setValid( time.isValid() );
      return;
    }
  case UserAttr::string_kind_datetime:
    {
      QDateTime datetime;// = QDateTime::currentDateTime();
      if (str.size()) {
        datetime = QDateTime::fromString( QString::fromStdString(str), format );
#if QT_VERSION < 0x050000
              // qt4 does not convert timezones (correctly)
              // the following code helps qt4 to convert utc to localtime
              if(QString::fromStdString(str).endsWith("+0000")) {
                datetime.setTimeSpec(Qt::UTC);
              }
#endif
        datetime = datetime.toLocalTime();
      }
      static_cast<QDateTimeEdit*>(m_dateEdit)->setDateTime(datetime);
      if (datetime.isNull()) {
 	static_cast<QDateTimeEdit*>(m_dateEdit)->setDate( QDate() );
      }
      static_cast<MyQDateTimeEdit*>(m_dateEdit)->setValid( datetime.isValid() );
      m_dateEdit->hide();
      m_dateEdit->show();
      return;
    }
  default:
    return;
  }
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* get date (and or time)                                                      */
/* --------------------------------------------------------------------------- */
bool GuiQtTextfield::getDate(std::string &str, Qt::DateFormat format) {
  assert(m_dateEdit);
  str.erase();
  UserAttr::STRINGtype dtype = m_param->DataItem()->getUserAttr()->StringType();
  switch(dtype) {
  case UserAttr::string_kind_date:
    {
      QString s  = static_cast<QDateEdit*>(m_dateEdit)->text();
      if (s.count(' ') > 3 || s.size() == 0) {
	return true;  // Invalid or empty date = > return empty String
      }
      QDate date = static_cast<QDateEdit*>(m_dateEdit)->date();
      if ( date.isValid() )
	str = date.toString(format).toStdString();
      return true;
    }
  case UserAttr::string_kind_time:
    {
      QTime time = static_cast<QTimeEdit*>(m_dateEdit)->time();
      if ( time.isValid() )
	str = time.toString(format).toStdString();
      return true;
    }
  case UserAttr::string_kind_datetime:
    {
      QDateTime datetime = static_cast<QDateTimeEdit*>(m_dateEdit)->dateTime();
      if ( datetime.isValid() )
	str = datetime.toString(format).toStdString();
      return true;
    }
  default:
    break;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* private intern class MyQComboBox                                            */
/* --------------------------------------------------------------------------- */
GuiQtTextfield::MyQComboBox::MyQComboBox(GuiQtTextfield *e)
  : QComboBox()
  , m_textfield( e ) {
}

void GuiQtTextfield::MyQComboBox::setFont( const QFont& font ){
  view()->setFont(font);
  if(lineEdit())
    lineEdit()->setFont(font);
  return QComboBox::setFont(font);
}

bool GuiQtTextfield::MyQComboBox::event ( QEvent *e ){
  if (e->type() == QEvent::ShortcutOverride) {
    if (AppData::Instance().Undo())
      return true;
  }
  return QComboBox::event(e);
}

void GuiQtTextfield::MyQComboBox::enterEvent ( QEnterEvent *e ){
  m_textfield->enterEvent( e );
  QComboBox::enterEvent( e );
}

void GuiQtTextfield::MyQComboBox::leaveEvent ( QEvent *e ){
  m_textfield->clearMessage();
  QComboBox::leaveEvent( e );
}

void GuiQtTextfield::MyQComboBox::wheelEvent(QWheelEvent* e) {
  if (m_textfield->Attr()->hasWheelEvent() ||
      AppData::Instance().GuiWheelEvent()) {
    QComboBox::wheelEvent(e);
  } else {
    e->ignore();
  }
}

/* --------------------------------------------------------------------------- */
/* private intern class MyQDateEdit                                            */
/* --------------------------------------------------------------------------- */

QChar              GuiQtTextfield::MyQDateEdit::s_delimeter;
QString            GuiQtTextfield::MyQDateEdit::s_displayFormat;
#if QT_VERSION < SELF_MADE_VALIDATION_TILL  // <= Qt 4.2.1 // workaround (FIXME)

QString            GuiQtTextfield::MyQDateEdit::s_lastEditString;
QValidator::State  GuiQtTextfield::MyQDateEdit::s_lastEditState = QValidator::Invalid;
#endif
short s_ylen=0, s_mlen=0, s_dlen=0, s_len;

GuiQtTextfield::MyQDateEdit::MyQDateEdit( QWidget* parent, GuiQtTextfield *e )
  : QDateEdit( QDate(), parent )
  , b_editmode( false )
  , b_valid( false )
#if QT_VERSION < SELF_MADE_VALIDATION_TILL  // <= Qt 4.2.1
  , m_regv(NULL)
#endif
  , m_textfield( e ) {
  assert( e != 0 );

  // fuer das validieren eines leeren Datums alles vorbereiten
  // (Qt 4.2.1 kann das leider nicht)
  if ( s_delimeter.isNull() ) {
    QString temp = displayFormat().replace(QRegularExpression("\\w"), "");
    if (temp.size() == 2) {
      if (temp[0] == temp[1]) {
	s_delimeter = temp[0];

	// change display format e.g. dd.MM.yyyy (2X (d, M), 4x (y))
	QStringList sl = displayFormat().split(s_delimeter);
	for (int i = 0; i < sl.size(); ++i) {
	  if (sl.at(i) == "d")
	    sl[i] = "dd";
	  if (sl.at(i) == "M")
	    sl[i] = "MM";
	  if (sl.at(i) == "yy" || sl.at(i) == "y")
	    sl[i] = "yyyy";
	}
	s_displayFormat = sl.join(s_delimeter);
      }
    }
  }
  setDisplayFormat(s_displayFormat);
  setCalendarPopup(true);

  if (!s_ylen) {
    s_ylen = displayFormat().count( 'y', Qt::CaseInsensitive);
    s_mlen = displayFormat().count( 'm', Qt::CaseInsensitive);
    s_dlen = displayFormat().count( 'd', Qt::CaseInsensitive);
    s_len  = displayFormat().size();
  }
  if (!s_delimeter.isNull()) {
    if (!m_regv) {
      QRegularExpression exp(QString("[\\d]+%1[\\d]+%1[\\d]+").arg(s_delimeter));
      m_regv = new QRegularExpressionValidator( exp, this  );
    }
#if QT_VERSION < SELF_MADE_VALIDATION_TILL  // <= Qt 4.2.1  // workaround (FIXME)
    lineEdit()->setInputMask(displayFormat().replace(QRegularExpression("\\w"), "0"));
#endif
  }
}

QValidator::State GuiQtTextfield::MyQDateEdit::validate ( QString & input, int & pos ) const {
#if QT_VERSION < SELF_MADE_VALIDATION_TILL  // <= Qt 4.2.1  // workaround (FIXME)
  // einfache Validierungen
  QValidator::State state = m_regv ? m_regv->validate(input, pos) : QDateEdit::validate( input, pos);
  if (input.count(' ')>0) state = QValidator::Intermediate;
  if (input.size()  < 7) return QValidator::Invalid;

  // speichern aktueller Daten fuer andere member methoden
  s_lastEditState = state;
  s_lastEditString = input;

   // cursor position setzen und return
  if (state == QValidator::Intermediate) {
    pos = lineEdit()->cursorPosition();
    return state;
  }

  // Weitere Validierungen fuer Validator
  if (m_regv && state == QValidator::Acceptable) {
    pos = lineEdit()->cursorPosition();
    lineEdit()->setCursorPosition(pos);
    if (currentSection() == QDateTimeEdit::YearSection &&  input.size() != s_len &&
	sectionText(QDateTimeEdit::YearSection).trimmed().size() < s_ylen) {
      return QValidator::Intermediate;
    }
    if (currentSection() == QDateTimeEdit::MonthSection && input.size() != s_len &&
	sectionText(QDateTimeEdit::MonthSection).trimmed().size() < s_mlen) {
      if (pos > 0 && input[pos-1] != s_delimeter)
	return QValidator::Intermediate;
    }
    if (currentSection() == QDateTimeEdit::DaySection &&  input.size() != s_len &&
	sectionText(QDateTimeEdit::DaySection).trimmed().size() < s_dlen) {
      if (pos > 0 && input[pos-1] != s_delimeter)
	return QValidator::Intermediate;
    }

    // weitere Validierungen
    bool dOk, mOk, yOk;
    int d = sectionText(QDateTimeEdit::DaySection).toInt(&dOk);
    int m = sectionText(QDateTimeEdit::MonthSection).toInt(&mOk);
    int y = sectionText(QDateTimeEdit::YearSection).toInt(&yOk);
    if (!dOk || !mOk || !yOk) return QValidator::Intermediate; // zur Sicherheit
    if (d < 1) d = 1;  else  if (d > 31) d = 31;
    if (m < 1) m = 1;  else  if (m > 12) m = 12;
    if (!y) y=2000;
    // Neuen String erzeugen
    do {
      input = QDate(y,m,d).toString(getDisplayFormat());
      --d;
    } while (input.size() == 0);
  }
  return state;
  // so waere es normal, wenn qt keine bugs haette
#else
  bool dOk, mOk, yOk;
  QString qsD = sectionText(QDateTimeEdit::DaySection).remove(s_delimeter);
  QString qsM = sectionText(QDateTimeEdit::MonthSection).remove(s_delimeter);
  QString qsY = sectionText(QDateTimeEdit::YearSection).remove(s_delimeter);
  int d = qsD.toInt(&dOk);
  int m = qsM.toInt(&mOk);
  int y = qsY.toInt(&yOk);
  int sum = dOk + mOk + yOk;
  int cntDel = input.count(s_delimeter);
  int prod = sectionText(QDateTimeEdit::DaySection).trimmed().size() *
    sectionText(QDateTimeEdit::MonthSection).trimmed().size() *
    sectionText(QDateTimeEdit::YearSection).trimmed().size();
  QDate dateS(2000, m, d);  // schaltjahr
  // simple checks
  if (cntDel > 2 || cntDel > sum ||
      (pos > 1 && input[pos-1] == s_delimeter && displayFormat()[pos-1] != s_delimeter) ||
      (pos > 1 && input[pos-1] != s_delimeter && displayFormat()[pos-1] == s_delimeter) ||
      (qsD.size() == 1 && input[pos-1] > '3') ||
      (qsM.size() == 1 && input[pos-1] > '1') ||
      (qsM.size() == 1 && input[pos-1] > '1') ||
      qsY.size() > 4 ||
      (qsD.size() >= 2 && qsM.size() == 2 && qsY.size() < 2 && !dateS.isValid()) ||
      pos && !input[pos-1].isDigit() && input[pos-1] != s_delimeter) {
    return QValidator::Invalid;
  }
  // day and month checks
  QValidator::State state = QValidator::Intermediate;
  if (dOk && d < 0 || d > 31)   return QValidator::Invalid;
  if (mOk && m < 0 || m > 12)   return QValidator::Invalid;
  if (sum >= 2 || sectionText(QDateTimeEdit::YearSection).size() < 1) {
    state = QValidator::Intermediate;
    if (qsD.size() == 2 && qsM.size() == 2) {
      state = QValidator::Acceptable;
    }
  }
  // year check
  if (cntDel == 2 && dOk && mOk && yOk) {
    if (y < 1000 &&
        sectionText(QDateTimeEdit::YearSection).trimmed().size() <= 2) {
      y += 100* floor(QDate::currentDate().year() / 100);
    }
    QDate date(y, m, d);
    if (state == QValidator::Acceptable && date.isValid() ) {
      return QValidator::Acceptable;
    }
  }
  return state;
#endif
}

void GuiQtTextfield::MyQDateEdit::fixup ( QString & input ) const {
#if QT_VERSION < SELF_MADE_VALIDATION_TILL  // <= Qt 4.2.1  // workaround (FIXME)
  if ( !s_delimeter.isNull() ) {
    QString inp(input);
    // validate String
    QString dStr = sectionText(QDateTimeEdit::DaySection).trimmed();
    QString mStr = sectionText(QDateTimeEdit::MonthSection).trimmed();
    QString yStr = sectionText(QDateTimeEdit::YearSection).trimmed();
    bool retD, retM, retY;
    int iD = dStr.toInt(&retD);
    int iM = mStr.toInt(&retM);
    int iY = yStr.toInt(&retY);
    QString date;
    QChar f('0');
    if (retD) {
      if (iD > 31) iD=31;
      if (iD == 0) iD=1;
      date = QString("%1%2").arg(iD, s_dlen, s_len, f).arg(s_delimeter);
    }
    else
      date = QString("  %1").arg(s_delimeter);
    if (retM) {
      if (iM > 12) iM=12;
      if (iM == 0) iM=1;
      date += QString("%1%2").arg(iM, s_mlen, s_len, f).arg(s_delimeter);;
    } else
      date += QString("  %1").arg(s_delimeter);
    if (retY)
      date += QString("%1").arg(iY, s_ylen);
    else
      date += "    ";

    input = date;
  }
#else
  // validate String
  QString dStr = sectionText(QDateTimeEdit::DaySection).remove(s_delimeter).trimmed();
  QString mStr = sectionText(QDateTimeEdit::MonthSection).remove(s_delimeter).trimmed();
  QString yStr = sectionText(QDateTimeEdit::YearSection).remove(s_delimeter).trimmed();
  bool retD, retM, retY;
  int iD = dStr.toInt(&retD);
  int iM = mStr.toInt(&retM);
  int iY = yStr.toInt(&retY);
  if (!retD || !retM || !retY)  input.clear();
#endif
}

void GuiQtTextfield::MyQDateEdit::enterEvent ( QEnterEvent *e ) {
  m_textfield->enterEvent( e );
  QDateEdit::enterEvent( e );
}

void GuiQtTextfield::MyQDateEdit::leaveEvent ( QEvent *e ) {
  m_textfield->clearMessage();
  QDateEdit::leaveEvent( e );
}

void GuiQtTextfield::MyQDateEdit::keyPressEvent(QKeyEvent* e) {
  m_textfield->m_textChanged = true;
  switch (e->key()) {
#if QT_VERSION < SELF_MADE_VALIDATION_TILL
  case Qt::Key_Delete:
    {
      setValid(false);
      // 	m_textfield->m_textChanged = true;
      QString s;
      bool sb = signalsBlocked();
      blockSignals(true);
      QDateEdit::lineEdit()->setText(s);
      e->accept();
      m_textfield->lostFocus();
      blockSignals(sb);
      setValid(true);
      return;
    }
#endif
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if (e->type() == QEvent::KeyPress && e->count() == 1) {
 	QKeyEvent* _e = new  QKeyEvent(e->type(), Qt::Key_Tab,e->modifiers(),
 				       "\t", e->isAutoRepeat());
 	e->accept();
 	// call new tab key event and handle foucs failure at last section
 	int oldSec = currentSection();
 	QDateEdit::keyPressEvent(_e);
 	int newSec = currentSection();
 	if (oldSec == newSec) {
 	  focusNextChild();
 	}
 	return;
      }
  }
  QDateEdit::keyPressEvent(e);
}

void GuiQtTextfield::MyQDateEdit::focusOutEvent( QFocusEvent* e ) {
  // Validierungen (ungueliges datum wird geleert, nicht ganz fertiges datum ergaenzt )
  bool dOk, mOk, yOk;
  int d = sectionText(QDateTimeEdit::DaySection).remove(s_delimeter).toInt(&dOk);
  int m = sectionText(QDateTimeEdit::MonthSection).remove(s_delimeter).toInt(&mOk);
  int y = sectionText(QDateTimeEdit::YearSection).remove(s_delimeter).toInt(&yOk);
  if (dOk && mOk && yOk) {
    if (y < 1000 &&
	sectionText(QDateTimeEdit::YearSection).trimmed().size() <= 2) {
      y += 100* floor(QDate::currentDate().year() / 100);
    }
    QDate date(y,m,d);
    setValid(date.isValid());
    QString s = date.toString(getDisplayFormat());
    setDate(date);
    lineEdit()->setText(s);
  } else if (!dOk || !mOk || !yOk) {
    lineEdit()->setText("");
    setValid(false);
    setDate(QDate());
  }
  QDateEdit::focusOutEvent( e);
}

QString GuiQtTextfield::MyQDateEdit::textFromDateTime(const QDateTime& datetime) const {
  return !isValid() ? "" : QDateEdit::textFromDateTime(datetime);
}

/* --------------------------------------------------------------------------- */
/* private intern class MyQDateTimeEdit                                        */
/* --------------------------------------------------------------------------- */
GuiQtTextfield::MyQDateTimeEdit::MyQDateTimeEdit( QWidget* parent, GuiQtTextfield *e )
  : QDateTimeEdit( parent )
  , m_textfield( e )
  , b_valid( false )
{
  assert( e != 0 );
}

QString GuiQtTextfield::MyQDateTimeEdit::textFromDateTime(const QDateTime& datetime) const {
  QString str = QDateTimeEdit::textFromDateTime(datetime);
#if QT_VERSION < SELF_MADE_VALIDATION_TILL  // <= Qt 4.2.1     // workaround (FIXME)
  if (!isValid()) {
    // (FIXME)
      str.replace(QRegularExpression("\\w"), " ");
  }
#endif  // workaround (FIXME) ENDE
  return str;
}

void GuiQtTextfield::MyQDateTimeEdit::contextMenuEvent( QContextMenuEvent *e ){
#ifdef __USE_GUITIMETABLE__
  QDateTimeEdit *datetimeEdit = static_cast<QDateTimeEdit*>(m_textfield->m_dateEdit);
  if ( e->x() > datetimeEdit->width() )
    return;
  QDialog  dialog(m_textfield->myWidget(), Qt::FramelessWindowHint);
  MyQCalMonthLookup lookupwidget( &dialog );
  if (datetimeEdit->dateTime().date().isValid())
    lookupwidget.setDate( datetimeEdit->dateTime().date() );
  dialog.move( e->globalX()-90, e->globalY()+12 );
  dialog.exec();
  if (lookupwidget.getDate().isValid() ) {
    datetimeEdit->setDate( lookupwidget.getDate() );
    m_textfield->FinalWork( datetimeEdit->dateTime().toString(Qt::ISODate).toStdString() );
  }
#endif
}

void GuiQtTextfield::MyQDateTimeEdit::enterEvent ( QEnterEvent *e )
{
  m_textfield->enterEvent( e );
  QDateTimeEdit::enterEvent( e );
}

void GuiQtTextfield::MyQDateTimeEdit::leaveEvent ( QEvent *e )
{
  m_textfield->clearMessage();
  QDateTimeEdit::leaveEvent( e );
}

void GuiQtTextfield::MyQDateTimeEdit::keyPressEvent(QKeyEvent* e) {
  switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
      m_textfield->lostFocus();
      e->accept();
      return;
  }
  QDateTimeEdit::keyPressEvent(e);
}

/* --------------------------------------------------------------------------- */
/* private intern class MyQTimeEdit                                            */
/* --------------------------------------------------------------------------- */
void GuiQtTextfield::MyQTimeEdit::enterEvent ( QEnterEvent *e )
{
  m_textfield->enterEvent( e );
  QTimeEdit::enterEvent( e );
}

void GuiQtTextfield::MyQTimeEdit::leaveEvent ( QEvent *e )
{
  m_textfield->clearMessage();
  QTimeEdit::leaveEvent( e );
}

void GuiQtTextfield::MyQTimeEdit::show(){
  QTimeEdit::show();
  QWidget * child = childAt(width()-5, height()-5);
  if (child) {
    QWidget *spw = dynamic_cast<QWidget*>(child);
//     if (spw) {
//       spw->resize( width(), spw->eight());
//       spw->setPaletteBackgroundColor("yellow");
//       setPaletteBackgroundColor("red");
//     }
  }
}

void GuiQtTextfield::MyQTimeEdit::keyPressEvent(QKeyEvent* e) {
  switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
      m_textfield->lostFocus();
      e->accept();
      return;
  }
  QTimeEdit::keyPressEvent(e);
}
