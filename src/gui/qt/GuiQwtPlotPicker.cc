
#include <sstream>
#include <limits>
#include <QApplication>
#include <QDate>

#include <qwt_plot_directpainter.h>
#include <qwt_plot_canvas.h>
#include <qwt_picker_machine.h>
#include <qwt_text.h>

#include "gui/qt/GuiQwtPlotPicker.h"
#include "gui/qt/GuiQwtPlot.h"
#include "gui/qt/GuiQwtScaleDraw.h"
#include "gui/qt/GuiQwtPlotBarChart.h"

#include "utils/gettext.h"

const int GuiQwtPlotPicker::PIXEL_TOLERANCE = 10;

//---------------------------------------------------
// Klasse GuiQwtPlotPicker
//---------------------------------------------------
GuiQwtPlotPicker::GuiQwtPlotPicker( int xAxis, int yAxis
				  , int selectionFlags
				  , RubberBand rubberBand
				  , DisplayMode trackerMode
				  , QwtPlot *qwtplot
				  , GuiQWTPlot* plot )
#if QWT_VERSION < 0x060000
  : QwtPlotPicker( xAxis,yAxis,selectionFlags,rubberBand,trackerMode,qwtplot->canvas() )
#elif QWT_VERSION < 0x060100
    : QwtPlotPicker( xAxis,yAxis,rubberBand,trackerMode,dynamic_cast<QwtPlotCanvas*>(qwtplot->canvas()) )
#else
    : QwtPlotPicker( xAxis,yAxis,rubberBand,trackerMode,qwtplot->canvas() )
#endif
    , m_qwtPlot(qwtplot)
    , m_plot(plot) {
  m_qwtPlot->canvas()->installEventFilter( this );
  }

//----------------------------------------------------
// getSelectionPoints
//----------------------------------------------------
void GuiQwtPlotPicker::getSelectionPoints(Gui2dPlot::tPointVector& pts,
					  std::vector<int>& axisType, std::vector<std::string>& axisTitle) {
  pts.clear();
  pts.reserve(m_selectedCurvePoints.data().size());
  axisType.clear();
  axisType.reserve(m_selectedCurvePoints.data().size());

  SelectedCurvePoints::CurvePointIterator it =  m_selectedCurvePoints.data().begin();
  for (; it !=  m_selectedCurvePoints.data().end(); ++it) {
    pts.push_back( Gui2dPlot::Point((*it)->xpos, (*it)->ypos) );
    axisType.push_back( (*it)->curve->yAxis()+1 ); // !!! 1 => Y1 , 2 => Y2  Axis
    axisTitle.push_back( (*it)->curveLabel->text().toStdString() );
  }
}


//----------------------------------------------------
// clearSelection
//----------------------------------------------------
bool GuiQwtPlotPicker::clearSelection(QwtPlotCurve* curve) {
  SelectedCurvePoints::CurvePointIterator it =  m_selectedCurvePoints.data().begin();
  for (int cnt=0; it !=  m_selectedCurvePoints.data().end(); ++it, ++cnt) {
    // get curve
    QwtPlotCurve* curveSel = getCurve((*it));
    if ( curveSel == curve) {
      m_selectedCurvePoints.data().erase(it);
      return clearSelection(curve); // clear next point recursively
    }
  }
  return false;
}

