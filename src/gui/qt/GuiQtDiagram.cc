
#include <typeinfo>
#include <algorithm>
#include <qsettings.h>
#include <qlistview.h>
#include <QApplication>

#include "datapool/DataDictionary.h"
#include "datapool/DataVector.h"
#include "xfer/XferDataItem.h"
#include "gui/qt/GuiQtNavigator.h"
#include "gui/qt/GuiQtNavigatorMenu.h"
#include "gui/qt/GuiQtNavRoot.h"
#include "gui/qt/GuiQtNavNode.h"
#include "gui/qt/GuiQtListView.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/GuiQtManager.h"
#include "utils/StringUtils.h"

#include <QtGui>
#include <iostream>
#include <string>

#include "gui/qt/GuiQtDiagram.h"
#include "gui/qt/GuiQtDiagramScene.h"
#include "gui/qt/GuiQtDiagramItem.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/GuiQtNavRoot.h"
#include "datapool/DataReference.h"
#include "xfer/XferDataItem.h"
#include "app/DataPoolIntens.h"
#include "app/UserAttr.h"
#include "utils/StringUtils.h"
#include "utils/gettext.h"
#include "utils/JsonUtils.h"

INIT_LOGGER();

class CompositePixmap : public QWidget {
public:
  CompositePixmap(QWidget *parent = 0);
  void clear();
  void addPixmap(QPixmap& pixmap, QRect );
  QRect getBoundingBox() const;
  void setScale( double scale ) { m_scale = scale; }
  double getScale() { return m_scale; }
  void setBgMode( Qt::BGMode mode ) { m_bgMode = mode; }

protected:
    void paintEvent(QPaintEvent *event);

private:
  QList<QPixmap> piecePixmaps;
  QList<QRect> pieceRects;
  QList<QPoint> pieceLocations;
  QRect boundingbox;
  Qt::BGMode  m_bgMode;
  double m_scale;
};

//----------------------------------------------------
// CompositePixmap Constructor
//----------------------------------------------------
CompositePixmap::CompositePixmap(QWidget *parent)
  : QWidget(parent)
  , m_scale(1.0)
  , m_bgMode(Qt::TransparentMode)
{
//     setAcceptDrops(true);
}
void CompositePixmap::clear()
{
    pieceLocations.clear();
    piecePixmaps.clear();
    pieceRects.clear();
//     highlightedRect = QRect();
//     inPlace = 0;
    update();
}

QRect CompositePixmap::getBoundingBox() const {
  QRect r(boundingbox);
//   r.setX(0);
//   r.setY(0);
  return r;
  //  return boundingbox;
};

void CompositePixmap::addPixmap(QPixmap& pixmap, QRect rect) {
  if (pixmap.isNull()) {
    return;
  }
  piecePixmaps.append(pixmap);
  pieceRects.append(rect);
  boundingbox |= rect;
  setMinimumSize(boundingbox.width(), boundingbox.height());
  setMaximumSize(boundingbox.width(), boundingbox.height());
}


void CompositePixmap::paintEvent(QPaintEvent *event)
{
  if (!pieceRects.size()) return;
  QPainter painter;
  painter.begin(this);
  painter.setBackgroundMode( m_bgMode );
  painter.fillRect(event->rect(), Qt::white);

//     if (highlightedRect.isValid()) {
//         painter.setBrush(QColor("#ffcccc"));
//         painter.setPen(Qt::NoPen);
//         painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
//     }
  for (int i = 0; i < pieceRects.size(); ++i) {
    QRect r(m_scale*(pieceRects[i].x()-boundingbox.x()),
	    m_scale*(pieceRects[i].y()-boundingbox.y()),
	    m_scale*pieceRects[i].width(),m_scale*pieceRects[i].height() );
    //      std::cout << i<< " Rect["<<pieceRects[i].x()<<", "<<pieceRects[i].y()<<"]  ["<<pieceRects[i].width()<<", "<<pieceRects[i].height()<<"]\n"<<std::flush;
    painter.drawPixmap(r, piecePixmaps[i]);
  }
  painter.end();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//----------------------------------------------------
// GuiQtDiagram Constructor
//----------------------------------------------------
GuiQtDiagram::GuiQtDiagram(GuiQtNavigator* navigator, GuiNavigator::RootsVector& roots,
                           GuiQtDiagramScene* scene, QWidget *parent)
  : QGraphicsView(scene, parent) //QWidget(parent)
  , m_navigator(navigator)
  , m_roots( roots )
  , m_deleteElement( this )
  , m_composite_scale_factor(1.0)
  , m_connectType(GuiQtDiagramConnection::OneStep)
  , m_scacleInMenu( this, sqrt(2) )
  , m_scacleOutMenu( this, sqrt(0.5) )
  , m_copyMenu( this )
  , m_connectionTypeMenu( this ) {
  setAcceptDrops(true);
  GRID_LEN_X = 12;
  GRID_LEN_Y = 12;

  // die Abweichungen zum Navigator (nur ein RootNode wird dargestellt)
  assert(m_roots.size() == 1);

  connect(scene, SIGNAL(itemMoved(std::vector<GuiQtDiagramPixmapItem*>, const QPointF &)),
          this, SLOT(slot_itemMoved(std::vector<GuiQtDiagramPixmapItem*>, const QPointF &)));


  // additional main diagram menu
  m_addPopupMenu[_("Copy")] = &m_copyMenu;
  m_addPopupMenu[_("Zoom In@Ctrl<Key>I@Ctrl+I")] = &m_scacleInMenu;
  m_addPopupMenu[_("Zoom Out@Ctrl<Key>O@Ctrl+O")] = &m_scacleOutMenu;
  m_addPopupMenu[_("Change Connection Type")] = &m_connectionTypeMenu;
  m_addPopupMenu[_("Delete selected Element/Connection@Del@Del")] = &m_deleteElement;

  setResizeAnchor(QGraphicsView::NoAnchor);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);//AnchorViewCenter);
  //  setRubberBandSelectionMode ( Qt::IntersectsItemShape );
  setRubberBandSelectionMode(Qt::ContainsItemShape);
  //  setDragMode(QGraphicsView::RubberBandDrag);
}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::clear() {
  highlightedRect = QRect();
  update();
}

