
#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_scale_widget.h>
#include <QMouseEvent>
#include <QColorDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QDrag>

#include "GuiQwtScaleDraw.h"
#include "GuiQwtPlotScalePicker.h"

GuiQwtPlotScalePicker::GuiQwtPlotScalePicker(QwtPlot *plot)
  : QObject(plot) {
  refresh();
}

  bool GuiQwtPlotScalePicker::eventFilter(QObject *object, QEvent *e)
  {
    QwtScaleWidget *scale = (QwtScaleWidget *)object;

    // double click mouse event
    if ( e->type() == QEvent::MouseButtonDblClick ){
      mouseDblClicked(scale, ((QMouseEvent *)e)->pos());
      return true;
    }

    if ( e->type() == QEvent::DragEnter ){
      QDragEnterEvent *dee = (QDragEnterEvent *)e;
      // only own drag will accept
      if (dee->source() == scale)
        e->accept();
    }
    if ( e->type() == QEvent::DragMove ){
      QDragMoveEvent *dme = (QDragMoveEvent *)e;
      dragMove(scale, dme);
    }

    // drop event
    if ( e->type() == QEvent::Drop ){
      const QDropEvent *de = (const QDropEvent *)e;
      drop(scale, de);
    }

    // mouse press event
    if ( e->type() == QEvent::MouseButtonPress ){
      const QMouseEvent *me = (const QMouseEvent *)e;
      QPoint pos = me->pos();
      if (me->button() == Qt::LeftButton){
        mouseLeftClicked(scale, pos);
        return true;
      } else if (me->button() == Qt::RightButton){
        mouseRightClicked(scale, pos);
        return true;
      } else if (me->button() == Qt::MiddleButton){
        mouseMiddleClicked(scale, pos);
        return true;
      }
    }
    return QObject::eventFilter(object, e);
  }

  void GuiQwtPlotScalePicker::refresh() {
    for ( uint i = 0; i < QwtPlot::axisCnt; i++ ) {
      QwtScaleWidget *scale = (QwtScaleWidget *)plot()->axisWidget(i);
      if ( scale )
        scale->installEventFilter(this);
    }
  }

  void GuiQwtPlotScalePicker::dragMove(QwtScaleWidget *scale, QDragMoveEvent* dme) {
#if QWT_VERSION >= 0x060000
    const QwtLinearColorMap* linearColormap = dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
#if QT_VERSION >= 0x060000
      QPoint pos(dme->position().toPoint());
#else
      QPoint pos(dme->pos());
#endif
    if (scaleRect(scale).contains(pos) && linearColormap && scale->isColorBarEnabled()) {
      QList< double > ticks = linearColormap->colorStops().toList();

      QRect rFirst = getTickLabelBoundingRect(scale, 0); // lowerband
      QRect rLast = getTickLabelBoundingRect(scale, ticks.size()-1); // upperband
      if (pos.x() > (rFirst.x()) || pos.y() > (rFirst.y()) ||
          pos.y() < (rLast.y()+rFirst.height()))
        dme->ignore();
      else
        dme->accept();
    }
#endif
  }

  void GuiQwtPlotScalePicker::drop(QwtScaleWidget *scale, const QDropEvent* dropEvent) {
#if QWT_VERSION >= 0x060000
    const QwtLinearColorMap* linearColormap = dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
#if QT_VERSION >= 0x060000
    QPoint pos(dropEvent->position().toPoint());
#else
    QPoint pos(dropEvent->pos());
#endif
    if (scaleRect(scale).contains(pos) && linearColormap && scale->isColorBarEnabled()) {
      QList< double > ticks = linearColormap->colorStops().toList();
      double tlen = scale->scaleDraw()->tickLength (QwtScaleDiv::MajorTick);

      QRect rFirst = getTickLabelBoundingRect(scale, 0); // first
      QRect rLast = getTickLabelBoundingRect(scale, ticks.size()-1); // last
      double value = 1.0-(pos.y()-rLast.y()-0.5*tlen)/(rFirst.y()-rLast.y());
      emit colorStopPositionChanged(scale, value, dropEvent->mimeData()->text().toInt());
    }
#endif
  }

  void GuiQwtPlotScalePicker::mouseLeftClicked(QwtScaleWidget *scale, const QPoint &pos) {
#if QWT_VERSION >= 0x060000
    const QwtLinearColorMap* linearColormap = dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
    // only interessed in linear colormap
    if (scaleRect(scale).contains(pos) && linearColormap && scale->isColorBarEnabled()) {

      // loop to get clicked label
      QList< double > ticks = linearColormap->colorStops().toList();
      for (int i=0; i < ticks.size(); ++i) {

        // get 'real' colorstop  position
        QRect r = getTickLabelBoundingRect(scale, i);
        double tlen = scale->scaleDraw()->tickLength (QwtScaleDiv::MajorTick);
        r.setWidth(tlen);
        r.moveLeft( r.x()-tlen );

        if (r.contains(pos)) {
          scale->setAcceptDrops (true);
          // create drag object
          QColor c = linearColormap->color(QwtInterval(0,1), ticks.at(i));
          QDrag* drag = new QDrag(scale);
          QMimeData *mimeData = new QMimeData;
          mimeData->setColorData(c);
          mimeData->setText(QString::number(i));
          drag->setMimeData(mimeData);
          drag->setHotSpot(QPoint(0.5*tlen,0.5*tlen));
          QPixmap tempPixmap(tlen, tlen);
          tempPixmap.fill(c);
          drag->setPixmap(tempPixmap);
          drag->exec();
          return;
        }
      }
    }
#endif
  }

  void GuiQwtPlotScalePicker::mouseMiddleClicked(QwtScaleWidget *scale, const QPoint &pos) {
#if QWT_VERSION >= 0x060000
    if (scaleRect(scale).contains(pos)) {
      const QwtLinearColorMap* linearColormap = dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());

      // loop to get clicked label to change existing color
      if (linearColormap && scale->isColorBarEnabled()) {
	QList< double > ticks = linearColormap->colorStops().toList();
	double tlen = scale->scaleDraw()->tickLength (QwtScaleDiv::MajorTick);
	int i;
	for (i=1; i < ticks.size()-1; ++i) {
	  // get 'real' colorstop  position
	  QRect r = getTickLabelBoundingRect(scale, i);
	  r.setWidth(3*tlen);
	  r.setHeight(2*tlen);
	  r.moveLeft( r.x()-3*tlen );
	  if (r.contains(pos)) {
	    QMessageBox msgBox;
	    QColor c = linearColormap->color(QwtInterval(0,1), ticks.at(i));
	    QPixmap tempPixmap(32,32);
	    tempPixmap.fill(c);
	    msgBox.setIconPixmap(tempPixmap);
	    msgBox.setText("The you really want to delete this color stop?");
	    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	    if (msgBox.exec() == QMessageBox::Yes) {
	      emit colorStopColorDelete(scale, i);
	    }
	    break;
	  }
	}
      }
    }
