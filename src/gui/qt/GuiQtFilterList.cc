
#include <QTableView>
#include <QHeaderView>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QStringList>
#include <qlayout.h>
#include <qlabel.h>
#include <qfont.h>
#include <qpainter.h>
#include <cmath>

#include "gui/qt/GuiQtFilterList.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/Debugger.h"
#include "utils/HTMLConverter.h"
#include "utils/NumLim.h"

class MyFilterQTableView : public QTableView {
public:
   // sizeHint ueberschreiben
   virtual QSize sizeHint() const {
     QSize size = QTableView::sizeHint();
     size.setHeight( QTableView::minimumSizeHint().height() );
     return size;
   }
};


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtFilterList::GuiQtFilterList( GuiElement *parent, GuiFilterListListener &listener, const std::string &name )
: GuiFilterList(listener), GuiQtElement( parent, name )
  , m_frameWidget( NULL )
  , m_listWidget( NULL ) {
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/*-----------------------------------------------------------------------------*/
/* SLOT: rowClicked --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtFilterList::rowClicked(const QModelIndex& index) { // int row, int col) {
  BUG(BugGuiTable,"GuiQtFilterList::rowClicked");
  QVariant var = index.sibling(index.row(),0).data();
  if (var.isValid()) {
    bool ret;
    int idx = var.toInt(&ret);
    if( ret == false ) return;
    if (idx >= 0 && idx < m_rows.size() ) {
      getListener().activated( QString::fromStdString( m_rows[idx].back()).toInt() );
    }
  }
}

/*-----------------------------------------------------------------------------*/
/* getSelection --                                                             */
/* --------------------------------------------------------------------------- */
int GuiQtFilterList::getSelection() {
  if ( m_listWidget && m_listWidget->selectionModel() )
    if (m_listWidget->selectionModel ()->selectedRows().size() == 1)
      return m_listWidget->selectionModel ()->selectedRows().at(0).row();
  return -1;
}

/*-----------------------------------------------------------------------------*/
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */
QWidget* GuiQtFilterList::myWidget() {
  return m_frameWidget;
}

/*-----------------------------------------------------------------------------*/
/* setLables --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtFilterList::setLabels(){
  BUG(BugGuiTable,"GuiQtFilterList::setLabels");
  GuiFilterListListener::StringsVector labels;
  GuiFilterListListener::StringsVector::iterator iter;
  if(getListener().getLabels( labels ) ){
    labels.insert( labels.begin(), "Index" );
    int i=0;
    QHeaderView* horzHeader = m_listWidget->horizontalHeader();
    QFont font = horzHeader->font();
    horzHeader->setFont( QtMultiFontString::getQFont( "@listtitle@", font ) );
    QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(m_listWidget->model());
    for(iter = labels.begin(); iter != labels.end(); ++iter, ++i ){
      std::string str((*iter));
      HTMLConverter::convertFromHTML(str);
      model->setHeaderData(i, Qt::Horizontal, QString::fromStdString(str));
    }
  }
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtFilterList::create() {
  BUG(BugGuiTable,"GuiQtFilterList::create");
  QWidget *parent = getParent()->getQtElement()->myWidget();
  assert( parent != 0 );

  // new frame with layout
  assert( m_frameWidget == 0);
  m_frameWidget = new QFrame( parent );
  QVBoxLayout* vboxlayout = new QVBoxLayout();
  vboxlayout->setContentsMargins(5,5,5,5);
  vboxlayout->setSpacing( 5 );
  m_frameWidget->setLayout( vboxlayout );

  // set list font
  QFont font =  m_frameWidget->font();
  m_frameWidget->setFont( QtMultiFontString::getQFont( "@list@", font ) );

  // add Title
  m_titleWidget = new QLabel("", m_frameWidget);
  vboxlayout->addWidget( m_titleWidget, 0, Qt::AlignCenter|Qt::AlignTop );

  getListConf();

  // create table (view +  model)
  m_listWidget = new MyFilterQTableView();
  QStandardItemModel *model = new QStandardItemModel(0, getColSize()+1, m_listWidget);
  m_listWidget->setModel( model );

  // set properties
  m_listWidget->verticalHeader()->hide();
  m_listWidget->horizontalHeader()->setSortIndicatorShown ( true );
  m_listWidget->setEnabled( true );
  m_listWidget->setEditTriggers( QAbstractItemView::NoEditTriggers );
  m_listWidget->setSelectionMode( QTableView::SingleSelection );
  m_listWidget->setSelectionBehavior ( QAbstractItemView::SelectRows );
  m_listWidget->setSortingEnabled ( true );
  m_listWidget->setAlternatingRowColors ( true );

  vboxlayout->addWidget( m_listWidget, 1000);
  vboxlayout->addStretch(1);
  QObject::connect( m_listWidget, SIGNAL(activated(const QModelIndex&)), this, SLOT(rowClicked(const QModelIndex&)) );

  // width Fixed, height flexible
  m_frameWidget->setSizePolicy( QSizePolicy(  QSizePolicy::Fixed,  QSizePolicy::MinimumExpanding )  );

  //Die folgenden Zeilen sind nur fuer die Bestimmung der Anfangsgroesse notwendig.
  setTitle();
  setLabels();
  doListConfig();
  clear();

startTimer(2000);
}


void GuiQtFilterList::timerEvent(QTimerEvent* e) {
  if (m_listWidget && m_listWidget->model() && getSearchMask().size()) {
    QStringList list( QString::fromStdString(getSearchMask()).split("&") );
    std::map< int, std::string> search_map;

    // search for Labels
    GuiFilterListListener::StringsVector::iterator iter;
    GuiFilterListListener::StringsVector labels;
    getListener().getLabels( labels );

    QStringListIterator siter(list);
    while ( siter.hasNext() ) {
      std::string _s = siter.next().toStdString();
      std::string::size_type pos = _s.find_first_of("=");
      if (pos == std::string::npos || pos+1 >= _s.size())  // shit
	continue;
      std::string _token = _s.substr(0, pos);
      std::string _value = _s.substr(pos+1);
      int i=0;
      for(iter = labels.begin(); iter != labels.end(); ++iter, ++i ){
	if ((*iter) == _token ) {
	  search_map[i] = _value;
	}
      }
    }

    // search whished row
    int row, column;
    GuiFilterListListener::RowsVector::iterator r_iter;
    GuiFilterListListener::StringsVector::iterator i_iter;
    for(r_iter=m_rows.begin(), row=0;r_iter!=m_rows.end();++r_iter, ++row){
      for(i_iter=(*r_iter).begin(), column=1 ;i_iter!=(*r_iter).end();++i_iter, ++column) {
	if (!search_map[column-1].size())  continue;
	if (search_map[column-1] != (*i_iter))  // kein treffer
	  break;
      }
      if (column > (*r_iter).size()) {
	rowClicked(m_listWidget->model()->index(row,1));
	break;
      }
    }
    if (r_iter==m_rows.end()) // kein Treffer ueberhaupt => ???? (erste row)
      rowClicked(m_listWidget->model()->index(0,0));
    getSearchMask().clear();
  }
  killTimer( e->timerId() );
}
/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */


bool GuiQtFilterList::destroy(){
  BUG(BugGuiTable,"GuiQtFilterList::destroy");
  clear();
  m_listWidget->deleteLater();
  m_titleWidget->deleteLater();;
  m_frameWidget->deleteLater();;
  m_listWidget  = 0;
  m_titleWidget = 0;
  m_frameWidget = 0;
  return true;
}

/* --------------------------------------------------------------------------- */
/* fillList --                                                                 */
/* --------------------------------------------------------------------------- */
int GuiQtFilterList::fillList(int selectedRowIndex){
  BUG(BugGuiTable,"GuiQtFilterList::fillList");

  getListItems(m_rows);

  QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(m_listWidget->model());
  model->removeRows(0, model->rowCount());

  // hinzufuegen eines Indexes
  int row, column;
  GuiFilterListListener::RowsVector::iterator r_iter;
  GuiFilterListListener::StringsVector::iterator i_iter;
  for(r_iter=m_rows.begin(), row=0;r_iter!=m_rows.end();++r_iter, ++row){
    model->insertRow(row);

    // 0 Zellen mit Index fuellen
    model->setData(model->index(row, 0), row);

    // andere Zellen fuellen
    for(i_iter=(*r_iter).begin(), column=1 ;i_iter!=(*r_iter).end();++i_iter, ++column) {
      if (m_rows.isColumnTypeNumeric(column-1)) {
	double dbl = QString::fromStdString((*i_iter)).toDouble();
	long long lng = QString::fromStdString((*i_iter)).toLongLong();
	if (std::isnan(dbl)/* || std::isnan(lng)*/) {
	  model->setData(model->index(row, column), "" );
	} else
	if ( fabs(dbl-lng) < NumLim::EPSILON_DOUBLE )
	  model->setData(model->index(row, column), lng );
	else
	  model->setData(model->index(row, column), dbl );
      } else
	model->setData(model->index(row, column), QString::fromStdString((*i_iter)));
    }
    (*r_iter).push_back( QString::number(row).toStdString() );
  }

  doListConfig();
  return m_rows.size();
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */
void  GuiQtFilterList::setTitle(){
  std::string title;
  if( getListener().getTitle( title ) ){
    m_titleWidget->setText( QString::fromStdString(title) );
  }
  else
    m_titleWidget->hide();
}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtFilterList::clear(){
  if( getRowSize() ){
    if( m_listWidget ){
      QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(m_listWidget->model());
      model->removeRows(0, model->rowCount());
    }
    getRowsConf().clear();
  }
  m_rows.clear();
}

/* --------------------------------------------------------------------------- */
/* rebuild --                                                                  */
/* --------------------------------------------------------------------------- */
int GuiQtFilterList::rebuild(){
  BUG(BugGuiTable,"GuiQtFilterList::rebuild");
  int itemsCount=0;
  int currentSel = getSelection();;
  clear();
  setTitle();
  setLabels();
  itemsCount = fillList( currentSel );
  return itemsCount;
}

/* --------------------------------------------------------------------------- */
/* doListConfig --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtFilterList::doListConfig() {
  BUG(BugGuiTable,"GuiQtFilterList::doListConfig");
  m_listWidget->hideColumn(0);
  int column = 1;
  int cnt_width=0;

  //Spaltenbreite setzen (Anzahl Zeichen / Spalte)
  int charlen = m_listWidget->horizontalHeader()->fontMetrics().horizontalAdvance( "W" );
  QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(m_listWidget->model());
  GuiFilterListListener::IntVector::iterator iter;
  for (iter=getColsConf().begin(); iter!=getColsConf().end();++iter, ++column ) {
    int len = 1+ model->headerData(column, Qt::Horizontal).toString().size();
    len = len <= abs(*iter) ? abs(*iter) : len;
    if ( abs(*iter) > 0 ) {
      m_listWidget->setColumnWidth( column, charlen*len );
      cnt_width += charlen*len;
    } else
      m_listWidget->hideColumn( column );
  }

  //Zeilenhoehe setzen (Anzahl Zeilen / Zeile)
  int row = 0;
  int charheight = m_listWidget->fontMetrics().height();
  for (iter=getRowsConf().begin(); iter!=getRowsConf().end();++iter, ++row ){
    m_listWidget->setRowHeight( row, charheight*( abs(*iter) ) );
  }
  cnt_width += 2*m_listWidget->frameWidth();
  int w = cnt_width;
  if ( m_listWidget->verticalScrollBar())
    w += m_listWidget->verticalScrollBar()->sizeHint().width();
  m_listWidget->setMinimumWidth( w );
  int height = 2 * m_listWidget->frameWidth() +
    getListener().getVisibleLines()*charheight +
    m_listWidget->horizontalHeader()->height();

  m_listWidget->setMinimumHeight(height);
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtFilterList::manage() {
  myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtFilterList::enable(){
  BUG(BugGuiTable,"GuiQtFilterList::enable");
  if( m_frameWidget != 0 ){
    m_frameWidget->setEnabled( true );
  }
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtFilterList::disable(){
  BUG(BugGuiTable,"GuiQtFilterList::disable");
  if( m_frameWidget != 0 ){
    if( m_frameWidget->isActiveWindow() )
      m_frameWidget->setEnabled( false );
  }
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtFilterList::serializeXML(std::ostream &os, bool recursive) {
  os << "<intens:FilterList";
  std::string s;
  getListener().getTitle( s );
  HTMLConverter::convert2HTML( s );
  os << " name=\"" << s << "\"";


  os << " value=\"" << "dbFilterList" << "\"";
//   if( m_function != 0 )
//     os << " action=\"" << m_function->Name() << "\"";
  os << ">" << std::endl;

  GuiFilterListListener::StringsVector labels;
  GuiFilterListListener::StringsVector::iterator iter;
  if(getListener().getLabels( labels ) ){
    labels.insert( labels.begin(), "Index" );
    int i = 0;
    for(iter = labels.begin(); iter != labels.end(); ++iter, ++i ){
      std::string label = (*iter);
      HTMLConverter::convert2HTML( label );
      os << "<intens:Column label=\"" << label;
      os << "\" value=\"column_" << i << "\"";
      os << "/>" << std::endl;
    }
  }
  os << "</intens:FilterList>"<< std::endl;
}
