#include <QGroupBox>
#include <QPalette>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <qlayout.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qsplitter.h>

#include <sstream>
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtFieldgroup.h"
#include "gui/qt/GuiQtFieldgroupLine.h"
#include "gui/qt/GuiQtDataField.h"
#include "gui/qt/GuiQtIndex.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/GuiFolder.h"
#include "gui/GuiStretch.h"
#include "gui/GuiVoid.h"
#include "app/Plugin.h"

#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtFieldgroup::GuiQtFieldgroup( GuiElement *parent, const std::string &name )
  : GuiFieldgroup( parent, name )
  , GuiQtElement( parent, name )
  , m_scrollview(0)
  , m_qgroupbox( 0 )
  , m_qgroupboxLayout( 0 )
{}

GuiQtFieldgroup::GuiQtFieldgroup( const GuiQtFieldgroup &fg )
  : GuiFieldgroup( fg ), GuiQtElement( fg )
  , m_scrollview(0)
  , m_qgroupbox( 0 )
  , m_qgroupboxLayout( 0 )
{
  std::ostringstream myname;
  myname << fg.Name() << "#" << getElement()->getCloneNumber();
  GuiFieldgroup::setName(myname.str());

  m_container.clear();
  GuiFieldgroupLine *old_line = 0;
  GuiFieldgroupLine *new_line = 0;
  GuiElementList::const_iterator it;
  for( it = fg.m_container.begin(); it != fg.m_container.end(); ++it ){
    assert( (*it)->Type() == GuiElement::type_FieldgroupLine );
    old_line = (*it)->getFieldgroupLine();
    if( old_line->isArrowbar() ){
      createArrowbar();
    }
    else{
      new_line = old_line->clone()->getFieldgroupLine();
      assert( new_line != 0 );
      new_line->getElement()->setParent( this->getElement() );
      m_container.push_back( new_line->getElement() );
      if(GuiFieldgroup::getGuiIndex() != 0){
        GuiFieldgroup::getGuiIndex()->registerIndexedElement( new_line->getElement() );
      }
    }
  }
}

