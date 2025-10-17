
#include <limits>

#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/utils.h"
#include "utils/gettext.h"
#include "job/JobFunction.h"
#include "app/DataPoolIntens.h"
#include "app/AppData.h"

#include "gui/qt/GuiQtIndex.h"

#include <qspinbox.h>
#include <QLineEdit>
#include <QWheelEvent>

class MyQSpinBox : public QSpinBox {
public:
  MyQSpinBox(GuiQtIndex* index, QWidget* par)
    : QSpinBox(par), m_index(index) {
  }

  virtual void wheelEvent(QWheelEvent* e) {
	if (!AppData::Instance().GuiWheelEvent())
	  e->ignore();
	else
	  QSpinBox::wheelEvent(e);
  }

  virtual void stepBy ( int steps ) {
    BUG_PARA( BugGuiFld, "stepBy", "steps=" << steps );
    if ( m_index->isIndexAccepted(value()) ) {
      // 2013-12-10 amg weil die signals valueChanged() viel zu oft ausgelöst wird
      // müssen folgende etwas unschöner Code angewendet werden
      int oldCursorPosition = lineEdit()->cursorPosition();
      int oldDisplayTextSize = lineEdit()->displayText().size();
      bool enabled = lineEdit()->isEnabled();
      if (hasFocus())
	QSpinBox::stepBy(steps);
      if (!signalsBlocked() )
      setDisabled(true);
      m_index->slot_editingFinished(); // änderung wirklich machen
      setEnabled(enabled);
      // evtl. Focus und CursorPosition setzen
      if (oldCursorPosition>0) {
	lineEdit()->setCursorPosition(oldCursorPosition <= lineEdit()->displayText().size()
				      ? oldCursorPosition+(lineEdit()->displayText().size()-oldDisplayTextSize) :
				      lineEdit()->displayText().size() );
	setFocus();
      }
    }
  }

private:
  GuiQtIndex* m_index;
};

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtIndex::GuiQtIndex( GuiElement *parent, const std::string &name )
  : GuiQtElement( parent, name ), GuiIndex( parent, name )
  , m_mySpinBox( 0 )
  // , m_width( 0 )
  // , m_height( 0 )
  , m_textlen( 4 )

  , m_CB_IndexIsModified( false )
{
}

