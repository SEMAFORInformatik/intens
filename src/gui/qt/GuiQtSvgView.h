
#if !defined(GUI_QTSVGVIEW_INCLUDED_H)
#define GUI_QTSVGVIEW_INCLUDED_H

#include <QGraphicsView>

class QWheelEvent;
class QPaintEvent;
class QContextMenuEvent;
class QGraphicsSvgItem;
class QSvgRenderer;
class QFile;
class GuiQtText;

class GuiQtSvgView : public QGraphicsView
{
  Q_OBJECT

public:
    enum RendererType { Native, OpenGL, Image };

    GuiQtSvgView(GuiQtText* textEdit, QWidget *parent = 0);

    void setRenderer(QSvgRenderer * renderer);

    void open(const QFile &file, QSvgRenderer* renderer=0);
    void setRenderer(RendererType type = Native);
    void drawBackground(QPainter *p, const QRectF &rect);

Q_SIGNALS:
    void mousePressed(const QGraphicsItem* item,  const QPointF& posScene);

protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent *event);

private:
    RendererType m_renderer;

    QGraphicsSvgItem *m_svgItem;

    QImage m_image;
    GuiQtText* m_textEdit;
};

#endif
