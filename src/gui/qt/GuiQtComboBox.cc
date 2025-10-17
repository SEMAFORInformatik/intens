
#include <qcombobox.h>
#include <QListView>
#include <QLineEdit>
#include <QKeyEvent>
#include <QCompleter>
#include <qapplication.h>

#include "utils/utils.h"
#include "utils/StringUtils.h"
#include "gui/GuiManager.h"
#include "gui/qt/GuiQtComboBox.h"
#include "gui/qt/QtMultiFontString.h"
#include "datapool/DataPoolDefinition.h"
#include "xfer/XferDataItem.h"
#include "app/DataPoolIntens.h"
#include "app/AppData.h"
#include "job/JobFunction.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtComboBox::GuiQtComboBox( GuiElement *parent )
  : GuiQtDataField( parent )
  , GuiComboBox()
  , m_combobox( 0 ){
}

GuiQtComboBox::GuiQtComboBox( GuiQtComboBox &option )
  : GuiQtDataField( option )
  , GuiComboBox( option )
  , m_combobox( 0 ){
}

GuiQtComboBox::~GuiQtComboBox(){
  delete m_combobox;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::create(){
  BUG_DEBUG("Begin of GuiQtComboBox::create(" <<
	    m_param->DataItem()->getFullName(false) << ")");
  assert( m_combobox == 0 );

  getAttributes();

  QWidget *pw = 0;
  if ( getParent() )
    pw = getParent()->getQtElement()->myWidget();
  m_combobox = new MyQComboBox( false, pw, this );

  m_combobox->setMaxVisibleItems( AppData::Instance().MaxOptions() );
  m_combobox->setDisabled( !isEditable() );

  // set completer
  if (AppData::Instance().GuiComboBoxEditable() && isEditable() && !m_combobox->completer()) {
    QCompleter *cmpl = new QCompleter(m_combobox->model(), m_combobox);
    cmpl->setCompletionMode(QCompleter::PopupCompletion);
    m_combobox->setEditable(true);
    m_combobox->setCompleter( cmpl );
    connect(m_combobox->lineEdit(),SIGNAL(editingFinished()), this, SLOT(textChanged()) );
  }
  updateWidgetProperty();

  // set font
  QFont default_font = m_combobox->font();
    BUG_DEBUG("Get Font: Family=" << default_font.family().toStdString() <<
	    "  Size=" << default_font.pointSize());
  bool available;
  m_css_name = m_param->DataItem()->getFullName(false);
  QFont font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font, &available );
  if( !available ){
    BUG_DEBUG("Font for " << m_css_name << " is NOT available");
    m_css_name = m_param->DataItem()->getName();
    font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font, &available );
    if( !available ){
      BUG_DEBUG("Font for " << m_css_name << " is NOT available");
      m_css_name = "combobox";
      font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font );
    }
  }

  BUG_DEBUG("Set Font: Family=" << font.family().toStdString() <<
	    "  Size=" << font.pointSize());
  m_combobox->setFont( font );

  // for stylesheet settings
  m_combobox->setObjectName( "GuiComboBox" );

  // groesse setzen
  std::string text( getLength(), 'W' );
  m_combobox->addItem( QString::fromStdString(text) );
  m_combobox->setMinimumContentsLength(getLength());
  m_combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  int h =  4 + QFontMetrics(font).height();
  m_combobox->setMinimumSize( m_combobox->sizeHint().width(), h );
  m_combobox->setMaximumSize( m_combobox->sizeHint().width(), h );
  m_combobox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed) );
  if (m_combobox->completer())
    m_combobox->completer()->setCompletionMode(QCompleter::PopupCompletion);
  m_combobox->clear();
  ///  createPickList();

  connect(m_combobox,SIGNAL(activated(int)), this, SLOT(activateCB(int)));

  setMyColors();
  setColors();

  // Als letztes muessen die Indizes in der DataReference initialisiert werden.
  // Diese Arbeiten uebernimmt immer das DataItem.
  if( !m_reCreate ){
    param()->DataItem()->setDimensionIndizes();
    m_reCreate=true ;
  }
  BUG_DEBUG("End of GuiQtComboBox::create");
}