/* --------------------------------------------------------------------------- */
/* initShortcuts --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::initShortcuts() {
  GuiQtNavElement *node = dynamic_cast<GuiQtNavElement*>(m_roots[0]->getNode());
  if( node != 0 ) {
    if (m_addPopupMenu.size()){
      node->addPopupMenu( m_addPopupMenu );
      m_addPopupMenu.clear();
      node->popup(QPoint(), 0, false);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* saveItemSelection --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::saveItemSelection() {
  QList<QGraphicsItem*> list = scene()->selectedItems();
  m_selectedItems.reserve(m_selectedItems.size() + list.size());
  for (int i=0; i< list.size(); ++i)
    if (list.at(i)->type() == GuiQtDiagramPixmapItem::Type) {
      GuiQtDiagramPixmapItem *item = dynamic_cast<GuiQtDiagramPixmapItem*>(list.at(i));
      m_selectedItems.push_back( ItemFragment(item->getId()) );
    } else
      if (list.at(i)->type() == GuiQtDiagramConnection::Type) {
	GuiQtDiagramConnection *item = dynamic_cast<GuiQtDiagramConnection*>(list.at(i));
	m_selectedItems.push_back( ItemFragment(item->startItem()->getId() + "<-->" +
						item->endItem()->getId()) );
      }
}

/* --------------------------------------------------------------------------- */
/* restoreItemSelection --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::restoreItemSelection() {
  std::vector<ItemFragment>::iterator it;
  QList<QGraphicsItem*> list = scene()->items();
  for (int i=0; i< list.size(); ++i)
    if (list.at(i)->type() == GuiQtDiagramPixmapItem::Type) {
      GuiQtDiagramPixmapItem *item = dynamic_cast<GuiQtDiagramPixmapItem*>(list.at(i));
      it = std::find( m_selectedItems.begin(), m_selectedItems.end(), ItemFragment(item->getId()) );
      if (it !=  m_selectedItems.end()) {
	bool isBlocked = item->signalsBlocked();
	item->blockSignals(true);
	item->setSelected(true);
	item->blockSignals(isBlocked);
      }
    } else
      if (list.at(i)->type() == GuiQtDiagramConnection::Type) {
	GuiQtDiagramConnection *item = dynamic_cast<GuiQtDiagramConnection*>(list.at(i));
	it = std::find( m_selectedItems.begin(), m_selectedItems.end(),
			ItemFragment(item->startItem()->getId()+ "<-->" + item->endItem()->getId()));
	if (it !=  m_selectedItems.end()) {
	  item->setSelected(true);
	}
      }

  m_selectedItems.clear();
}

/* --------------------------------------------------------------------------- */
/* setItemSelection --                                                         */
/* --------------------------------------------------------------------------- */

void  GuiQtDiagram::setItemSelection(const std::string& id, bool bSelected) {
  std::vector<ItemFragment>::iterator it;
  BUG_INFO("setItemSelection, id="<< id << ", selected: " << bSelected);

  QList<QGraphicsItem*> list = scene()->items();
  for (int i=0; i< list.size(); ++i)
    if (list.at(i)->type() == GuiQtDiagramPixmapItem::Type) {
      GuiQtDiagramPixmapItem *item = dynamic_cast<GuiQtDiagramPixmapItem*>(list.at(i));
      if (item->getId() ==  id) {
	bool isBlocked = item->signalsBlocked();
	item->blockSignals(true);
	item->setSelected( bSelected );
	item->blockSignals(isBlocked);
	return;
      }
    }

  // not found
  m_selectedItems.push_back( ItemFragment(id) );
}

