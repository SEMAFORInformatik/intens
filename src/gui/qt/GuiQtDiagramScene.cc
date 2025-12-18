
#include <QtGui>
#include <iostream>
#include <QGraphicsSceneMouseEvent>

#include "gui/qt/GuiQtDiagram.h"
#include "gui/qt/GuiQtDiagramScene.h"
#include "gui/qt/GuiQtDiagramItem.h"
#include "gui/qt/GuiQtDiagramItem.h"
#include "utils/Debugger.h"

INIT_LOGGER();

//----------------------------------------------------
// GuiQtDiagramScene Constructor
//----------------------------------------------------
GuiQtDiagramScene::GuiQtDiagramScene(const QRectF& rect, QObject *parent)
  : QGraphicsScene(rect, parent)
    ,  m_line(0)
 {
  BUG( BugGui, "GuiQtDiagramScene::GuiQtDiagramScene");

}

//----------------------------------------------------
// mousePressEvent
//----------------------------------------------------
void GuiQtDiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  BUG( BugGui, "GuiQtDiagramScene::mousePressEvent");

  QTransform transform;
  QGraphicsItem* pItemUnderMouse = itemAt(event->scenePos(), transform);
  if (!pItemUnderMouse)
    views ().first()->setDragMode(QGraphicsView::RubberBandDrag);
  else
    views ().first()->setDragMode(QGraphicsView::NoDrag);

  if (event->modifiers() == Qt::ControlModifier &&
      event->button() == Qt::LeftButton) {
    QTransform transform;
    QGraphicsItem* pItemUnderMouse = itemAt(event->scenePos(), transform);

    if (pItemUnderMouse &&
	pItemUnderMouse->isEnabled() &&
	pItemUnderMouse->flags() & QGraphicsItem::ItemIsSelectable)
      pItemUnderMouse->setSelected(!pItemUnderMouse->isSelected());
    return;
  } else {

    // item verschieben
    if (event->button() == Qt::LeftButton) {
      m_oldCursor = event->widget()->cursor();
    }

    // Right Button :: PopupMenu :: ignore this event to prevent unselection
    if (event->button() == Qt::RightButton) {
      event->accept();
      return;
    }
  }

  // connection beginnen
  if (event->button() == Qt::MiddleButton) {
    QList<QGraphicsItem *> startItems = items(event->scenePos());
    if (startItems.empty())
      return;
    m_line = new QGraphicsLineItem( QLineF( event->scenePos(),  event->scenePos() ));
    m_line ->setPen( QPen(QColor("magenta"), 3) );
    addItem(m_line);
  }
  QGraphicsScene::mousePressEvent(event);
}

//----------------------------------------------------
// getPixmapItem
//----------------------------------------------------
void GuiQtDiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  BUG( BugGui, "GuiQtDiagramScene::mouseReleaseEvent");

  // move items
  if (views ().first()->dragMode() == QGraphicsView::NoDrag &&
      selectedItems().size()) {
    // invalid position => move to old position
    if (event->widget()->cursor().shape() != m_oldCursor.shape()){
      GuiManager::Instance().update( GuiElement::reason_Process );
      event->widget()->setCursor(  m_oldCursor );

      // move to old position
      std::vector<GuiQtDiagramPixmapItem*> listVec;
      QList<QGraphicsItem*> list = selectedItems();
      for (int i=0; i< list.size(); ++i) {
        if (list.at(i)->type() == GuiQtDiagramPixmapItem::Type)
          listVec.push_back(qgraphicsitem_cast<GuiQtDiagramPixmapItem *>(list.at(i)) );
      }
      QPointF pt(0,0);
      emit itemMoved(listVec, pt);
      return;
    }
    // throw signal
    if (event->button() == Qt::LeftButton) {
      if (views ().first()->dragMode() == QGraphicsView::NoDrag &&
          event->buttonDownScenePos(Qt::LeftButton) != event->scenePos()) {
        std::vector<GuiQtDiagramPixmapItem*> listVec;
        QList<QGraphicsItem*> list = selectedItems();
        for (int i=0; i< list.size(); ++i) {
          if (list.at(i)->type() == GuiQtDiagramPixmapItem::Type)
            listVec.push_back(qgraphicsitem_cast<GuiQtDiagramPixmapItem *>(list.at(i)) );
        }
        QPointF pt(event->lastScenePos() - event->buttonDownScenePos(Qt::LeftButton));
        ///        std::cout << "move pt: ["<<pt.x()<<", "<<pt.y()<<"]\n";
        emit itemMoved(listVec, pt);
      }
    }
  }

  // create connections
  if (m_line) {
    QList<QGraphicsItem *> startItems = items(m_line->line().p1());
    if (startItems.count() && startItems.first() == m_line)
      startItems.removeFirst();
    QList<QGraphicsItem *> endItems = items(m_line->line().p2());
    if (endItems.count() && endItems.first() == m_line)
      endItems.removeFirst();

    removeItem(m_line);
    delete m_line;

    if (startItems.count() > 0 && endItems.count() > 0 &&
        startItems.first()->type() == GuiQtDiagramPixmapItem::Type &&
        endItems.first()->type() == GuiQtDiagramPixmapItem::Type &&
        startItems.first() != endItems.first()) {
      GuiQtDiagramPixmapItem *startItem =
        qgraphicsitem_cast<GuiQtDiagramPixmapItem *>(startItems.first());
      GuiQtDiagramPixmapItem *endItem =
        qgraphicsitem_cast<GuiQtDiagramPixmapItem *>(endItems.first());

      // call function
      if (views().size())
        dynamic_cast<GuiQtDiagram*>(views()[0])->
          newConnection(startItem, endItem);
    }
    m_line = 0;
  }

  QGraphicsScene::mouseReleaseEvent(event);
}

