
#include <algorithm>

#include <QTableView>
#include <QResizeEvent>
#include <QHeaderView>
#include <QScrollBar>
#include <QLayout>
#include <QLabel>
#include <QStandardItemModel>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QApplication>
#include <QtGui/QClipboard>
#include <QtCore/QSettings>
#include <QCheckBox>
#include <QPushButton>
#include <QList>

#include "utils/Debugger.h"
#include "utils/Date.h"
#include "utils/gettext.h"
#include "utils/HTMLConverter.h"

#include "gui/GuiFactory.h"
#include "gui/qt/GuiQtScrolledText.h"
#include "gui/qt/GuiQtList.h"
#include "gui/qt/GuiQtTableViewBase.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtPrinterDialog.h"
#include "gui/GuiIndex.h"
#include "gui/GuiSeparator.h"
#include "gui/GuiMenuButton.h"
#include "gui/UnitManager.h"

#include "xfer/XferDataParameter.h"
#include "job/JobFunction.h"
#include "job/JobStarter.h"
#include "app/DataSet.h"
#include "app/ColorSet.h"
#include "streamer/StreamManager.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtList::GuiQtList(GuiElement *parent, const std::string &name)
  : GuiQtElement(parent, name)
  , m_frame( 0 )
  , m_tablewidget(0)
  , m_configMenu( this )
  , m_sort_column(0)
  , m_sort_block(false)
{
}

GuiQtList::GuiQtList( GuiQtList& list)
  : GuiQtElement(list), GuiList(list)
  , m_frame( 0 )
  , m_tablewidget(0)
  , m_configMenu( list.m_configMenu )
  , m_clonedList( list.m_clonedList )
  , m_sort_column(list.m_sort_column)
  , m_sort_criteria(list.m_sort_criteria)
  , m_sort_block(list.m_sort_block)
  , m_selectedIdxs(list.m_selectedIdxs)
{
}

GuiQtList::~GuiQtList() {
}

/*=============================================================================*/
/* public member functions from GuiQtList                                      */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */
GuiElement* GuiQtList::clone() {
  GuiElement* baseElem = findElement( getName() );
  if (baseElem == this->getElement())
    m_clonedList.push_back( new GuiQtList( *this ) );
  else
    return baseElem->clone();
  return m_clonedList.back();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtList::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  cList.insert(cList.begin(), m_clonedList.begin(), m_clonedList.end());
}