/* --------------------------------------------------------------------------- */
/* copy --                                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::copy(std::string filename) {
  m_navigator->getElement()->update(GuiElement::reason_Always);
  QRect rect = scene()->itemsBoundingRect().toRect();
  // margin
  rect.setWidth(rect.width() + 2*rect.x());
//  rect.setHeight(rect.height() + abs(2*rect.y()));

  QPixmap pm(rect.width(), rect.height());
  pm.fill();
  QPainter p(&pm);
  render(&p, rect, rect);
  if (filename.size()) {
    pm.save( QString::fromStdString(filename) );
  } else {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(pm);
    clipboard->setPixmap(pm, QClipboard::Selection);
  }
}

/* --------------------------------------------------------------------------- */
/* dragEnterEvent --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::dragEnterEvent(QDragEnterEvent *event) {
  if (!m_navigator->getElement()->isEnabled()) return;
  if (event->mimeData()->hasFormat(GuiNavigator::MIME_COMPONENT))
    event->accept();
  else
    event->ignore();
}

/* --------------------------------------------------------------------------- */
/* dragLeaveEvent --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::dragLeaveEvent(QDragLeaveEvent *event)
{
    QRect updateRect = highlightedRect;
    highlightedRect = QRect();
    update(updateRect);
    event->accept();
}

/* --------------------------------------------------------------------------- */
/* dragMoveEvent --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::dragMoveEvent(QDragMoveEvent *event) {
  if (!m_navigator->getElement()->isEnabled()) return;
#if QT_VERSION >= 0x060000
  QPoint pos(event->position().toPoint());
#else
  QPoint pos(event->pos());
#endif
  QRect updateRect = highlightedRect.united(targetSquare(pos));
  bool bad(true);
  highlightedRect = QRect();

  if (event->mimeData()->hasFormat(GuiNavigator::MIME_COMPONENT) ) {

    // get json drag object
    Json::Value dragObject;
    try {
      dragObject = ch_semafor_intens::JsonUtils::parseJson(event->mimeData()->text().toStdString());
    } catch (ch_semafor_intens::JsonException e) {
      std::cerr << "error message '"<< e.what() <<"' string ('"<< event->mimeData()->text().toStdString() <<"')\n";
      return;
    }

    // ONLY first element in varname list will be used
    std::string varname;
    std::string& key = ch_semafor_intens::JsonUtils::DRAG_OBJECT_VARNAME;
    if (dragObject[ key ].isArray() && dragObject[key].size() > 0) {
      varname = (*dragObject[key].begin()).asString();
    }

    QPixmap pixmap;
    getPixmap(pixmap, varname, true);
#if QT_VERSION >= 0x060000
  QPoint pos(event->position().toPoint());
#else
  QPoint pos(event->pos());
#endif
    QPointF pt(pos.x()-0.5*pixmap.width(),
               pos.y()-0.5*pixmap.height());
    QPointF ptG( toGridPoint(pt) );
    QPointF ptG2( ptG.x()+0.5*pixmap.width(), ptG.y()+0.5*pixmap.height()  );
    if (!pixmap.isNull() && findPiece(targetSquare(ptG2.toPoint(), pixmap)) == -1) {
      highlightedRect = targetSquare(ptG2.toPoint(), pixmap);
      event->setDropAction(Qt::MoveAction);
      event->accept();
      bad = false;
    }
    BUG_DEBUG("is pixmap valid: " << !pixmap.isNull() << "  badFlag: " << bad);
  }

  if (bad) {
    highlightedRect = QRect();

    event->ignore();
  }
  BUG_DEBUG("update rect, pos["<<updateRect.x()<<", "<<updateRect.y()<<"] size["<<updateRect.width()<<", "<<updateRect.height()<<"]");
  update(updateRect);
}

/* --------------------------------------------------------------------------- */
/* dropEvent --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::dropEvent(QDropEvent *event) {
  BUG_DEBUG("dropEvent");
  if (!m_navigator->getElement()->isEnabled()) return;

#if QT_VERSION >= 0x060000
  QPoint pos(event->position().toPoint());
#else
  QPoint pos(event->pos());
#endif
  if (event->mimeData()->hasFormat(GuiNavigator::MIME_COMPONENT)
      && findPiece(targetSquare(pos)) == -1) {

    // get json drag object
    Json::Value dragObject;
    try {
      dragObject = ch_semafor_intens::JsonUtils::parseJson(event->mimeData()->text().toStdString());
    } catch (ch_semafor_intens::JsonException e) {
      std::cerr << "error message '"<< e.what() <<"' string ('"<< event->mimeData()->text().toStdString() <<"')\n";
      BUG_INFO("error message '"<< e.what() <<"' string ('"<< event->mimeData()->text().toStdString() <<"')");
      return;
    }

    // ONLY first element in varname list will be used
    std::string varname;
    std::string& key = ch_semafor_intens::JsonUtils::DRAG_OBJECT_VARNAME;
    if (dragObject[ key ].isArray() && dragObject[key].size() > 0) {
      varname = (*dragObject[key].begin()).asString();
    }
    BUG_DEBUG("object with varname '"<< varname << "'");
    if ( varname.size() )  {
      DataReference *ref= DataPoolIntens::getDataReference( varname );
      XferDataItem *source = 0;
      if( ref != 0 )
	source = new XferDataItem( ref );
      if (!source)
	return;

      QPixmap pixmap;
      getPixmap(pixmap, source->getFullName(true), true);
#if QT_VERSION >= 0x060000
      QPoint pos(event->position().toPoint());
#else
      QPoint pos(event->pos());
#endif
      QPointF vPt = mapViewToSceneCenteredGridPoint(pos, pixmap);

      // function call
      dynamic_cast<GuiQtNavElement*>(m_roots[0]->getNode())
	->runJobFunction( JobElement::cll_Drop, source,
			  vPt.x()/GRID_LEN_X, vPt.y()/GRID_LEN_Y);
      delete source;
      highlightedRect = QRect();
      event->setDropAction(Qt::MoveAction);
      event->accept();
    }
  } else {
    highlightedRect = QRect();
    event->ignore();
  }
}

/* --------------------------------------------------------------------------- */
/* findPiece --                                                                */
/* --------------------------------------------------------------------------- */

int GuiQtDiagram::findPiece(const QRect &pieceRect) const
{
  int  ret = -1;
  QPoint pt = pieceRect.topLeft();

  // check if top left point is outside
  pt.setX(pt.x()+1); pt.setY(pt.y()+1);
  if (pt.x() < 0 || pt.y() < 0)
    return -2;

  // check all corner points
  QGraphicsItem* item = itemAt(pt);
  if (item) ret = 1;
  pt = pieceRect.topRight();
  pt.setX(pt.x()-2); pt.setY(pt.y()+1);
  item = itemAt(pt);
  if (item) ret = 2;
  pt = pieceRect.bottomRight();
  pt.setX(pt.x()-2); pt.setY(pt.y()-2);
  item = itemAt(pt);
  if (item) ret = 3;
  pt = pieceRect.bottomLeft();
  pt.setX(pt.x()+1); pt.setY(pt.y()-2);
  item = itemAt(pt);
  if (item) ret = 4;

  return ret;
}