GuiQtFieldgroup::~GuiQtFieldgroup(){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

GuiElement* GuiQtFieldgroup::clone() {
  GuiElement* baseElem = findElement( getName() );
  if (baseElem == this->getElement())
    m_clonedFieldgroup.push_back( new GuiQtFieldgroup( *this ) );
  else
    return baseElem->clone();
  return m_clonedFieldgroup.back();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  cList.insert(cList.begin(), m_clonedFieldgroup.begin(), m_clonedFieldgroup.end());
}

/* --------------------------------------------------------------------------- */
/* streamableObject --                                                         */
/* --------------------------------------------------------------------------- */

BasicStream* GuiQtFieldgroup::streamableObject() {
  return GuiFieldgroup::streamableObject();
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
#include "gui/qt/QtDialogProgressBar.h"

void GuiQtFieldgroup::create(){
  if (getName() == DialogProgressBar::FIELDGROUP_NAME){
    // defined in DialogProgressBar
    QtDialogProgressBar::Instance().create_fieldgroup(this);
  }
  BUG_DEBUG( "--- begin create() --- Lines = " << m_container.size() );
  assert( m_qgroupbox == 0 );
  if(AppData::Instance().HeadlessWebMode()) {
    doNormalisation();
  }

  QWidget *parent = getParent()->Type() == GuiElement::type_Plugin ?
    (QWidget*) dynamic_cast<Plugin*>(getParent())->getBaseWidget() :
    getParent()->getQtElement()->myWidget();

  createContainer( parent );

  // Zuerst werden alle Elemente im erstem Schritt erstellt
  // !!!!!  (TabOrder wird gleich der Order beim Kreieren )
  GuiElementList::iterator it;
  // erstellen der FieldgroupLine (ohne deren Elemente)
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    (*it)->create();
  }

  // erstellen der Elemente innerhalb der FieldgroupLine (VERTICAL)  ==> Vertikale TabOrder
  if ( getTableSize() ==  0 || m_navigation == orient_Vertical ) {
    for( it = m_container.begin(); it != m_container.end(); ++it ){
      GuiElementList::iterator it2;
      for( it2 = static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.begin();
           it2 != static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.end(); ++it2 ){
        // invisible gui items => continue
        GuiQtDataField *df = dynamic_cast<GuiQtDataField*>(*it2);
        if (df && df->getLength() == 0) {
          continue;
        }
        // create gui item
        (*it2)->create();
      }
    }
  }
  else {
    // erstellen der Elemente innerhalb der FieldgroupLine (HORIZONTAL) => Horizontal TabOrder
    int maxRows = 1;
    for (int r=0; r < maxRows; ++r) {
      for( it = m_container.begin(); it != m_container.end(); ++it ) {
        int numElem =  static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.size();
        if ( maxRows < numElem )  maxRows = numElem;
        if ( r >= numElem ) 	  continue;
        GuiElementList::iterator it2 = static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.begin() + r;

        // invisible gui item => continue
        GuiQtDataField *df = dynamic_cast<GuiQtDataField*>(*it2);
        if (df && df->getLength() == 0)
          continue;

        // create gui item
        if ( it2 != static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.end() ) {
          (*it2)->create();
        }
      }
    }
  }

  // Das Positionieren erfolgt in diesem zweiten Schritt.
  int row = 0, col = 0, max_col=0;
  row = m_qgroupboxLayout->rowCount();
  std::map<int, int> rowSpanMap;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    // insert Cell
    GuiElementList::iterator it2;
    col = ( !withFrame() && getTitle().size() && m_orientation == orient_Vertical) ? 1 : 0;

    for( it2 =  static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.begin();
         it2 != static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.end(); ++it2 ){
      int _colspan = (*it2)->getColSpan();  // get span of column
      int _rowspan = (*it2)->getRowSpan();  // get span of column

      // check for previous rowspan to increase column
      if (rowSpanMap.size()) {
        std::map<int, int>::iterator it = rowSpanMap.begin();
        for(; it != rowSpanMap.end(); ++it) {
          if ((*it).first == col && (*it).second > 0) {
            (*it).second -= 1;
            ++col;
          }
        }
      }

      // save rowspan
      if (_rowspan > 1 && m_orientation == orient_Horizontal) {
        rowSpanMap[col] = _rowspan-1;
      } else if (_rowspan > 1 && m_orientation == orient_Vertical) {
        rowSpanMap[row] = _colspan-1;
      }

      // Qt::AlignmentFlags
      Qt::Alignment _colalign = getQtAlignment( (*it2)->getAlignment() );  // get alignment of column
      // set alignflags for fieldgroups to top
      if( _colalign == align_Default &&
          (*it2)->Type() == GuiElement::type_Fieldgroup) {
        _colalign =  (m_orientation == orient_Vertical) ?  Qt::AlignLeft : Qt::AlignTop;

        if (getContainerExpandPolicy() != GuiElement::orient_Default) {
          // eine expandable FG besser kein Alignment
          _colalign = Qt::Alignment();
        }
      }
      if ( static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.size() == 1 &&
           ( (*it2)->Type() == GuiElement::type_Pixmap ||
             (*it2)->Type() == GuiElement::type_Label  ||
             (*it2)->Type() == GuiElement::type_Separator) ) {
        m_qgroupboxLayout->addWidget( (*it2)->getQtElement()->myWidget(), row, col, 1, -1, _colalign);
      }
      else {
        if ( (*it2)->getQtElement() && (*it2)->getQtElement()->myWidget() ) {
          // is a hidden line =>  break
          GuiQtFieldgroupLine *line = static_cast<GuiQtFieldgroupLine*>(*it);
          if (line->isHiddenLine()) {
            --row;
            break;
          }

          if ( m_orientation == orient_Vertical ) {
            m_qgroupboxLayout->addWidget( (*it2)->getQtElement()->myWidget(),
                                          col, row,
                                          _colspan, _rowspan, _colalign );
          }
          else {
            BUG_DEBUG("AddWidget row["<<row<<"]["<<_rowspan<<"] col["<<col<<"]["<<_colspan
                      <<"] type["<<(*it2)->getQtElement()->StringType()<<"] align["<<_colalign<<"]");
            // overlay widget are not added to layout
            if (!(*it2)->getFieldgroup() ||
                (*it2)->getFieldgroup() && !(*it2)->getFieldgroup()->getOverlayGeometry().isValid()) {
              m_qgroupboxLayout->addWidget( (*it2)->getQtElement()->myWidget(),
                                            row, col,
                                            _rowspan, _colspan, _colalign );
            }
          }
          if ( (*it2)->Type() == GuiElement::type_Separator ) {
            if (m_orientation == orient_Vertical) {
              (*it2)->getQtElement()->myWidget()->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed) );
            }
            else {
              (*it2)->getQtElement()->myWidget()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding) );
            }
          }
          // Wir haben ein Void Element
        }
        else if ( (*it2)->Type() == GuiElement::type_Void ) {
          (*it2)->create();
          QWidget *widget = reinterpret_cast<QWidget*>(dynamic_cast<GuiVoid*>(*it2)->myWidget());
          if ( m_orientation == orient_Vertical ) {
            m_qgroupboxLayout->addWidget( widget, col, row, _colspan, _rowspan, _colalign );
          }
          else {
            m_qgroupboxLayout->addWidget( widget, row, col, _rowspan, _colspan, _colalign );
          }
        }
        // Wir haben ein Stretch Element
        else if ( (*it2)->Type() == GuiElement::type_Stretch ) {
          int hsFac, vsFac;
          dynamic_cast<GuiStretch*>( (*it2) )->getStretchFactor( hsFac, vsFac );
          QWidget *widget = new QWidget( );
          widget->setSizePolicy(QSizePolicy(hsFac?QSizePolicy::MinimumExpanding:QSizePolicy::Fixed,
                                            vsFac?QSizePolicy::MinimumExpanding:QSizePolicy::Fixed));
          if ( m_orientation == orient_Vertical ) {
            m_qgroupboxLayout->addWidget( widget, col, row, _colspan, _rowspan, _colalign );
          }
          else {
            m_qgroupboxLayout->addWidget( widget, row, col, _rowspan, _colspan, _colalign );
          }
          if (hsFac && hsFac > m_qgroupboxLayout->columnStretch( col)) {
            m_qgroupboxLayout->setColumnStretch( col, hsFac );
          }
          if (vsFac && vsFac > m_qgroupboxLayout->rowStretch( row)) {
            m_qgroupboxLayout->setRowStretch( row, vsFac );
          }
        }
      }
      col += _colspan;
    }
    max_col = std::max(max_col, col-1);

    ++row;
  }


  myWidget()->installEventFilter(this);

  QList<QWidget*> children = myWidget()->findChildren<QWidget*>();
  for (QWidget* w : children) {
    w->installEventFilter(this);
  }

  if (getContainerExpandPolicy() == GuiElement::orient_Default) {
    m_qgroupbox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  }

  setDebugTooltip();
  if (getOverlayGeometry().isValid()) {
    myWidget()->hide();
  }
  // accordian, set checked option
  if (hasAccordion()) {
    slot_accordian(isAccordionOpen());
    dynamic_cast<QGroupBox*>(m_qgroupbox)->setChecked(isAccordionOpen());
  }
  BUG_DEBUG( "--- end create() ---");
}