/* --------------------------------------------------------------------------- */
/* fill --                                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::fill( int n ){
  std::string text;
  for( int i=0; i<n; i++ ){
    m_set_input->GetValue( text, i );
    if (text == " ") text.clear();
    m_combobox->addItem( QtMultiFontString::getQString( text )  );
  }
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtComboBox::destroy(){
  if( m_combobox != 0 ){
    delete m_combobox;
    m_combobox = 0;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::enable(){
  setDisabled(false);
  if( m_combobox == 0 ){
    return;
  }
  getAttributes();
  m_combobox->setEnabled( isEditable() );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::disable(){
  setDisabled(true);
  if( m_combobox == 0 ){
    return;
  }
  getAttributes();
  m_combobox->setDisabled( true );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::update( UpdateReason reason ){
  BUG_DEBUG( "update( Reason is " << reason << " )" );

  if( m_param == 0 || m_combobox == 0 ){
    return;
  }
  updateWidgetProperty();
  bool changed = getAttributes();

  // Falls sich an den Set-Daten etwas geaendert hat, wird die Combobox neu
  // aufgebaut.
  if( reason == reason_Cycle || reason == reason_Always ){
    BUG_DEBUG( "updated picklist (reason)" );
    changed = true;  // to be sure the color is set
    setUpdated();
    rebuildPickList();
  }
  else
  if( m_set_input->isDataItemUpdated( DataReference::ValueUpdated, getPickList_timestamp() ) ){
    BUG_DEBUG( "updated picklist found (input updated)" );
    changed = true;  // to be sure the color is set
    rebuildPickList();
  }
  else
  if( m_set_output->isDataItemUpdated( DataReference::ValueUpdated, getPickList_timestamp() ) ){
    BUG_DEBUG( "updated picklist found (output updated)" );
    changed = true;  // to be sure the color is set
    rebuildPickList();
  }
  else{
    switch( reason ){
    case reason_FieldInput:
    case reason_Process:
      break;
    case reason_Cancel:
      setUpdated();
      break;
    default:
      setUpdated();
      break;
    }
  }

  // --------------------------------------------------------------
  // Zuerst wird nur der Wert geprüft.
  // --------------------------------------------------------------
  m_alarmColor = false;
  int inx=m_combobox->count() -1;
  if( isUpdated() ){
    std::string text;
    // get value, considering a possible scale factor
    Scale* scale = m_param->getScalefactor();
    if (scale && scale->getValue() != 1) {
      // get value, considering a possible scale factor
      m_param->getFormattedValue( text );
      // trim white space
      trim(text);
    } else
      getValue( text );

    inx = getPickListIndex( text );
    m_alarmColor = false;
    if( inx < 0 ){
      inx = -1; //unselect old select last:: m_combobox->count() -1;
      m_combobox->setCurrentIndex( inx );
      if (text.size() > 0 && useInput()) {
        m_combobox->setEditText(QString::fromStdString(compose("(%1)", text)));
        m_alarmColor = true;
      }
    }
    else{
      std::string text2;
      if( m_set_input->GetValue( text2, inx ) ){
	// 2016-05-13 amg invalid Eintrag wird nicht mehr selektiert
	m_combobox->setCurrentIndex( text.size() ? inx : -1);
      }
      else{
	inx = -1; //unselect old select last:: m_combobox->count() -1;
	m_combobox->setCurrentIndex( inx );
      }
    }

    if( useColorSet() ){
      changed = true;
    }
  }
  //not used?  setCurrentIndex( inx );

  // cursor position check and makes crowded string left align
  if (m_combobox->lineEdit())  {
    if (m_combobox->lineEdit()->cursorPosition() > getLength())
      m_combobox->lineEdit()->setCursorPosition(0);
  }

  // --------------------------------------------------------------
  // Falls keine Attribute (exl. Updated) geaendert haben, haben
  // wir hier nicht mehr zu tun.
  // --------------------------------------------------------------
  if( !changed ){
    return;
  }

  m_combobox->setEnabled( isEditable() );
  setColors();

  // set combobox font
  QFont font =  m_combobox->font();
  QFont new_font = QtMultiFontString::getQFont( "@" + m_css_name + "@", font );
  if( font != new_font ){
    m_combobox->setFont( new_font );
  }
}

/* --------------------------------------------------------------------------- */
/* CloneForFieldgroupTable --                                                  */
/* --------------------------------------------------------------------------- */

