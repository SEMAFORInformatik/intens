
#ifndef GUIQTDIAGRAMITEM_H
#define GUIQTDIAGRAMITEM_H

#include <QGraphicsPolygonItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>

#include "job/JobElement.h"
#include "gui/GuiNavigator.h"

class GuiQtNavElement;

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
QT_END_NAMESPACE

class GuiQtDiagramItem : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 1 };
    enum DiagramType { Box, Triangle };

    GuiQtDiagramItem(DiagramType diagramType, QGraphicsItem *item = 0,
            QGraphicsScene *scene = 0);

    DiagramType diagramType() const {
        return polygon() == boxPolygon ? Box : Triangle;
    }
    int type() const { return Type; }

private:
    QPolygonF boxPolygon;
    QPolygonF trianglePolygon;
 };

class GuiQtDiagramPixmapItem: public QObject, public QGraphicsPixmapItem
{
  Q_OBJECT
public:
    enum { Type = UserType + 2 };
    GuiQtDiagramPixmapItem(const QPixmap& pixmap, GuiQtNavElement *navElem
			  ,int GRID_X, int GRID_Y);
    virtual ~GuiQtDiagramPixmapItem();

    int type() const override { return Type; }
    bool isValid() const { return m_valid; }
    QRectF boundingRect() const override;
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) override;
    std::string getVarname();
    std::string getId();
    void refresh();
    void call_function(JobElement::CallReason reason);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

signals:
    void selectedChange(GuiQtDiagramPixmapItem *item);
    void lostFocus(GuiQtDiagramPixmapItem *item);
    void activated(GuiQtDiagramPixmapItem *item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
private:
    QPoint getPositionDataPool();
    unsigned int getMenuIndexDataPool();

    QPolygonF boxPolygon;
    QPolygonF trianglePolygon;

 private:
    GuiQtNavElement* m_navElem;
    std::string      m_id;
    bool             m_valid;

    int GRID_LEN_X;
    int GRID_LEN_Y;
};


class GuiQtDiagramConnection : public QGraphicsLineItem
{
public:
  enum { Type = UserType + 4 };
  enum ConnectType { Line, HalfStep, OneStep, TwoStep };

  GuiQtDiagramConnection(GuiQtDiagramPixmapItem *startItem, QPoint& startAnchor,
                         GuiQtDiagramPixmapItem *endItem, QPoint& endAnchor,
                         ConnectType connectType, int lineWidth=-1, std::string lineStyle=std::string(),
                         QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
  GuiQtDiagramConnection(GuiQtDiagramPixmapItem *startItem,
                         GuiQtDiagramPixmapItem *endItem,
                         GuiNavigator::ConnectionAttr* attr);
  virtual ~GuiQtDiagramConnection();

  int type() const
  { return GuiQtDiagramConnection::Type; }
  QRectF boundingRect() const;
  QPainterPath shape() const;
  bool getPointList(std::vector<double>& xpos, std::vector<double>& ypos);
  void setColor(const QColor &color)
  { myColor = color; }
  GuiQtDiagramPixmapItem *startItem() const
  { return myStartItem; }
  GuiQtDiagramPixmapItem *endItem() const
  { return myEndItem; }

  void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
private:
  void updateConnectLines();
  bool isValid(const QPoint& pt) const;

private:
    GuiQtDiagramPixmapItem *myStartItem;
    GuiQtDiagramPixmapItem *myEndItem;
    ConnectType connectType;
    int lineWidth;
    std::string lineStyle;
    QColor myColor;
    QPoint m_startAnchor, m_endAnchor;
    QPolygonF m_connectLine;
};

#endif