/* --------------------------------------------------------------------------- */
/* call_function --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::call_function(std::string varname, JobElement::CallReason reason) {
  if ( varname.size() )  {

    DataReference *ref= DataPoolIntens::getDataReference( varname );
    XferDataItem *source = 0;
    if( ref != 0 )
      source = new XferDataItem( ref );
    if (!source)
      return;
    dynamic_cast<GuiQtNavElement*>(m_roots[0]->getNode())
      ->runJobFunction( reason, source );
    delete source;
  }
}

/* --------------------------------------------------------------------------- */
/* call_function --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::call_function(const std::vector<std::string>& varnames, JobElement::CallReason reason) {
  if ( varnames.size() )  {
    std::vector<XferDataItem*> sourceVec;
    for (std::vector<std::string>::const_iterator it = varnames.begin();
	 it != varnames.end(); ++it) {
      DataReference *ref= DataPoolIntens::getDataReference( (*it) );
      XferDataItem *source = 0;
      if( ref != 0 )
	source = new XferDataItem( ref );
      if (source)
	sourceVec.push_back( source );
    }

    dynamic_cast<GuiQtNavElement*>(m_roots[0]->getNode())
      ->runJobFunction( reason, sourceVec );

    // delete XferDataItems
    for (std::vector<XferDataItem*>::const_iterator it = sourceVec.begin();
	 it != sourceVec.end(); ++it)
      delete (*it);
  }
}

/* --------------------------------------------------------------------------- */
/* call_function --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::call_function(std::string varname, QPoint pos, JobElement::CallReason reason) {
  if ( varname.size() )  {

    DataReference *ref= DataPoolIntens::getDataReference( varname );
    XferDataItem *source = 0;
    if( ref != 0 )
      source = new XferDataItem( ref );
    if (!source)
      return;
    dynamic_cast<GuiQtNavElement*>(m_roots[0]->getNode())
      ->runJobFunction( reason, source,
			pos.x()/GRID_LEN_X, pos.y()/GRID_LEN_Y);
    delete source;
  }
}

/* --------------------------------------------------------------------------- */
/* call_function --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::call_function(const std::vector<std::string>& varnames, QPoint pos,
				 JobElement::CallReason reason) {
  if ( varnames.size() )  {

    std::vector<XferDataItem*> sourceVec;
    for (std::vector<std::string>::const_iterator it = varnames.begin();
	 it != varnames.end(); ++it) {
      DataReference *ref= DataPoolIntens::getDataReference( (*it) );
      XferDataItem *source = 0;
      if( ref != 0 )
	source = new XferDataItem( ref );
      if (source)
	sourceVec.push_back( source );
    }

    dynamic_cast<GuiQtNavElement*>(m_roots[0]->getNode())
      ->runJobFunction( reason, sourceVec,
                        pos.x(), pos.y());
    // delete XferDataItems
    for (std::vector<XferDataItem*>::const_iterator it = sourceVec.begin();
	 it != sourceVec.end(); ++it)
      delete (*it);
  }
}

/* --------------------------------------------------------------------------- */
/* call_function --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::call_function(std::string varname1, std::string varname2, JobElement::CallReason reason) {
  if ( varname1.size() && varname2.size())  {

    XferDataItem *xferStart = 0, *xferEnd = 0;
    DataReference *ref= DataPoolIntens::getDataReference( varname1 );
    if( ref != 0 )
      xferStart = new XferDataItem( ref );
    ref =  DataPoolIntens::getDataReference( varname2 );
    if( ref != 0 )
      xferEnd = new XferDataItem( ref );
    if (!xferStart || !xferEnd)
      return;
    dynamic_cast<GuiQtNavElement*>(m_roots[0]->getNode())
      ->runJobFunction( reason, xferStart, xferEnd);
    delete xferStart;
    delete xferEnd;
  }
}

/* --------------------------------------------------------------------------- */
/* getGridPoint --                                                             */
/* --------------------------------------------------------------------------- */

QPoint GuiQtDiagram::getGridPoint(const QPointF& vPt) const {
  return QPoint((vPt.x()+GRID_LEN_X/2)/GRID_LEN_X,
		(vPt.y()+GRID_LEN_Y/2)/GRID_LEN_Y );
}


/* --------------------------------------------------------------------------- */
/* toGridPoint --                                                              */
/* --------------------------------------------------------------------------- */

QPointF GuiQtDiagram::toGridPoint(const QPointF& vPt) const {
  return QPointF(GRID_LEN_X*floor((vPt.x()+GRID_LEN_X/2)/GRID_LEN_X),
		 GRID_LEN_Y*floor((vPt.y()+GRID_LEN_Y/2)/GRID_LEN_Y) );
}


/* --------------------------------------------------------------------------- */
/* slot_itemMoved --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::slot_itemMoved(std::vector<GuiQtDiagramPixmapItem*> items, const QPointF & pt)  {
  if (!items.size()) return;
  // event position crash
  if (abs(pt.x()) > 500 || abs(pt.y()) > 500) {
    scene()->update(scene()->sceneRect());
    return;
  }

  // call function
  std::vector<std::string> varnames;
  for ( std::vector<GuiQtDiagramPixmapItem*>::iterator it = items.begin(); it !=  items.end(); ++it)
    varnames.push_back( (*it)->getVarname() );
  call_function(varnames, getGridPoint(pt), JobElement::cll_Move);
}

/* --------------------------------------------------------------------------- */
/* mousePressEvent --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::mousePressEvent(QMouseEvent *event){
  if (m_navigator->getElement()->isEnabled()) {
    QGraphicsView::mousePressEvent(event);
  }
}


/* --------------------------------------------------------------------------- */
/* contextMenuEvent --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::contextMenuEvent(QContextMenuEvent *event) {

 QGraphicsItem* item = itemAt(event->pos());
 if (item) {
   QGraphicsView::contextMenuEvent(event);
   return;
 }

 // main diagram menu
 assert(m_roots.size() == 1);
 GuiQtNavElement *node = dynamic_cast<GuiQtNavElement*>(m_roots[0]->getNode());
 if( node != 0 ) {
   if (m_addPopupMenu.size()){
     node->addPopupMenu( m_addPopupMenu );
     m_addPopupMenu.clear();
   }
   event->accept();

   // validate delete element/connection menu
   m_deleteElement.disallow();
   if (scene()->selectedItems().size())
     m_deleteElement.allow();

   // validate zoom menu
   m_scacleOutMenu.validateState();
   m_scacleInMenu.validateState();

   node->popup( mapToScene(event->pos()).toPoint() );
   return;
 }
 QGraphicsView::contextMenuEvent(event);
}

/* --------------------------------------------------------------------------- */
/* targetSquare --                                                             */
/* --------------------------------------------------------------------------- */

const QRect GuiQtDiagram::targetSquare(const QPoint &position, const QPixmap& icon) const
{
  int nx = 1;
  int ny = 1;
  const QPointF pos(position.x()-icon.width()/2, position.y()-icon.height()/2);
  QPoint posGrid( toGridPoint(pos).toPoint() );
  if (icon.width()>GRID_LEN_X) {
    nx = icon.width()/GRID_LEN_X;
    if (icon.width() % GRID_LEN_X) ++nx;
  }
  if (icon.height()>GRID_LEN_Y) {
    ny = icon.height()/GRID_LEN_Y;
    if (icon.height() % GRID_LEN_Y) ++ny;
  }
  return QRect(posGrid.x(), posGrid.y(),
 	       nx*GRID_LEN_X, ny*GRID_LEN_Y);
}