#endif
  }

  void GuiQwtPlotScalePicker::mouseRightClicked(QwtScaleWidget *scale, const QPoint &pos) {
#if QWT_VERSION >= 0x060000
    if (scaleRect(scale).contains(pos)) {
      const QwtLinearColorMap* linearColormap = dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());

      // loop to get clicked label to change existing color
      if (linearColormap && scale->isColorBarEnabled()) {
	QList< double > ticks = linearColormap->colorStops().toList();
	double tlen = scale->scaleDraw()->tickLength (QwtScaleDiv::MajorTick);
	int i;
	for (i=0; i < ticks.size(); ++i) {

	  // get 'real' colorstop  position
	  QRect r = getTickLabelBoundingRect(scale, i);
	  r.setWidth(3*tlen);
	  r.setHeight(tlen);
	  r.moveLeft( r.x()-3*tlen );
	  r.setHeight(2*tlen);
	  if (r.contains(pos)) {
	    QColor c = linearColormap->color(QwtInterval(0,1), ticks.at(i));
	    QColor color = QColorDialog::getColor(c);
	    if (color.isValid()) {
	      emit colorStopColorChanged(scale, color, i);
	    }
	    break;
	  }
	}

	// no existing color clicked => insert new colorstop
	if (i == ticks.size() && pos.x() < 26) {
	  QColor color = QColorDialog::getColor(Qt::magenta);
	  if (color.isValid()) {
	    QRect rFirst = getTickLabelBoundingRect(scale, 0); // first
	    QRect rLast = getTickLabelBoundingRect(scale, ticks.size()-1); // last
	    double value = 1.0-(pos.y()-rLast.y()-0.5*tlen)/(rFirst.y()-rLast.y());
	    emit colorStopColorNew(scale, color, value);
	  }
	}
      }

      emit axisRightClicked(scale->alignment());
    } else
      emit axisTitleRightClicked(scale->alignment());
