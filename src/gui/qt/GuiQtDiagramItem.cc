
#include <QtGui>
#include <iostream>
#include <QGraphicsSceneContextMenuEvent>

#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtDiagramItem.h"
#include "gui/qt/GuiQtDiagramScene.h"
#include "gui/qt/GuiQtDiagramItem.h"
#include "gui/qt/GuiQtNavElement.h"
#include "gui/qt/GuiQtNavigator.h"

#include "app/DataPoolIntens.h"
#include "xfer/XferDataItem.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

//----------------------------------------------------
// GuiQtDiagramItem::GuiQtDiagramItem
//----------------------------------------------------

GuiQtDiagramItem::GuiQtDiagramItem(DiagramType diagramType, QGraphicsItem *item,
                         QGraphicsScene *scene)
    : QGraphicsPolygonItem(item)
{
    if (diagramType == Box) {
        boxPolygon << QPointF(0, 0) << QPointF(0, 30) << QPointF(30, 30)
                   << QPointF(30, 0) << QPointF(0, 0);
        setPolygon(boxPolygon);
    } else {
        trianglePolygon << QPointF(15, 0) << QPointF(30, 30) << QPointF(0, 30)
                        << QPointF(15, 0);
        setPolygon(trianglePolygon);
    }

    QColor color(static_cast<int>(QRandomGenerator::global()->generate()) % 256,
                 static_cast<int>(QRandomGenerator::global()->generate()) % 256,
                 static_cast<int>(QRandomGenerator::global()->generate()) % 256);
    QBrush brush(color);
    setBrush(brush);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
}

//----------------------------------------------------
// GuiQtDiagramItem::GuiQtDiagramItem
//----------------------------------------------------

GuiQtDiagramPixmapItem::GuiQtDiagramPixmapItem(const QPixmap& pixmap, GuiQtNavElement* navElem
					       , int GRID_X, int GRID_Y)
  : QGraphicsPixmapItem(pixmap)
  , m_navElem(navElem)
  , m_valid(false)
  , GRID_LEN_X(GRID_X)
  , GRID_LEN_Y(GRID_Y) {
  setShapeMode( BoundingRectShape );
  setAcceptHoverEvents(true);
//   QColor color(static_cast<int>(qrand()) % 256,
// 	       static_cast<int>(qrand()) % 256, static_cast<int>(qrand()) % 256);
//   QBrush brush(color);
  //    setBrush(brush);
  setFlag(QGraphicsItem::ItemIsSelectable);
  setFlag(QGraphicsItem::ItemIsMovable);

  QPoint pt = getPositionDataPool();
  QPointF scPt;
  scPt = mapToScene(pt);
  setPos( scPt );
}

//----------------------------------------------------
// GuiQtDiagramItem::~GuiQtDiagramItem
//----------------------------------------------------

GuiQtDiagramPixmapItem::~GuiQtDiagramPixmapItem() {
}

//----------------------------------------------------
// getVarname
//----------------------------------------------------

std::string GuiQtDiagramPixmapItem::getVarname() {
  return m_navElem->getDataItem()->getFullName(true);
};


//----------------------------------------------------
// getId
//----------------------------------------------------

std::string GuiQtDiagramPixmapItem::getId() {
  m_navElem->getDiagramNodeName(m_id);
  return m_id;
};

//----------------------------------------------------
// getPositionDataPool
//----------------------------------------------------

QPoint GuiQtDiagramPixmapItem::getPositionDataPool() {
  // get icon coordinate
  QPoint pt;
  int xpos, ypos;
  m_valid = m_navElem->getDiagramPosition(xpos, ypos);
  if (m_valid) {
    pt.setX(xpos * GRID_LEN_X);
    pt.setY(ypos * GRID_LEN_Y);
  }
  return pt;
}

//----------------------------------------------------
// getMenuIndexDataPool
//----------------------------------------------------

unsigned int GuiQtDiagramPixmapItem::getMenuIndexDataPool() {
  int menuId(0);
  m_navElem->getDiagramMenuId(menuId);
  return menuId < 0 ? 0 : (unsigned int) menuId;
}

//----------------------------------------------------
// refresh
//----------------------------------------------------

void GuiQtDiagramPixmapItem::refresh() {
  // get icon coordinate
  QPoint pt = getPositionDataPool();
  if (isValid())//!pt.isNull())
    setPos( pt );// mapToScene(pt) );
  else
    return;
  if (scene())
    dynamic_cast<GuiQtDiagramScene*>(scene())->updateScreenSize(this);
}


//----------------------------------------------------
// mouseMoveEvent
//----------------------------------------------------

void GuiQtDiagramPixmapItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
  QGraphicsPixmapItem::mouseMoveEvent(event);
}


//----------------------------------------------------
// itemChange
//----------------------------------------------------

QVariant GuiQtDiagramPixmapItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
  if (change == QGraphicsItem::ItemSelectedHasChanged)
    emit selectedChange(this);
  return value;
}

//----------------------------------------------------
// focusOutEvent
//----------------------------------------------------