//---------------------------------------------------------------------
// getPixmap
//---------------------------------------------------------------------
void GuiQtDiagram::getPixmap(QPixmap& icon, const std::string& name, bool bVarname) {
  // try to find icon via varname
  if (bVarname) {
    DataReference *ref= DataPoolIntens::getDataReference( name );
    DataReference *refType= DataPoolIntens::getDataReference( name + ".type");
    DataReference *refDisabled= DataPoolIntens::getDataReference( name + ".disabled");
    XferDataItem *sourceType = 0, *sourceDisabled = 0;

    // get pixmap of component type
    if( refType != 0 )
      sourceType = new XferDataItem( refType );
    // try to get type (".type") icon
    if (sourceType) {
      std::string s;
       sourceType->getValue(s);
       if (s.size()) {
	getDiagramPixmap(icon, s);
      }
      delete sourceType;
    }

    // get disabled datapool value
    QPixmap iconDisabled;
    if( refDisabled != 0 )
      sourceDisabled = new XferDataItem( refDisabled );
    // try to get type (".disabled") icon
    int disabled(0);
    if (sourceDisabled) {
      sourceDisabled->getValue(disabled);
      delete sourceDisabled;
    }

    // get disabled pixmap
    if (disabled) {
      getDiagramPixmap(iconDisabled, "disabled");
    }

    // try to get node icon
    XferDataItem *source = 0;
    if( ref != 0 )
      source = new XferDataItem( ref );
    if (icon.isNull() && source) {
      getDiagramPixmap(icon, source->getName());
    }
    delete source;

    // disabled? => overload with disabled pixmap
    if (!iconDisabled.isNull()) {
      CompositePixmap img;
      QRect rect(0,0, icon.width(), icon.height());
      img.addPixmap(icon, rect);
      int diff = 0.08 * icon.width();
      rect =  QRect(diff, diff, icon.width()-2*diff, icon.height()-2*diff);
      img.addPixmap(iconDisabled, rect);
      img.render(&icon);
    }

    return;
  }

  // try to find a icon
  getDiagramPixmap(icon, name);
}


//---------------------------------------------------------------------
// getDiagramPixmap
//---------------------------------------------------------------------
double GuiQtDiagram::getDiagramConnectionRatio() {
  std::string str = "Diagram/"+m_navigator->getElement()->getName()+".connection_axis_xy_ratio";
  double xyRatio = GuiQtManager::Settings()->value
    (QString::fromStdString(str), 2.).toDouble();
  return xyRatio;
}
//---------------------------------------------------------------------
// getDiagramPixmap
//---------------------------------------------------------------------
void GuiQtDiagram::getDiagramPixmap(QPixmap& icon, const std::string& name) {
  // try to find a icon
  std::string str = "Diagram/"+name+".iconPixmap";
  QString pm=GuiQtManager::Settings()->value
    (QString::fromStdString(str), QString::fromStdString(name)).toString();
  if( QtIconManager::Instance().getPixmap( pm.toStdString(), icon ) ){
  } else
    if( QtIconManager::Instance().getPixmap( ::lower(name), icon ) ){
    } else
      QtIconManager::Instance().getPixmap( ::lower(name+"-small"), icon );
  if (icon.isNull()) {
 //    std::cout << "DIAGRAM  NICHT GEFUNDEN ["<<str<<"] pm["<<pm.toStdString()<<"]\n";
    BUG_INFO("DIAGRAM Pixmap not found["<<str<<"] name["<<name<<"] pm["<<pm.toStdString()<<"]");
    if (name != "default") {
      getDiagramPixmap(icon, "default");
 //      std::cout << "DIAGRAM  NICHT GEFUNDEN ["<<str<<"]\n";
    }
  }
}

/* --------------------------------------------------------------------------- */
/* isCompositePixmap --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiQtDiagram::isCompositePixmap(const std::string& varname) {
  std::string compositeTag("trainUnit");
  std::string type;

  // check if type member variable has value of compositeTag
  DataReference *refT = DataPoolIntens::getDataReference( varname + ".type");
  DataReference *refC = DataPoolIntens::getDataReference( varname + ".components");
  // check type
  if (refT) {
    XferDataItem *xferT = new XferDataItem( refT );
    if (xferT)
      xferT->getValue(type);
  }
  // check child components
  if (refC) {
    XferDataItem *xferC = new XferDataItem( refC );
    DataContainer::SizeList dims;
    int numdims = xferC->getAllDataDimensionSize(dims);
    if( !(numdims && dims.size() && dims[0]))
      return false;
  } else
    return false;

  if (type == compositeTag) {
      return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getCompositePixmap --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::getCompositePixmap(QPixmap& icon, const std::string& varname, double scale) {
  BUG( BugGui, "GuiQtDiagram::getCompositePixmap" );
  int GRID_LEN_X = 12, GRID_LEN_Y=12;
  std::vector<QPixmap>  pixmaps;
  std::vector<QRect> rects;
  QPoint pt;
  DataReference *refC = DataPoolIntens::getDataReference( varname + ".components");
  DataReference *refX = DataPoolIntens::getDataReference( varname + ".components.xpos");
  DataReference *refY = DataPoolIntens::getDataReference( varname + ".components.ypos");
  DataReference *refT = DataPoolIntens::getDataReference( varname + ".components.type");
  if (refC && refX && refY && refT) {
    XferDataItem *xferC = new XferDataItem( refC );
    XferDataItem *xferX = new XferDataItem( refX );
    XferDataItem *xferY = new XferDataItem( refY );
    XferDataItem *xferT = new XferDataItem( refT );
    xferX->newDataItemIndex(xferX->getNumberOfLevels()-2);
    xferY->newDataItemIndex(xferY->getNumberOfLevels()-2);
    xferT->newDataItemIndex(xferT->getNumberOfLevels()-2);

    DataContainer::SizeList dims;
    int numdims = xferC->getAllDataDimensionSize(dims);
    if( numdims > 0 ){
      int xpos = -1, ypos = -1;
      std::string type;
      CompositePixmap img;
      img.setScale( scale );
      for( int i = 0; i < dims[0]; i++ ){
	xferX->setIndex( 0, i );
	xferY->setIndex( 0, i );
	xferT->setIndex( 0, i );
	if( xferX->getValue(xpos) && xferY->getValue(ypos) &&
	    xferT->getValue(type) ){
	  QPixmap pixmap;
	  getDiagramPixmap(pixmap, type);
	  if (pixmap.isNull()) continue;
	  QRect rect(xpos*GRID_LEN_X, ypos*GRID_LEN_Y,
		     pixmap.width(), pixmap.height());
	  img.addPixmap(pixmap, rect);
	}
	else {
	  BUG_MSG("INVALID composite component varname["<<varname<<"] XPos["<<xpos
		  <<"] YPos["<<ypos<<"] Type["<<type<<"]");
	}
      }
      if (dims[0]) {
        icon = QPixmap(scale * img.getBoundingBox().width(),
                       scale * img.getBoundingBox().height());
	img.render(&icon);
      }
    }
    delete xferC;
    delete xferX;
    delete xferY;
    delete xferT;
  }
}

/* --------------------------------------------------------------------------- */
/* mapViewToSceneCenteredGridPoint --                                          */
/* --------------------------------------------------------------------------- */

