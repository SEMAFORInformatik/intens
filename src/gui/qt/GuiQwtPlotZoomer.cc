
#include <iostream>
#include <limits>
#include <QDate>
#include <QLocale>

#if QWT_VERSION < 0x060000
#include <qwt_plot_canvas.h>
#else
#include <qwt_plot_barchart.h>
#endif
#include <qwt_text.h>

#include "gui/qt/GuiQwtPlot.h"
#include "gui/qt/GuiQwtPlotZoomer.h"
#include "gui/qt/GuiQwtScaleDraw.h"
#include "gui/qt/GuiQwtPlotBarChart.h"
#include "gui/qt/GuiQtFactory.h"
#include "utils/gettext.h"

const int GuiQwtPlotZoomer::PIXEL_TOLERANCE = 10;

//---------------------------------------------------
// Klasse GuiQwtPlotZoomer
//---------------------------------------------------
GuiQwtPlotZoomer::GuiQwtPlotZoomer( int xAxis, int yAxis
				  , int selectionFlags
				  , DisplayMode trackerMode
				  , QWidget *canvas )
#if QWT_VERSION < 0x060000
  : QwtPlotZoomer( xAxis,yAxis,selectionFlags,trackerMode,dynamic_cast<QwtPlotCanvas*>(canvas) ) {}
#else
#if QWT_VERSION < 0x060100
  : QwtPlotZoomer( xAxis,yAxis, dynamic_cast<QwtPlotCanvas*>(canvas), false )
#else
  : QwtPlotZoomer( xAxis,yAxis, canvas, true )
#endif
{
  setTrackerMode(trackerMode);
#if defined( Q_OS_WIN ) || defined( Q_OS_CYGWIN )
  removeMouseButtonPattern(Qt::RightButton);
#endif
}
#endif

void GuiQwtPlotZoomer::removeMouseButtonPattern(int key) {
 QVector<MousePattern>& pl = mousePattern();
 QVector<MousePattern> plNew;
 for (int i=0; i<pl.size(); ++i) {
   if (pl[i].button != key)
     plNew.append( pl[i] );
 }
 setMousePattern(plNew);
}

void GuiQwtPlotZoomer::begin(){
  try {
    QwtPlotZoomer::begin();
  } catch (const std::exception &e) {
    std::cerr << "  <<QwtPlotZoomer::begin>> Exception catched msg["<<e.what()<<"]\n"<<std::flush;
    GuiQtFactory::Instance()->showDialogWarning( NULL, "Exception catched", e.what() );
  }
}

bool GuiQwtPlotZoomer::end(bool ok){
  try {
    return QwtPlotZoomer::end(ok);
  } catch (const std::exception &e) {
    std::cerr << "  <<QwtPlotZoomer::end>> Exception catched msg["<<e.what()<<"]\n"<<std::flush;
    GuiQtFactory::Instance()->showDialogWarning( NULL, "Exception catched", e.what() );
  }
  return false;
}

QwtText GuiQwtPlotZoomer::trackerText(const QPoint &pos)  const {

  // special case BarChart
  QwtPlotItemList curveList = plot()->itemList( QwtPlotItem::Rtti_PlotMultiBarChart );
  if (curveList.count()) {
    GuiQwtPlotBarChart *curve = NULL;
    double dist = std::numeric_limits<double>::max();
    int index = -1;
    for ( QwtPlotItemIterator it = curveList.begin(); it != curveList.end(); ++it ) {

      if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotMultiBarChart ) {
	GuiQwtPlotBarChart *c = dynamic_cast<GuiQwtPlotBarChart *>( *it );
	if (!c || !c->isVisible()) continue;  // invisible

	double d = std::numeric_limits<double>::max();
	int idx = c->closestPoint( pos, &d );
	if ( d < dist ) {
	  curve = c;
	  index = idx;
	  dist = d;
	}
      }
    }

    if (curve && dist < PIXEL_TOLERANCE ) { // 10 pixels tolerance
      QColor bg( Qt::yellow );
      QColor fg( Qt::black );
      std::ostringstream os;
#if 1
      if (curve->isBarStyleOptionPlotItemGrouped() )
	os <<  "X: " <<  curve->getAnnoLabel(index);
      else
	os <<  "X: " <<  curve->sample(index).value;
      os <<  "  Y: " << (curve->sample(index).set.size() > 1 ? "[": "");
      for (int x=0; x < curve->sample(index).set.size(); ++x) {
	os << (x ? ", " : "");
	os << (curve->getYScaleFactor(index) != 0 ?
	       (curve->sample(index).set[x] / curve->getYScaleFactor(index)) :
	       curve->sample(index).set[x]);
      }
      os <<  (curve->sample(index).set.size() > 1 ? "]": "");
#else // no multiBarChart
      os <<  " X: " <<  curve->sample(index).x();
      os <<  " Y: " <<  curve->sample(index).y();
#endif

      QwtText text( QString::fromStdString(os.str()) );
      text.setBackgroundBrush( QBrush( bg.lighter(180) ) );
      text.setColor( fg );
      return text;

    }
  }

  // general case
  std::ostringstream os;
  os << _("X:") << "";
  if (dynamic_cast<const GuiQwtScaleDraw*>(plot()->axisScaleDraw(QwtPlot::xBottom))->isDateType() ) {
    os << QLocale::system().toString(QDate::fromJulianDay(plot()->invTransform(QwtPlot::xBottom, pos.x())),
                                     QLocale::ShortFormat).toStdString();
  }
  else os << plot()->invTransform(QwtPlot::xBottom, pos.x());
  os << "  Y:" << plot()->invTransform(QwtPlot::yLeft, pos.y());
  if ( plot()->axisEnabled(QwtPlot::yRight) ) {
    os << _("  Y2:") << plot()->invTransform(QwtPlot::yRight, pos.y());
  }
  return QString::fromStdString(os.str());
}

//----------------------------------------------------
// widgetMousePressEvent
//----------------------------------------------------
void GuiQwtPlotZoomer::widgetMousePressEvent(QMouseEvent* mouseEvent) {
  select( mouseEvent->pos());
  QwtPlotZoomer::widgetMousePressEvent(mouseEvent);
}

//----------------------------------------------------
// select
//----------------------------------------------------
void GuiQwtPlotZoomer::select( const QPoint &pos )
{
  QwtPlotCurve *curve = NULL;
  double dist = std::numeric_limits<double>::max();

  const QwtPlotItemList& itmList = plot()->itemList();
  for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it ) {
    if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve ) {
      QwtPlotCurve *c = static_cast<QwtPlotCurve *>( *it );
      if (!c->isVisible()) continue;  // invisible
      double d;
      int idx = c->closestPoint( pos, &d );
      if ( idx != -1 && d < dist ) {
	curve = c;
	dist = d;
      }
    }
  }

  if ( curve && dist < PIXEL_TOLERANCE ) { // 10 pixels tolerance
    emit selectedCurve( *curve );
  }
}