//----------------------------------------------------
// getPixmapItem
//----------------------------------------------------
GuiQtDiagramPixmapItem* GuiQtDiagramScene::getPixmapItem(const std::string& id) const{
  QList<QGraphicsItem *> list = items();
  for (int i=0; i < list.size(); ++i)
    if (list.at(i)->type() == GuiQtDiagramPixmapItem::Type) {
      GuiQtDiagramPixmapItem *item =
	qgraphicsitem_cast<GuiQtDiagramPixmapItem *>(list.at(i));
      if (item->getId() == id)
	return item;
    }
  return NULL;
}

//----------------------------------------------------
// removeAllConnections
//----------------------------------------------------
void GuiQtDiagramScene::removeAllConnections(){
  QList<QGraphicsItem *> list = items();
  for (int i=0; i < list.size(); ++i)
    if (list.at(i)->type() == GuiQtDiagramConnection::Type) {
      removeItem(list.at(i));
      delete list.at(i);
    }
}

//----------------------------------------------------
// getAllConnections
//----------------------------------------------------
bool GuiQtDiagramScene::getAllConnections(std::vector<GuiQtDiagramConnection*>& res){
  res.clear();
  QList<QGraphicsItem *> list = items();
  for (int i=0; i < list.size(); ++i) {
    if (list.at(i)->type() == GuiQtDiagramConnection::Type) {
      GuiQtDiagramConnection* c = dynamic_cast<GuiQtDiagramConnection*>(list.at(i));
      res.push_back(dynamic_cast<GuiQtDiagramConnection*>(list.at(i)));
    }
  }
  return res.empty() > 0;
}

//----------------------------------------------------
// mouseMoveEvent
//----------------------------------------------------
void GuiQtDiagramScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
  // no button pressed => return
  if (event->buttons() == Qt::NoButton) {
    if (m_line) {
      removeItem(m_line);
      delete m_line;
      m_line = 0;
    }
    return;
  }

  // draw connection
  if (m_line != 0) {
    QLineF newLine(m_line->line().p1(), event->scenePos());
    m_line->setLine(newLine);
  }

  // change cursor if move is invalid
  QList<QGraphicsItem*> list = selectedItems();
  if(list.size()) {
    bool collides = false;
    for (int i=0; i< list.size(); ++i) {
      if (list.at(i)->type() ==  GuiQtDiagramConnection::Type) continue;
      QList<QGraphicsItem *> clist = list.at(i)->collidingItems();
      for (int x = 0; x < clist.size(); ++x) {
        if (list.contains(clist.at(x)))
          continue;
        if (clist.at(x)->type() !=  GuiQtDiagramConnection::Type) {
          collides = true;
          break;
        }
      }
      if (!collides &&
          list.at(i)->scenePos().x() < sceneRect().x() ||
          list.at(i)->scenePos().y() < sceneRect().y()) {
        collides=true;
      }
      if (collides) break;
    }
    // setze den Cursor
    if ( collides ) {
      event->widget()->setCursor(  QCursor( Qt::ForbiddenCursor ) );
    } else  {
      event->widget()->setCursor( m_oldCursor );
    }
  }
  QGraphicsScene::mouseMoveEvent(event);
}

//----------------------------------------------------
// updateScreenSize
//----------------------------------------------------
void GuiQtDiagramScene::updateScreenSize(QGraphicsItem* item) {
  QPointF pt(item->scenePos());

    // get left bottom point of item ==maybe==> increase sceneRect
    const int add = 4 + 16; // border + scrollbar
    QPoint ptBL (add + pt.toPoint().x() + item->boundingRect().width(),
		 add + pt.toPoint().y() + item->boundingRect().height());
    if (ptBL.x() > sceneRect().width()) {
      setSceneRect( QRect(sceneRect().x(),sceneRect().y(),sceneRect().x()+ptBL.x(),sceneRect().height()) );
    }
    if (ptBL.y() > sceneRect().height()) {
      setSceneRect(QRect(sceneRect().x(),sceneRect().y(),sceneRect().width(),sceneRect().y()+ptBL.y()));
    }
    // update x, y
    QRectF r = sceneRect();
    if (pt.y() < r.y()) {
      double diff = r.y() - pt.y();
      r.setY(r.y() - diff);
      setSceneRect(r);
    }
    if (pt.x() < r.x()) {
      double diff = r.x() - pt.x();
      r.setX(r.x() - diff);
      setSceneRect(r);
    }
}