void GuiQtDiagramPixmapItem::focusOutEvent(QFocusEvent *event)
{
  emit lostFocus(this);
  QGraphicsPixmapItem::focusOutEvent(event);
}

//----------------------------------------------------
// mouseDoubleClickEvent
//----------------------------------------------------

void GuiQtDiagramPixmapItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  emit activated(this);
  QGraphicsPixmapItem::mouseDoubleClickEvent(event);
}

//----------------------------------------------------
// contextMenuEvent
//----------------------------------------------------

void GuiQtDiagramPixmapItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  if (!isSelected() && scene()->selectedItems().size()) {
    scene()->clearSelection();
    setSelected(true);
  }
  if(m_navElem) {
    m_navElem->popup( event->scenePos().toPoint(), getMenuIndexDataPool() );
  }
}

//----------------------------------------------------
// call_function
//----------------------------------------------------

void GuiQtDiagramPixmapItem::call_function(JobElement::CallReason reason) {
  m_navElem->runJobFunction( reason );
}

//----------------------------------------------------
// boundingRect
//----------------------------------------------------

QRectF GuiQtDiagramPixmapItem::boundingRect() const
{
  return QGraphicsPixmapItem::boundingRect();
}

//----------------------------------------------------
// paint
//----------------------------------------------------

void GuiQtDiagramPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QGraphicsPixmapItem::paint(painter, option, widget);

  if (isSelected() && GuiQtManager::navItemBorderWidth() > 0) {
      QPen pen(QColor(GuiQtManager::navItemBorderColor()), GuiQtManager::navItemBorderWidth());  // Color and thickness of border
      painter->setPen(pen);
      painter->setBrush(Qt::NoBrush);
      painter->drawRect(boundingRect());
  }
}


//
// class GuiQtDiagramConnection
//
GuiQtDiagramConnection::GuiQtDiagramConnection(GuiQtDiagramPixmapItem *startItem, QPoint& startAnchor,
                                               GuiQtDiagramPixmapItem *endItem, QPoint& endAnchor,
                                               ConnectType cType, int lw, std::string ls,
                                               QGraphicsItem *parent, QGraphicsScene *scene)
  : QGraphicsLineItem(parent),
    myStartItem(startItem),
    m_startAnchor(startAnchor),
    myEndItem(endItem),
    m_endAnchor(endAnchor),
    connectType(cType),
    lineWidth(lw),
    lineStyle(lower(ls)){
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  myColor = Qt::black;
  setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

GuiQtDiagramConnection::GuiQtDiagramConnection(GuiQtDiagramPixmapItem *startItem,
                                               GuiQtDiagramPixmapItem *endItem,
                                               GuiNavigator::ConnectionAttr* attr)
  : myStartItem(startItem),
    myEndItem(endItem),
    connectType((GuiQtDiagramConnection::ConnectType) attr->connectType),
    lineWidth(attr->lineWidth),
    lineStyle(lower(attr->lineStyle)){
  if (attr->anchor_xpos.size() == 2 && attr->anchor_ypos.size() == 2) {
    m_startAnchor = QPoint(attr->anchor_xpos[0], attr->anchor_ypos[0]);
    m_endAnchor = QPoint(attr->anchor_xpos[1], attr->anchor_ypos[1]);
  }
}

GuiQtDiagramConnection::~GuiQtDiagramConnection(){
}

//----------------------------------------------------
// boundingRect
//----------------------------------------------------

QRectF GuiQtDiagramConnection::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

//----------------------------------------------------
// shape
//----------------------------------------------------

QPainterPath GuiQtDiagramConnection::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(m_connectLine);
    return path;
}

//----------------------------------------------------
// getPointList
//----------------------------------------------------

bool GuiQtDiagramConnection::getPointList(std::vector<double>& xpos, std::vector<double>& ypos){
  updateConnectLines();
  xpos.clear();
  ypos.clear();
  for (qsizetype i = 0; i < m_connectLine.size(); ++i) {
    xpos.push_back(m_connectLine.at(i).x());
    ypos.push_back(m_connectLine.at(i).y());
  }
  return xpos.size() > 0;
}


//----------------------------------------------------
// updatePosition
//----------------------------------------------------

void GuiQtDiagramConnection::updatePosition()
{
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}

//----------------------------------------------------
// paint
//----------------------------------------------------

void GuiQtDiagramConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                                   QWidget *){
  if (myStartItem->collidesWithItem(myEndItem, Qt::ContainsItemBoundingRect)){
    return;
  }

  QPen myPen = pen();
  myPen.setColor(myColor);

  // line width
  if (lineWidth > 0) {
    myPen.setWidth(lineWidth);
  }

  // line style
  myPen.setStyle(Qt::SolidLine);
  if (lineStyle.size()) {
    if(lineStyle == "dashline") myPen.setStyle(Qt::DashLine);
    if(lineStyle == "dotline") myPen.setStyle(Qt::DotLine);
    if(lineStyle == "dashdotline") myPen.setStyle(Qt::DashDotLine);
    if(lineStyle == "dashdotdotline") myPen.setStyle(Qt::DashDotDotLine);
  }

  // selected
  if (isSelected()) {
    myPen.setColor(QColor("blue"));
    myPen.setWidth(2*myPen.width());
  }

  painter->setPen(myPen);

  updateConnectLines();

  painter->drawPolyline(m_connectLine);
}