QPointF GuiQtDiagram::mapViewToSceneCenteredGridPoint(const QPoint& pos, const QPixmap& pixmap) const {
  QRect square=targetSquare(pos, pixmap);
  QPointF scPt = mapToScene(square.center());
  scPt.setX( scPt.x()- pixmap.width()/2 );
  scPt.setY( scPt.y()- pixmap.height()/2 );
  return toGridPoint(scPt);
}


/* --------------------------------------------------------------------------- */
/* slot_selectedChange --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::slot_selectedChange(GuiQtDiagramPixmapItem *item) {
  item->call_function( item->isSelected() ? JobElement::cll_Select : JobElement::cll_Unselect );
}

/* --------------------------------------------------------------------------- */
/* slot_activated --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::slot_activated(GuiQtDiagramPixmapItem *item) {
  // call function
  item->call_function(JobElement::cll_Activate);
}

/* --------------------------------------------------------------------------- */
/* newConnection --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::newConnection(GuiQtDiagramPixmapItem* startItem,
				 GuiQtDiagramPixmapItem* endItem) {

  call_function(startItem->getVarname(), endItem->getVarname(), JobElement::cll_NewConnection);
}


/* --------------------------------------------------------------------------- */
/* removeConnection --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::removeConnection(GuiQtDiagramPixmapItem* startItem,
				    GuiQtDiagramPixmapItem* endItem){
  call_function(startItem->getVarname(), endItem->getVarname(), JobElement::cll_RemoveConnection);
}

/* --------------------------------------------------------------------------- */
/* removeElement --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::removeElement(const std::vector<GuiQtDiagramPixmapItem*>& items){
  std::vector<GuiQtDiagramPixmapItem*>::const_iterator it =  items.begin();
  std::vector<std::string> varnames;
  for (; it !=  items.end(); ++it)
    varnames.push_back( (*it)->getVarname() );
  call_function(varnames, JobElement::cll_RemoveElement);
}

/* --------------------------------------------------------------------------- */
/* refreshConnections --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::refreshConnections(){
  XferDataItemIndex *newIndex = 0;
  std::string s0, s1;

  // remove all connections
  GuiQtDiagramScene* diag_scene = dynamic_cast<GuiQtDiagramScene*>(scene());
  assert(diag_scene);
  diag_scene->removeAllConnections();

  std::vector<std::string> vecA, vecB;
  std::vector<GuiNavigator::ConnectionAttr> attr;
  getConnections(vecA, vecB, attr);
  std::vector<std::string>::iterator it, it1;
  std::vector<GuiNavigator::ConnectionAttr>::iterator attrIt;
  for(it = vecA.begin(), it1 = vecB.begin(), attrIt = attr.begin();
      it != vecA.end() && it1 != vecB.end() && attrIt != attr.end();
      ++it, ++it1, ++attrIt) {
    GuiQtDiagramPixmapItem *i0, *i1;
    i0 = diag_scene->getPixmapItem(*it);
    i1 = diag_scene->getPixmapItem(*it1);

    // new connection
    if (i0 &&  i1) {
      GuiQtDiagramConnection *con = new GuiQtDiagramConnection(i0, i1, &(*attrIt));
      if (!m_navigator->getElement()->isEnabled()) {
        con->setFlag(QGraphicsItem::ItemIsSelectable, false);
      }
      if (attrIt->lineColor.size()) con->setColor(attrIt->lineColor.c_str());
      con->setZValue(-1000.0);
      scene()->addItem(con);
      con->updatePosition();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* refreshPixmapItem --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::refreshPixmapItem(GuiQtNavElement* navElement, GuiQtDiagramPixmapItem* item) {
  if (!item) return;
  item->refresh();
}

/* --------------------------------------------------------------------------- */
/* addPixmapItem --                                                            */
/* --------------------------------------------------------------------------- */

GuiQtDiagramPixmapItem* GuiQtDiagram::addPixmapItem(GuiQtNavElement* navElement) {
  QPoint pt;
  QPixmap pixmap;
  std::string varname = navElement->getDataItem()->getFullName(true);
  if ( isCompositePixmap(varname) ) {
    getCompositePixmap(pixmap, varname, m_composite_scale_factor);
  } else
    getPixmap(pixmap, navElement->getDataItem()->getFullName(true), true);
  if (pixmap.isNull())
    return NULL;

  // create new pixmap item
  GuiQtDiagramPixmapItem* myDiagramItem = new GuiQtDiagramPixmapItem(pixmap, navElement,GRID_LEN_X, GRID_LEN_Y);
  if (!m_navigator->getElement()->isEnabled()){
    myDiagramItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
    myDiagramItem->setFlag(QGraphicsItem::ItemIsMovable, false);
  }
  if (m_navigator->getElement()->isDragDisabled()){
    myDiagramItem->setFlag(QGraphicsItem::ItemIsMovable, false);
  }
  // Invalid? (no valid datatruct (xpos, ypos))
  if (!myDiagramItem->isValid()) {
    delete myDiagramItem;
    return NULL;
  }
  scene()->addItem(myDiagramItem);

  scene()->update();
  connect(myDiagramItem, SIGNAL(selectedChange(GuiQtDiagramPixmapItem *)),
          this, SLOT(slot_selectedChange(GuiQtDiagramPixmapItem *)));
  connect(myDiagramItem, SIGNAL(activated(GuiQtDiagramPixmapItem *)),
          this, SLOT(slot_activated(GuiQtDiagramPixmapItem *)));

  return myDiagramItem;
}

