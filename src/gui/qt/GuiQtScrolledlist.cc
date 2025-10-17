
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QString>
//#include <qheader.h>

#include <sstream>

#include "gui/qt/GuiQtScrolledlist.h"
#include "gui/qt/GuiQtTableViewBase.h"
#include "gui/qt/QtMultiFontString.h"

#include "utils/Debugger.h"
#include "utils/gettext.h"

INIT_LOGGER();

class QtTableView : public QTableView {
public:
  QtTableView(QWidget *par, GuiQtScrolledlist* l)
    : QTableView(par), m_list(l) {}
  virtual ~QtTableView(){}
  int sizeHintForRow(int r) const {
    return QTableView::sizeHintForRow(r);
  }

  private:
  void commitData(QWidget *editor) {
    QModelIndex index(currentIndex());
    QTableView::commitData(editor);
    m_list->commitData(index);
  }

  GuiQtScrolledlist* m_list;
};


/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* myWidget  --                                                                */
/* --------------------------------------------------------------------------- */
QWidget* GuiQtScrolledlist::myWidget(){
  return m_myList;
}

/* --------------------------------------------------------------------------- */
/* setTabOrder --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::setTabOrder(){
  std::cout << "GuiQtScrolledlist::setTabOrder()\n";
}

/* --------------------------------------------------------------------------- */
/* addTabGroup --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::addTabGroup(){
  std::cout << "GuiQtScrolledlist::addTabGroup()\n";
}

/* --------------------------------------------------------------------------- */
/* grabFocus --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::grabFocus(){
  std::cout << "GuiQtScrolledlist::grabFocus()\n";
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::create(){
  BUG(BugGui,"GuiQtScrolledlist::create");

  QWidget *pw = getParent()->getQtElement()->myWidget();
  assert( pw != 0);

  // create table (view +  model)
  m_myList = new QtTableView(pw, this);
  QStandardItemModel *model = new QtStandardItemModel(0, 2, m_myList);
  m_myList->setModel( model );
  connect(m_myList,SIGNAL(doubleClicked(const QModelIndex&)), SLOT(slot_doubleClicked(const QModelIndex&)));

  // set font
  QFont font = m_myList->font();
  m_myList->setFont( QtMultiFontString::getQFont( "@list@", font ) );
  int charlen = m_myList->fontMetrics().horizontalAdvance( 'W' );

  // set font
  font = m_myList->horizontalHeader()->font();
  m_myList->horizontalHeader()->setFont( QtMultiFontString::getQFont( "@listTitle@", font ) );
  model->setHeaderData(0, Qt::Horizontal, QString::fromStdString(_("#")));
  model->setHeaderData(1, Qt::Horizontal, getHeaderLabel().empty() ? _("Cases") : QString::fromStdString(getHeaderLabel()));
  m_myList->setColumnWidth(0, getColumnLength());

#if QT_VERSION < 0x050000
  m_myList->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
#else
  m_myList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
#endif
  m_myList->verticalHeader()->hide();
  m_myList->setEnabled( true );
  //  m_myList->setEditTriggers( QAbstractItemView::NoEditTriggers );
  m_myList->setSelectionMode( QTableView::SingleSelection );
  m_myList->setSelectionBehavior ( QAbstractItemView::SelectRows );
  m_myList->setMinimumWidth( std::max(400, getColumnLength()*m_myList->fontMetrics().horizontalAdvance( "W" )) );
  BUG_INFO("columnLen ["<<getColumnLength()<<"] => " << m_myList->minimumWidth());

  m_myList->setWindowTitle( QString::fromStdString(getTitle()) );
  int rowh =  m_myList->sizeHintForRow(1);
  int height = 2*m_myList->frameWidth() + tableSize() * rowh + rowh;
  m_myList->setMinimumHeight( height );
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */
bool GuiQtScrolledlist::destroy(){
  std::cout << "GuiQtScrolledlist::destroy()\n";
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::getSize( int &w, int &h, bool hint){
  h = m_myList->height();
  w = m_myList->width();
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::enable(){
  GuiElement::enable();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::disable(){
  GuiElement::disable();
}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::clear(){
  QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(m_myList->model());
  model->removeRows(0, model->rowCount());

}

/* --------------------------------------------------------------------------- */
/* getSelectedIndex --                                                         */
/* --------------------------------------------------------------------------- */
int GuiQtScrolledlist::getSelectedIndex(){
  if (m_myList->selectionModel ()->selectedRows().size() == 1)
    return m_myList->selectionModel ()->selectedRows().at(0).row();
  return -1;
}

/* --------------------------------------------------------------------------- */
/* setSelectedPosition --                                                      */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::setSelectedPosition( int pos ){
  if( pos < 0 ) pos = 0;
  m_myList->selectRow(pos);
}

/* --------------------------------------------------------------------------- */
/* createSelectionList                                                         */
/* --------------------------------------------------------------------------- */
int GuiQtScrolledlist::createSelectionList( bool sorted ){
  int index = 0;
  std::string listText;
  QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(m_myList->model());
  while( getListender()->getNextItem( listText ) ){
    int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, 0), row+1, Qt::DisplayRole);
    model->setData(model->index(row, 1), QString::fromStdString(listText));
    index++;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* createSelectList                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtScrolledlist::createSelectList( bool sorted ){
  std::cout << "GuiQtScrolledlist::createSelectList()\n";
  return true;
}

/* --------------------------------------------------------------------------- */
/* createUnsortedList                                                          */
/* --------------------------------------------------------------------------- */
bool GuiQtScrolledlist::createUnsortedList(){
  std::cout << "GuiQtScrolledlist::createUnsortedList()\n";
  return true;
}

/* --------------------------------------------------------------------------- */
/* createSortedList                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtScrolledlist::createSortedList(){
  std::cout << "GuiQtScrolledlist::createSortedList()\n";
  return true;
}

/* --------------------------------------------------------------------------- */
/* deleteSelectionList                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::deleteSelectionList(){
  std::cout << "GuiQtScrolledlist::deleteSelectionList()\n";
}

/* --------------------------------------------------------------------------- */
/* slot_doubleClicked                                                          */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::slot_doubleClicked(const QModelIndex& index) {
  int row = getSelectedIndex();
  if( row != -1 ){
    getListender()->activated( row );
    editData(row, 1);
  }
}

/* --------------------------------------------------------------------------- */
/* editData                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::editData(int row, int column) {
  QModelIndex index =  m_myList->model()->index(row, column);
  m_myList->selectRow(row);
  m_myList->setCurrentIndex(index);
  m_myList->edit(index);
}

/* --------------------------------------------------------------------------- */
/* commitData                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::commitData(const QModelIndex& index) {
  getListender()->setValue(index.row(), index.model()->data(index).toString().toStdString());
}

/* --------------------------------------------------------------------------- */
/* setEditable                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::setEditable(bool flag) {
  m_myList->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledlist::serializeXML(std::ostream &os, bool recursive){
//   os << "<GuiQtScrolledlist";
//   os << ">" << endl;
//   os<<"</GuiQtScrolledlist>"<<endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtScrolledlist::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return GuiScrolledlist::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtScrolledlist::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiScrolledlist::serializeProtobuf(eles, onlyUpdated);
}
#endif