GuiQtIndex::GuiQtIndex( GuiQtIndex &index )
  : GuiQtElement( index.getParent() ), GuiIndex( index )
  , m_mySpinBox( 0 )
  // , m_width( 0 )
  // , m_height( 0 )
  , m_CB_IndexIsModified( false )
{
  m_textlen = index.m_textlen;
  registerIndexedElement( &index );
  index.registerIndexedElement( this );

  GuiIndexListenerList::const_iterator lsnr;
  for( lsnr = index.m_listeners.begin(); lsnr != index.m_listeners.end(); ++lsnr ){
    if ((*lsnr) != this)
      registerIndexedElement( (*lsnr) );
  }

  DataReference *ref = DataPool::newDataReference( *index.m_param.DataItem()->Data() );
  setDataReference( ref );
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* slot_indexChanged --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::slot_indexChanged( int i ){
  BUG_PARA( BugGuiFld, "slot_indexChanged", "New Index is " << i );

  if( isIndexAccepted(i) ){
    setNewIndex( i - m_min_index );
    indexChanged();
  }
}

/* --------------------------------------------------------------------------- */
/* slot_editingFinished --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::slot_editingFinished(){
  BUG( BugGuiFld, "slot_editingFinished" );

  if( m_activeFunc ){
    BUG_EXIT( "I do nothing! My function is activ" );
    return;
  }

  int i = m_mySpinBox->value();
  BUG_MSG( "New Index is " << i );
  if( isIndexAccepted(i) ){
    setNewIndex( i - m_min_index );
    indexChanged();
  }
}

/* --------------------------------------------------------------------------- */
/* addTabGroup --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::addTabGroup(){
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::update( UpdateReason reason ){
  BUG( BugGuiFld, "update" );

  // Der Index soll im DataPool gespeichert werden. Bei einem Update nach einem
  // Cycle-Switch kann der entsprechende Index restauriert werden. Bei override
  // ist ein Cycle-Switch getätigt worden. Alle Kunden des Index müssen in diesem
  // Fall mit dem neuen Index beliefert werden.

  // Ein Update erfolgt nur, wenn override gesetzt ist, d.h. nach einen Cycle-
  // switch oder durch einen internen Aufruf. Ansonsten ist dies nicht noetig,
  // da nur der Index selbst die Variable veraendern kann.
  if(reason == reason_Cycle || reason == reason_Always ){
    ResetLastWebUpdated();  // reason_Always for webtens
  } else {
    if( !m_param.DataItem()->isUpdated( GuiQtManager::Instance().LastGuiUpdate(), true ) ){
      return;
    }
  }
  // Der Index im Datapool entspricht der Anzeige. Der interne Index jedoch ist
  // immer 0-relativ.
  m_index = getIndex();
  BUG_MSG( "update Index Value " << m_index );
  updateMyself();

  // Achtung: Eine Aenderung des Index wird forsiert. Vermutlich ein Cycle-Switch.
  GuiIndexListenerList::iterator lsnr;
  for( lsnr = m_listeners.begin(); lsnr != m_listeners.end(); ++lsnr ){
    (*lsnr)->setIndex( GuiIndex::getName(), m_index );
  }
}

/* --------------------------------------------------------------------------- */
/* updateMyself --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::updateMyself(){
  BUG( BugGuiFld, "updateMyself" );

  if( m_mySpinBox ){
    m_mySpinBox->blockSignals(true);
    BUG_MSG( "setValue( " << m_index );
    m_mySpinBox->setValue( (m_index + m_min_index) );
    m_mySpinBox->blockSignals(false);
  }
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget *GuiQtIndex::myWidget(){
  return m_mySpinBox;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::create(){
  BUG(BugGui,"create");

  m_mySpinBox = new MyQSpinBox( this, getParent()->getQtElement()->myWidget() );
  m_mySpinBox->setAlignment(Qt::AlignHCenter);
  if( m_orientation == orient_Horizontal )
    m_mySpinBox->setButtonSymbols(QSpinBox::PlusMinus);

  // set font
  QFont font = m_mySpinBox->font();
  m_mySpinBox->setFont( QtMultiFontString::getQFont( "@index@", font ) );

  // set extreme value
  if( m_min_index > 0 )
    m_mySpinBox->setMinimum( m_min_index );
  if( m_max_index > -1 )
    m_mySpinBox->setMaximum( m_max_index );
  else
    m_mySpinBox->setMaximum( std::numeric_limits<int>::max() );
  if( m_step > 1 )
    m_mySpinBox->setSingleStep( m_step );

  // set size
  int h = (int)floor(0.5 + (1.8*QFontInfo(QtMultiFontString::getQFont( "@text@", font )).pixelSize()));
  if (0&&m_mySpinBox)  {
    m_mySpinBox->setMinimumHeight( h );
    m_mySpinBox->setMaximumHeight( h );
  }

  connect( m_mySpinBox, SIGNAL(editingFinished()), this, SLOT(slot_editingFinished()) );

  setColors();
  m_mySpinBox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed) );//MinimumExpanding) );

  int index = 0;
  m_param.DataItem()->getValue( index );
  m_index = index;
  updateMyself();
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtIndex::destroy(){
  // m_width      = 0;
  // m_height     = 0;
  return true;
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

GuiElement* GuiQtIndex::clone() {
  m_clonedIndex.push_back( new GuiQtIndex( *this ) );
  return m_clonedIndex.back();
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::manage(){
  BUG(BugGui,"manage");

  m_mySpinBox->show();
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::enable(){
  // enable cloned and parent
  m_disabled = false;
  if( m_mySpinBox ){
    m_mySpinBox->setEnabled( true );
  }
  GuiElement::enable();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::disable(){
  // disable cloned and parent
  m_disabled = true;
  if( m_mySpinBox ){
    m_mySpinBox->setDisabled( true );
  }
  GuiElement::disable();
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::getSize( int &w, int &h ){
  // w = m_width;
  // h = m_height;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::setIndex( int index ){
  BUG_PARA( BugGuiFld, "setIndex", "index=" << index );

  if( !sendNewIndex( index ) ){
    printMessage( compose(_("Index '%1' is not possible."),index ), msg_Information );
  }
  m_index = index;
  setDataValue( m_index );
  updateMyself();
}

/* --------------------------------------------------------------------------- */
/* setDataReference --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::setDataReference( DataReference *ref ){
  XferDataItem *dataitem = new XferDataItem( ref );
  m_param.setDataItem( dataitem );
}

/* --------------------------------------------------------------------------- */
/* registerIndexedElement --                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::registerIndexedElement( GuiIndexListener *l ){
  GuiIndexListenerList::iterator i;
  for( i=m_listeners.begin(); i!=m_listeners.end(); ++i ){
    if( (*i) == l ){
      return;
    }
  }
  m_listeners.push_back( l );
}

/* --------------------------------------------------------------------------- */
/* unregisterIndexedElement --                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::unregisterIndexedElement( GuiIndexListener *l ){
  m_listeners.remove( l );
}

/* --------------------------------------------------------------------------- */
/* setMinIndex --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::setMinIndex( int index ){
  if( m_cloned ) return;
  assert( index >= 0 );
  m_min_index = index;
  if (m_mySpinBox)
	m_mySpinBox->setMinimum(m_min_index);
}

/* --------------------------------------------------------------------------- */
/* getMinIndex --                                                              */
/* --------------------------------------------------------------------------- */

int GuiQtIndex::getMinIndex(){
  m_min_index = m_mySpinBox->minimum();
  return m_min_index;
}

/* --------------------------------------------------------------------------- */
/* setMaxIndex --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::setMaxIndex( int index ){
  if( m_cloned ) return;
  if( index >= 0 )
    m_max_index = index > m_min_index ? index : m_min_index;
  else
    m_max_index = -1;
  if (m_mySpinBox)
	m_mySpinBox->setMaximum(m_max_index > -1 ? m_max_index : std::numeric_limits<int>::max());
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */

int GuiQtIndex::getIndex(){
  BUG( BugGuiFld, "getIndex" );
  int index = 0;
  m_param.DataItem()->getValue( index );
  BUG_EXIT( "return " << index );
  return index;
}

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtIndex::setColors(){
  QColor background, foreground;

  bool editable = false;
  if( areIndexActivated() ){
    editable = true;
    background = GuiQtManager::optionalBackgroundColor();
    foreground = GuiQtManager::optionalForegroundColor();
  }
  else{
    background = GuiQtManager::readonlyBackgroundColor();
    foreground = GuiQtManager::readonlyForegroundColor();
  }
  QPalette pal=  m_mySpinBox->palette();
  pal.setColor( QPalette::WindowText, foreground );
  pal.setColor( QPalette::Window, background );
  m_mySpinBox->setPalette( pal  );
  m_mySpinBox->setEnabled( editable & !m_disabled);

  return editable;
}

/* --------------------------------------------------------------------------- */
/* areIndexActivated --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiQtIndex::areIndexActivated(){
  if( !m_cloned ){
    return isIndexActivated();
  }
  GuiIndexListenerList::iterator lsnr;
  for( lsnr = m_listeners.begin(); lsnr != m_listeners.end(); ++lsnr ){
    if( (*lsnr)->isIndexActivated() ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getAttributes --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::getAttributes( int &min_inx, int &max_inx, int &step ){
  if( m_mySpinBox ){
    m_min_index = m_mySpinBox->minimum();
    m_max_index = m_mySpinBox->maximum();
    m_step      = m_mySpinBox->singleStep();
  }
  min_inx = m_min_index;
  max_inx = m_max_index;
  step    = m_step;
}

/* --------------------------------------------------------------------------- */
/* sendNewIndex --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtIndex::sendNewIndex( int index ){
  BUG_PARA(BugGuiFld,"sendNewIndex", "index=" << index );
  if( isIndexAccepted( index ) ){
    setNewIndex();
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* indexChanged --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::indexChanged(){
  BUG(BugGuiFld,"indexChanged");

  m_CB_IndexIsModified = false;

  QString value = QString("%1").arg(m_mySpinBox->value());
  std::istringstream input( value.toStdString() );

  int index = 0;

  if( !m_param.read( input, index ) ){
    printMessage(_("Conversion error."), msg_Information );
    updateMyself();
    BUG_EXIT("checkFormat failed");
    return;
  }

  if( (index - m_min_index) == m_index ){
    updateMyself();
    BUG_EXIT("index not changed");
    return;
  }

  if( index < m_min_index ){
    printMessage( _("Skip is not possible !"), msg_Information );
    updateMyself();
    return;
  }

  if( index > m_max_index && m_max_index >= 0 ){
    printMessage( _("Skip is not possible !"), msg_Information );
    updateMyself();
    return;
  }

  if( !isIndexAccepted( index - m_min_index ) ){
    printMessage( _("Skip is not possible !"), msg_Information );
    updateMyself();
    return;
  }

  if( !startFunction() ){
    doEndOfWork( false, true );
    if( !m_cloned ){
      setDataValue( m_new_index );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* LeftArrow --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::LeftArrow(){
  BUG( BugGuiFld, "LeftArrow" );

  if( m_index <= 0 ){
    if( m_orientation == orient_Horizontal ){
      printMessage( _("Skip to the left is not possible !"), msg_Information );
    }
    else{
      printMessage( _("Cannot scroll up anymore"), msg_Information );
    }
    return;
  }
  int new_index = m_index - m_step;
  if( new_index < 0 ) new_index = 0;

  if( !isIndexAccepted( new_index ) ){
    if( m_orientation == orient_Horizontal ){
      printMessage( _("Skip to the left is not possible !"), msg_Information );
    }
    else{
      printMessage( _("Cannot scroll up anymore"), msg_Information );
    }
    return;
  }

  if( !startFunction() ){
    doEndOfWork( false, false );
    if( !m_cloned ){
      setDataValue( m_new_index );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* shiftRight --                                                               */
/* --------------------------------------------------------------------------- */

int GuiQtIndex::shiftRight(){
  BUG( BugGuiFld, "shiftRight" );

  if( (m_index + m_min_index) >= m_max_index && m_max_index >= 0 ){
    return 0;
  }
  int new_index = m_index + m_step;
  if( m_max_index >= 0 ){
    if( new_index + m_min_index > m_max_index ){
      new_index = m_max_index - m_min_index;
    }
  }
  int step = new_index - m_index;

  if( !sendNewIndex( new_index ) ){
    return 0;
  }
  setDataValue( m_index );
  updateMyself();
  clearMessage();
  return step;
}

/* --------------------------------------------------------------------------- */
/* RightArrow --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::RightArrow(){
  BUG( BugGuiFld, "RightArrow" );

  if( (m_index + m_min_index) >= m_max_index && m_max_index >= 0 ){
    if( m_orientation == orient_Horizontal ){
      printMessage( _("Skip to the right is not possible !"), msg_Information );
    }
    else{
      printMessage( _("Cannot scroll down anymore"), msg_Information );
    }
    BUG_EXIT("Skip failed (index " << (m_index + m_min_index) <<
             " out of range (" << m_min_index << "," << m_max_index << ")");
    return;
  }
  int new_index = m_index + m_step;
  if( m_max_index >= 0 ){
    if( new_index + m_min_index > m_max_index ){
      new_index = m_max_index - m_min_index;
    }
  }
  if( !isIndexAccepted( new_index ) ){
    if( m_orientation == orient_Horizontal ){
      printMessage( _("Skip to the right is not possible !"), msg_Information );
    }
    else{
      printMessage( _("Cannot scroll down anymore"), msg_Information );
    }
    BUG_EXIT("Skip failed (block by others)");
    return;
  }

  if( !startFunction() ){
    doEndOfWork( false, false );
    if( !m_cloned ){
      setDataValue( m_new_index );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* setPlusMinusStyle --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::setPlusMinusStyle() {
  if( m_mySpinBox ){
    m_mySpinBox->setButtonSymbols(QSpinBox::PlusMinus);
  }
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::serializeXML(std::ostream &os, bool recursive){
  return GuiIndex::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtIndex::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return GuiIndex::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtIndex::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiIndex::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtIndex::getVisibleDataPoolValues( GuiValueList& vmap ) {
  GuiIndex::getVisibleDataPoolValues( vmap );
}