//
// class ScaleListener
//

GuiQtDiagram::ScaleListener::ScaleListener( GuiQtDiagram *diagram, double fac )
  : m_diagram( diagram )
  , m_fac( fac )
  , m_min( 0.1)
  , m_max( 4.0001 )
{}


/* --------------------------------------------------------------------------- */
/* ScaleListener::ButtonPressed --                                             */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::ScaleListener::ButtonPressed(){
#if QT_VERSION < 0x050000
  qreal old_scale =  sqrt(m_diagram->matrix().det()) ;
#else
  qreal old_scale =  sqrt(m_diagram->transform().determinant()) ;
#endif
  if (m_max < (old_scale *  m_fac))
    return;
  if (m_min > (old_scale *  m_fac))
    return;

  int delta = 10;
  QRectF ir = m_diagram->scene()->itemsBoundingRect();

  // scale
  QTransform transform = m_diagram->transform();
  transform.scale(m_fac, m_fac);
  transform.translate(0, 0);
  m_diagram->setTransform(transform);
  m_diagram->update(); // to be sure

  // refresh scene rect
  ir.setTopLeft(QPointF(0,0));
#if QT_VERSION < 0x050000
  qreal new_scale =  sqrt(m_diagram->matrix().det()) ;
#else
  qreal new_scale =  sqrt(m_diagram->transform().determinant()) ;
#endif
  if (new_scale < 1.) {
    m_diagram->setSceneRect(0,0, delta+ir.width()/new_scale, delta+ir.height()/new_scale);
  } else {
    m_diagram->setSceneRect(0,0, ir.width()+delta, ir.height()+delta);
  }

  return;

}

/* --------------------------------------------------------------------------- */
/* ScaleListener::validateState --                                             */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::ScaleListener::validateState(){
#if QT_VERSION < 0x050000
  qreal old_scale =  sqrt( m_diagram->matrix().det()  );
#else
  qreal old_scale =  sqrt( m_diagram->transform().determinant()  );
#endif

  if (m_max < (old_scale *  m_fac) ||
      m_min > (old_scale *  m_fac) ) {
    disallow();
  }
  else {
    allow();
  }
}

//
// class CopyListener
//
GuiQtDiagram::CopyListener::CopyListener( GuiQtDiagram *diagram )
  : m_diagram( diagram )
{}

/* --------------------------------------------------------------------------- */
/* CopyListener::ButtonPressed --                                              */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::CopyListener::ButtonPressed(){
  m_diagram->copy();
}

//
// class ConnectionTypeListener
//
GuiQtDiagram::ConnectionTypeListener::ConnectionTypeListener( GuiQtDiagram *diagram )
  : m_diagram( diagram )
{}

/* --------------------------------------------------------------------------- */
/* ConnectionTypeListener::ButtonPressed --                                    */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::ConnectionTypeListener::ButtonPressed(){
  switch (m_diagram->getConnectionType()) {
    case GuiQtDiagramConnection::Line:
     m_diagram->setConnectionType(GuiQtDiagramConnection::HalfStep);
     break;
    case GuiQtDiagramConnection::HalfStep:
     m_diagram->setConnectionType(GuiQtDiagramConnection::OneStep);
     break;
    case GuiQtDiagramConnection::OneStep:
     m_diagram->setConnectionType(GuiQtDiagramConnection::TwoStep);
     break;
    case GuiQtDiagramConnection::TwoStep:
    default:
      m_diagram->setConnectionType(GuiQtDiagramConnection::Line);
  }
  m_diagram->refreshConnections();
}

//
// class DeleteElementListener
//
GuiQtDiagram::DeleteElementListener::DeleteElementListener( GuiQtDiagram *diagram )
  : m_diagram( diagram )
{}

/* --------------------------------------------------------------------------- */
/* DeleteElementListener::ButtonPressed --                                     */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::DeleteElementListener::ButtonPressed(){
  QList<QGraphicsItem*> list = m_diagram->scene()->selectedItems();
  std::vector<GuiQtDiagramPixmapItem*> remElemList;
  for (int i=0; i< list.size(); ++i) {
    switch ( list.at(i)->type() ) {
      case GuiQtDiagramConnection::Type:
	{
	  GuiQtDiagramConnection *ci = dynamic_cast<GuiQtDiagramConnection*>(list.at(i));
	  m_diagram->removeConnection(ci->startItem(), ci->endItem());
	  m_diagram->scene()->removeItem(list.at(i));
	  delete list.at(i);
	}
	break;
      case GuiQtDiagramPixmapItem::Type:
	{
	  GuiQtDiagramPixmapItem *ci = dynamic_cast<GuiQtDiagramPixmapItem*>(list.at(i));
	  if (ci)
	    remElemList.push_back(ci);
	}
	break;
      case GuiQtDiagramItem::Type:
	break;
    }
  }
  m_diagram->removeElement( remElemList );
}