//----------------------------------------------------
// eventFilter
//----------------------------------------------------
bool GuiQwtPlotPicker::eventFilter( QObject *object, QEvent *event ) {
  // Rect Selection
  if (stateMachine() &&
      stateMachine()->selectionType() == QwtPickerMachine::RectSelection ) {
    return QwtPlotPicker::eventFilter( object, event );
  }

  // Point Selection => our work
  if ( m_plot == NULL || object != m_qwtPlot->canvas() )
        return false;

    switch( event->type() )
    {
        case QEvent::FocusIn:
        {
            showSelectedPoints();
            break;
        }
        case QEvent::FocusOut:
        {
            showSelectedPoints();
            break;
        }
        case QEvent::Paint:
        {
	  showSelectedPoints(); // damit sie zumindest mal wieder gezeichnet werden
	  break;
        }
        case QEvent::MouseButtonPress:
        {
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
	    if ( mouseEvent->button() == Qt::LeftButton) {
	      select( mouseEvent->pos() );
	      return true;
	    } else if ( mouseEvent->button() == Qt::MiddleButton) {
	      select( mouseEvent->pos(), true );
	      return true;
	    }
        }
        case QEvent::KeyPress:
        {
            const QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );

            switch( keyEvent->key() )
            {
                case Qt::Key_Right:
                case Qt::Key_Plus:
		    shiftPointCursor( true );
		    break;
                case Qt::Key_Left:
                case Qt::Key_Minus:
		  shiftPointCursor( false );
		  break;
                default:
                    break;
            }
        }
        default:
            break;
    }

    return QwtPlotPicker::eventFilter( object, event );
}

// Select the point at a position. If there is no point
// deselect the selected point
//----------------------------------------------------
// select
//----------------------------------------------------
void GuiQwtPlotPicker::select( const QPoint &pos, bool unselect )
{
    QwtPlotCurve *curve = NULL;
    double dist = std::numeric_limits<double>::max();
    int index = -1;

    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve *>( *it );
	    if (!c->isVisible()) continue;  // invisible

            double d;
            int idx = c->closestPoint( pos, &d );
            if ( idx != -1 && d < dist )
            {
                curve = c;
                index = idx;
                dist = d;
            }
        }
    }

    if ( curve && dist < PIXEL_TOLERANCE ) // 10 pixels tolerance
    {
      if ( ! m_plot->hasFunction() ) {
        // append selected point
        if ( !append(curve, index, unselect) ) {
          hideSelectedPoint(curve, index);
        }
        showSelectedPoints();
      }
    } else {
      curve = NULL;
      index = -1;
    }
    if (m_plot->hasFunction() && !unselect) {
      QPointF flpos(plot()->invTransform(QwtPlot::xBottom, pos.x()),
                    plot()->invTransform(QwtPlot::yLeft, pos.y()));
      QPointF frpos;
      if (true)
        frpos =QPointF(plot()->invTransform(QwtPlot::xBottom, pos.x()),
                       plot()->invTransform(QwtPlot::yRight, pos.y()));
      emit selectedPoint(curve, index, flpos, frpos);
    }
}

//----------------------------------------------------
// shiftPointCursor
//----------------------------------------------------
void GuiQwtPlotPicker::shiftPointCursor( bool up ) {
  SelectedCurvePoints::CurvePoint* curvePoint = m_selectedCurvePoints.back();
  if ( !curvePoint )
        return;
    QwtPlotCurve* curve = getCurve( curvePoint );
    if ( !curve )
        return;

    int index = m_selectedCurvePoints.back()->pointIdx + ( up ? 1 : -1 );
    index = ( index + curve->dataSize() ) % curve->dataSize();

    // skip a already selected point => recall this function
    SelectedCurvePoints::CurvePointIterator it = m_selectedCurvePoints.data().begin();
    for (;it !=  m_selectedCurvePoints.data().end(); ++it) {
      if ( index == (*it)->pointIdx &&
	   curve == (*it)->curve ) {
	hideSelectedPoint(curve, m_selectedCurvePoints.back()->pointIdx);
        m_selectedCurvePoints.back()->pointIdx = index;
	m_selectedCurvePoints.back()->xpos = curve->data()->sample( index ).x();
	m_selectedCurvePoints.back()->ypos = curve->data()->sample( index ).y();
	shiftPointCursor( up );
	return;
      }
    }

    // select new point
    if ( index != m_selectedCurvePoints.back()->pointIdx )
    {
	hideSelectedPoint(curve, m_selectedCurvePoints.back()->pointIdx);
        m_selectedCurvePoints.back()->pointIdx = index;
	m_selectedCurvePoints.back()->xpos = curve->data()->sample( index ).x();
	m_selectedCurvePoints.back()->ypos = curve->data()->sample( index ).y();
        showSelectedPoints();
    }
}