GuiQtDataField *GuiQtComboBox::CloneForFieldgroupTable(){
  GuiQtComboBox *txt = new GuiQtComboBox( *this );
  return txt;
}

/* --------------------------------------------------------------------------- */
/* deletePickList --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::deletePickList(){
  if( m_combobox == 0 ){
    return;
  }
  m_combobox->clear();
}

/* --------------------------------------------------------------------------- */
/* rebuildPickList --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::rebuildPickList(){
  BUG( BugUndo, "rebuildPickList" );
  deletePickList();
  createPickList();
  setUpdated();
}

/* --------------------------------------------------------------------------- */
/* getPickListIndex --                                                         */
/* --------------------------------------------------------------------------- */

int GuiQtComboBox::getPickListIndex( const std::string &value ){
  if( value.length() != 0 ){
    DataReference *set = m_set_output;
    if( useInput() ){
      set = m_set_input;
    }
    std::string set_value;
    int n = 0;
    while( set->GetValue( set_value, n ) ){
      if( value == set_value ){
	return n;
      }
      n++;
    }
  }
  if (value.length()) {
    int ret = m_combobox->findText(value.c_str());
    if (ret == -1) {
      ret = m_combobox->findText(value.c_str());//, Qt::MatchStartsWith);
      BUG_DEBUG("getPickListIndex useInput["<<useInput()<<"]  value["<<value<<"] => RET["<<ret<<"]\n");
      return ret;
    }
  } else {
  }
  return m_combobox->findText(value.length() ? value.c_str() : "");
}

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::setColors(){
  BUG_DEBUG("Begin setColors");

  QColor background, foreground;
  int dark_fac;
  bool clearStyleSheet(false);
  bool setColor = getTheColor( background, foreground, dark_fac );
  BUG_DEBUG("- Background=" << background.darker(100).name().toStdString() <<
	    "  Foreground=" << foreground.darker(100).name().toStdString() <<
	    "  setColor=" << setColor);

  if (m_alarmColor) {
    background = QColor(Qt::red);
    setColor = true;
  }
  setColor = true;
  m_alarmColor = false;

  if (!qApp->styleSheet().contains("QComboBox")) {
    QPalette palette = m_combobox->palette();
    palette.setColor(QPalette::ButtonText, foreground);
    palette.setColor(QPalette::Button, background);
    palette.setColor(QPalette::Text, foreground);
    palette.setColor(QPalette::Base, background);
    palette.setColor( m_combobox->backgroundRole(),      background );
    palette.setColor( m_combobox->foregroundRole(),      foreground );
    palette.setColor( QPalette::Disabled, m_combobox->backgroundRole(),      background.darker(115) );
    palette.setColor( QPalette::Disabled, m_combobox->foregroundRole(),      foreground.darker(115) );

    m_combobox-> setPalette( palette );
  }

  // check if we need to set style sheet to update a previously set style sheet
  // TODO: If we have an application style sheet that sets QComboBox palette things,
  //       it would be better to just set an empty style sheet for setColor=false so that the
  //       application style sheets colors would be used
  //       But if we don't have one, we should use the palette colors...
  //       For the moment, we set the palette colors in the resource file even if you use style sheets!
  if ( !setColor ) {
    QString styleSheet = m_combobox->styleSheet();
    if ( !styleSheet.isEmpty() ) {
      clearStyleSheet = true;
    }
  }

  std::string css;
  if ( setColor ) {
    BUG_DEBUG("- set color");
    std::string fontcss( QtMultiFontString::getCss( m_css_name ) );
    css = compose("QComboBox { background-color:%1; color: %2; %3}",
		  background.name().toStdString(),
		  foreground.name().toStdString(),
		  fontcss);
    css += compose("\nQComboBox:disabled { background-color:%1; color: %2; %3}",
		   background.darker(dark_fac).name().toStdString(),
		   foreground.darker(dark_fac).name().toStdString(),
		   fontcss);
    BUG_DEBUG("- set Stylesheet(" << css << ")");
  }

  const std::string& attrCss = GuiDataField::getStylesheet();
  if( attrCss.size() ){
    BUG_DEBUG("- add Stylesheet(" << attrCss << ")");
  }

  if (css.size() || attrCss.size()) {
    QString sheet(QString::fromStdString(css + "\n" + attrCss));
    if(m_combobox->styleSheet() != sheet) {
      m_combobox->setStyleSheet(sheet);
    }
  } else if (clearStyleSheet) {
    m_combobox->setStyleSheet(QString()); // clear stylesheet
  }
  BUG_DEBUG("End setColors");
}

