
#ifndef GUIQTDIAGRAMSCENE_H
#define GUIQTDIAGRAMSCENE_H

#include <QObject>
#include <QCursor>
#include <QGraphicsScene>

class GuiQtDiagramPixmapItem;
class GuiQtDiagramConnection;
QT_BEGIN_NAMESPACE
class QGraphicsSceneDragDropEvent;
class QGraphicsViewItem;
QT_END_NAMESPACE

class GuiQtDiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GuiQtDiagramScene(const QRectF& rect, QObject *parent = 0);
    /** get pixmap item */
    GuiQtDiagramPixmapItem* getPixmapItem(const std::string& id) const;
    /** remove all  Connection items */
    void removeAllConnections();
    /** get list of Connection items */
    bool getAllConnections(std::vector<GuiQtDiagramConnection*>& res);
    /** increase size of Scene if item is near the border */
    void updateScreenSize(QGraphicsItem* item);

signals:
    void itemMoved(std::vector<GuiQtDiagramPixmapItem*> movedItem, const QPointF &movedFromPosition);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );

private:
    QGraphicsItem     *m_movingItem;
    QGraphicsLineItem *m_line;
    QCursor            m_oldCursor;
};

#endif