/* --------------------------------------------------------------------------- */
/* getConnections --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::getConnections(std::vector<std::string>& vecA,
                                  std::vector<std::string>& vecB,
                                  std::vector<GuiNavigator::ConnectionAttr>& attr) const {
  // get connection matrix
  std::string s;
  DataReference *ref0= DataPoolIntens::getDataReference( m_roots[0]->getDataItem()->Data(), "connect.node0");
  DataReference *ref1= DataPoolIntens::getDataReference( m_roots[0]->getDataItem()->Data(), "connect.node1");
  DataReference *ref2= DataPoolIntens::getDataReference( m_roots[0]->getDataItem()->Data(), "connect.lineWidth");
  DataReference *ref3= DataPoolIntens::getDataReference( m_roots[0]->getDataItem()->Data(), "connect.lineStyle");
  DataReference *ref4= DataPoolIntens::getDataReference( m_roots[0]->getDataItem()->Data(), "connect.lineColor");
  // nodes
  if (ref0 && ref1) {
    int id[1] = {-1};
    DataVector dataVec;
    ref0->getDataVector(dataVec, 1, id);
    for(DataVector::iterator i=dataVec.begin(); i != dataVec.end(); i++) {
      (*i)->getValue(s);
      vecA.push_back(s);
    }
    delete ref0;

    dataVec.clear();
    ref1->getDataVector(dataVec, 1, id);
    for(DataVector::iterator i=dataVec.begin(); i != dataVec.end(); i++) {
      (*i)->getValue(s);
      vecB.push_back(s);
    }
    delete ref1;
  }

  // lineWidth
  DataVector dataVecLw, dataVecLs, dataVecLc;
  int id[1] = {-1};
  if (ref2) {
    ref2->getDataVector(dataVecLw, 1, id);
    delete ref2;
  }
  // lineStyle
  if (ref3) {
    ref3->getDataVector(dataVecLs, 1, id);
    delete ref3;
  }
  // lineColor
  if (ref4) {
    ref4->getDataVector(dataVecLc, 1, id);
    delete ref4;
  }

  // get all connections
  GuiQtDiagramScene* diag_scene = dynamic_cast<GuiQtDiagramScene*>(scene());
  std::vector<GuiQtDiagramConnection*> connections;
  if (diag_scene) {
    diag_scene->getAllConnections(connections);
  }

  DataVector::iterator iLw=dataVecLw.begin();
  DataVector::iterator iLs=dataVecLs.begin();
  DataVector::iterator iLc=dataVecLc.begin();
  int maxAttr(std::max(std::max(dataVecLw.size(),
                                std::max(dataVecLs.size(), dataVecLc.size())),
                       std::max(vecA.size(), vecB.size())));
  for(int i=0; i < maxAttr; i++) {
    // read from datapool
    std::string ls,lc;
    int lw(1);
    if (dataVecLw.isValid(i)) {
      dataVecLw.getValue(i)->getValue(lw);
    }
    if (dataVecLs.isValid(i)) {
      dataVecLs.getValue(i)->getValue(ls);
    }
    if (dataVecLc.isValid(i)) {
      dataVecLc.getValue(i)->getValue(lc);
    }
    // connection data
    std::vector<double> xpos, ypos;
    if (i < connections.size()) {
      connections[i]->getPointList(xpos, ypos);
    }
    GuiNavigator::ConnectionAttr cattr(lw, lc, ls, xpos, ypos, GuiQtDiagramConnection::OneStep);

    GuiQtDiagramConnection::ConnectType connectType;
    if (i < vecA.size() && i < vecB.size())
      updateConnectionAttr(vecA[i], vecB[i], cattr);
    attr.push_back(cattr);
    BUG_DEBUG("LineAttr: lw: "<< cattr.lineWidth<<", ls: " << cattr.lineStyle << ", lc: " << cattr.lineColor);
  }
}

/* --------------------------------------------------------------------------- */
/* getItemSelection --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::getItemSelection(std::vector<std::string>& select_items) const {
  QList<QGraphicsItem*> list = scene()->selectedItems();
  for (int i=0; i< list.size(); ++i) {
    if (list.at(i)->type() == GuiQtDiagramPixmapItem::Type) {
      GuiQtDiagramPixmapItem *item = dynamic_cast<GuiQtDiagramPixmapItem*>(list.at(i));
      select_items.push_back(item->getId());
    }
  }
}

/* --------------------------------------------------------------------------- */
/* updateConnectionAttr --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtDiagram::updateConnectionAttr(std::string& node0, std::string& node1,
                                        GuiNavigator::ConnectionAttr& cattr) const {
  // "<component_name>:<anchor_x>,<<anchor_y>:<line_type>:<line_color>:<line_width>"
  // line_type: [I,L,S,W]
  // e.g. "component_name:0.30,I,#ff0000,2"
  if (node0.empty() || node1.empty()) {
    return;
  }
  std::vector<std::string> nodeProps0 = split(node0, ":");
  std::vector<std::string> nodeProps1 = split(node1, ":");
  node0 = nodeProps0[0];
  node1 = nodeProps1[0];

  // anchor position
  cattr.anchor_xpos.clear();
  cattr.anchor_ypos.clear();
  int x, y;
  char c;
  if (nodeProps0.size() > 1) {
    std::istringstream is(nodeProps0[1]);
    is >> x >> c >> y;
    cattr.anchor_xpos.push_back(x);
    cattr.anchor_ypos.push_back(y);
  }
  if (nodeProps1.size() > 1) {
    std::istringstream is(nodeProps1[1]);
    is >> x >> c >> y;
    cattr.anchor_xpos.push_back(x);
    cattr.anchor_ypos.push_back(y);
  }

  // lineColor
  if (nodeProps0.size() > 2) {
    if (nodeProps0[2] == "I") {
      cattr.connectType = GuiQtDiagramConnection::Line;
    }
    if (nodeProps0[2] == "L") {
      cattr.connectType = GuiQtDiagramConnection::HalfStep;
    }
    if (nodeProps0[2] == "S") {
      cattr.connectType = GuiQtDiagramConnection::OneStep;
    }
    if (nodeProps0[2] == "W") {
      cattr.connectType = GuiQtDiagramConnection::TwoStep;
    }
  }

  // lineColor
  if (nodeProps0.size() > 3) cattr.lineColor = nodeProps0[3];

  // lineWidth
  if (nodeProps0.size() > 4) cattr.lineWidth = atoi(nodeProps0[4].c_str());
}