//----------------------------------------------------
// updateConnectLines
//----------------------------------------------------

void GuiQtDiagramConnection::updateConnectLines()
{
    QRectF sBB(myStartItem->boundingRect());
    QRectF eBB(myEndItem->boundingRect());
    qreal dX =  myEndItem->pos().x() - myStartItem->pos().x();
    qreal dY =  myEndItem->pos().y() - myStartItem->pos().y();
    if (isValid(m_startAnchor) && isValid(m_endAnchor)) {
      dX +=  m_endAnchor.x() - m_startAnchor.x();
      dY +=  m_endAnchor.y() - m_startAnchor.y();
    }
    double xFac = 0.5;

    if (xFac*abs(dX) < abs(dY)) {  // VERT
      qreal sdx = isValid(m_startAnchor) ? m_startAnchor.x() : 0.5 * sBB.width();
      qreal edx = isValid(m_endAnchor) ? m_endAnchor.x() : 0.5 * eBB.width();

      qreal sdy = isValid(m_startAnchor) ? m_startAnchor.y() : dY > 0  ? sBB.bottom() : sBB.top();
      qreal edy = isValid(m_endAnchor) ? m_endAnchor.y() : dY < 0  ? eBB.bottom() : eBB.top();
      setLine(QLineF(myEndItem->pos().x() + edx,
                     myEndItem->pos().y() + edy,
                     myStartItem->pos().x() + sdx,
                     myStartItem->pos().y() + sdy));
   } else {  // HORZ
      qreal sdy = isValid(m_startAnchor) ? m_startAnchor.y() : 0.5 * sBB.height();
      qreal edy = isValid(m_endAnchor) ? m_endAnchor.y() : 0.5 * eBB.height();

      qreal sdx = isValid(m_startAnchor) ? m_startAnchor.x() : dX > 0  ? sBB.right() : sBB.left();
      qreal edx = isValid(m_endAnchor) ? m_endAnchor.x() : dX < 0  ? eBB.right() : eBB.left();
      setLine(QLineF(myEndItem->pos().x() + edx,
                     myEndItem->pos().y() + edy,
                     myStartItem->pos().x() + sdx,
                     myStartItem->pos().y() + sdy));
    }

    m_connectLine.clear();
    dX = line().p2().x() - line().p1().x();
    dY = line().p2().y() - line().p1().y();

    qreal diffX = line().p2().x() - line().p1().x();
    qreal diffY = line().p2().y() - line().p1().y();
    QPointF pm1(line().p1()), pm2(line().p2());
    QPointF pm0(line().p1()), pm3(line().p2());
    bool step1(false);
    if (xFac*abs(dX) < abs(dY)) {  // VERT
      if (connectType == HalfStep) {
        pm1.setY(line().p2().y());
      } else if (connectType == OneStep) {
        pm1.setY(line().p1().y()+0.5*diffY);
        pm2.setY(pm1.y());
      } else if (connectType == TwoStep) {
        pm1.setY(line().p1().y()+1./3*diffY);
        pm1.setX(line().p1().x()+0.5*diffX);
        pm2.setY(line().p1().y()+2./3*diffY);
        pm2.setX(line().p1().x()+0.5*diffX);
        pm0.setY(line().p1().y()+1./3*diffY);
        pm3.setY(line().p1().y()+2./3*diffY);
      }
    } else { // HORZ
      if (connectType == HalfStep) {
        pm1.setX(line().p2().x());
      } else if (connectType == OneStep) {
        pm1.setX(line().p1().x()+0.5*diffX);
        pm2.setX(pm1.x());
      } else if (connectType == TwoStep) {
        pm1.setX(line().p1().x()+1./3*diffX);
        pm1.setY(line().p1().y()+0.5*diffY);
        pm2.setX(line().p1().x()+2./3*diffX);
        pm2.setY(line().p1().y()+0.5*diffY);

        pm0.setX(line().p1().x()+1./3*diffX);
        pm3.setX(line().p1().x()+2./3*diffX);
      }
    }
    switch(connectType) {
    case Line:
      m_connectLine << line().p1() << line().p2();
      break;
    case HalfStep:
      m_connectLine << line().p1() << pm1 << line().p2();
      break;
    case TwoStep:
      m_connectLine << line().p1() << pm0 << pm1 << pm2 << pm3 << line().p2();
      break;
    case OneStep:
    default:
      m_connectLine << line().p1() << pm1 << pm2 << line().p2();
    }
}

//----------------------------------------------------
// isValid
//----------------------------------------------------

bool GuiQtDiagramConnection::isValid(const QPoint& pt) const{
  return (!m_startAnchor.isNull() && pt.x() >= 0 &&  pt.y() >= 0);
}