/*=============================================================================*/
/* public member functions of GuiQtElement                                       */
/*=============================================================================*/
void GuiQtList::create() {
  QWidget *parent = getParent()->getQtElement()->myWidget();
  assert( parent != 0 );
  QVBoxLayout* vboxlayout;

  // new frame with layout
  assert( m_frame == 0);
  m_frame = new QFrame();

  // set list font
  QFont font =  m_frame->font();
  m_frame->setFont( QtMultiFontString::getQFont( "@list@", font ) );

  // has optional cloumns?
  ColsVector::iterator iter;
  for(iter = m_cols.begin(); iter != m_cols.end(); ++iter ) {
    if ((*iter)->isOptional()) {
      setResizeable(true);
      break;
    }
  }

  // create layouts
  if( getTitle().size() ){
    QHBoxLayout* hboxlayout = new QHBoxLayout();
    hboxlayout->setContentsMargins(0,0,0,0);
    hboxlayout->setSpacing( 5 );
    m_frame->setLayout( hboxlayout );

    vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    hboxlayout->addLayout( vboxlayout );
    if (!isResizeable())
      hboxlayout->addStretch(1);

  } else {
    vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing( 5 );
    m_frame->setLayout( vboxlayout );
  }

  // add Title
  if( getTitle().size() ){
    Qt::AlignmentFlag align = getQtAlignment(getTitleAlignment());
    QLabel* titlewidget = new QLabel(QString::fromStdString(getTitle()), m_frame);

    vboxlayout->addWidget( titlewidget, 0, align|Qt::AlignTop );
  }

  // create table (view +  model)
  QStandardItemModel *model = new QtStandardItemModel(0, m_cols.size(), this);
  m_tablewidget = new GuiQtTableViewBase(model, this, NULL, new QtHeaderView(this, Qt::Horizontal));
  m_tablewidget->setFont( QtMultiFontString::getQFont( "@list@", font ) );
  m_tablewidget->setItemDelegate(new SpreadSheetDelegate(this, getCountDecimalPlace()));

  // set properties
  m_tablewidget->verticalHeader()->hide();
  m_tablewidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  m_tablewidget->setCornerButtonEnabled(true);
  m_tablewidget->horizontalHeader()->setSortIndicatorShown ( true );
  m_tablewidget->setEnabled( true );
  m_tablewidget->setEditTriggers( QAbstractItemView::NoEditTriggers );
  m_tablewidget->setSelectionMode(
	  (isMultipleSelection() ? QTableView::ExtendedSelection : QTableView::SingleSelection) );
  m_tablewidget->setSelectionBehavior ( QAbstractItemView::SelectRows );
  m_tablewidget->setSortingEnabled ( sortEnabled() );
  if (!GuiQtManager::Instance().isWindowsInDarkMode()) {
    m_tablewidget->setAlternatingRowColors ( true );
  }
  QObject::connect( m_tablewidget, SIGNAL(activated(const QModelIndex&)),
 		    this, SLOT( rowActivate(const QModelIndex&) ) );
  int stretchFac = isResizeable() ? 1000 : 0;
  vboxlayout->addWidget( m_tablewidget, stretchFac );
  int charlenStr = m_tablewidget->fontMetrics().horizontalAdvance( "W" );
  int charlenNb  = m_tablewidget->fontMetrics().horizontalAdvance( "0" );

  // set labels
  setLabels();

  if (isResizeable()) {
    // Minimale Listenbreite aus den Spaltenbreiten des Description Files berechnen
    int sumW=0;
    for(iter = m_cols.begin(); iter != m_cols.end(); ++iter ) {
      if ((*iter)->isOptional() || (*iter)->getWidth() == 0) {
        continue;
      }

      DataDictionary::DataType dataType;
      (*iter)->getDataType( dataType );
      int charlen = (dataType == DataDictionary::type_String ||
                     dataType == DataDictionary::type_CharData) ? charlenStr : charlenNb;
      sumW += GuiQtTableViewBase::COLUMN_MARGIN_WIDTH + charlen*(0.5+(*iter)->getWidth());
    }
    //?? + 1 sehr wichtig, weil Qt komischerweise intern zu 5% ein Pixel verliert
    int width = 2*m_tablewidget->frameWidth() + sumW + 1;
    m_tablewidget->setMinimumWidth( width + m_tablewidget->verticalScrollBar()->sizeHint().width());
  }

  // readSettings
  readSettings();

  // Breite der Spalten setzen
  int col = 0, row = 0;
  int sumW=0;
  for(iter = m_cols.begin(); iter != m_cols.end(); ++iter, col++ ) {
    if ((*iter)->isOptional() || (*iter)->getWidth() == 0) {
      m_tablewidget->hideColumn( col );
      continue;
    }

    DataDictionary::DataType dataType;
    (*iter)->getDataType( dataType );
    int charlen = (dataType == DataDictionary::type_String ||
		   dataType == DataDictionary::type_CharData) ? charlenStr : charlenNb;
    sumW += GuiQtTableViewBase::COLUMN_MARGIN_WIDTH + charlen*(0.5+(*iter)->getWidth());
    m_tablewidget->setColumnWidth( col, GuiQtTableViewBase::COLUMN_MARGIN_WIDTH +
				   charlen*(0.5+(*iter)->getWidth()) );
  }

  // set margins
  QMargins margins(m_tablewidget->contentsMargins());
  int left(margins.left());
  int top(margins.top());
  int right(margins.right());
  int bottom(margins.bottom());
  left = right = 0;
  m_tablewidget->setContentsMargins ( left, top, right, bottom );
  if (!isResizeable()) {
    m_tablewidget->setMaximumWidth(sumW+2*m_tablewidget->frameWidth() + 1 +
     				   m_tablewidget->verticalScrollBar()->sizeHint().width());
  }

  // width fixed, height flexible
  int rowh =  m_tablewidget->sizeHintForRow(1);
  if (GuiQtManager::listItemHeight() > 0) {
    rowh = GuiQtManager::listItemHeight();
  }
  m_tablewidget->verticalHeader()->setDefaultSectionSize( rowh );
  m_tablewidget->horizontalHeader()->setMaximumHeight(getHorizontalHeaderLines()*rowh);

  rowh =  m_tablewidget->sizeHintForRow(1);

  int height = 2*m_tablewidget->frameWidth() + tableSize() * rowh + rowh;
  //?? + 1 sehr wichtig, weil Qt komischerweise intern zu 5% ein Pixel verliert
  int width = 2*m_tablewidget->frameWidth() + sumW + 1;
  m_tablewidget->setMinimumHeight( height );
  if (isResizeable()) {
    m_frame->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding) );
  } else {
    m_tablewidget->setMinimumWidth( width + m_tablewidget->verticalScrollBar()->sizeHint().width());
    m_frame->setSizePolicy( QSizePolicy(QSizePolicy::Fixed,  QSizePolicy::MinimumExpanding) );
  }

  // create popup menu
  installPopupMenu();

  setDebugTooltip();

  m_tablewidget->setObjectName(QString::fromStdString(getName()));
}

GuiElement* GuiQtList::getElement() {
  return this;
}