//----------------------------------------------------
// hideSelectedPoint
//----------------------------------------------------
void GuiQwtPlotPicker::hideSelectedPoint( QwtPlotCurve* curve, int index )
{

#if QWT_VERSION >= 0x060000
  // unselect (hide) this point

  // get brush, pen, size
  const QwtSymbol *csymbol = curve->symbol();
  QBrush brush = csymbol ? csymbol->brush() :  curve->brush();
  QPen pen = csymbol ? csymbol->pen() : curve->pen();
  QSize size = csymbol ? csymbol->size() : QSize(6,6);

  // get symbol style
  QwtSymbol::Style symbolStyleUSel = csymbol == NULL ? QwtSymbol::Diamond :
    (csymbol->style() != QwtSymbol::NoSymbol ? csymbol->style() : QwtSymbol::Diamond);
  QwtSymbol::Style symbolStyleOrg = csymbol == NULL ? QwtSymbol::NoSymbol :
    (csymbol->style() != QwtSymbol::NoSymbol ? csymbol->style() : QwtSymbol::NoSymbol);

  // create new QSymbols
  QwtSymbol* symbolUSel = new QwtSymbol(symbolStyleUSel, QBrush( m_qwtPlot->canvasBackground().color() ),
					QPen( m_qwtPlot->canvasBackground().color() ), size);
  QwtSymbol* symbolSel = new QwtSymbol(symbolStyleUSel, brush, pen, size);

  // set selection symbol and do drawing
  QwtPlotDirectPainter directPainter;
  curve->setSymbol( symbolUSel );
  directPainter.drawSeries( curve, index, index );
  curve->setSymbol( NULL );
  directPainter.drawSeries( curve, (index ? index-1 : index), index+1 );
  curve->setSymbol( symbolSel );

  // set orignal symbol
  QwtSymbol* symbolOrg = new QwtSymbol(symbolStyleOrg, brush, pen, size);
  curve->setSymbol( symbolOrg );
#endif
}


//----------------------------------------------------
// showSelectedPoints
//----------------------------------------------------
void GuiQwtPlotPicker::showSelectedPoints()
{
#if QWT_VERSION >= 0x060000
  QwtPlotDirectPainter directPainter;
  SelectedCurvePoints::CurvePointIterator it = m_selectedCurvePoints.data().begin();
  for (int cnt=0; it !=  m_selectedCurvePoints.data().end(); ++it, ++cnt) {

    // get curve
    QwtPlotCurve* curve = getCurve((*it));
    if (!curve->isVisible()) continue;

    // get symbol style
    const QwtSymbol *csymbol = curve->symbol();
    QwtSymbol::Style symbolStyleSel = csymbol == NULL ? QwtSymbol::Diamond : (csymbol->style() != QwtSymbol::NoSymbol ? csymbol->style() : QwtSymbol::Diamond);
    QwtSymbol::Style symbolStyleOrg = csymbol == NULL ? QwtSymbol::NoSymbol : (csymbol->style() != QwtSymbol::NoSymbol ? csymbol->style() : QwtSymbol::NoSymbol);

    // get brush and pen
    QBrush brush = csymbol ? csymbol->brush() :  curve->brush();
    QPen pen = csymbol ? csymbol->pen() : curve->pen();
    QSize size = csymbol ? csymbol->size() : QSize(6,6);

    // create new QSymbols
    QwtSymbol* symbolSel = new QwtSymbol(symbolStyleSel, brush, pen, size);
    QwtSymbol* symbolOrg = new QwtSymbol(symbolStyleOrg, brush, pen, size);
    if (cnt <  m_selectedCurvePoints.data().size()-1) {
      symbolSel->setBrush( brush.color().lighter( 250 ) );
    } else {
      symbolSel->setBrush( QColor(Qt::magenta) );
    }

    // set selection symbol and do drawing
    curve->setSymbol( symbolSel );
    directPainter.drawSeries( curve, (*it)->pointIdx, (*it)->pointIdx );
    directPainter.reset();

    // set orignal symbol
    curve->setSymbol( symbolOrg );
  }
#endif
}