/* --------------------------------------------------------------------------- */
/* activateCB --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::activateCB( int index ){
  BUG_DEBUG( "activateCB(" << index << ")" );
  m_position = index;
  FinalWork();
}

/* --------------------------------------------------------------------------- */
/* textChanged --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::textChanged(){
  BUG_DEBUG("textChanged" );
  int cnt = m_combobox->completer() ? m_combobox->completer()->completionCount() : 1;
  int idxCB = m_combobox->findText( m_combobox->currentText() );
  int idx =  m_combobox->currentIndex();

  // nothing changed
  if( idx == idxCB ) {
    if( isValid() ){
      BUG_DEBUG("nothing changed and dataitem is invalid -> exit");
      return;
    }
    BUG_DEBUG("nothing changed, but wait and see");
  }

  // empty text and is first or last in item list
  if( m_combobox->currentText().isEmpty() &&
      ( idxCB == 0 || (idxCB == (cnt-1) && idx <0 ) ) ) {
    m_position = -1;
    BUG_DEBUG("nothing changed Empty Text -> exit");
    return;
  }

  if ( m_combobox->completer() && cnt > 0) {
    if (idxCB < 0) {   // current text not found (may be incomplete but unique)
      QString search = (cnt == 1) ? m_combobox->completer()->currentCompletion() : "";
      idx = m_combobox->findText( search );
      m_combobox->setEditText( search );
      BUG_DEBUG("completion '" << search.toStdString() << "' (" << idx << ")");
    }
    else{
      idx = idxCB;
    }
    m_combobox->setCurrentIndex( idx );
    if (m_position != idx) {
      activateCB(idx);
    }
    BUG_DEBUG("set index to " << idx << " and activate -> exit");
  }
  else{
    BUG_DEBUG("no completion: set index to " << idx << " -> exit");
    m_combobox->setCurrentIndex( idx );  // muss irgendwo sein
  }
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::serializeXML(std::ostream &os, bool recursive ){
  os<<"<intens:OptionMenu";
  JobFunction *func = Attr()->getFunction();
  if( func )
    os << " action=\"" << func->Name() << "\"";
  GuiComboBox::serializeXML( os, recursive );
  os << "</intens:OptionMenu>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtComboBox::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  bool updated =  GuiComboBox::serializeJson(jsonObj, onlyUpdated);
  JobFunction *func = Attr()->getFunction();
  if (/*updated &&*/ func && m_combobox) {
    jsonObj["gui_value"] = m_combobox->currentText().toStdString();
  }
  return updated;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtComboBox::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  bool updated =  GuiComboBox::serializeProtobuf(eles, onlyUpdated);
  return updated;
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::getVisibleDataPoolValues( GuiValueList& vmap ){
  if (m_param) {
    std::string::size_type posA, posE;
    std::string s;
    m_param->getFormattedValue( s );
    posA = s.find_first_not_of(" ");
    if (posA == std::string::npos)  posA = 0;
    posE = s.find_last_not_of(" ");
    if (posE == std::string::npos)  posE = s.length();

    getAttributes();
    std::string flag("false");
    if( !isEditable() )
      flag="true";
    std::string value = s.substr(posA, posE+1)+"@"+flag;

    vmap.insert( GuiValueList::value_type(m_param->getFullName(), value ) );
  }
  GuiComboBox::getVisibleDataPoolValues( vmap );
}