//=============================================================================//
// openConfigDialog                                                            //
//=============================================================================//
void GuiQtList::openConfigDialog(const std::string name){
  QDialog dialog(myWidget());
  QGridLayout* layout = new QGridLayout();
  layout->setSpacing( 5 );

  // add Title label
  QLabel *label = new QLabel( QString::fromStdString(_("ColumnConfiguration") ) );
  if (name.size())
    label->setText( label->text() + "\n\n" + QString::fromStdString(name));
  QFont font = label->font();
  label->setFont( QtMultiFontString::getQFont( "@title@", font ) );
  layout->addWidget(label, 0, 0, 1, -1);
  layout->addWidget(new QWidget(), 1, 0, 1, -1);
  layout->setRowMinimumHeight ( 1, 10 );

  // column title
  label = new QLabel( QString::fromStdString(_("Name") ) );
  label->setFont( QtMultiFontString::getQFont( "@text@", font ) );
  layout->addWidget(label, 2, 0);
  label = new QLabel( QString::fromStdString(_("Visible") ) );
  label->setFont( QtMultiFontString::getQFont( "@text@", font ) );
  layout->addWidget(label, 2, 1);

  // fill columns
  std::vector<QCheckBox*> checkBoxList;
  ColsVector::iterator iter;
  int r= 3;
  for(iter = m_cols.begin(); iter != m_cols.end(); ++iter, ++r ){
    // ignore this columns
    if ((*iter)->getWidth() == 0) continue;

    // label
    QLabel *label = new QLabel( QString::fromStdString((*iter)->getLabel()) );
    font = label->font();
    label->setFont( QtMultiFontString::getQFont( "@text@", font ) );
    layout->addWidget(label, r, 0);

    // visible
    QCheckBox* toggle = new QCheckBox();
    toggle->setChecked( !(*iter)->isOptional() );
    layout->addWidget(toggle, r, 1);
    checkBoxList.push_back(toggle);
  }
  layout->setColumnMinimumWidth ( 0, 80 );

  // accept button
  QPushButton *accepteButton = new QPushButton(QString::fromStdString(_("Accept")), &dialog);
  layout->addWidget(accepteButton, r, 0);
  QObject::connect( accepteButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
  dialog.setLayout( layout );

  // close button
  QPushButton *closeButton = new QPushButton(QString::fromStdString(_("Close")), &dialog);
  layout->addWidget(closeButton, r, 1, 1, -1);
  QObject::connect( closeButton, SIGNAL(clicked()), &dialog, SLOT(reject()));

  // exec
  dialog.setMaximumSize( dialog.sizeHint() );
  int ret = dialog.exec();
  if (ret == 1) {

    // set new flags
    int col = 0;
    bool oneChecked = false;
    std::vector<QCheckBox*>::iterator it = checkBoxList.begin();
    int charlenStr = m_tablewidget->fontMetrics().horizontalAdvance( "W" );
    int charlenNb  = m_tablewidget->fontMetrics().horizontalAdvance( "0" );

    for (iter = m_cols.begin(); iter != m_cols.end(); ++iter, ++col) {
      // ignore this columns
      if ((*iter)->getWidth() == 0) continue;

      (*iter)->setOptional( !(*it)->isChecked() );
      if ((*it)->isChecked()) {
	DataDictionary::DataType dataType;
	(*iter)->getDataType( dataType );
	int charlen = (dataType == DataDictionary::type_String ||
		       dataType == DataDictionary::type_CharData) ? charlenStr : charlenNb;

	m_tablewidget->showColumn( col );
	m_tablewidget->setColumnWidth( col, charlen*(1+(*iter)->getWidth()) );
	oneChecked = true;
      } else
	m_tablewidget->hideColumn( col );

      ++it;
    }
    if (!oneChecked) {
      openConfigDialog(_("Minimum one column should be selected. "));
    }
  }
}

//=============================================================================//
// popup                                                                       //
//=============================================================================//
void GuiQtList::popup(){
  if( getPopupMenu() != 0 ) {
    getPopupMenu()->popup();
  }
}

//=============================================================================//
// manage                                                                      //
//=============================================================================//
void GuiQtList::manage() {
  BUG(BugGui,"GuiQtList::manage");

  m_tablewidget->clearSelection();
  myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
}

//=============================================================================//
// destroy                                                                     //
//=============================================================================//
bool GuiQtList::destroy() {
  return false;
}

//=============================================================================//
// update                                                                      //
//=============================================================================//
void GuiQtList::update( UpdateReason reason ) {
  BUG(BugGuiTable,"GuiQtList::update");
  if( myWidget() == 0 )
    return;

  switch( reason ){
  case reason_Process:
  case reason_FieldInput:
    if( !isDataItemUpdated( GuiManager::Instance().LastGuiUpdate() ) ){
      return;
    }
  case reason_Unit:
    updateItemUnits();
  case reason_Cycle:
  case reason_Always:
  case reason_Cancel:
    {
      BUG_MSG("numRows: " << getMaxRows() );
      ResetLastWebUpdated();  // reason_Always for webtens

      // selektion speichern, liste fuellen, wieder selektieren und sortieren
      // (Viel Code nur das die Selektion evtl.erhalten bleibt)
      int old_sb_pos = m_tablewidget->verticalScrollBar()->sliderPosition();
      QModelIndexList selIdxs =  m_tablewidget->selectedIndexes();
      QStringList old_rowValues, new_rowValues;  // old and new selected row values
      int oldId = -1;
      std::vector<int> oldIds;
      QVariant var;
	// special case, selection before creation
      if (m_selectedIdxs.size() && selIdxs.size() == 0) {
	oldIds = m_selectedIdxs;
	if (!isMultipleSelection())  oldId =  m_selectedIdxs[0];
      } else
      // normal case
      if (isMultipleSelection()) {
        bool ret;
        for (int i = 0; i < selIdxs.size(); ++i) {
          var = selIdxs.at(i).sibling(selIdxs.at(i).row(),0).data();
          if (var.isValid()) {
            int inx = var.toInt(&ret);
            if( ret == true ) {
              if (std::find(oldIds.begin(), oldIds.end(), inx) == oldIds.end()) oldIds.push_back(inx);
            }
          }
        }
      } else {
        if (selIdxs.size()) {
          var = selIdxs.first().sibling(selIdxs.first().row(),0).data();
          if (var.isValid()) {
            bool ret;
            int inx = var.toInt(&ret);
            if( ret == true ) {
              oldId = inx;
            }
            for (int i = 0; i < selIdxs.size(); ++i)
              old_rowValues << selIdxs.at(i).data().toString();
          }
        }
      }

      insertItemList();

      if (isMultipleSelection()) {
        std::vector<int>::iterator it =  oldIds.begin();
        if (oldIds.size())
          m_tablewidget->scroll_blocked_selectRows( oldIds );
      } else {
        if (oldId != -1 && oldId < m_tablewidget->model()->rowCount()) {
          QList<QStandardItem *> items = dynamic_cast<QStandardItemModel*>(m_tablewidget->model())->findItems ( QString::number(oldId) );
          if (items.size()) {
            m_tablewidget->scroll_blocked_selectRow( items.first()->row() );

            // check if selection is correct (old and new values are equal)
            selIdxs =  m_tablewidget->selectedIndexes();
            for (int i = 0; i < selIdxs.size(); ++i)
              new_rowValues << selIdxs.at(i).data().toString();
            if (new_rowValues != old_rowValues && old_rowValues.size()) {
              m_tablewidget-> clearSelection();
            }
          }
        }
      }

      // set labels
      if (hasModifiableLabels()) { // if labels are modifiable
        setLabels();
        int rowh =  m_tablewidget->sizeHintForRow(1);
        m_tablewidget->horizontalHeader()->setMaximumHeight(getHorizontalHeaderLines()*rowh);
      }

      m_sort_block = true;
      sortColumn(sortOrder());
      m_sort_block = false;
      if (old_sb_pos < m_tablewidget->verticalScrollBar()->maximum())
        m_tablewidget->verticalScrollBar()->setSliderPosition(old_sb_pos);
    }
    break;
  default:
    BUG_MSG("Unhandled Update Reason");
    break;
  }
}

QWidget* GuiQtList::myWidget(){
  return m_frame;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtList::setIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtList::setIndex",name << " =[" << inx << "]");
  ColsVector::iterator iter;
  for( iter = m_cols.begin(); iter != m_cols.end(); ++iter ){
    (*iter)->setIndex( name, inx );
  }
  update( reason_Always );

  // process clones
  std::vector<GuiQtList*>::iterator it =  m_clonedList.begin();
  for (; it != m_clonedList.end(); ++it)
    (*it)->setIndex(name, inx);
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtList::acceptIndex( const std::string &name, int inx ){
  BUG(BugGuiFld,"GuiQtList::acceptIndex");
  ColsVector::iterator iter;
  for( iter = m_cols.begin(); iter != m_cols.end(); ++iter ){
    if( !(*iter)->acceptIndex( name, inx ) ){
      return false;
    }
  }
  return true;
}

/*=============================================================================*/
/* private methods                                                             */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setLabels() --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtList::setLabels(){
  QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(m_tablewidget->model());
  int i=0;
  for(ColsVector::iterator iter = m_cols.begin(); iter != m_cols.end(); ++iter, ++i ) {
    // get max of multi row header labels
    int c =QtMultiFontString::getQString((*iter)->getLabel()).count("\n");
    if (c >= getHorizontalHeaderLines())
      setHorizontalHeaderLines(c+1);
    // set label
    std::string str((*iter)->getLabel());
    std::string ttp((*iter)->getTooltip());
    HTMLConverter::convertFromHTML(str);
    model->setHeaderData(i, Qt::Horizontal, QtMultiFontString::getQString(str) );
    model->horizontalHeaderItem(i)->setToolTip(QString::fromStdString(ttp));
  }
}

/* --------------------------------------------------------------------------- */
/* insertItemList --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQtList::insertItemList(){
  int i, maxRows = getMaxRows();

  QtStandardItemModel *model = dynamic_cast<QtStandardItemModel*>(m_tablewidget->model());
  bool append_mode = maxRows != model->rowCount();

  // append_row ist schneller als alles andere
  // nur wenn wenig oder keine Daten aendern kann man jede einzelne Zelle setzen
  if (append_mode)
    model->removeRows(0, model->rowCount());

  // Werte setzen
  model->beginResetModel(); // model reset !!!!
  model->endResetModel(); // model reset !!!!
  for(int r=0 ;r <maxRows;++r ) {
    QList<QStandardItem*> items;
    QVariant valueTP;
    QColor background, foreground;

    if (getToolTipColumn() != -1)
      valueTP = columnCellValue(getToolTipColumn(), r);
    if (getColorColumn() != -1)
      columnCellColor(getColorColumn(), r, foreground, background);

    for( int c=0;c<m_cols.size();++c ){
      if (getColorColumn() == -1)
        columnCellColor(c, r, foreground, background);
      if (c == getToolTipColumn() || c == getColorColumn()) continue;
      QVariant value = columnCellValue(c, r);
      if (append_mode) {
        QStandardItem *item = new QStandardItem();
        item->setData( value, Qt::DisplayRole );
        item->setData(valueTP, Qt::ToolTipRole);

        // set color
        if (background.isValid()) {
          item->setData(background, Qt::BackgroundRole);
          item->setData(foreground, Qt::ForegroundRole);
        }

        items.push_back(item);
      } else {
        QModelIndex index = model->index(r,c);
        if (model->data( index ) !=  value) {
          model->setData(index, value);
        }

        // set color
        if (background.isValid() && model->data( index, Qt::BackgroundRole) !=  background) {
          model->setData(index, background, Qt::BackgroundRole);
          model->setData(index, foreground, Qt::ForegroundRole);
        }

        // set tooltip
        if (model->data( index, Qt::ToolTipRole) != valueTP)
          model->setData(index, valueTP, Qt::ToolTipRole);
      }
    }
    //   model->insertRow(r, items);
    if (append_mode)
      model->appendRow(items);
  }
}

/* --------------------------------------------------------------------------- */
/* installPopupMenu --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtList::installPopupMenu(){
  if(hasOptionalColumns()) {
    if( !getPopupMenu() )
      GuiElement::newPopupMenu();
    GuiQtPopupMenu* menu = dynamic_cast<GuiQtPopupMenu*>(getPopupMenu());
    if(menu->getTitle() != _("ListMenu")){
      menu->setTitle( _("ListMenu") );
      getPopupMenu()->setTitle( _("ListMenu") );
      menu->attach( GuiFactory::Instance()->createSeparator( menu )->getElement() );
      GuiMenuButton *button = GuiFactory::Instance()->createMenuButton( menu->getElement(), &m_configMenu );
      button->setLabel( _("Column Configuration") );
      menu->attach( button->getElement() );
    }
  }
  if( getPopupMenu() ) {
    getPopupMenu()->resetMenuPost();
    getPopupMenu()->getElement()->getQtElement()->create();
  }
}


/* --------------------------------------------------------------------------- */
/* sortColumn --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtList::sortColumn(int column) {
  if(column == 0 || !m_tablewidget) {
    // Spalte 0 ist der versteckte Index.
    // Diese kann übers GUI nicht selektiert werden.
    // Es soll daher keine Sortierung gemacht werden.
    return;
  }
  // unschön, aber nur so wird das sortieren auch wirklich gemacht
  m_tablewidget->setSortingEnabled(false);
  m_tablewidget->setSortingEnabled( sortEnabled() );
  // sortieren
  if (column > 0) {
    m_tablewidget->sortByColumn( column, Qt::AscendingOrder );
  } else {
    m_tablewidget->sortByColumn( -column, Qt::DescendingOrder );
  }
  // clones sortieren
  for (std::vector<GuiQtList*>::iterator it =  m_clonedList.begin();
       it != m_clonedList.end(); ++it) {
    if ((*it)->m_tablewidget){
      if (column > 0) {
        (*it)->m_tablewidget->sortByColumn( column, Qt::AscendingOrder );
      } else {
        (*it)->m_tablewidget->sortByColumn( -column, Qt::DescendingOrder );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* updateItemUnits --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtList::updateItemUnits() {
  DataDictionary::DataType dataType;
  for(auto col: m_cols) {
    updateScale(col->getParam());
  }
}

/* --------------------------------------------------------------------------- */
/* rowActivate --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtList::rowActivate(const QModelIndex& index) {

  // maybe toggle selection => unselect selected row
  if (m_selectedIdxs.size()) {
    std::vector<int>::iterator it = std::find(m_selectedIdxs.begin(),
                                              m_selectedIdxs.end(), index.row());
    if (it != m_selectedIdxs.end() && !isMultipleSelection())
      m_tablewidget->clearSelection();
  }


  if(getFunction() == 0) return;
  QVariant var = index.sibling(index.row(),0).data();
  if (var.isValid()) {
    bool ret;
    int inx = var.toInt(&ret);
    if( ret == false ) return;
    JobStarter *starter = new GuiListTrigger(getFunction());
    starter->setReason( JobElement::cll_Activate );
    starter->setIndex( inx );
    starter->startJob();
  }

  // get new selected row list
  getSelectedRows(m_selectedIdxs);
}


/* --------------------------------------------------------------------------- */
/* rowSelect --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtList::rowSelect(const QModelIndex& index) {
  m_tablewidget->scrollTo(index, QAbstractItemView::EnsureVisible);

  GuiElement* baseElem = findElement( getName() );
  if (baseElem == this->getElement())
    for(auto it: m_clonedList)
      it->selectRow(index.row(), false, true);
  else
    baseElem->getList()->selectRow(index.row(), true, true);

  if (getFunction() == 0) return;
  QVariant var = index.sibling(index.row(),0).data();
  if (var.isValid()) {
    bool ret;
    int inx = var.toInt(&ret);
    if( ret == false ) return;
    JobStarter *starter = new GuiListTrigger(getFunction());
    starter->setReason( JobElement::cll_Select );
    starter->setIndex( inx );
    starter->startJob();
  }
}

/* --------------------------------------------------------------------------- */
/* rowUnselect --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtList::rowUnselect(const QModelIndex& index) {
  if (getFunction() == 0) return;
  QVariant var = index.sibling(index.row(),0).data();
  if (var.isValid()) {
    bool ret;
    int inx = var.toInt(&ret);
    if( ret == false ) return;
    JobStarter *starter = new GuiListTrigger(getFunction());
    starter->setReason( JobElement::cll_Unselect );
    starter->setIndex( inx );
    starter->startJob();
  }
}

/* --------------------------------------------------------------------------- */
/* sortOrder --                                                                */
/* --------------------------------------------------------------------------- */
int GuiQtList::sortOrder(){

  if( m_tablewidget && m_tablewidget->isSortingEnabled() && m_tablewidget->horizontalHeader()) {
     int column = m_tablewidget->horizontalHeader()->sortIndicatorSection();
     m_sort_column = (m_tablewidget->horizontalHeader()->sortIndicatorOrder() == Qt::AscendingOrder) ? column : -column;
     std::ostringstream os;
     if (col(column) && col(column)->getParam()) {
       int order =  m_sort_column < 0 ? -1 : 1;
       os << col(column)->getParam()->getName() << ":" << order;
       m_sort_criteria = os.str();
     }

  }
  return m_sort_column;
}

/* --------------------------------------------------------------------------- */
/* selectRow --                                                                */
/* --------------------------------------------------------------------------- */
bool GuiQtList::selectRow( int row, bool recursive, bool interactiveSelect ) {

  if ( m_tablewidget ) {
    // gui update
    update(reason_Process); //Always);
    m_tablewidget->raise();  // sehr wichtig!!!
  }

  // clones selektieren
  if (recursive)
    for (std::vector<GuiQtList*>::iterator it =  m_clonedList.begin();
	 it != m_clonedList.end(); ++it)
      (*it)->selectRow( row, false, interactiveSelect);

  if (interactiveSelect || !m_tablewidget) {
    m_selectedIdxs.clear();
    m_selectedIdxs.push_back(row);
    return true;
  }

  int oldSel =  m_tablewidget->selectedIndexes().size() ?  m_tablewidget->selectedIndexes().first().row() : -1;
  bool b;
  if( row >=0 && row < m_tablewidget->model()->rowCount() ) {
    if (m_tablewidget->model()->index(row, 0).isValid()) {
      bool b;
      if (row == m_tablewidget->model()->index(row, 0).data().toInt(&b))  {// keine Sortierung
        if (oldSel != -1 && row == oldSel) {
          return true;
        }
        m_tablewidget->blocked_selectRow( row );
        m_tablewidget->scrollTo( m_tablewidget->model()->index(row, 0), QAbstractItemView::EnsureVisible);
      }
      else {
	QList<QStandardItem *> items = dynamic_cast<QStandardItemModel*>(m_tablewidget->model())->findItems ( QString::number(row) );
	int rowSel(items.first()->row());
        if (oldSel != -1 && rowSel == oldSel) {
          return true;
        }
        m_tablewidget->blocked_selectRow(rowSel);
        m_tablewidget->scrollTo( m_tablewidget->model()->index(rowSel, 0), QAbstractItemView::EnsureVisible);
      }
      return true;
    }
  }
  else
    if (m_tablewidget)
      m_tablewidget->clearSelection();

  return true;
}

/* --------------------------------------------------------------------------- */
/* selectRows --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtList::selectRows( std::vector<int> rows, bool recursive ) {

  // clones selektieren
  if (recursive)
    for (std::vector<GuiQtList*>::iterator it =  m_clonedList.begin();
	 it != m_clonedList.end(); ++it)
      (*it)->selectRows( rows, false );

  if ( !m_tablewidget ) {
    m_selectedIdxs = rows;
    return true;
  }

  // maybe sorted => get list index for sorting
  std::vector<int> r;
  std::vector<int>::iterator it = rows.begin();
  for (;it != rows.end(); ++it) {
    QList<QStandardItem *> items = dynamic_cast<QStandardItemModel*>(m_tablewidget->model())->findItems ( QString::number(*it) );
    if (items.size())
      r.push_back( items.first()->row() );
  }
  QWidget *focusWidget = QApplication::focusWidget();
  m_tablewidget->setFocus();
  m_tablewidget->blocked_selectRows( r );
  if (focusWidget) focusWidget->setFocus();
  m_tablewidget->update();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSelectedRows --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiQtList::getSelectedRows(std::vector<int>& idxs) {
  idxs.clear();

  if ( !m_tablewidget ) {
    // m_selectedIdxs is correct as long as m_tablewidget is not yet created
    std::copy(m_selectedIdxs.begin(), m_selectedIdxs.end(),
              std::back_inserter(idxs));
    return true;
  }
  // get all selected rows
  QModelIndexList idxList = m_tablewidget->selectedIndexes();
  for (int i = 0; i < idxList.size(); ++i) {
    long row = idxList.at(i).row();
    int id = idxList.at(i).sibling(row,0).data().toInt();
    std::vector<int>::iterator it = std::find(idxs.begin(), idxs.end(), id);
    if (!i || it == idxs.end())
      idxs.push_back( id );
  }
  // sort
  std::sort(idxs.begin(), idxs.end());
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSortCriteria --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiQtList::getSortCriteria(std::string& sortCriteria) {
  // not created, we read settings
  if (m_sort_criteria.empty()) readSettings();

  // get sort criteria
  std::ostringstream os;
  int column = abs(m_sort_column);
  if (m_sort_column > 0 &&
      col(column) && col(column)->getParam()){
    int order =  m_sort_column < 0 ? -1 : 1;
    os << col(column)->getParam()->getName() << ":" << order;
    sortCriteria = os.str();
    return true;
  } else if (m_sort_criteria.size()) {
    sortCriteria = m_sort_criteria;
    return true;
  }

  // not found
  sortCriteria.clear();
  return false;
}

/* --------------------------------------------------------------------------- */
/* resetSortCriteria --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtList::resetSortCriteria() {
  m_sort_column = 0;
  m_sort_criteria.clear();
  JobStarter *starter = new GuiListTrigger(getFunction());
  starter->setReason( JobElement::cll_Sort );
  starter->setSortCriteria("");
  starter->startJob();
}

/* --------------------------------------------------------------------------- */
/* clearSelection --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQtList::clearSelection() {
  BUG(BugGui,"GuiQtList::clearSelection");
  if (m_tablewidget)
    m_tablewidget->clearSelection();
  else
    m_selectedIdxs.clear();

  for(auto it: m_clonedList)
    it->clearSelection();
}

/* --------------------------------------------------------------------------- */
/* sort --                                                                     */
/* --------------------------------------------------------------------------- */
void GuiQtList::sort(int column, int order) {
  if (m_sort_block || getFunction() == 0 || column == 0) return;
  JobStarter *starter = new GuiListTrigger(getFunction());
  starter->setReason( JobElement::cll_Sort );
  std::ostringstream os;
  if (col(column) && col(column)->getParam()) {
    os << col(column)->getParam()->getName() << ":" << order;
    starter->setSortCriteria( os.str() );
    m_sort_criteria = os.str();
  }
  starter->startJob();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtList::serializeXML(std::ostream &os, bool recursive){
  GuiList::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtList::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return GuiList::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtList::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiList::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtList::getVisibleDataPoolValues( GuiValueList& vmap ) {

  QVariant s;
  int rows = getMaxRows();

  int cols = m_cols.size();
//   int rows = m_currentItemList.size();
  for( int c=0;c<cols;++c ){
    for( int r=0;r<rows;++r ){
      s = columnCellValue(c, r);
      //      s = m_cols[ c ]->cellValue( r );
      if ( m_cols[ c ]->getParam()){
	std::string value = s.toString().trimmed().toStdString()+"@false";
	vmap.insert( GuiValueList::value_type(m_cols[ c ]->getParam()->getFullName(), value ) );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getStream --                                                          */
/* --------------------------------------------------------------------------- */

Stream* GuiQtList::getStream() {
  Stream *in = StreamManager::Instance().newStream();

  int cols = m_cols.size();
  for( int c=0;c<cols;++c ){
    XferDataParameter *param = m_cols[c]->getParam();
    if (!param) continue;
    if( param->DataItem()->getNumOfWildcards() == 1 ){
      in->beginGroup();
      XferDataItemIndex *index = 0;
      index = param->DataItem()->getDataItemIndexWildcard( 1 );
      assert( index != 0 );
      index->setIndexName( "@row_idx@","" );

      StreamParameter *s = in->addParameter( param->DataItem(), 0, -1, 0, false, false, false );
      in->endGroup();
      in ->addToken( "\n", 1, false);
    }
  }
  return in;
}
/* --------------------------------------------------------------------------- */
/* writeSettings --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtList::writeSettings() {
  // only with optional columns defined in DesFile will be written
  if( !m_tablewidget )
    return;

  // write settings
  QSettings *settings = GuiQtManager::Settings();
  settings->beginGroup( QString::fromStdString("List") );
  QString _tmp = QString::fromStdString(getName());
  if (hasOptionalColumns()) {
    QList<QVariant> widthList;
    QList<QVariant> optionalList;
    int col=0;
    int charlenStr = m_tablewidget->fontMetrics().horizontalAdvance( "W" );
    int charlenNb  = m_tablewidget->fontMetrics().horizontalAdvance( "0" );

    ColsVector::iterator iter;
    for(iter = m_cols.begin(); iter != m_cols.end(); ++iter, col++ ) {
      DataDictionary::DataType dataType;
      (*iter)->getDataType( dataType );
      int charlen = (dataType == DataDictionary::type_String ||
                     dataType == DataDictionary::type_CharData) ? charlenStr : charlenNb;
      int w = m_tablewidget->isColumnHidden(col) ? (*iter)->getWidth() :
        (m_tablewidget->columnWidth(col) - GuiQtTableViewBase::COLUMN_MARGIN_WIDTH)/charlen;
      if (__debugLogger__ && w > 200) {
        BUG_DEBUG("ColumnWidth is huge: columnWidth("<<col<<") = "<<m_tablewidget->columnWidth(col)
                  << "charlen: "<< charlen);
      }

      // mindeständerung ist "2", max breite 100
      widthList.push_back( w ? (std::abs(w-(*iter)->getWidth()) >= 2 && w <= 100  ?
                                w : (*iter)->getWidth()) : 0 );
      optionalList.push_back((*iter)->isOptional());
    }
    settings->setValue(_tmp + ".optionalColumn", optionalList);
    settings->setValue(_tmp + ".widthColumn", widthList);
  }
  if (sortOrder() != 0) {
    settings->setValue(_tmp + ".sortOrder", sortOrder());
  } else {
    settings->remove(_tmp + ".sortOrder");
  }
  settings->value(_tmp + ".sortCriteria", m_sort_criteria.c_str());
  settings->endGroup();
}
/* --------------------------------------------------------------------------- */
/* readSettings --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtList::readSettings(){
  BUG( BugGuiTable, "readSettings" );
  QSettings *settings = GuiQtManager::Settings();
  settings->beginGroup( QString::fromStdString("List") );
  QString _tmp = QString::fromStdString(getName());
  if(hasOptionalColumns()) {
    // only with optional columns defined in DesFile will be read
    //    assert(m_tablewidget);
    QList<QVariant> optionalList = settings->value(_tmp + ".optionalColumn").toList();
    QList<QVariant> widthList = settings->value(_tmp + ".widthColumn").toList();

    if (m_cols.size() == optionalList.size()) {
      for (int i = 0; i < optionalList.size(); ++i) {
	if (col(i)) col(i)->setOptional(optionalList.at(i).toBool());
      }
    }
    if (m_cols.size() == widthList.size()) {
      for (int i = 0; i < widthList.size(); ++i) {
        if (col(i)) col(i)->setWidth(std::min(100, widthList.at(i).toInt()));
        int w = widthList.at(i).toInt();
        if (__debugLogger__ && w > 100) {
          BUG_DEBUG("ColumnWidth is huge: column("<<i<<") = "<<w);
        }
      }
    }
  }

  // read sortOrder
  m_sort_column = settings->value(_tmp + ".sortOrder", 0).toInt();
  m_sort_criteria = settings->value(_tmp + ".sortCriteria", "").toString().toStdString();
  sortColumn(m_sort_column);
  settings->endGroup();
}

/* --------------------------------------------------------------------------- */
/* columnCellValue --                                                          */
/* --------------------------------------------------------------------------- */

QVariant GuiQtList::columnCellValue(int icol, int idx){
  std::string value;
  if( col(icol)->getParam() != 0 ){
    XferDataParameter *param = col(icol)->getParam();
    XferDataItem *dataItem   = param->DataItem();
    XferDataItemIndex *index = dataItem->getDataItemIndexWildcard( 1 );
    dataItem->setIndex( index, idx );
    param->getFormattedValue( value );

    // falls dataset
    std::string str;
    if(dataItem->getDataSetInputValue(str, value)) {
      return QString::fromStdString(str).trimmed();
    }

    // empty
    if (value.empty()){
      return QString::fromStdString("");  // empty string
    }

    // Real with fixed Length and Precision
    if (param->DataItem()->Data()->getDataType() == DataDictionary::type_Real ){
      if( param->getLength() > 0 && param->getPrecision() >= 0 ){
        return QString::fromStdString(value).trimmed();
      }
    }

    // normalfall
    QString qvalue = QString::fromStdString(value).trimmed();

    if (dataItem->Data()->getDataType() == DataDictionary::type_Real &&
        RealConverter::decimalPoint() == ',') {
      qvalue.replace(',','.');
    }

    QVariant variant(qvalue);

    switch( dataItem->Data()->getDataType() ) {
    case DataDictionary::type_String:
      {
        UserAttr::STRINGtype dtype = dataItem->getUserAttr()->StringType();
        switch(dtype) {
        case UserAttr::string_kind_date:
          return QDate::fromString( qvalue, Qt::ISODate);
        case UserAttr::string_kind_time:
          return QTime::fromString( qvalue, Qt::ISODate);
        case UserAttr::string_kind_datetime:
          {
            QDateTime dt = QDateTime::fromString(qvalue, Qt::ISODate);
#if QT_VERSION < 0x050000
            // qt4 does not convert timezones (correctly)
            // the following code helps qt4 to convert utc to localtime
            if(qvalue.endsWith("+0000")) {
              dt.setTimeSpec(Qt::UTC);
            }
#endif
            return dt.toLocalTime();
          }
        default:
          return qvalue;
        }
      }

    case DataDictionary::type_Real:
      {
        bool ret(false);
        double num;
        if (value.size()) {
          if (variant.canConvert<double>()) {
#if QT_VERSION >= 0x060000
            if (variant.convert(QMetaType(QMetaType::Double)))
              return variant;
#else
            if (variant.convert(QVariant::Double))
              return variant;
#endif
          }
          return qvalue;
        }
      }

    case DataDictionary::type_Integer:
      {
        bool ret(false);
        int num;
        if (value.size())
          num = qvalue.toInt(&ret);
        if (dataItem->getUserAttr()->IsToggle()) {
          if (ret && num) {
            QString s(col(icol)->getWidth(), ' ');
            s.replace(col(icol)->getWidth()/2, 1, "☉");
            return s;
          }
          else return "";
        }
        if (ret)
          return num;
        else return "";
      }
    default:
      return QString::fromStdString(value);
    }
  }
  else{
    std::ostringstream os;
    os << idx;
    value = os.str();
    return idx;
  }
}

/* --------------------------------------------------------------------------- */
/*  -- cellColor                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtList::columnCellColor(int icol, int idx,  std::string& foregroundStr,  std::string& backgroundStr){
  QColor foreground(QString::fromStdString(foregroundStr));
  QColor background(QString::fromStdString(backgroundStr));
  bool ret = columnCellColor(icol, idx, foreground, background);
  if (ret){
    foregroundStr = foreground.name().toStdString();
    backgroundStr = background.name().toStdString();
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/*  -- cellColor                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtList::columnCellColor(int icol, int idx, QColor& foreground, QColor& background){
  std::string value;
  double dvalue;
  BUG_PARA(BugGui,"GuiQtList::Column::cellColor", idx);
  // reset color
  foreground = QColor();
  background = QColor();

  if( col(icol)->getParam() != 0 ){
    XferDataItem *dataItem = col(icol)->getParam()->DataItem();
    XferDataItemIndex *index = dataItem->getDataItemIndexWildcard( 1 );
    dataItem->setIndex( index, idx );
    ColorSet *colorset = DataPoolIntens::Instance().getColorSet(dataItem->getUserAttr()->ColorsetName());
    if (!colorset) {
      return GuiQtDataField::getStandardColor(*col(icol)->getParam(), background, foreground);
    }

    ColorSetItem *colorsetItem;
    // string type, get simple coloritem
    if ( col(icol)->getParam()->DataItem()->getDataType() == DataDictionary::type_String) {
      bool ret = col(icol)->getParam()->DataItem()->getValue( value );
      colorsetItem = ret ? colorset->getColorItem( value ) : colorset->getColorItemInvalid();
    } else {
      bool ret = col(icol)->getParam()->DataItem()->getValue( dvalue );
      // if no item always, we try to interpolate colors
      if (ret && colorset->getColorItemAlways() == 0) {
        std::string bg, fg;
        if (colorset->getInterpolatedColors( dvalue, bg, fg )) {
          foreground = QColor(QString::fromStdString(fg));
          background = QColor(QString::fromStdString(bg));
          return true;
        }
      }
      // finally general case
      colorsetItem = ret ? colorset->getColorItem( dvalue ) : colorset->getColorItemInvalid();
    }

    if (colorsetItem) {
      foreground = QColor(QString::fromStdString(colorsetItem->foreground()));
      background = QColor(QString::fromStdString(colorsetItem->background()));
    }
    return colorsetItem ? true : false;
  }
  return false;
}