//----------------------------------------------------
// getCurve
//----------------------------------------------------
QwtPlotCurve* GuiQwtPlotPicker::getCurve( const SelectedCurvePoints::CurvePoint* curvePoint )
{
    QwtPlotCurve *curve = NULL;

    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve *>( *it );
	    // found ?
	    if (c->title() == *(curvePoint->curveLabel))
	      return c;
        }
    }
    // not found
    return NULL;
}

//----------------------------------------------------
// append
//----------------------------------------------------
bool GuiQwtPlotPicker::append(QwtPlotCurve* curve, int pointIdx, bool unselect ) {

  if (pointIdx < 0) return false;

  SelectedCurvePoints::CurvePointIterator nearestPointIterator = m_selectedCurvePoints.data().end();
  double nearestDiff = std::numeric_limits<double>::max();
  SelectedCurvePoints::CurvePointIterator it = m_selectedCurvePoints.data().begin();
  for (;it !=  m_selectedCurvePoints.data().end(); ++it) {

    // exactly found
    if (*((*it)->curveLabel) == curve->title() && (*it)->pointIdx == pointIdx) {
	// unselect mode : remove selected curve point
	if (unselect) {
	  m_selectedCurvePoints.data().erase( it );
	  return false;
	} else {
	  break;
	}
    }

    // unselect mode : find nearest selected curve point to remove
    if (unselect && *(*it)->curveLabel == curve->title()) {
      QPointF pt = curve->sample(pointIdx);
      double diff = sqrt( pow((pt.rx() - (*it)->xpos), 2) + pow( (pt.ry() - (*it)->ypos), 2) );
      if (diff < PIXEL_TOLERANCE && diff < nearestDiff ) {
	nearestPointIterator = it;
	nearestDiff = diff;
      }
    }
  }

  // unselect mode : remove nearest point
  if (unselect && nearestPointIterator !=  m_selectedCurvePoints.data().end()) {
    hideSelectedPoint(curve, (*nearestPointIterator)->pointIdx);
    m_selectedCurvePoints.data().erase( nearestPointIterator );
    return false;
  }

  // select mode : append new selection point
  if ( !unselect && it ==  m_selectedCurvePoints.data().end() ) {
    m_selectedCurvePoints.data().push_back( new SelectedCurvePoints::CurvePoint(curve, pointIdx) );
    return true;
  }

  return false;
}

//----------------------------------------------------
// CurvePoint::CurvePoint Constructor
//----------------------------------------------------
GuiQwtPlotPicker::SelectedCurvePoints::CurvePoint::CurvePoint(QwtPlotCurve* cur, int idx) {
#if QWT_VERSION >= 0x060000
  //  curve = cur;
  pointIdx = idx;
  curve = cur;
  curveLabel = new QwtText(cur->title());
  xpos = curve->data()->sample( pointIdx ).x();
  ypos = curve->data()->sample( pointIdx ).y();
#endif
}

//----------------------------------------------------
// refresh
//----------------------------------------------------
void GuiQwtPlotPicker::refresh() {
 startTimer(0);
}

/* --------------------------------------------------------------------------- */
/* timerEvent --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQwtPlotPicker::timerEvent ( QTimerEvent * event ) {
  showSelectedPoints();
  killTimer( event->timerId() );
}

QwtText GuiQwtPlotPicker::trackerText(const QPoint &pos)  const {

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