/*=============================================================================*/
/* Class QtListViev                                                            */
/*=============================================================================*/

class QtListView : public QListView {
public:
  QtListView (QWidget * parent = 0 )
    : QListView (parent)
    , m_opt(
#if QT_VERSION < 0x060400
            QListView::viewOptions()
#endif
            ) {
    QFont font = m_opt.font;
    bool available;
    m_opt.font = QtMultiFontString::getQFont( "@combolist@", font, &available );
    if( !available ){
      m_opt.font = QtMultiFontString::getQFont( "@combobox@", font );
    }
  }

  virtual QStyleOptionViewItem viewOptions() const {
    return m_opt;
  }

private:
  QStyleOptionViewItem m_opt;
};

/*=============================================================================*/
/* Constructor / Destructor of MyQComboBox                                     */
/*=============================================================================*/

GuiQtComboBox::MyQComboBox::MyQComboBox( bool rw, QWidget* parent, GuiQtElement *e )
  : QComboBox( parent )
  , m_element( e ) {
  setEditable( rw );
  assert( e != 0 );
  setView( new QtListView() );
}

GuiQtComboBox::MyQComboBox::~MyQComboBox(){
}
/*=============================================================================*/
/* member functions of MyQComboBox                                             */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* MyQComboBox::setFont --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::MyQComboBox::setFont( const QFont& font ){
  view()->setFont(font);
  if(lineEdit())
    lineEdit()->setFont(font);
  return QComboBox::setFont(font);
}

/* --------------------------------------------------------------------------- */
/* MyQComboBox::event --                                                       */
/* --------------------------------------------------------------------------- */

bool GuiQtComboBox::MyQComboBox::event ( QEvent *e ){
  // BUG_DEBUG( "MyQComboBox::event" );
  if (e->type() == QEvent::ShortcutOverride) {
    if (AppData::Instance().Undo()){
      return true;
    }
  }
  return QComboBox::event(e);
}

/* --------------------------------------------------------------------------- */
/* MyQComboBox::enterEvent --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::MyQComboBox::enterEvent( QEnterEvent *e ){
  BUG_DEBUG( "MyQComboBox::enterEvent" );
  m_element->enterEvent( e );
  QComboBox::enterEvent( e );
}

/* --------------------------------------------------------------------------- */
/* MyQComboBox::leaveEvent --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::MyQComboBox::leaveEvent( QEvent *e ){
  BUG_DEBUG( "MyQComboBox::leaveEvent" );
  m_element->clearMessage();
  QComboBox::leaveEvent( e );
}

/* --------------------------------------------------------------------------- */
/* MyQComboBox::keyPressEvent --                                               */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::MyQComboBox::keyPressEvent( QKeyEvent *e ){
  BUG_DEBUG( "MyQComboBox::keyPressEvent" );
  if (completer() && completer()->completionCount() > 0 && e->key() == Qt::Key_Return) {
    return;
  }
  QComboBox::keyPressEvent( e );

  // letzte Eingabge ignorieren (Backspace)
  if (completer() && completer()->completionCount() == 0 && e->key() != Qt::Key_Delete) {
    bool blocked= lineEdit()->signalsBlocked();
    lineEdit()->blockSignals(true);
    QKeyEvent * ke =  new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    QComboBox::keyPressEvent( ke );
    delete ke;
    lineEdit()->blockSignals(blocked);
  }
}

/* --------------------------------------------------------------------------- */
/* MyQComboBox::wheelEvent --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtComboBox::MyQComboBox::wheelEvent(QWheelEvent* e) {
  BUG_DEBUG( "MyQComboBox::wheelEvent" );
  if( dynamic_cast<GuiQtComboBox*>(m_element)->Attr()->hasWheelEvent() ||
      AppData::Instance().GuiWheelEvent() ){
    QComboBox::wheelEvent(e);
  }
  else {
    e->ignore();
  }
}