/* --------------------------------------------------------------------------- */
/* onClicked --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtFieldgroup::onClicked() {
  if (getFunction() == 0) return;

  JobStarter *starter = new FieldgroupTrigger(getFunction());
  starter->setReason(JobElement::cll_Select);
  starter->startJob();
}

void GuiFieldgroup::FieldgroupTrigger::backFromJobStarter(JobAction::JobResult rslt)
{
  return;
}

/* --------------------------------------------------------------------------- */
/* eventFilter --                                                                   */
/* --------------------------------------------------------------------------- */
bool GuiQtFieldgroup::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
    QWidget *target = qobject_cast<QWidget*>(obj);
    if (!target)
      return false;

    QWidget *root = myWidget();

    if (target == root || root->isAncestorOf(target))
    {
      onClicked();
      return false;
    }
  }

  return GuiQtElement::eventFilter(obj, event);
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtFieldgroup::enable(){
  if (AppData::Instance().HeadlessWebMode() && myWidget() && !isEnabled()) {
    BUG_INFO("ENABLE FG: " << Name());
    setAttributeChangedFlag(true);
  }
  // enable cloned and parent
  GuiElement::enable();

  m_container.enable();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::disable(){
  if (AppData::Instance().HeadlessWebMode() && myWidget() && isEnabled()) {
    BUG_INFO("DISABLE FG: " << Name());
    setAttributeChangedFlag(true);
  }

  // disable cloned and parent
  GuiElement::disable();

  m_container.disable();
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

 bool GuiQtFieldgroup::destroy(){
   BUG(BugGui,"GuiQtFieldgroup::destroy");
   if( m_container.empty()) return true;

   bool rslt = true;
   GuiElementList::iterator it;
   for( it = m_container.begin(); it != m_container.end(); ++it ){
     if( !(*it)->destroy() ){
       rslt = false;
     }
   }
   delete m_scrollview;
   delete m_qgroupbox;
   m_qgroupbox = 0;
   m_qgroupboxLayout = 0;
   return rslt;
 }

/* --------------------------------------------------------------------------- */
/* createContainer --                                                          */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtFieldgroup::createContainer( QWidget* parent ){
  BUG_DEBUG("--- begin createContainer() ---");

  // scrollbar?
  if (withScrollbars()){
    QScrollArea* sv = new QScrollArea(parent);
    sv->setWidgetResizable( true );
    parent = sv;
    m_scrollview = sv;
  }

  // set margins, spacing, ...
  int margin  = getMargin();
  int spacing = getSpacing();

  m_qgroupboxLayout = new QGridLayout();
  m_qgroupboxLayout->setSpacing( spacing );
  m_qgroupboxLayout->setContentsMargins(margin,margin,margin,margin);
  m_qgroupboxLayout->setOriginCorner(Qt::TopLeftCorner );

  BUG_DEBUG(" - with frame: " << withFrame());
  QGroupBox* groupbox = new QGroupBox(QtMultiFontString::getQString(getTitle()), parent);
  m_qgroupbox = groupbox;
  QObject::connect( groupbox, SIGNAL(clicked(bool)), this, SLOT(slot_accordian(bool)) );
  if (hasAccordion()) {
    groupbox->setCheckable(true);
  }

  int h = spacing;// = 6;
  if( getTitle().size() ){
    BUG_DEBUG(" - set title '" << getTitle() << "'");
    groupbox->setTitle( QtMultiFontString::getQString(getTitle()) );
    // set title alignment
    std::string alignStr;
    switch (getTitleAlignment()) {
    case align_Default:
    case align_Left:
      groupbox->setAlignment(Qt::AlignLeft);
      alignStr = "left";
      break;
    case align_Center:
      groupbox->setAlignment(Qt::AlignHCenter);
      alignStr = "center";
      break;
    case align_Right:
      groupbox->setAlignment(Qt::AlignRight);
      alignStr = "right";
      break;
    default:
      groupbox->setAlignment(Qt::AlignLeft);
      alignStr = "left";
      break;
    }

    // set font (ignore Bold weight)
    QString qss(groupbox->styleSheet() +
                QString::fromStdString(compose(" QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top %1;}", alignStr)));
    QFont font = groupbox->font();
    font = QtMultiFontString::getQFont( "@groupboxTitle@", font );
    // stylesheet font-weight will not be inheritance to childs (unlike font do)
    if ( (int)font.weight() > QFont::Normal ) {
      BUG_DEBUG(" - weight not normal");
      groupbox->setTitle( QtMultiFontString::getQString(getTitle()) );
      font.setWeight(QFont::Normal);
      qss  += " QGroupBox { font-weight: bold; } ";
    }
    groupbox->setStyleSheet( qss  + " QGroupBox { font-weight: bold; } ");
    groupbox->setFont( font );

    h = QFontInfo(QtMultiFontString::getQFont( "@groupboxTitle@", font )).pixelSize();
  }
  groupbox->setFlat( !withFrame());
  if (withFrame()){
    BUG_DEBUG(" - pixel size = " << h);
    groupbox->setContentsMargins( spacing, h, spacing, spacing );
  }else{
    m_qgroupbox->setContentsMargins( 0,0,0,0 );
  }
  m_qgroupbox->setLayout( m_qgroupboxLayout );

  if (m_scrollview)
    dynamic_cast<QScrollArea*>(m_scrollview)->setWidget( m_qgroupbox );

  // set colors
  QPalette pal = m_qgroupbox->palette();
  pal.setColor(QPalette::WindowText,  GuiQtManager::fieldgroupForegroundColor() );
  pal.setColor(QPalette::Window, GuiQtManager::fieldgroupBackgroundColor() );
  m_qgroupbox->setPalette( pal );
  m_qgroupbox->setObjectName( QString::fromStdString(getName()) );
  updateWidgetProperty();
  BUG_DEBUG("--- end createContainer() ---");
  return m_qgroupbox;
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::unmanage(){
  BUG(BugGui,"GuiQtFieldgroup::unmanage");
  GuiElementList::iterator it;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    (*it)->unmanage();
  }
  if (myWidget()) {
    myWidget()->hide();
  }
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::manage(){
  BUG(BugGui,"GuiQtFieldgroup::manage");
  if (myWidget()) {
    if (getOverlayGeometry().isValid())
      myWidget()->hide();
    else {
      myWidget()->show();
      myWidget()->setVisible( getVisibleFlag() );
    }
  }
  QApplication::style()->unpolish(myWidget());
  m_container.manage();
  GuiQtIndex *qIndex = dynamic_cast<GuiQtIndex*>(GuiFieldgroup::getGuiIndex());

  if(getTableSize() > 0 && getTableMax() == getTableMin()){
    if( qIndex && qIndex ->myWidget() ){
      qIndex ->myWidget()->hide();
    }
  }

  if(getTableStep() == 0){
    // Ein einmaliges Setzen des Index reicht.
    for( GuiElementList::iterator it = m_container.begin(); it != m_container.end(); ++it ){
      if( (*it)->Type() != type_Index ){
	(*it)->setIndex( "@fg_index@", 0 );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* unmap --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::unmap(){
  if (myWidget() && myWidget()->isVisible())
    setAttributeChangedFlag(true);
  if (getOverlayGeometry().isValid()) {
    myWidget()->lower();
    myWidget()->hide();
    return;
  }
  GuiQtElement::unmap();
}

/* --------------------------------------------------------------------------- */
/* map --                                                                      */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::map(){
  if (myWidget() && !myWidget()->isVisible())
    setAttributeChangedFlag(true);
  if (getOverlayGeometry().isValid()) {
    QSize hsize = myWidget()->sizeHint();
    QSize psize = getParent()->getQtElement()->myWidget()->size();
    QSize phsize = getParent()->getQtElement()->myWidget()->sizeHint();
    BUG_DEBUG("OverlayGeometry ownHintSize: " << hsize.width() << ", " << hsize.height());
    BUG_DEBUG("OverlayGeometry parentSize: " << psize.width() << ", " << psize.height());
    BUG_DEBUG("OverlayGeometry parentHintSize: " << phsize.width() << ", " << phsize.height());
    BUG_DEBUG("OverlayGeometry orientVert: " << (m_orientation == orient_Vertical));

    //
    int xpos(getOverlayGeometry().xpos >= 0 ? getOverlayGeometry().xpos :
             phsize.width() + getOverlayGeometry().xpos - hsize.width());
    int ypos(getOverlayGeometry().ypos >= 0 ? getOverlayGeometry().ypos :
             phsize.height() + getOverlayGeometry().ypos - hsize.height());
    int width(std::max(hsize.width(), m_orientation == orient_Vertical ? psize.width() :
                       getOverlayGeometry().width == -1 ? hsize.width() :
                       std::max(psize.width(), hsize.width())));
    int height(std::max(hsize.height(), m_orientation == orient_Vertical ? psize.height() :
                        getOverlayGeometry().height == -1 ? hsize.height() :
                        std::max(psize.height(), hsize.height())));
    if (ypos > 0 && getOverlayGeometry().height == 0) height -= 2*ypos;
    if (xpos > 0 &&  getOverlayGeometry().width == 0) width -= 2*xpos;
    BUG_DEBUG("OverlayGeometry pos: " << xpos << ", "<< ypos << " : " << width << ", "<< height);
    if (m_scrollview){
      QScrollArea* sv =  dynamic_cast<QScrollArea*>(m_scrollview);
      if (getOverlayGeometry().height != 0) height += sv->horizontalScrollBar()->height();
      if (getOverlayGeometry().width != 0) width += sv->verticalScrollBar()->width();
    }
    myWidget()->setGeometry(xpos, ypos, width, height);
    myWidget()->show();
    myWidget()->raise();
    return;
  }
  GuiQtElement::map();
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::update( UpdateReason reason ){
  if (!myWidget()) return;
  BUG_PARA(BugGui,"GuiQtFieldgroup::update",reason );
  updateWidgetProperty();
  GuiElementList::iterator it;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    if( (*it)->Type() != type_Index ){
      (*it)->update( reason );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldgroup::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG(BugGui, "GuiQtFieldgroup::hasChanged");
  GuiContainer::iterator it = m_container.begin();
  for(; it != m_container.end(); ++it)
    if ((*it)->hasChanged( trans, xfer, show ))
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* getStretchFactor --                                                         */
/* --------------------------------------------------------------------------- */

int GuiQtFieldgroup::getStretchFactor( GuiElement::Orientation orient ) {
  BUG_PARA(BugGui,"GuiQtFieldgroup::getStretchFactor",
          "Orientation: "<< orient);
  int maxStretch=0;
  if (orient == GuiElement::orient_Horizontal) {
    for (int i=0; i< m_qgroupboxLayout->columnCount(); ++i)
      if ( maxStretch < m_qgroupboxLayout->columnStretch(i) )
       maxStretch = m_qgroupboxLayout->columnStretch(i);
  } else if (orient == GuiElement::orient_Vertical) {
    for (int i=0; i< m_qgroupboxLayout->rowCount(); ++i)
      if ( maxStretch < m_qgroupboxLayout->rowStretch(i) )
       maxStretch = m_qgroupboxLayout->rowStretch(i);
  }
  // for future use: When child are expandable
  GuiElementList::iterator it;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    GuiElementList::iterator it2;
    for( it2 =  static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.begin();
         it2 != static_cast<GuiQtFieldgroupLine*>(*it)->m_elements.end(); ++it2 ){
      GuiQtElement* qtElem = dynamic_cast<GuiQtElement*>(*it2);
      if (!qtElem) continue;
      GuiElement::Orientation exp = qtElem->getContainerExpandPolicy();
      if (exp & m_orientation)
        if (GuiQtManager::DefaultStretchFactor() > maxStretch)
          maxStretch = GuiQtManager::DefaultStretchFactor();
    }
  }
  BUG_MSG("Name["<<getName()<<"] Result: "<< maxStretch);
  return maxStretch;
}

/* --------------------------------------------------------------------------- */
/* showColumn --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtFieldgroup::showColumn(int columnIdx, bool visible, int rowOffset) {
  if (columnIdx < m_qgroupboxLayout->columnCount()) {
	for (int r=0; r < m_qgroupboxLayout->rowCount(); ++r) {
	  if (m_qgroupboxLayout->itemAtPosition(r, columnIdx)) {
		if (r >= rowOffset && m_qgroupboxLayout->itemAtPosition(r, columnIdx)->widget()) {
		  m_qgroupboxLayout->itemAtPosition(r, columnIdx)->widget()->setVisible(visible);
		}
	  }
	}
  }
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtFieldgroup::serializeXML(std::ostream &os, bool recursive){
  GuiFieldgroup::serializeXML( os, recursive );
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtFieldgroup::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiFieldgroup::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtFieldgroup::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiFieldgroup::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* serializeContainerElements --                                               */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::serializeContainerElements( std::ostream &os ){
  GuiElementList::iterator el;
  for( el = m_container.begin(); el != m_container.end(); ++el ){
    (*el)->serializeXML(os);
  }
}

/* --------------------------------------------------------------------------- */
/* serializeContainerElements --                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldgroup::serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated){
  GuiElementList::iterator el;
  bool ret(false);
  for( el = m_container.begin(); el != m_container.end(); ++el ){
    Json::Value jsonElem = Json::Value(Json::arrayValue);
    ret |= (*el)->serializeJson(jsonElem, onlyUpdated);
    jsonObj.append(jsonElem);
  }
  return ret;
}
#if HAVE_PROTOBUF
bool GuiQtFieldgroup::serializeContainerElements(in_proto::ElementList* eles, in_proto::FieldGroup* element, bool onlyUpdated) {
  GuiElementList::iterator el;
  bool ret(false);
  for( el = m_container.begin(); el != m_container.end(); ++el ){
    Json::Value jsonElem = Json::Value(Json::arrayValue);
    auto line = static_cast<GuiQtFieldgroupLine*>(*el);
    ret |= line->serializeProtobuf(eles, element, onlyUpdated);
  }
  return ret;
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleElement --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::getVisibleElement(GuiElementList& res) {
  for( GuiElementList::iterator iter = m_container.begin(); iter != m_container.end(); ++iter ){
    (*iter)->getVisibleElement( res );
  }
}

/* --------------------------------------------------------------------------- */
/* getElement --                                                               */
/* --------------------------------------------------------------------------- */

GuiElement *GuiQtFieldgroup::getElement(){
  return this;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::getSize(int &w, int &h){
  w = 0; h = 0;
  if (myWidget() && !myWidget()->isVisible()) return;
  GuiQtElement::getSize(w, h);
  BUG_DEBUG("getSize: " << getName() << "  ["<<w << ", " << h << "]");
}

/* --------------------------------------------------------------------------- */
/* slot_accordian --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroup::slot_accordian(bool checked){
  for(auto c : m_container){
    for(auto it : static_cast<GuiQtFieldgroupLine*>(c)->m_elements)
      it->getQtElement()->myWidget()->setVisible(checked);
  }
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtFieldgroup::myWidget(){
  if( withScrollbars() && m_scrollview ) return m_scrollview;
  return m_qgroupbox;
}

/* --------------------------------------------------------------------------- */
/* getExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

Qt::Orientations GuiQtFieldgroup::getExpandPolicy() {
  int ed = 0;
  GuiElementList::iterator iter;

  for( iter = m_container.begin(); iter != m_container.end(); ++iter ){
    ed |= (int) (*iter)->getQtElement()->getExpandPolicy();
  }
  return ( Qt::Orientations ) ed;
}

/* --------------------------------------------------------------------------- */
/* getContainerExpandPolicy --                                                 */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtFieldgroup::getContainerExpandPolicy() {
  int ed = 0;
  GuiElementList::iterator iter;

  for( iter = m_container.begin(); iter != m_container.end(); ++iter ){
    ed |= (int) (*iter)->getQtElement()->getContainerExpandPolicy();
  }
  return (GuiElement::Orientation) ed;
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtFieldgroup::getDialogExpandPolicy() {
  int ed = 0;

  GuiElementList::iterator iter;
  for( iter = m_container.begin(); iter != m_container.end(); ++iter ){
    ed |= (int) (*iter)->getQtElement()->getDialogExpandPolicy();
  }
  return (GuiElement::Orientation) ed;
}

/* --------------------------------------------------------------------------- */
/* setScrollbar --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtFieldgroup::setScrollbar( ScrollbarType sb ){
  m_container.setScrollbar( sb );
}
