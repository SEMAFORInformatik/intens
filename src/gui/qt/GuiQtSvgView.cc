#include <QFile>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#if QT_VERSION < 0x060000
#include <QtSvg/QGraphicsSvgItem>
#else
#include <QtSvgWidgets/QGraphicsSvgItem>
#endif
#include <QtSvg/QSvgRenderer>
#include <QPaintEvent>
#include <qmath.h>
#include <iostream>

#include "gui/qt/GuiQtSvgView.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtText.h"
#include "utils/gettext.h"
#include "utils/Debugger.h"

#define QT_NO_OPENGL 1  // not used

#ifndef QT_NO_OPENGL
#include <QGLWidget>
#endif

INIT_LOGGER();

GuiQtSvgView::GuiQtSvgView(GuiQtText* textEdit, QWidget *parent)
    : QGraphicsView(parent)
    , m_renderer(Native)
    , m_svgItem(0)
    , m_textEdit(textEdit)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);
}

void GuiQtSvgView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

void GuiQtSvgView::open(const QFile &file, QSvgRenderer* renderer)
{
    if (!file.exists() && !renderer)
        return;

    QGraphicsScene *s = scene();

    s->clear();
    resetTransform();

    if (file.exists())
      m_svgItem = new QGraphicsSvgItem(file.fileName());
    else {
      m_svgItem = new QGraphicsSvgItem();
      m_svgItem->setSharedRenderer(renderer);
    }

    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::NoCache);
    m_svgItem->setZValue(0);
    m_svgItem->update();

    s->addItem(m_svgItem);
}

void GuiQtSvgView::setRenderer(RendererType type)
{
    m_renderer = type;

#if QT_VERSION < 0x060000
    if (m_renderer == OpenGL) {
#ifndef QT_NO_OPENGL
        setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    } else {
#else
      {
#endif
        setViewport(new QWidget);
    }
}

void GuiQtSvgView::paintEvent(QPaintEvent *event)
{
  // 2017-11-15 amg evtl. verhindern wir so den Absturz
  try {
    if (m_renderer == Image) {
        if (m_image.size() != viewport()->size()) {
            m_image = QImage(viewport()->size(), QImage::Format_ARGB32_Premultiplied);
        }

        QPainter imagePainter(&m_image);
        QGraphicsView::render(&imagePainter);
        imagePainter.end();

        QPainter p(viewport());
        p.drawImage(0, 0, m_image);

    } else {
        QGraphicsView::paintEvent(event);
    }
   
  } catch( const std::exception e ) {
    std::cerr << "GuiQtSvgView::paintEven Error: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "GuiQtSvgView::paintEvent" << std::endl;
  }
}

void GuiQtSvgView::wheelEvent(QWheelEvent *event)
{
    qreal factor = qPow(1.2, event->angleDelta().y() / 240.0);
    scale(factor, factor);
    event->accept();
}

void GuiQtSvgView::setRenderer(QSvgRenderer * renderer) {
  if (!m_svgItem) {
    QFile f;
    open(f, renderer);
  } else {
    m_svgItem->setSharedRenderer(renderer);
  }

  // own fit to content
  // this is not working properly
  // fitInView(m_svgItem, Qt::KeepAspectRatioByExpanding);
  QRectF r = m_svgItem->boundingRect();
  QSize c = contentsRect().size();
  if (m_textEdit)
    c = m_textEdit->myWidget()->size();
  double fac = 0.99 * std::min(c.width()/r.width(),c.height()/r.height());
  BUG_DEBUG("Fac["<<fac<<"] <= Content["<<c.width()<<", "<<c.height()<<"] bb["<<r.width()<<", "<<r.height()<<"]");
  scale(fac,fac);
}

/* --------------------------------------------------------------------------- */
/*  -- contextMenuEvent                                                        */
/* --------------------------------------------------------------------------- */
void GuiQtSvgView::contextMenuEvent ( QContextMenuEvent * event ) {
  GuiQtMenuButton* button;
  GuiQtPopupMenu* _popup_menu(0);
  QMenu *menu = findChild<QMenu *>();

  // no m_textEdit, return
  if (!m_textEdit) return;
  
  if (!menu)
    menu =  new QMenu();
  QFont font = menu->font();
  menu->setFont( QtMultiFontString::getQFont( "@popup@", font ) );
  menu->addSeparator();

  _popup_menu = new GuiQtPopupMenu(m_textEdit, m_textEdit->MenuLabel(), menu);
  _popup_menu->setTearOff( true );
  _popup_menu->clearDefaultMenu( false );
  _popup_menu->attach( new GuiQtSeparator( _popup_menu->getElement() ) );
    
  // Save Menu
  button = new GuiQtMenuButton( _popup_menu, &(m_textEdit->m_save_listener) );
  _popup_menu->attach( button );
  button->setDialogLabel( _("Save") );

  // Print Menu
  button = new GuiQtMenuButton( _popup_menu, &(m_textEdit->m_print_listener) );
  _popup_menu->attach( button  );
  button->setDialogLabel( _("Print") );

  _popup_menu->create();
  _popup_menu->popup();
  delete _popup_menu;
}

void GuiQtSvgView::mousePressEvent(QMouseEvent * event ) {
  QList<QGraphicsItem *> selected = items(event->pos());
  QRectF sr = this->sceneRect();
  QRectF vr = m_svgItem->renderer()->viewBoxF();
  QPointF spos = this->mapToScene(event->pos());

  // shrink sceneRect
#if QT_VERSION >= 0x050900
  sr = sr - QMarginsF(-sr.x(),-sr.y(),-sr.x(),-sr.y());
#else
  sr = QRectF(0, 0, sr.width() + 2 * sr.x(), sr.height() + 2 * sr.y());
#endif

  // recalc point on sceneRect
  QPointF sposOut((vr.x() + vr.width()*spos.x()/sr.width()),
                  (vr.y() + vr.height()*spos.y()/sr.height()));
  if (selected.size() == 1) {
    QGraphicsItem* svgitem = selected[0];
    emit mousePressed(svgitem, sposOut);
  }
  QGraphicsView::mousePressEvent(event);
}
