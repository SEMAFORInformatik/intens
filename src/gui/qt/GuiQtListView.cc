
#include <QtGui>
#include <iostream>
#include <assert.h>

#include "gui/qt/GuiQtNavigator.h"
#include "gui/qt/GuiQtListView.h"
#include "gui/qt/GuiQtNavElement.h"
#include "xfer/XferDataItem.h"
#include "utils/JsonUtils.h"

GuiQtListView::GuiQtListView(GuiQtNavigator *navigator)
    : QListView(0)
    , m_navigator(navigator) {
  setWindowFlags (windowFlags()|Qt::ToolTip);
  setDragEnabled(true);
  setViewMode(QListView::IconMode);
  setIconSize(QSize(60, 40));
  setGridSize(QSize(60, 40));

  setSpacing(0);
  setMovement(QListView::Snap);
  setAcceptDrops(false);
  setDropIndicatorShown(false);

  // set model
  m_model = new MyQtListModel(this);
  setModel(m_model);
}

GuiQtListView::~GuiQtListView() {}

void GuiQtListView::removeAll() {
  m_model->removeRows(0, m_model->rowCount(), QModelIndex());
}

void GuiQtListView::contextMenuEvent ( QContextMenuEvent *event ) {
  std::string vn = m_model->data(currentIndex(), Qt::UserRole + 1).toString().toStdString();
  const GuiQtNavElement *navElem = m_model->dataNode(currentIndex());
  if( navElem != 0 ) {
    if (navElem->popup(event->pos()))
      return;
  }
  m_navigator->menuRequested(event->pos());
}

//
// Class MyQtListModel
//

MyQtListModel::MyQtListModel(QObject *parent)
  : QAbstractListModel(parent) {
}

MyQtListModel::~MyQtListModel() {}

const GuiQtNavElement*  MyQtListModel::dataNode(const QModelIndex &index) const {
  if (index.row() < m_navElements.size())
    return m_navElements.value(index.row());
  return 0;
}

QVariant MyQtListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::ToolTipRole) {
    return m_tooltips.value(index.row());
  }
  if (role == Qt::StatusTipRole)
    return QString("StatusTipRole unknown");

  if (role == Qt::DecorationRole)
    return QIcon(pixmaps.value(index.row()) ); //.scaled(60, 60,                        Qt::KeepAspectRatio, Qt::SmoothTransformation));
  else if (role == Qt::UserRole)
    return pixmaps.value(index.row());
  else if (role == Qt::UserRole + 1)
    return m_varnames.value(index.row());
  return QVariant();
}

void MyQtListModel::setToolTipData(long rowindex, const std::string& tooltip) {
  if (rowindex >= 0)
    m_tooltips.insert(rowindex, QString::fromStdString(tooltip));
}

long MyQtListModel::addItem(const QPixmap &pixmap, const GuiQtNavElement* navElem) {
  if (pixmap.isNull()) {
    std::cout << "IS NULL <MyQtListModel><addItem>\n"<<std::flush;
    return -1;
  }
  QString varname = QString::fromStdString(navElem->getDataItem()->getFullName(true));
  int row = pixmaps.size();

  beginInsertRows(QModelIndex(), row, row);
  pixmaps.insert(row, pixmap);
  m_varnames.insert(row, varname);
  m_navElements.insert(row, navElem);
  endInsertRows();
  return row;
}

Qt::ItemFlags MyQtListModel::flags(const QModelIndex &index) const {
  if (index.isValid())
    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

  return Qt::NoItemFlags;//Qt::ItemIsDropEnabled;
}

bool MyQtListModel::removeRows(int row, int count, const QModelIndex &parent) {
 if (parent.isValid())
   return false;

 if (row >= pixmaps.size() || row + count <= 0)
   return false;

 int beginRow = qMax(0, row);
 int endRow = qMin(row + count - 1, pixmaps.size() - 1);

 beginRemoveRows(parent, beginRow, endRow);

 while (beginRow <= endRow) {
   pixmaps.removeAt(beginRow);
   m_varnames.removeAt(beginRow);
   m_navElements.removeAt(beginRow);
   m_tooltips.removeAt(beginRow);
   --endRow;
 }
 endRemoveRows();
 return true;
}

  const char* c = GuiNavigator::MIME_COMPONENT;
QStringList MyQtListModel::mimeTypes() const {
  QStringList types;
  types << c;
  return types;
}

QMimeData *MyQtListModel::mimeData(const QModelIndexList &indexes) const {
  QMimeData *mimeData = new QMimeData();
  std::vector<std::string> varnameList;
  std::vector<bool> isFolderList;

  foreach (QModelIndex index, indexes) {
    if (index.isValid()) {
      QString varname = data(index, Qt::UserRole+1).toString();
      //      mimeData->setText( varname );
      varnameList.push_back( varname.toStdString() );
      QVariant pixmap  = data(index, Qt::UserRole);
mimeData->setImageData(pixmap);
    }
  }

  Json::Value dragJson = ch_semafor_intens::JsonUtils::createDragJsonObject( "IconView",
									     varnameList,
									     isFolderList );
  mimeData->setText( QString::fromStdString( ch_semafor_intens::JsonUtils::value2string(dragJson, true) ) );

  return mimeData;
}

bool MyQtListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int row, int column, const QModelIndex &parent)
{
  std::cout << "<MyQtListModel> <dropMimeData>\n"<<std::flush;
  assert(false);
  if (!data->hasFormat(GuiNavigator::MIME_COMPONENT))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    int endRow;

    if (!parent.isValid()) {
        if (row < 0)
            endRow = pixmaps.size();
        else
            endRow = qMin(row, pixmaps.size());
    } else
        endRow = parent.row();

    QByteArray encodedData = data->data(GuiNavigator::MIME_COMPONENT);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QPixmap pixmap;
        QPoint location;
        QString varname;
        stream >> pixmap >> location >> varname;

        beginInsertRows(QModelIndex(), endRow, endRow);
        pixmaps.insert(endRow, pixmap);
        m_varnames.insert(endRow, varname);
        endInsertRows();

        ++endRow;
	std::cout << "<MyQtListModel> <dropMimeData>   Varname["<<varname.toStdString()<<"]\n"<<std::flush;
    }

    return true;
}

int MyQtListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  else
    return pixmaps.size();
}

Qt::DropActions MyQtListModel::supportedDropActions() const {
 return Qt::CopyAction;// | Qt::MoveAction;
}