#endif
  }

  void GuiQwtPlotScalePicker::mouseDblClicked(QwtScaleWidget *scale, const QPoint &pos) {
#if QWT_VERSION >= 0x060000
    if (scaleRect(scale).contains(pos) ) {
      emit axisDblClicked(scale->alignment());
      const QwtLinearColorMap* linearColormap = dynamic_cast<const QwtLinearColorMap*>
        (scale->colorMap());

      // get colormap stop or major ticks depends on usage
      QList< double > ticks = (scale->isColorBarEnabled() && linearColormap) ?
        linearColormap->colorStops().toList() :
        scale->scaleDraw()->scaleDiv().ticks (QwtScaleDiv::MajorTick);

      // loop to get clicked label
      for (int i=0; i < ticks.size(); ++i) {

        // colormap trigger levels (lowerbound, upperbound) ==> continue
        if (scale->isColorBarEnabled() && !linearColormap) continue;

        // get 'real' Label position
        QRect r = getTickLabelBoundingRect(scale, i);
        if (r.contains(pos)) {
          emit axisMajorTickDblClicked(scale, ticks.at(i), r);
          return;
        }
      }
    } else
      emit axisTitleDblClicked(scale->alignment());
#endif
  }

  QRect GuiQwtPlotScalePicker::getTickLabelBoundingRect(const QwtScaleWidget *scale, int tickIdx) {
#if QWT_VERSION >= 0x060000
    const QwtLinearColorMap* linearColormap = dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
    QList< double > ticks = (scale->isColorBarEnabled() && linearColormap) ?
      linearColormap->colorStops().toList() :
      scale->scaleDraw()->scaleDiv().ticks (QwtScaleDiv::MajorTick);

    // if index is too big, set to last index
    if (tickIdx >= ticks.size())
      tickIdx = ticks.size()-1;
    else
      if (tickIdx < 0) tickIdx = 0;

    // get 'real' Label position
    QRectF r = scale->scaleDraw()->labelRect (scale->font(), ticks.at(tickIdx));
    QPointF pt = scale->scaleDraw()->labelPosition(ticks.at(tickIdx));
    r.moveTo( r.x() + pt.x(), r.y() + pt.y() );
    return r.toRect();
#endif
  }

  // The rect of a scale without the title
  QRect GuiQwtPlotScalePicker::scaleRect(const QwtScaleWidget *scale) const {
    int margin = 1; // pixels tolerance
    QRect rect = scale->rect();
    rect.setRect(rect.x() - margin, rect.y() - margin, rect.width() + 2 * margin, rect.height() +  2 * margin);

    if (scale->title().text().isEmpty())
      return rect;

    int dh = scale->title().textSize().height();
    switch(scale->alignment())
      {
      case QwtScaleDraw::LeftScale:
        {
          rect.setLeft(rect.left() + dh);
          break;
        }
      case QwtScaleDraw::RightScale:
        {
          rect.setRight(rect.right() - dh);
          break;
        }
      case QwtScaleDraw::BottomScale:
        {
          rect.setBottom(rect.bottom() - dh);
          break;
        }
      case QwtScaleDraw::TopScale:
        {
          rect.setTop(rect.top() + dh);
          break;
        }
      }
    return rect;
  }

  QwtPlotScaleLineEdit::QwtPlotScaleLineEdit(QwtPlot* plot, QwtScaleWidget* scale, QRect boundingRect,
                                             double value, bool typeColorMap)
    : QLineEdit(QString::number(value), scale)
    , plot(plot)
    , scaleWidget(scale)
    , oldValue(value)
    , typeColorMap(typeColorMap) {
    int _width  = fontMetrics().horizontalAdvance( QString::number(value)+"00" );
    _width = std::max(_width, (int)(0.5*width())); // minimum 50% of width
    int _height  = 2*floor((fontMetrics().xHeight() + 1)/2) ;

    // move to coordinates based on ParentWidget QwtPlot
    boundingRect.moveTo( boundingRect.x(), boundingRect.y()-_height);
    boundingRect.setWidth(  (boundingRect.width() > _width ? boundingRect.width() : _width) );
    boundingRect.setHeight( boundingRect.height()+_height);

    // if outside parent rect => move inside
    if (boundingRect.y() < 0)
      boundingRect.moveTop(0);
    if ((boundingRect.y()+boundingRect.height()) > scale->height())
      boundingRect.moveTop( scale->height()-boundingRect.height());
    if (boundingRect.x() < 0)
      boundingRect.moveLeft(0);
    if ((boundingRect.x()+boundingRect.width()) > scale->width())
      boundingRect.moveLeft( scale->width()-boundingRect.width());
    if (boundingRect.x() < 0)
      boundingRect.setX(0);

    // set geometry, show and focus
    setGeometry( boundingRect );
    show();
    setFocus();

    connect(this, SIGNAL(editingFinished()),
            SLOT(slot_editingFinished()) );
    }

  QwtPlotScaleLineEdit::~QwtPlotScaleLineEdit() {
  }

  void QwtPlotScaleLineEdit::slot_editingFinished() {
#if QWT_VERSION >= 0x060000
    if (text().trimmed().size() > 0 && text().toDouble() == oldValue) {
      deleteLater();
      return;
    }

    QList< double > ticks = scaleWidget->scaleDraw()->scaleDiv().ticks ( QwtScaleDiv::MajorTick);
    GuiQwtScaleDraw* qwtScaleDraw = dynamic_cast<GuiQwtScaleDraw*>(scaleWidget->scaleDraw());
    if (text().trimmed().size() == 0) {
        int i = 0;
        while (i < QwtPlot::axisCnt) {
          if (plot->axisWidget(i) == scaleWidget) {
            plot->setAxisAutoScale(i, true);
            QwtInterval interval = plot->axisInterval( QwtPlot::yRight );
          }
          ++i;
        }
        if (qwtScaleDraw) {
          qwtScaleDraw->resetUserScale();
          scaleWidget->update();
        }
        emit resetScale(scaleWidget);
    } else {
      // special case colormap => only emit signal
      if (typeColorMap) {
	if (oldValue <= ticks.first() || oldValue >= ticks.back()) {
	  qwtScaleDraw->setMinUserScale( ticks.first() );
	  qwtScaleDraw->setMaxUserScale( ticks.back() );
	}
        emit finished(scaleWidget, text().toDouble());
        deleteLater();
        return;
      }
      // set wished user scale
      double min = ticks.first();
      double max = ticks.back ();
      QwtInterval interval = scaleWidget->scaleDraw()->scaleDiv().interval();
      if (min >=  oldValue) {
        min = text().toDouble();
        max = interval.maxValue();
      }
      else
        if (max <= oldValue) {
          max = text().toDouble();
          min = interval.minValue();
        }
      if (min==max) {
        return;
      } else {
        int i=0;
        do {
          if (plot->axisWidget(i) != scaleWidget)
            continue;
          plot->setAxisScale(i,
                             std::min(min, max),
                             std::max(min, max));
          plot->setAxisAutoScale(i, false);

        } while (++i < QwtPlot::axisCnt);
      }
      if (qwtScaleDraw) {
        qwtScaleDraw->setMinUserScale( std::min(min, max) );
        qwtScaleDraw->setMaxUserScale( std::max(min, max) );
      }
      emit finished(scaleWidget, text().toDouble());
    }

    plot->replot();
    deleteLater();
#endif
  }
