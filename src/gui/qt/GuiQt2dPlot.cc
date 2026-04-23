#if HAVE_QCHARTS
// #include <math.h>
// #include <stdexcept>
// #include <limits>
// #include <algorithm>
// #include <numeric>

// #include <QTime>
// #include <QTemporaryFile>
// #include <QDir>
#include <QApplication>
#include <QClipboard>
#include <QPixmap>
// #include <QtSvg/QSvgGenerator>
// #include <QByteArray>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QStackedWidget>
// #include <QRegularExpression>

// #include <qwt_plot.h>
// #if QWT_VERSION >= 0x060000
// #include <qwt_plot_renderer.h>
// #endif
// #include <qwt_legend.h>
// #if QWT_VERSION < 0x060100
// #include <qwt_legend_item.h>
// #else
// #include <qwt_legend_label.h>
// #endif
// #include <qwt_plot_zoomer.h>
// #include <qwt_plot_canvas.h>
// #include <qwt_plot_grid.h>
// #include <qwt_plot_curve.h>
//  #include <qwt_scale_widget.h>
// #include <qwt_plot_marker.h>
// #include <qwt_plot_layout.h>
// #include <qwt_picker_machine.h>
// #include <qwt_symbol.h>
// #include <qwt_painter.h>
// #include <qwt_scale_map.h>
#include "gui/qt/GuiQt2dPlot.h"
// QT_USE_NAMESPACE

// #include <app/UiManager.h>
// #include <app/AppData.h>
// #include "app/ColorSet.h"
// #include "datapool/DataVector.h"
// #include "datapool/DataStringValue.h"
// #include "datapool/DataIntegerValue.h"
// #include "datapool/DataRealValue.h"
// #include <xfer/XferDataItem.h>

#include "gui/UnitManager.h"
// #include "gui/GuiIndex.h"
// #include "gui/GuiFieldgroup.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/GuiPlotDataItem.h"
// #include "gui/qt/GuiQwtPlotPicker.h"
// #include "gui/qt/GuiQwtPlotZoomer.h"
#include "gui/qt/GuiQtFactory.h"

#include "gui/qt/QtMultiFontString.h"

// #include "plot/qt/QtPlot2dConfigDialog.h"
// #include "plot/qt/QtScaleDialog.h"
#include "plot/qt/QtCyclesDialog.h"
#include "gui/qt/GuiQtPopupMenu.h"
// #include "gui/qt/GuiQtMenuButton.h"
// #include "gui/qt/GuiQtSeparator.h"
// #include "gui/qt/GuiQtPrinterDialog.h"
// #include "gui/qt/GuiQwtScaleDraw.h"
// #include "gui/qt/GuiQwtPlotBarChart.h"
// #include "gui/qt/GuiQwtScaleEngine.h"
// #include "gui/qt/GuiQwtPropertyDialog.h"
// #if HAVE_QPOLAR
// #include "gui/qt/GuiQtPolarPlot.h"
// #endif
// #include "gui/qt/GuiQwtPolarPlot.h"
// #include "gui/qt/GuiQwtPlotLayout.h"

// #include "operator/InputChannelEvent.h"
// #include "job/JobManager.h"

#include "utils/gettext.h"
// #include "utils/StringUtils.h"
// #include "utils/FileUtilities.h"

// const int GuiQt2dPlot::MAJOR_TICKS = 8;
// const int GuiQt2dPlot::MINOR_TICKS = 5;
// bool GuiQt2dPlot::drawXAxisPlotItemTitles = true;
// GuiQt2dPlot::eUserInteractionMode GuiQt2dPlot::s_userInteractionMode = ZOOM;

// #include <QPalette>
// #include <QPainter>
// #include <QPrinter>
// #include <QPrintDialog>
// #include <QFileInfo>

INIT_LOGGER();

// // use QwtAxis with qwt >= 6.2.0
// #if QWT_VERSION >= 0x060200
// #define AXIS_Y_LEFT QwtAxis::YLeft
// #define AXIS_Y_RIGHT QwtAxis::YRight
// #define AXIS_X_BOTTOM QwtAxis::XBottom
// #define AXIS_X_TOP QwtAxis::XTop
// #define AXIS_POSITION QwtAxis::Position
// #else
// #define AXIS_Y_LEFT QwtPlot::yLeft
// #define AXIS_Y_RIGHT QwtPlot::yRight
// #define AXIS_X_BOTTOM QwtPlot::xBottom
// #define AXIS_X_TOP QwtPlot::xTop
// #define AXIS_POSITION QwtPlot::Axis
// #endif

class MyQChartView : public QChartView {
public:
  MyQChartView(GuiQt2dPlot* plot2d, QChart *chart, QWidget *parent=0)
    :QChartView(chart, parent)
    , m_plot2d(plot2d)
  {
    setRubberBand(QChartView::RectangleRubberBand);
   // setRenderHint(QPainter::Antialiasing);
   // setDragMode(QGraphicsView::ScrollHandDrag); // Enable panning
  }

protected:
    void wheelEvent(QWheelEvent *event) override {
        // Zoom-Faktor bestimmen
        qreal factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
        // Zoom-Funktion des zugrundeliegenden QChart aufrufen
        chart()->zoom(factor);
        // Event als verarbeitet markieren
        event->accept();
    }
  void keyPressEvent(QKeyEvent *event)
  {
    switch (event->key()) {
    case Qt::Key_Plus:
      chart()->zoomIn();
      break;
    case Qt::Key_Minus:
      chart()->zoomOut();
      break;
      //![1]
    case Qt::Key_Left:
      chart()->scroll(-10, 0);
      break;
    case Qt::Key_Right:
      chart()->scroll(10, 0);
      break;
    case Qt::Key_Up:
      chart()->scroll(0, 10);
      break;
    case Qt::Key_Down:
      chart()->scroll(0, -10);
      break;
    default:
      QGraphicsView::keyPressEvent(event);
      break;
    }
  }
   void contextMenuEvent ( QContextMenuEvent* event ){
     if (event->modifiers() == Qt::NoModifier) {
       m_plot2d->popupMenu(event);
       event->accept();
       event->ignore();
     } else {
       QChartView::contextMenuEvent(event);
     }
   }
private:
  GuiQt2dPlot* m_plot2d;
};

// //---------------------------------------------------
// // Klasse GuiQwtPlotCurve
// //---------------------------------------------------
// #if QWT_VERSION >= 0x060000
// void GuiQwtPlotCurve::drawCurve( QPainter *painter, int style,
// 		  const QwtScaleMap &xMap, const QwtScaleMap &yMap,
// 		  const QRectF &canvasRect, int from, int to ) const {

//     const GuiQwtPlotCurveData*curveData = dynamic_cast<const GuiQwtPlotCurveData*>(data());

//     if ( curveData && curveData->hasValuesNan() ) {
//       int _from=0, _to=0;
//       for ( int i = from; i <= to; i++ ) {
// #if QWT_VERSION < 0x060100
//         double xi = yMap.transform( d_series->sample( i ).x() );
//         double yi = yMap.transform( d_series->sample( i ).y() );
// #else
//         double xi = yMap.transform( sample( i ).x() );
//         double yi = yMap.transform( sample( i ).y() );
// #endif
// 	// is nan?
// 	if (std::isnan(xi) || std::isnan(yi)) {
// 	  _to = i ? i-1 : 0;
// 	  // draw previous piece
// 	  if (i && _from <= _to)
// 	    QwtPlotCurve::drawCurve(painter, style, xMap, yMap, canvasRect, _from, _to);
// 	  _from = i+1;
// 	}
//       }
//       // draw rest
//       if (_from <= to) {
// 	QwtPlotCurve::drawCurve(painter, style, xMap, yMap, canvasRect, _from, to);
//       }
//     }
//     else {
//       QwtPlotCurve::drawCurve(painter, style, xMap, yMap, canvasRect, from, to);
//     }
//   }
// #endif


// //----------------------------------------------------
// // getQwtCurveStyle
// //----------------------------------------------------
// GuiQwtPlotCurve::CurveStyleExt GuiQt2dPlot::getQwtCurveStyle(eAxisType axis){
//   eStyle style;
//   switch( axis ){
//   case Y1AXIS :
//     style = m_style[0];
//     break;
//   case Y2AXIS :
//     style = m_style[1];
//     break;
//   default :
//     assert(false);
//   }
//   switch( style ){
//   case BAR :
//     return GuiQwtPlotCurve::Bar;
//     break;
//   case STACKING_BAR:
//     return GuiQwtPlotCurve::StackingBar;
//     break;
//   case STEP :
//     return GuiQwtPlotCurve::Steps;
//     break;
//     break;
//   case DOTS :
//     return GuiQwtPlotCurve::Dots;
//     break;
//   case POLAR:
//     return GuiQwtPlotCurve::Polar;
//     break;
//   case PLOT:
//   case AREA:
//   default :
//    return GuiQwtPlotCurve::Lines;
//     break;
//   }
// }

// //---------------------------------------------------
// // Klasse MyQwtPlot
// //---------------------------------------------------
// //----------------------------------------------------
// // Constructor
// //----------------------------------------------------
// GuiQt2dPlot::MyQwtPlot::MyQwtPlot( QWidget *parent, GuiQt2dPlot *plot )
//   : QwtPlot( parent )
//   , m_barChartOrientation(Qt::Vertical)
//   , m_chart( plot ){
//   setPlotLayout(new GuiQwtPlotLayout(*plot));

//   m_rescaler = new QwtPlotRescaler(canvas(), AXIS_X_BOTTOM, QwtPlotRescaler::Fixed);
//   m_rescaler->setAspectRatio(AXIS_X_BOTTOM, 1.0);
//   m_rescaler->setAspectRatio(AXIS_Y_LEFT, 1.0);
//   m_rescaler->setAspectRatio(AXIS_Y_RIGHT, 0.0);
//   m_rescaler->setAspectRatio(AXIS_X_TOP, 0.0);
//   m_rescaler->setRescalePolicy(QwtPlotRescaler::Fixed);
// }

// //----------------------------------------------------
// // print
// //----------------------------------------------------
// void GuiQt2dPlot::MyQwtPlot::print(QPaintDevice& pd, bool bPrinter){
// #if QWT_VERSION < 0x060000
//     QwtPlotPrintFilter filter;
//     int options = QwtPlotPrintFilter::PrintAll;
//     if ( bPrinter ) // printer -> discard background
//       options &= ~QwtPlotPrintFilter::PrintBackground;
//     options |= QwtPlotPrintFilter::PrintFrameWithScales;
//     filter.setOptions(options);
//     QwtPlot::print(pd, filter);
// #else
//     QwtPlotRenderer renderer;
//     if ( bPrinter ) // printer -> discard background
//       renderer.setDiscardFlags(QwtPlotRenderer::DiscardCanvasBackground|
// 			       QwtPlotRenderer::DiscardBackground|QwtPlotRenderer::DiscardCanvasFrame);
//     renderer.setLayoutFlag(QwtPlotRenderer::FrameWithScales);

//     QwtPlotItemList curveList = itemList( QwtPlotItem::Rtti_PlotMultiBarChart );
// #if QWT_VERSION < 0x060103
//     // Speziallfall BarChart
//     // bei versteckter y-Achse sind die y-Werte Fehlerhaft
//     // das ist ein QWT-Bug
//     // Ist ein BarPlot sichtbar, dann müssen beider y-Achsen sichtbar sein
//     bool yAxisLeftEnabled = axisEnabled( QwtPlot::yLeft );
//     bool yAxisRightEnabled = axisEnabled( QwtPlot::yRight );

//     if (curveList.count()) {
//       int index = -1;
//       for ( QwtPlotItemIterator it = curveList.begin(); it != curveList.end(); ++it ) {
// 	GuiQwtPlotBarChart *c = dynamic_cast<GuiQwtPlotBarChart *>( *it );
// 	if (!c || !c->isVisible()) continue;  // invisible
// 	enableAxis(QwtPlot::yLeft,  true);
// 	enableAxis(QwtPlot::yRight,  true);
// 	break;
//       }
//     }
//     renderer.renderTo(this, pd);
//     enableAxis( QwtPlot::yLeft, yAxisLeftEnabled );
//     enableAxis( QwtPlot::yRight, yAxisRightEnabled );
// #else
//     if (curveList.count()) {
//       std::cerr << "QWT <= 6.1.2 hatte bei dieser Funktion ein Problem mit 'breiten' BarPlots und versteckten Achsen."
// 		<< std::endl << "Die Y-Werte der Kurven mit einer versteckten Achse waren fehlerhaft."
// 		<< std::endl << "Ist dieser Bug auch in der neuen QWT-Version vorhanden, bitte überprüfen. "
// 		<< std::endl;
//     }
//     renderer.renderTo(this, pd);
// #endif
// #endif
//   }

// //----------------------------------------------------
// // getBarChartOrientation
// //----------------------------------------------------
// Qt::Orientation GuiQt2dPlot::MyQwtPlot::getBarChartOrientation() { return m_barChartOrientation; }

// //----------------------------------------------------
// // setBarChartLayoutSettings
// //----------------------------------------------------
// void GuiQt2dPlot::MyQwtPlot::setBarChartLayoutSettings(GuiQwtPropertyDialog::Settings::BarChart& settings) {
//   m_plot->setBarChartLayoutSettings( settings );
// }

// //----------------------------------------------------
// // applySettings
// //----------------------------------------------------
// void GuiQt2dPlot::MyQwtPlot::applySettings( const GuiQwtPropertyDialog::Settings &settings ) {
//     // legend position
//     if ( plotLayout()->legendPosition() !=
// 	 QwtPlot::LegendPosition(settings.general.legend_position) )  {
//       plotLayout()->setLegendPosition( QwtPlot::LegendPosition(settings.general.legend_position),
// 					     plotLayout()->legendRatio() );
//     }
//     // major minor ticks
//     if ( axisMaxMajor(AXIS_X_BOTTOM) != settings.general.major_ticks ) {
//       m_plot->setXAxisMajorTicks( settings.general.major_ticks );
//     }
//     if ( axisMaxMinor(AXIS_X_BOTTOM) != settings.general.minor_ticks ) {
//       m_plot->setXAxisMinorTicks( settings.general.minor_ticks );
//     }

//     // annotation angle
//     if ( axisScaleDraw(AXIS_X_BOTTOM)->labelRotation() != settings.general.annotation_angle ) {
//       setAxisLabelRotation(AXIS_X_BOTTOM, settings.general.annotation_angle);
//       m_plot->setAxisLabelRotation(settings.general.annotation_angle);
//     }

//     QwtPlotItemList curveList = itemList( QwtPlotItem::Rtti_PlotMultiBarChart );
//     m_barChartOrientation = settings.barChart.orientation == 2 ? Qt::Vertical : Qt::Horizontal;
//     for ( int i = 0; i < curveList.count(); i++ ) {
//       GuiQwtPlotBarChart* barCurve = dynamic_cast<GuiQwtPlotBarChart*>( curveList[i] );
//       if (barCurve) {

// 	// orientation
// 	AXIS_POSITION axis1, axis2;
// 	if (settings.barChart.orientation == 0) {
// 	  axis1 = (AXIS_POSITION) barCurve->xAxis(); //AXIS_X_BOTTOM;
// 	  axis2 = (AXIS_POSITION) barCurve->yAxis(); //AXIS_Y_LEFT;
// 	  ///	  barCurve->setOrientation( Qt::Vertical );
// 	} else {
// 	  axis1 = (AXIS_POSITION) barCurve->yAxis(); //AXIS_Y_LEFT;
// 	  axis2 = (AXIS_POSITION) barCurve->xAxis(); //AXIS_X_BOTTOM;
// 	  ///	  barCurve->setOrientation( Qt::Horizontal );
// 	}
// 	setAxisAutoScale( axis2 );

// 	// setting in barCurve
// 	barCurve->setBarChartSettings( settings.barChart );

// 	// setting in m_plot
// 	m_plot->setBarChartLayoutSettings( settings.barChart );

//       }
//       if (curveList.count()) {
// 	replot();
//       }
//     }
//     m_plot->update(GuiElement::reason_Always);
//   }

// //----------------------------------------------------
// // getPlotRescaler
// //----------------------------------------------------
// QwtPlotRescaler* GuiQt2dPlot::MyQwtPlot::getPlotRescaler() {
//   return m_rescaler;
// }

// //----------------------------------------------------
// // event
// //----------------------------------------------------
// bool GuiQt2dPlot::MyQwtPlot::event(QEvent * event) {
//   if (event->type() == QEvent::CursorChange) {
//     startTimer(0);  // own set cursor logic in timer event
//   }
//   return QwtPlot::event(event);
// }

// //----------------------------------------------------
// // enterEvent
// //----------------------------------------------------
// void GuiQt2dPlot::MyQwtPlot::enterEvent(QEnterEvent * event) {
//   // only if default Cursor is set
//   if ( parentWidget() == 0 || // plot clone created with 'fullscreen' menu option has no parentWidget
//        parentWidget()->cursor().shape() == Qt::ArrowCursor)
//     m_plot->setCursor();
// }
// //----------------------------------------------------
// // print
// //----------------------------------------------------
// void GuiQt2dPlot::MyQwtPlot::leaveEvent(QEvent * event) {
//   unsetCursor();
// }

// //----------------------------------------------------
// // contextMenuEvent
// //----------------------------------------------------
//   void GuiQt2dPlot::MyQwtPlot::contextMenuEvent ( QContextMenuEvent* event ){
//     m_plot->popupMenu(event);
//   }

// //----------------------------------------------------
// // keyPressEvent
// //----------------------------------------------------
// void GuiQt2dPlot::MyQwtPlot::keyPressEvent ( QKeyEvent *event ) {
//   //
//   switch (event->key()) {
//   case Qt::Key_C:
//     if (event->modifiers() == Qt::ControlModifier) {
//       m_plot->copy();
//       break;
//     }
//   case Qt::Key_S: // Scale (Z used for Undo)
//     if (event->modifiers() &  Qt::ControlModifier) {
//       m_plot->setUserInteractionMode(GuiQt2dPlot::ZOOM);
//       break;
//     }
//   case Qt::Key_D: // Dot
//     if (event->modifiers() == Qt::ControlModifier) {
//       m_plot->setUserInteractionMode(GuiQt2dPlot::SELECT_POINT);
//       break;
//     }
//   case Qt::Key_A: // Area
//     if (event->modifiers() == Qt::ControlModifier) {
//       m_plot->setUserInteractionMode(GuiQt2dPlot::SELECT_RECTANGLE);
//       break;
//     }
//   }
//   QwtPlot::keyPressEvent(event);
// }

// //---------------------------------------------------
// // Klasse GuiQwtPlotCurve
// //---------------------------------------------------

// //----------------------------------------------------
// // Constructor / Destructor
// //----------------------------------------------------

GuiQt2dPlot::GuiQt2dPlot( const std::string &name )
   : GuiQtElement( 0, name )
   , Gui2dPlot( name )
   , m_chart( 0 )
   , m_chartView( 0 )
// #if HAVE_QPOLAR
//   , m_polarPlot(0)
//   , m_polarChart(0)
//   , m_angularAxis(0)
//   , m_radialAxis(0)
// #endif
   , m_widgetStack( 0 )
//   , m_picker( 0 )
//   , m_userInteractionMenu( this )
//   , m_resetListener( this )
//   , m_printListener( this )
//   , m_openScaleListener( this )
//   , m_openCycleListener( this )
//   , m_openConfigListener( this )
//   , m_annotationListener( this )
//   , m_propertyListener( this )
   , m_currentFileFormat( HardCopyListener::Postscript )
   , m_lastSelectedCurve( 0 )
//   , m_configButton( 0 )
//   , m_cyclesButton( 0 )
   , m_showAnnotationLabels( false )
   , m_withAnnotationOption( false )
//   , m_majorTicks( MAJOR_TICKS )
//   , m_minorTicks( MINOR_TICKS )
//   , m_annoAngle( 0 )
//   , m_lastClosestIdx(-1)
//   , m_selectionRectMode(false)
//   , m_userInteractionMode(ZOOM)
   , m_curveAttrs(this)
   , m_lineWidth(1)
   , m_xaxis(0)
   , m_y1axis(0)
   , m_y2axis(0)
{
  clearBoundingBox();
//   m_zoomer[0] = 0;
//   m_zoomer[1] = 0;

  // default plot color settings
  if( Plot2dCurveAttributes::s_default_colors.empty() ) {
    const QStringList& colorList = GuiQtManager::Instance().defaultColorPalette();
    if (colorList.size() > 1) {
      std::vector<std::string> vec;
      toStdVector(colorList, vec);
      Plot2dCurveAttributes::s_default_colors = vec;
    }
    if (Plot2dCurveAttributes::s_default_colors.empty()) {
      Plot2dCurveAttributes::s_default_colors.push_back("blue");
      Plot2dCurveAttributes::s_default_colors.push_back("red");
      Plot2dCurveAttributes::s_default_colors.push_back("green");
      Plot2dCurveAttributes::s_default_colors.push_back("orange");
      Plot2dCurveAttributes::s_default_colors.push_back("cyan");
      Plot2dCurveAttributes::s_default_colors.push_back("magenta");
      Plot2dCurveAttributes::s_default_colors.push_back("black");
      Plot2dCurveAttributes::s_default_colors.push_back("white");
      Plot2dCurveAttributes::s_default_colors.push_back("gray");
      Plot2dCurveAttributes::s_default_colors.push_back("darkblue");
      Plot2dCurveAttributes::s_default_colors.push_back("darkred");
      Plot2dCurveAttributes::s_default_colors.push_back("darkgreen");
      Plot2dCurveAttributes::s_default_colors.push_back("darkyellow");
      Plot2dCurveAttributes::s_default_colors.push_back("darkcyan");
      Plot2dCurveAttributes::s_default_colors.push_back("darkmagenta");
      Plot2dCurveAttributes::s_default_colors.push_back("lightgreen");
      Plot2dCurveAttributes::s_default_colors.push_back("darkgray");
    }
  }
//   readSettings();
//   m_background="lightgray";
  const int maxNumber(21);

  // default plot symbol settings
  if( Plot2dCurveAttributes::s_default_curveSymbol.empty() ) {
    int i = 0;
    while (++i < maxNumber) {
      Plot2dCurveAttributes::s_default_curveSymbol.push_back(-1);
    }
  }

  // default plot symbol size settings
  if( Plot2dCurveAttributes::s_default_curveSymbolSize.empty() ) {
    int i = 0;
    while (++i < maxNumber) {
      Plot2dCurveAttributes::s_default_curveSymbolSize.push_back(6);
    }
  }

  // default plot symbol settings
  if( Plot2dCurveAttributes::s_default_penStyles.empty() ) {
    int i = 0;
    while (++i < maxNumber) {
      Plot2dCurveAttributes::s_default_penStyles.push_back(Qt::SolidLine);
    }
  }

  // default plot pen width settings
  if( Plot2dCurveAttributes::s_default_lineWidths.empty() ) {
    int i = 0;
    while (++i < maxNumber) {
      Plot2dCurveAttributes::s_default_lineWidths.push_back(m_lineWidth);
    }
  }

  // default plot symbol pen color settings
  if( Plot2dCurveAttributes::s_default_pen_colors.empty() ) {
    int i = 0;
    while (++i < maxNumber) {
      Plot2dCurveAttributes::s_default_pen_colors.push_back("black");
    }
  }


//   m_supportedFileFormats[HardCopyListener::Postscript] = HardCopyListener::OWN_CONTROL;
//   m_supportedFileFormats[HardCopyListener::PDF]        = HardCopyListener::OWN_CONTROL;
// //  m_supportedFileFormats[HardCopyListener::Postscript] = HardCopyListener::FILE_EXPORT;
// //  m_supportedFileFormats[HardCopyListener::PDF]        = HardCopyListener::FILE_EXPORT;
//   m_supportedFileFormats[HardCopyListener::PNG]        = HardCopyListener::FILE_EXPORT;
//   m_supportedFileFormats[HardCopyListener::JPEG]       = HardCopyListener::FILE_EXPORT;
//   m_supportedFileFormats[HardCopyListener::SVG]        = HardCopyListener::FILE_EXPORT;

//   m_supportedFileFormats[HardCopyListener::BMP]        = HardCopyListener::FILE_EXPORT;
//   m_supportedFileFormats[HardCopyListener::PPM]        = HardCopyListener::FILE_EXPORT;
//   m_supportedFileFormats[HardCopyListener::TIFF]       = HardCopyListener::FILE_EXPORT;
//   m_supportedFileFormats[HardCopyListener::XBM]        = HardCopyListener::FILE_EXPORT;
//   m_supportedFileFormats[HardCopyListener::XPM]        = HardCopyListener::FILE_EXPORT;

//   if ( name.size() ) { // mit popupMenu
//     UImanager::Instance().addHardCopy( name, this );

//     GuiEventData *event = new GuiEventData();
//     std::vector<std::string> labels;
//     labels.push_back( _("Item name") );
//     labels.push_back( _("Y-Axis 1") );
//     labels.push_back( _("Y-Axis 2") );
//     labels.push_back( _("X-Axis") );
//     labels.push_back( _("Line") );
//     labels.push_back( _("Symbol") );
//     labels.push_back( _("LineStyle") );
//     labels.push_back( _("SymbolStyle") );
//     labels.push_back( _("SymbolSize") );
//     labels.push_back( _("Unit") );

//     setConfigDialog( new QtPlot2dConfigDialog( this, this, name, "Config Dialog", labels, false, event ) );
//   }
// }

// void GuiQt2dPlot::printInfo(char* msg ) {
//   std::cout << " ++++ QWTPlot Named " << getName() << " msg["<<msg<<"] +++++++ \n";
//   std::cout << " + Styles: " << m_style[0] << " " << m_style[1] << std::endl;
//   tPlotIterator plotIter;
//   std::cout << " + Plots: " << m_plots.size()<< std::endl;
//   for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
//     std::cout << " + Plot: " << (*plotIter) << std::endl;

//     // x plotdataitems
//     std::set<GuiPlotDataItem *>::iterator itemIter;
//     std::set<GuiPlotDataItem *> items;
//     (*plotIter)->getXPlotDataItems( items );
//     for( itemIter = items.begin(); itemIter != items.end(); ++itemIter ){
//       if( (*itemIter) != 0 ){
//         if ((*itemIter)->XferData())
//           std::cout << " + xPlotDataItems: This: " << (*itemIter) << " vn: " << (*itemIter)->XferData()->getFullName(true) << std::endl;
//       }
//     }

//     // y plotitems
//     eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
//     for( int axis = 0; axis < 2; ++axis ) {
//       tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
//       std::cout << " +  YY PlotItems: " << (*plotIter)->plotItems( axisType[axis] ).size()<< std::endl;
//        while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
//          std::cout << " ++  getAxisType: " << (*it)->getAxisType() << "  xPlotDataItem: " << (*it)->xPlotDataItem() << " xvn: " << (*it)->xPlotDataItem()->XferData()->getFullName(true) << " yvn: " << (*it)->plotDataItem()->XferData()->getFullName(true)<< "  xPlotItem: " << (*it)->xPlotItem() <<  "  THIS: " << (*it) << std::endl;
//          ++it;
//        }
//     }
//   }
}


GuiQt2dPlot::GuiQt2dPlot( const GuiQt2dPlot &plot )
  : GuiQtElement( plot )
  , Gui2dPlot( plot )
  , m_chart( 0 )
  , m_chartView( 0 )
// #if HAVE_QPOLAR
//   , m_polarPlot(0)
//   , m_polarChart(0)
//   , m_angularAxis(0)
//   , m_radialAxis(0)
// #endif
   , m_widgetStack( 0 )
//   , m_picker( 0 )
//   , m_userInteractionMenu( this )
//   , m_resetListener( this )
//   , m_printListener( this )
//   , m_openScaleListener( this )
//   , m_openCycleListener( this )
//   , m_openConfigListener( this )
//   , m_annotationListener( this )
//   , m_propertyListener( this )
   , m_currentFileFormat( plot.m_currentFileFormat )
//   , m_supportedFileFormats( plot.m_supportedFileFormats )
   , m_lastSelectedCurve( 0 )
//   , m_configButton( 0 )
//   , m_cyclesButton( 0 )
   , m_showAnnotationLabels( plot.m_showAnnotationLabels )
   , m_withAnnotationOption( plot.m_withAnnotationOption )
//   , m_majorTicks( plot.m_majorTicks )
//   , m_minorTicks( plot.m_minorTicks )
//   , m_annoAngle( plot.m_annoAngle )
//   , m_lastClosestIdx( plot.m_lastClosestIdx )
//   , m_bcSettings( plot.m_bcSettings )
//   , m_selectionRectMode( plot.m_selectionRectMode )
//   , m_userInteractionMode(plot.m_userInteractionMode)
   , m_curveAttrs(this)
   , m_xaxis(0)
   , m_y1axis(0)
   , m_y2axis(0)
{
  std::map<QString, bool>::const_iterator lit = plot.m_legendVis.begin();
  for(; lit != plot.m_legendVis.end(); ++lit)
    m_legendVis.insert(m_legendVis.begin(), std::pair<QString, bool>((*lit).first,
								   (*lit).second));
//   m_zoomer[0] = 0;
//   m_zoomer[1] = 0;

//   readSettings();
//   m_background="lightgray";

//   // UImanager::Instance().addHardCopy( name, this );
//   GuiEventData *event = new GuiEventData();
//   setConfigDialog( new QtPlot2dConfigDialog( this, this, getName(),
// 											 "Config Dialog", plot.getConfigDialog()->getColumnLabels(), false, event ) );
}

GuiQt2dPlot::~GuiQt2dPlot(){
//   delete m_zoomer[0];
//   delete m_zoomer[1];
   delete m_chart;
   delete m_chartView;
// #if HAVE_QPOLAR
//   delete m_polarPlot;
//   delete m_polarChart;
//   delete m_angularAxis;
//   delete m_radialAxis;
// #endif
   delete m_widgetStack;
}

//----------------------------------------------------
// create
//----------------------------------------------------
void GuiQt2dPlot::create(){
   BUG(BugGui, "GuiQt2dPlot::create()");
    m_chart = new QChart;

    // legend
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    QFont font = m_chart->legend()->font();
    m_chart->legend()->setFont( QtMultiFontString::getQFont( "@plotLegend@", font ) );

//   // create plot
//   QWidget *parent = getParent() ? getParent()->getQtElement()->myWidget() : 0;
//   m_chart = new MyQwtPlot( parent, this );

//   // rescaler attributes
//   m_chart->getPlotRescaler()->setReferenceAxis(AspectRatioReferenceAxis() == Y1AXIS ? AXIS_Y_LEFT :
//                                               AspectRatioReferenceAxis() == Y2AXIS ? AXIS_Y_RIGHT :
//                                               AXIS_X_BOTTOM);

//   readSettings();

//   // grid
//   QwtPlotGrid *_grid = new QwtPlotGrid;
//   _grid->setPen( QPen((Qt::PenStyle) Qt::DotLine) );
//   _grid->enableX(true);
//   _grid->enableY(true);
//   _grid->attach( m_chart );
//   _grid->setZ( 0 );

//   m_chart->enableAxis(AXIS_Y_RIGHT, false);
//   m_chart->enableAxis(AXIS_Y_LEFT,  false);
//   m_chart->enableAxis(AXIS_X_BOTTOM, isAxisShow(XAXIS));
//   m_chart->enableAxis(AXIS_X_TOP, false);


    // Axes
    m_xaxis = createAxis(0, getAxis(2));
    m_y1axis = createAxis(0, getAxis(0));
    m_y2axis = createAxis(0, getAxis(1));

    ///m_xaxis->setTickCount(10);
    m_chart->addAxis(m_xaxis, Qt::AlignBottom);
    m_chart->addAxis(m_y1axis, Qt::AlignLeft);
    m_chart->addAxis(m_y2axis, Qt::AlignRight);
//   for (int i= 0; i < 2; ++i) {
//     // nun auch immer, weil wir eine evtl. YAxis Annotation unterstuetzen wollen
//     m_chart->setAxisScaleDraw( i==0 ? AXIS_Y_LEFT : AXIS_Y_RIGHT,
// 				new GuiQwtScaleDraw( GuiQwtScaleDraw::type_real,
// 						     getAxis(i).getFmt(), getAxis(i).getFieldWidth(), getAxis(i).getPrec() ) );
//   }

//   // nun immer, weil wir Achsenbeschriftung mit Datum unterstuetzen wollen
//   // xBottom Axis
//   m_chart->setAxisScaleDraw( AXIS_X_BOTTOM,
// 			    new GuiQwtScaleDraw( GuiQwtScaleDraw::type_real,
// 						 getAxis(2).getFmt(), getAxis(2).getFieldWidth(), getAxis(2).getPrec() ) );
//   m_chart->setAxisLabelRotation(AXIS_X_BOTTOM, m_annoAngle);

    // xTop Axis
//   if (getAxis(3).getFieldWidth())
//     m_chart->setAxisScaleDraw( AXIS_X_TOP,
//     			      new GuiQwtScaleDraw( GuiQwtScaleDraw::type_real,
//     						   getAxis(3).getFmt(), getAxis(3).getFieldWidth(), getAxis(3).getPrec() ) );


//   QwtText pa;
//   QFont font = m_chart->titleLabel()->font();
//   m_chart->titleLabel()->setFont( QtMultiFontString::getQFont( "@plotTitle@", font ) );

//   font = m_chart->axisFont( AXIS_Y_LEFT );
//   font =  QtMultiFontString::getQFont( "@plotAxis@", font );
//   pa.setFont(QtMultiFontString::getQFont( "@plotAxisTitle@", font ));
//   for (int atype=0; atype< QwtPlot::axisCnt ; ++atype) {
//     m_chart->setAxisFont( atype, font );
//     m_chart->setAxisTitle( atype, pa );
//   }

//   if (m_background.size()) {
//     QPalette pal = m_chart->canvas()->palette();
//     pal.setColor( QPalette::Window, m_background );
//     pal.setColor( QPalette::Base, m_background );
//     m_chart->canvas()->setPalette(pal);
//     m_chart->canvas()->setObjectName( QString::fromStdString(getName()) );
//   }
//   const QColor &color = GuiQtManager::foregroundColor();
//   m_chart->axisTitle( AXIS_X_BOTTOM ).setColor(color);

// #if QWT_VERSION < 0x060000
//   m_zoomer[0]= new  GuiQwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,
// 				    QwtPicker::DragSelection, QwtPicker::AlwaysOn, m_chart->canvas() );
//   m_zoomer[1]= new  GuiQwtPlotZoomer(QwtPlot::xTop, QwtPlot::yRight,
// 				    QwtPicker::PointSelection, QwtPicker::AlwaysOff, m_chart->canvas() );
// #else
//   m_zoomer[0]= new  GuiQwtPlotZoomer(AXIS_X_BOTTOM, AXIS_Y_LEFT,
// 				    QwtPicker::RectRubberBand, QwtPicker::AlwaysOn, m_chart->canvas() );
//   m_zoomer[1]= new  GuiQwtPlotZoomer(AXIS_X_TOP, AXIS_Y_RIGHT,
// 				    QwtPicker::CrossRubberBand, QwtPicker::AlwaysOff, m_chart->canvas() );
// #endif

//   m_zoomer[0]->setRubberBandPen( QColor(Qt::blue) );
//   m_zoomer[0]->setTrackerPen(QColor(Qt::darkBlue));
//   m_zoomer[0]->setTrackerFont(QtMultiFontString::getQFont( "@plotMarker@", font ));
//   m_zoomer[1]->setTrackerFont(QtMultiFontString::getQFont( "@plotMarker@", font ));
//   m_zoomer[1]->setTrackerPen(QColor(Qt::darkBlue));

//   createPicker(false);
//   setUserInteractionMode(GuiQt2dPlot::ZOOM);
//   connect(m_picker, SIGNAL(selected(const QRectF&)),
// 	this, SLOT(slot_selected(const QRectF&)));
//   connect(m_picker, SIGNAL(selectedPoint(const QwtPlotCurve*, int, const QPointF&, const QPointF&)),
//           this, SLOT(slot_selected(const QwtPlotCurve*, int, const QPointF&, const QPointF&)));
//   connect(m_zoomer[0], SIGNAL(selectedCurve(QwtPlotCurve&)),
// 	this, SLOT(slot_selectedCurve(QwtPlotCurve&)));
//   // connect(m_zoomer[1], SIGNAL(selectedCurve(QwtPlotCurve&)),
//   // 	  this, SLOT(slot_selectedCurve(QwtPlotCurve&)));

    // Popup Menue installieren
    createPopupMenu();

//   // init axis scales
//   setScale();

//   // set axis types
//   setLogX( getAxis(2).isLogarithmic() );
//   setLogY( getAxis(0).isLogarithmic() );

    setYPlotStyle(m_style[0], Y1AXIS );
    setYPlotStyle(m_style[1], Y2AXIS );

//   setdrefAxis();

    // set size
    int width(getInitialWidth()), height(getInitialHeight());
    setDebugTooltip();
//   m_chart->updateLayout();

   // widget stack
   m_widgetStack = new QStackedWidget();
   m_chartView = new MyQChartView(this, m_chart);
   m_widgetStack->addWidget(m_chartView);
   if (hasInitialSize() ) {  // SIZE is set!
     QSize hs = m_widgetStack->sizeHint();
     QSize hsNew = hs.expandedTo(QSize(width, height));
     width = hsNew.width();
     height = hsNew.height();
   }
   m_widgetStack->setMinimumSize(QSize(width, height));
   m_widgetStack->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                            QSizePolicy::MinimumExpanding));

//   // polarplot
//   if (getQwtCurveStyle(Gui2dPlot::Y1AXIS)  == GuiQwtPlotCurve::Polar ||
//       getQwtCurveStyle(Gui2dPlot::Y2AXIS)  == GuiQwtPlotCurve::Polar) {
//     QWidget *parent = getParent() ? getParent()->getQtElement()->myWidget() : 0;
//     createPolarPlot();
//   }

}

// //----------------------------------------------------
// // legendChecked
// //----------------------------------------------------

// void  GuiQt2dPlot::legendChecked (const QVariant &itemInfo, bool on, int index) {
// #if QWT_VERSION >= 0x060100
//   QwtPlotItem* item = m_chart->infoToItem(itemInfo);
//   showCurve(item, on);
//   m_picker->refresh();  // refresh selection points
// #endif
// }

// //----------------------------------------------------
// // showCurve
// //----------------------------------------------------

// bool GuiQt2dPlot::showCurve(QwtPlotItem *item, bool on, bool refresh)
// {
//   bool ret(false);
//   if (item->isVisible() != on) {
//     item->setVisible(on);
//     ret = true;
//   }
//   if (on) item->show(); else item->hide();
//   QString title = item->title().text();
//   m_legendVis[ title ] = on;
// #if QWT_VERSION < 0x060100
//   QWidget *w = m_chart->legend()->find(item);
// #else
//   QWidget *w = 0;
//   QwtLegend* legend = dynamic_cast<QwtLegend*>(m_chart->legend());
//   if (legend) {
//     w = legend->legendWidget( m_chart->itemToInfo(item) );
//   }
// #endif
//   QString tp;
//   switch (item->yAxis()) {
//     case AXIS_Y_LEFT:
//       tp = "Y-Axis 1";
//       break;
//     case AXIS_Y_RIGHT:
//       tp = "Y-Axis 2";
//       break;
//   }

//   if (w) {
// #if QT_VERSION > 0x050200
//     w->setToolTipDuration(AppData::Instance().ToolTipDuration());
// #endif
//     w->setToolTip(tp);
//   }
//   if ( m_chart->legend() ) {
//     QFont font = m_chart->legend()->font();
//     if ( w && w->inherits("QwtLegendLabel") ) {
//       ((QwtLegendLabel *)w)->setChecked(on);
//     }
//   }

//   if (refresh) {
//     m_chart->replot();
//   }
//   return ret;
// }

// //----------------------------------------------------
// // updatePlot
// //----------------------------------------------------

// void GuiQt2dPlot::updatePlot() {
//   m_chart->applySettings( m_propertyListener.Settings() );
// }

// //----------------------------------------------------
// // slot_selected
// //----------------------------------------------------

// void GuiQt2dPlot::slot_selected(const QRectF& r) {
//   if ( getFunction() == 0 || !r.isValid() ) {
//     return;
//   }
//   JobStarter *starter = new Trigger( getFunction(), this );
//   starter->setReason( JobElement::cll_SelectRectangle );

//   // set rectangle values
//   DataReference *ref = DataPoolIntens::Instance().getDataReference( "Global_Rect.X1" );
//   if ( ref != 0 ) { ref->SetValue( r.left() ); }
//   ref = DataPoolIntens::Instance().getDataReference( "Global_Rect.X2" );
//   if ( ref != 0 ) { ref->SetValue( r.right() ); }
//   ref = DataPoolIntens::Instance().getDataReference( "Global_Rect.Y1" );
//   if ( ref != 0 ) { ref->SetValue( r.top() ); }
//   ref = DataPoolIntens::Instance().getDataReference( "Global_Rect.Y2" );
//   if ( ref != 0 ) { ref->SetValue( r.bottom() ); }

//   // yRight
//   if (true) {
//     double t = m_chart->invTransform(AXIS_Y_RIGHT, m_chart->transform(AXIS_Y_LEFT, r.top()));
//     double b = m_chart->invTransform(AXIS_Y_RIGHT, m_chart->transform(AXIS_Y_LEFT, r.bottom()));

//     ref = DataPoolIntens::Instance().getDataReference( "Global_Rect[1].X1" );
//     if ( ref != 0 ) { ref->SetValue(r.left()); }
//     ref = DataPoolIntens::Instance().getDataReference( "Global_Rect[1].X2" );
//     if ( ref != 0 ) { ref->SetValue(r.right()); }
//     ref = DataPoolIntens::Instance().getDataReference( "Global_Rect[1].Y1" );
//     if ( ref != 0 ) { ref->SetValue(t); }
//     ref = DataPoolIntens::Instance().getDataReference( "Global_Rect[1].Y2" );
//     if ( ref != 0 ) { ref->SetValue(b); }
//   } else {
//     ref = DataPoolIntens::Instance().getDataReference( "Global_Rect[1]" );
//     if ( ref != 0 ) { ref->clearAllElements(); }
//   }


//   starter->startJob();
// }

// //----------------------------------------------------
// // slot_selected
// //----------------------------------------------------

// void GuiQt2dPlot::slot_selected(const QwtPlotCurve* curve, int index,
//                                const QPointF& pt_l, const QPointF& pt_r) {
//   if ( getFunction() == 0) {
//     return;
//   }
//   QPointF pt;
//   if (curve) {
//     if (index >= 0 && index >= curve->dataSize()) return;
//     pt = curve->sample( index );
//     if (pt.isNull()) {
//       return;
//     }
//   }

//   JobStarter *starter = new Trigger( getFunction(), this );
//   starter->setReason( JobElement::cll_SelectPoint );
//   if (index >= 0)
//     starter->setIndex( index );

//   // set curve point
//   DataReference *ref = DataPoolIntens::Instance().getDataReference( "Global_Point.X" );
//   if ( ref != 0 ) {
//     if (!curve) ref->clearAllElements();
//     else
//       ref->SetValue( pt.x() );
//   }
//   ref = DataPoolIntens::Instance().getDataReference( "Global_Point.Y" );
//   if ( ref != 0 ) {
//     if (!curve) ref->clearAllElements();
//     else
//       ref->SetValue( pt.y() );
//   }

//   // set rectangle values
//   ref = DataPoolIntens::Instance().getDataReference( "Global_Point[1].X" );
//   if ( ref != 0 ) { ref->SetValue(pt_l.x()); }
//   ref = DataPoolIntens::Instance().getDataReference( "Global_Point[1].Y" );
//   if ( ref != 0 ) { ref->SetValue(pt_l.y()); }
//   ref = DataPoolIntens::Instance().getDataReference( "Global_Point[1].Y2" );
//   if ( ref != 0) {
//     if (pt_r.isNull())
//       ref->clearAllElements();
//     else
//       ref->SetValue(pt_r.y());
//   }

//   starter->startJob();
// }

// //----------------------------------------------------
// // getCurveInfo
// //----------------------------------------------------
// std::string GuiQt2dPlot::getCurveInfo(const QwtPlotCurve& curve) {
//   tPointVector pts;
//   const GuiQwtPlotCurveData*curveData = dynamic_cast<const GuiQwtPlotCurveData*>(curve.data());
//   pts.reserve(curveData->size());
//   if (!curveData) return std::string();
//   for ( int i = 0; i < curveData->size(); i++ ) {
//     pts.push_back( Point(curve.sample( i ).x(), curve.sample( i ).y()) );
//   }

//   return Gui2dPlot::getCurveInfo(pts);
// }

// /* --------------------------------------------------------------------------- */
// /* configureCycleXfer --                                                       */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::configureCycleXfer(XferDataItem* xfer, int pos) {
//   if (xfer->getIndex(1) == 0) {
// 	xfer->newDataItemIndex();
// 	xfer->setDimensionIndizes();
//   }

// }

// /* --------------------------------------------------------------------------- */
// /* createPolarPlot --                                                          */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::createPolarPlot() {
// #if HAVE_QPOLAR
//   if (m_polarPlot) return;
//   m_polarChart= new GuiQtPolarChart();
//   m_polarPlot = new GuiQtChartView(m_polarChart);
//   m_polarChart->create(dynamic_cast<GuiQtChartView*>(m_polarPlot));
//   m_widgetStack->addWidget(m_polarPlot);
// #endif
// }
// //----------------------------------------------------
// // slot_selectedCurve
// //----------------------------------------------------
// void GuiQt2dPlot::slot_selectedCurve(QwtPlotCurve& curve) {
//   QPen pen = curve.pen();
//   const int selPenWidth = 2*pen.width();

//   if (m_lastSelectedCurve == &curve) {
//     return;
//   }

//   // mark new selected Curve
//   pen.setWidth(2*pen.width());
//   curve.setPen( pen );

//   // reset mark old selected Curve
//   if (m_lastSelectedCurve) {
//     QPen pen2 = m_lastSelectedCurve->pen();
//     pen2.setWidth(0.5*pen2.width());
//     m_lastSelectedCurve->setPen( pen2 );
//   }
//   m_lastSelectedCurve = &curve;

//   // set label
//   printMessage(this, getCurveInfo(curve), msg_Warning, 5);
//   m_chart->replot();
// }

// //----------------------------------------------------
// // zoomed
// //----------------------------------------------------
// void GuiQt2dPlot::zoomed(){
//   if(m_zoomer[0]->zoomRectIndex() == 0){
//     setScale();
//   }
// }

//----------------------------------------------------
// setAutoScale
//----------------------------------------------------
void GuiQt2dPlot::setAutoScale(){
  double min, max;
  if (!getAxis(0).isScaleEnabled()) {
    getBoundingBox(Y1AXIS, min, max);
    ///    m_y1axis->setRange(0,0); // trick, autoscale
    m_y1axis->setRange(min, max);
    BUG_DEBUG("range y1axis: " << min << ", " << max);
    m_y1axis->setLabelsEditable(true);
  }
  if (!getAxis(1).isScaleEnabled())  {
    getBoundingBox(Y2AXIS, min, max);
    m_y2axis->setRange(min, max);
    BUG_DEBUG("range y2xis: " << min << ", " << max);
    m_y2axis->setLabelsEditable(true);
  }
  if (!getAxis(2).isScaleEnabled()) {
    getBoundingBox(XAXIS, min, max);
    m_xaxis->setRange(min, max);
    BUG_DEBUG("range xaxis: " << min << ", " << max);
    m_xaxis->setLabelsEditable(true);
  }
}

// //----------------------------------------------------
// // createPicker
// //----------------------------------------------------
// void GuiQt2dPlot::createPicker(bool state){

//   if (!m_picker) {
// #if QWT_VERSION < 0x060000
//     m_picker = new GuiQwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
// 				    QwtPicker::PointSelection,
// 				    QwtPlotPicker::CrossRubberBand, QwtPicker::ActiveOnly, //Off,
// 				    m_plot, this);
// #else
//     m_picker = new GuiQwtPlotPicker(AXIS_X_BOTTOM, AXIS_Y_LEFT,
// 				    QwtPicker::CrossRubberBand,
// 				    QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, //Off,
// 				    m_plot, this);
// #endif
//     m_picker->setRubberBandPen( QColor(Qt::red) );
//     m_picker->setRubberBand(QwtPicker::RectRubberBand);
//     m_picker->setTrackerPen(QColor(Qt::darkRed));
//   }

//   m_picker->setStateMachine( m_selectionRectMode ?
// 			     static_cast<QwtPickerMachine*>(new QwtPickerDragRectMachine()) :
// 			     static_cast<QwtPickerMachine*>(new QwtPickerDragPointMachine()) );
//   m_picker->setEnabled(state);
//   setCursor();
// }

// /* --------------------------------------------------------------------------- */
// /* setDrefAxis --                                                              */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::setdrefAxis(){
//   tPlotItemIterator it;
//   if (getConfigDialog() &&
//       getConfigDialog()->getStyle() == ConfigDialog::LIST) {
// 	// reset all
// 	for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it )
//       (*it)->initAxisTypeValues( true, 0 );
//   }
//   for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it ){
//     if( (*it)->plotDataItem() != 0 ){
//       if (getConfigDialog() &&
//           getConfigDialog()->getStyle() == ConfigDialog::LIST) {
//         (*it)->initAxisTypeValues( true, (*it)->getAxisType() == Gui2dPlot::XAXIS );
//       } else {
//         PlotItem * xPlotItem = (*it)->xPlotItem();
//         if (!xPlotItem) continue;
//         xPlotItem->initAxisTypeValues( true, 1 );
//         int posOfX = std::find( m_plotItems.begin(), m_plotItems.end(), xPlotItem ) - m_plotItems.begin();
//         (*it)->initAxisTypeValues( true, posOfX );
//       }
//     }
//   }
// }

//----------------------------------------------------
// update
//----------------------------------------------------
void GuiQt2dPlot::update( GuiElement::UpdateReason reason ){
//  if (!m_plot || // plot (noch) nicht kreiert
//      (!m_chart->isVisible() &&
// #if HAVE_QPOLAR
//       (!m_polarPlot || !m_polarPlot->isVisible()) &&
// #endif
//        reason != reason_Always && // reason_Always: update durchführen, es könnte z.B. ein SAVE folgen!
//        reason != reason_Cycle &&
//        reason != reason_Unit &&
//        reason != reason_Cancel
//       )
//      ) return; // interessiert uns (noch) nicht

// // QTime st=QTime::currentTime();
// // std::cout << ">>>>+++ Update reason["<<reason<< "]\n"<<std::flush;
   TransactionNumber trans = GuiQtManager::Instance().LastGuiUpdate();
//   if( getConfigDialog() && m_configButton != 0 ){
// 	// update popupMenu
// 	QtPlot2dConfigDialog *configDialog = dynamic_cast<QtPlot2dConfigDialog*>(getConfigDialog());
// 	if (configDialog->isShown()) {
// 	  getConfigDialog()->getForm()->getElement()->update( reason );
// 	  configDialogGetCurvePalette();
// 	}
//   }

  bool always( false );
  switch( reason ) {
  case reason_FieldInput:
  case reason_Process:
    //    BUG_MSG( "reason_FieldInput || reason_Process" );
    if( !isDataItemUpdated( trans ) && !isAxisUpdated( trans )){
      if( isAnnotationLabelsUpdated( trans ) ) {
        setAnnotationLabels();
        // m_chart->updateAxes();
        // m_chart->replot();
      }
      setAxisTitles();
      return;
    }
    //    always = true;
    break;
  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:{
    ResetLastWebUpdated();  // reason_Always for webtens
    always = true;
    break;
  }
  case reason_Unit:
    if(isScaleUnitUpdated()){
      always = true;
      break;
    }
    return;
  default:
    drawHeaderText();
    return;
    break;
  }

// #if HAVE_QPOLAR
//   // polarplot
//   if (getQwtCurveStyle(Gui2dPlot::Y1AXIS)  == GuiQwtPlotCurve::Polar ||
//       getQwtCurveStyle(Gui2dPlot::Y2AXIS)  == GuiQwtPlotCurve::Polar) {
//     if (!m_polarPlot) {
//       createPolarPlot();
//     }
//     m_widgetStack->setCurrentWidget(m_polarPlot);
//   } else
// #endif
//   m_widgetStack->setCurrentWidget(m_plot);

//   // set user interaction mode
//   eUserInteractionMode uiMode(m_userInteractionMode);
//   std::string dbMode(getXferUiMode());
//   if (dbMode == "SELECT_POINT") {
// 	uiMode = SELECT_POINT;
//   } else if (dbMode == "SELECT_RECTANGLE") {
// 	uiMode = SELECT_RECTANGLE;
//   } else { // everything else is ZOOM
// 	uiMode = ZOOM;
// 	if (dbMode != "ZOOM") { // unknown value -> ZOOM
// 	  setXferUiMode("ZOOM");
// 	}
//   }
//   setUserInteractionMode(uiMode);

//   //  TransactionNumber trans = GuiQtManager::Instance().LastUpdate();

//   if( getDRefStruct() ) {
//     if( getDRefStruct()->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
//       always = true;
//     }
//   }

//   if( !always && isAxisUpdated( trans ) ){
//     always = true;
//   }
//   if( !always && getOverrideSymbolSize()) {  // ALWAYS!
//     always = true;
//   }

//   // enable or disable cycle popup menu button
//   if( m_cyclesButton != 0 ) {
//     if( dpi().numCycles() > 1 ){
//       m_cyclesButton->enable();
//     } else {
//       m_cyclesButton->disable();
//     }
//   }

//   updateAxisTypes();

//   // update axis scale
//   updateAxisScaleValues();

  doPlot(always);

  // (re)set axis titles
  setAxisTitles();

//   // set x Axis Scale Draw
//   // dynamic_cast<GuiQwtScaleDraw*>(m_chart->axisScaleDraw(AXIS_X_BOTTOM))->setAnnotationType( m_showAnnotationLabels );

  bool replot( false );
  clearBoundingBox();
  int currentCycle = dpi().currentCycle();
  for(auto plot: m_plots) {
    dpi().goCycle( plot->cycle(), false );  // do not clear undo stack
    if (hasCyclePlotMode()) refreshAllDataItemIndexedList();
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      tPlotItemIterator it = plot->plotItems( axisType[axis] ).begin();
      while( it != plot->plotItems( axisType[axis] ).end() ) {
        void* plotItem = (*it)->getPlotCurve(plot->cycle(),
                                             plot->xIndex(), plot->yIndex());
        if( (*it)->getAxisType() !=XAXIS && (*it)->getAxisType() !=HIDDEN ){
          if( (*it) && (*it)->isDataItemUpdated (trans) || always || plotItem == 0 ||
              (*it)->xPlotDataItem() && (*it)->xPlotDataItem()->isDataItemUpdated(trans) ){
            if (m_showCycleVector[plot->cycle() ]) {
              replot |= updateCurve( *it,  plot->cycle(), plot->xIndex(), plot->yIndex());
            }
          }
        }
        ++it;
      }
    }
  }
  dpi().goCycle( currentCycle, false ); // do not clear undo stack
//   if (hasCyclePlotMode()) refreshAllDataItemIndexedList();

  if( replot || always ){
    setAutoScale();

//     setStartScaleValues();

//     // leider muessen hier das zoom zurueck setzen
//     if (m_zoomer[0]->zoomRectIndex() > 0) {
//       m_zoomer[0]->zoom(0);
//       m_zoomer[1]->zoom(0);
//     }
  }

//   setAnnotationLabels();
//   // 2014-04-23 erzwingt das update beim Annnotation
//   if (isAnnotationLabelsUpdated( trans )   && m_showAnnotationLabels) {
//     showAnnotationLabels( false );
//     showAnnotationLabels( true );
//   } else
//     showAnnotationLabels( m_showAnnotationLabels );

  drawHeaderText();
  drawFooterText();

//   if(m_zoomer[0]->zoomRectIndex() == 0 && replot || always )
//     setScale(trans);

//   setRescaleMode();
//   m_chart->replot();
// #if HAVE_QPOLAR
//   if (m_polarPlot) m_polarPlot->update();  // repaint?
// #endif
//   if ( m_chart->legend() ) {
//     QList<QWidget*> list;
//   }
//   m_chart->updateLayout(); // z.B. header text neu da -> zeigen
//   //std::cout << "<<<<+++ Update reason["<<reason<< "] Ende  ts["<<st.msecsTo(QTime::currentTime())<<"]\n"<<std::flush;

//   // 2013-01-11 amg: zoomBase () wird nun nach jeden GuiUpdate gemacht
//   if (!getAxis(0).isScaleEnabled()) {
//     m_zoomer[0]->setZoomBase(false);
//   }
//   if (!getAxis(1).isScaleEnabled()) {
//     m_zoomer[1]->setZoomBase(false);
//   }

  connectMarkers();
  ///  m_chart->zoomReset();
  ///  m_chart->update();
  ///  m_chart->createDefaultAxes();
}

/* --------------------------------------------------------------------------- */
/* doPlot --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::doPlot(bool clear_always) {

  int currentCycle = dpi().currentCycle();
  int value = 0;
  if (getAllCycles())
    value = 1;
  m_showCycleVector.clear();
  if( getCyclesDialog() != 0 )
    getCyclesDialog()->getValues( m_showCycleVector, value );
  m_showCycleVector.resize( dpi().numCycles() -1, value );
  if( currentCycle == m_showCycleVector.size() )
    m_showCycleVector.push_back( 1 );
  else{
    std::vector<int>::iterator intIterator = m_showCycleVector.begin() + currentCycle;
    m_showCycleVector.insert( intIterator, 1 );
  }

  clearPlots(clear_always);

  int cycle = 0;
  PlotItem *xItem = 0;
  std::vector<int>::iterator intIt;
  for( intIt = m_showCycleVector.begin(); intIt != m_showCycleVector.end(); ++intIt, ++cycle ) {
    if( *intIt == 1 ) {
      updateAxisTypes();

      // check if already exits
      tPlotIterator plotIt;
      for( plotIt = m_plots.begin(); plotIt != m_plots.end(); ++plotIt )
        if ((*plotIt)->cycle() == cycle && !(*plotIt)->hasAxisWildcardIndex())
          break;

      if ( plotIt != m_plots.end())
        continue;

      // get guiIndex dimension
      int yGuiIndexdimSize = -1, xGuiIndexdimSize = -1;
      for(auto plotItem: m_plotItems){
        if (plotItem->hasAxisWildcardIndex()) {
          // reset y index
          GuiPlotDataItem *yItem = plotItem->plotDataItem();
          XferDataItemIndex *yIndex = yItem->getDataItemIndexWildcard( plotItem->getDataPlotItemIndex() );
          yIndex->setIndex( yItem->Data(), 0);
          // get dimension
          PlotItem *itemGuiIndex = plotItem;
          dpi().goCycle( cycle, false ); // do not clear undo stack
          if (hasCyclePlotMode()) refreshAllDataItemIndexedList();
          if (plotItem->getAxisType() == XAXIS) {
            xGuiIndexdimSize = std::max(xGuiIndexdimSize, itemGuiIndex->getAxisWildcardIndexDimension());
          } else {
            yGuiIndexdimSize = std::max(yGuiIndexdimSize, itemGuiIndex->getAxisWildcardIndexDimension());
          }
        }
        BUG_DEBUG("AxisType: " << plotItem->getAxisType());
      }
      BUG_DEBUG("Y yGuiIndexdimSize: " << yGuiIndexdimSize << ", m_plotItems SIZE: " << m_plotItems.size());
      BUG_DEBUG("X xGuiIndexdimSize: " << xGuiIndexdimSize << ", m_plotItems SIZE: " << m_plotItems.size());

      // create Plots
      // xIndex und yIndex sind synchron
      int yIndex = 0;
      xAxisPlotItemTitles.clear();
      yIndex = 0;
      int maxIndex = xGuiIndexdimSize > 0 ? std::min(xGuiIndexdimSize, yGuiIndexdimSize) : yGuiIndexdimSize;
      do {
        Plot *plot = new Plot( cycle, yIndex, yIndex );
        xItem = 0;
        for(auto plotItem: m_plotItems) {
          if( plot->addPlotItem( plotItem ) == XAXIS ) {

            xItem = plotItem;
            // if (drawXAxisPlotItemTitles) {
            //   addLabel( xAxisPlotItemTitles, xItem->xPlotDataItem() );
            // }
          }
        }
        // beim xrtgraph erhalten alle plots die selbe X-Achse
        // if( m_plot )
        //   plot->setAllXPlotItems( xItem );
        m_plots.push_back( plot );
      } while (++yIndex < maxIndex);
      BUG_DEBUG("Number of Plots: " << m_plots.size());
    }
    else {
      // hide plot
      for(auto plotItem: m_plotItems) {
        QAbstractSeries *lineCurve = reinterpret_cast<QAbstractSeries*>(plotItem->getPlotCurve(cycle, 0, 0));
        if (lineCurve) {
           lineCurve->hide();
//           plotItem->setItemAttribute(QwtPlotItem::Legend, false);
//           m_chart->updateLegend(plotItem);
        }
      }
    }
  }

  dpi().goCycle( currentCycle, false ); // do not clear undo stack
  if (hasCyclePlotMode()) refreshAllDataItemIndexedList();
  updateAxisTypes();
}

// /* --------------------------------------------------------------------------- */
// /* setScale --                                                                 */
// /* --------------------------------------------------------------------------- */
// bool GuiQt2dPlot::setScale(TransactionNumber trans){
//   double min[AXIS_TYPE_COUNT], max[AXIS_TYPE_COUNT], origin[AXIS_TYPE_COUNT];
//   double lowerBound[AXIS_TYPE_COUNT], upperBound[AXIS_TYPE_COUNT];
//   AXIS_POSITION axis[AXIS_TYPE_COUNT] = { AXIS_Y_LEFT, AXIS_Y_RIGHT, AXIS_X_BOTTOM, AXIS_X_TOP };
//   bool refresh(false);
//   for( int n = 0; n < AXIS_TYPE_COUNT; ++n ) {
//     bool with_factor = true;
//     bool use_scale = ( getAxis(n).getMin( min[n], with_factor ) &&
//                        getAxis(n).getMax( max[n], with_factor ) &&
//                        getAxis(n).isScaleEnabled() );
//     bool use_origin = getAxis(n).getOrigin( origin[n] );
//     if( use_scale || use_origin ) { // manual scale
//       // get bounds
// #if QWT_VERSION >= 0x060100
//       lowerBound[n] = m_chart->axisScaleDiv(axis[n]).lowerBound();
//       upperBound[n] = m_chart->axisScaleDiv(axis[n]).upperBound();
// #elif QWT_VERSION >= 0x050200
//       lowerBound[n] = m_chart->axisScaleDiv(axis[n])->lowerBound();
//       upperBound[n] = m_chart->axisScaleDiv(axis[n])->upperBound();
// #else
//       lowerBound[n] = m_chart->axisScaleDiv(axis[n])->lBound();
//       upperBound[n] = m_chart->axisScaleDiv(axis[n])->hBound();
// #endif
//       if ( !use_scale ) { // no scale enabled -> use bounds as min, max
//         min[n] = lowerBound[n];
//         max[n] = upperBound[n];
//       }

//       // use origin
//       if ( use_origin ) {
//         if ( min[n] > origin[n] ) {
//           min[n] = origin[n];
//         } else if ( max[n] < origin[n] ) {
//           max[n] = origin[n];
//         }
//       }

// #if HAVE_QPOLAR
//     if (m_polarPlot) {
//       if (n == 0) {
//         QList<QAbstractAxis*> lst = m_polarChart->axes(QPolarChart::PolarOrientationRadial);
//         for (QList<QAbstractAxis*>::iterator  i = lst.begin(); i != lst.end(); ++i) {
//           (*i)->setRange(min[n], max[n]);
//         }
//       }
//       if (n == 2) {
//         QList<QAbstractAxis*> lst = m_polarChart->axes(QPolarChart::PolarOrientationAngular);
//         for (QList<QAbstractAxis*>::iterator  i = lst.begin(); i != lst.end(); ++i) {
//           (*i)->setRange(min[n], max[n]);
//         }
//       }
//     }
// #endif
//       // change axis scale, if needed
//       if (lowerBound[n] != min[n] || upperBound[n] != max[n] ) {
//         BUG_DEBUG("setAxisScale axis: " << axis[n] << ", range: " << min[n] << ", " << max[n]);
//         m_chart->setAxisScale( axis[n], min[n], max[n] );
//         m_chart->updateAxes();
//         refresh = true;
//       }
//     }
//     else{
//       if (!m_chart->axisAutoScale( axis[n] )) {
//         m_chart->setAxisAutoScale( axis[n] );
//         refresh = true;
//       }
//     }
//   }
// m_zoomer[0]->setZoomBase(false);
// m_zoomer[1]->setZoomBase(false);
//   if (!trans && refresh) {
//     m_chart->replot();
//     m_zoomer[0]->setZoomBase(false);
//     m_zoomer[1]->setZoomBase(false);
//     return false;
//   }
//   else return refresh;
// }

// /* --------------------------------------------------------------------------- */
// /* setStartScaleValues --                                                      */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::setStartScaleValues(){
//   if( m_zoomer[0] == 0 )
//     return;

//   const QRectF &rect = m_zoomer[0]->zoomBase();
//   const QRectF &rect2 = m_zoomer[1]->zoomBase();

//   if (rect.isValid() && rect.left() != 0 &&
//       abs(rect.right()-1000.) < std::numeric_limits<double>::epsilon()) {
//     getAxis(0).setMinStartValue( rect.top() );
//     getAxis(0).setMaxStartValue( rect.bottom() );
//     getAxis(2).setMinStartValue( rect.left() );
//     getAxis(2).setMaxStartValue( rect.right() );
//   }
//   if (rect2.isValid() && rect2.left() != 0 &&
//       abs(rect2.right()-1000.) < std::numeric_limits<double>::epsilon()) {
//     getAxis(1).setMinStartValue( rect2.top() );
//     getAxis(1).setMaxStartValue( rect2.bottom() );
//   }
// }


// /* --------------------------------------------------------------------------- */
// /* addLabel --                                                                 */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::addLabel( std::vector<std::string> &labels,
// 			   GuiPlotDataItem *item ){
//   std::string label;
//   item->getLabelAndUnit( label, m_showAnnotationLabels );
//   if( label.empty() )
//     return;
//   if( std::find( labels.begin(), labels.end(), label ) == labels.end() )
//     labels.push_back( label );
// }

// //----------------------------------------------------
// // getSymbolPen
// //----------------------------------------------------
// QPen GuiQt2dPlot::getSymbolPen(QwtSymbol::Style sym, QString symCol, QString symPenCol) {
//   // symbol color is used by brush, but some symbols don't use the brush
//   // they only use the pen
//   // > use symbol color for the pen for symbols that don't use the brush
//   QPen pen = QPen(QColor(symPenCol));  // default solid line pen with 1 width
//   if (sym == QwtSymbol::Cross ||
//       sym == QwtSymbol::XCross ||
//       sym == QwtSymbol::HLine ||
//       sym == QwtSymbol::VLine ||
//       sym == QwtSymbol::Star1) {
//     pen.setColor(QColor(symCol));
//   }
//   return pen;
// }

//----------------------------------------------------
// updateCurve
//----------------------------------------------------
bool GuiQt2dPlot::updateCurve( PlotItem *item, int cycle, int xWildcardIndex, int yWildcardIndex){
   BUG_DEBUG("GuiQt2dPlot::updateCurve()");

   bool cyclePlotMode( hasCyclePlotMode() );
//   int curve;
   GuiPlotDataItem *yItem = item->plotDataItem();
   GuiPlotDataItem *xItem = item->xPlotDataItem();

  // simple yWildcardIndex check
  if (!item->hasAxisWildcardIndex() && yWildcardIndex > 0)
    return false;

  if( !xItem || item->getAxisType() == XAXIS )
    return false;
  if( item->isMarker() ){
//     return updateMarker(item, xWildcardIndex, yWildcardIndex);
  }

//   // yAxisType
//   int yAxisType;
//   if( item->getAxisType() == Y1AXIS || item->getAxisType() == HIDDEN ) {
//     // HIDDEN = Y1AXIS bis wir was schlaueres haben....
//     yAxisType = AXIS_Y_LEFT;
//   } else {
//     yAxisType = AXIS_Y_RIGHT;
//   }

//   //
//   // Nur damit die Farbe des Kurve oder ähnliches reserviert wird
//   // if WildcardIndex Mode: set y WildcardIndex
//   if (item->hasAxisWildcardIndex()) {
//     item->setAxisWildcardIndexValue(yWildcardIndex);

//     // ignore invalid plotitems
//     if (item->getNPoints() == 0 && yWildcardIndex > 0) {
//       m_curveAttrs.getPalette(yItem->getUniqueName(xWildcardIndex, yWildcardIndex, cyclePlotMode));
//       BUG_MSG("Ignore Invalid plotItem with Varname: "+ yItem->XferData()->getFullName(true));
//       return false;
//     }
//   }

//   // if WildcardIndex Mode: set x WildcardIndex
//   if (item->xPlotItem() && item->xPlotItem()->hasAxisWildcardIndex()) {
//     item->xPlotItem()->setAxisWildcardIndexValue(xWildcardIndex);

//     // ignore invalid plotitems
//     if (item->xPlotItem()->getNPoints() == 0 && xWildcardIndex > 0) {
// 	  m_curveAttrs.getPalette(yItem->getUniqueName(xWildcardIndex, yWildcardIndex, cyclePlotMode));
//       BUG_MSG("Ignore Invalid xplotItem with Varname: "+ xItem->XferData()->getFullName(true));
//       return false;
//     }
//   }

  // get plotitem data
  std::vector<Point> pts;
  tMarkerDataVector markerLabels;
  int validCnt = readData(item, xWildcardIndex, yWildcardIndex, pts, markerLabels);

  // get label
  std::string label = PlotItemLabel(yItem);

  int npoints = item->getNPoints();
//   QwtPlotCurve* plotCurve = 0;
//   void* voidItem = 0;
//   QwtPlotItem* plotItem = 0;
//   GuiQwtPlotBarChart* plotBarItem = 0;
// #if HAVE_QPOLAR
//   QLineSeries *polarCurve = 0;
// #endif
  Plot2dCurvePalette* curvePalette = m_curveAttrs.getPalette(yItem->getUniqueName(xWildcardIndex, yWildcardIndex, cyclePlotMode));

//   // get existing or create a new plotItem
//   if (!(voidItem = item->getPlotCurve(cycle, xWildcardIndex, yWildcardIndex))) {

//     // create
//     GuiQwtPlotCurve::CurveStyleExt style;
//     style = getQwtCurveStyle( item->getAxisType() );

// #if QWT_VERSION >= 0x060100
// #if HAVE_QPOLAR
//     else
//     if ( style == GuiQwtPlotCurve::Polar) {
//       QPen pen(QString::fromStdString(curvePalette->lineColor));
//       pen.setWidth(2);
//       QLineSeries *series = m_polarChart->addSeries(pts, label, m_polarPlot, pen)
//       item->setPlotCurve(cycle, xWildcardIndex, yWildcardIndex, series);
//       return true;
//     }
// #endif
//     else {
// #endif
//     //
//     // normale PlotCurve
//     plotCurve = new GuiQwtPlotCurve( QString::fromStdString(label) );
//     plotCurve->setTitle( QString::fromStdString(label) );
//     plotCurve->setZ(curvePalette->index+1);
//     if (curvePalette->lineStyle == Qt::NoPen)
//       plotCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);
//     else if (curvePalette->lineStyle != Qt::SolidLine) {
//       plotCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine);
//       plotCurve->setLegendIconSize(QSize(curvePalette->lineStyle == Qt::DashDotLine ? 15 : 22, 10));
//     }

//     if (curvePalette->lineStyle == Qt::NoPen)
//       plotCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);

//     // color
//     QPen pen((Qt::PenStyle) curvePalette->lineStyle);

//     pen.setWidth(curvePalette->lineWidth);
//     pen.setColor(QString::fromStdString(curvePalette->lineColor));
//     plotCurve->setPen( pen );

//     plotItem = plotCurve;
// #if QWT_VERSION >= 0x060100
//     }
// #endif
// #if QWT_VERSION < 0x060000
//     plotCurve->setAxis( QwtPlot::xBottom, yAxisType );
// #else
//     plotItem->setAxes( AXIS_X_BOTTOM, yAxisType );
// #endif

//     plotItem->attach(m_plot);
//     item->setPlotCurve( cycle, xWildcardIndex, yWildcardIndex, plotItem );
//     m_chart->enableAxis( yAxisType, isAxisShow(item->getAxisType()));
//     m_chart->axisTitle( yAxisType ).setColor( GuiQtManager::foregroundColor() );

//     // Inverted if Steps style
//     if( style == GuiQwtPlotCurve::Steps ) {
//       plotCurve->setCurveAttribute(QwtPlotCurve::Inverted, true);
//     }

//   } else {
//   }

//   // if y-Axis logarithmic set baseline to e-100 (default 0. makes no sense)
//   // for barplots and area plots
//   if ( getAxis(0).isLogarithmic() &&
//        (getQwtCurveStyle( item->getAxisType() ) == GuiQwtPlotCurve::Bar ||
//         getQwtCurveStyle( item->getAxisType() ) == GuiQwtPlotCurve::StackingBar ||
//         m_style[ (item->getAxisType() == Y1AXIS ? 0 : 1) ] == Gui2dPlot::AREA) ) {
//     if (plotCurve) plotCurve->setBaseline(1e-100);
// #if QWT_VERSION >= 0x060100
//     else if(dynamic_cast<GuiQwtPlotBarChart*>(plotItem)) {
//       dynamic_cast<GuiQwtPlotBarChart*>(plotItem)->setBaseline(1e-100);
//     }
// #endif
//   }

//   // advanced settings
//   GuiQwtPlotCurve::CurveStyleExt style;
//   style = getQwtCurveStyle( item->getAxisType() );
//   if (plotCurve) {
//     if( style == GuiQwtPlotCurve::Steps ){
//       plotCurve->setCurveAttribute(QwtPlotCurve::Inverted, true);
//     }
//     plotCurve->setStyle( (QwtPlotCurve::CurveStyle) style );
//     if (m_style[ (item->getAxisType() == Y1AXIS ? 0 : 1) ] == Gui2dPlot::AREA) {
//       plotCurve->setBrush(QBrush( plotCurve->pen().color(), Qt::Dense5Pattern) );
//     } else {
//       plotCurve->setBrush( Qt::NoBrush );
//     }
//   }

//   // append plotitem data
//   GuiQwtPlotCurveData*  _curveData = new GuiQwtPlotCurveData(npoints);

  QString qTitle(QString::fromStdString(label));
  QAbstractSeries* series(0);
  for(QAbstractSeries* s: m_chart->series()){
    if (s->name() == qTitle){
      series = s;
      // qt 6.10 imperfection
      QXYSeries* xyseries = dynamic_cast<QXYSeries*>(s);
      if (xyseries) xyseries->clear();
      QBarSeries* barseries = dynamic_cast<QBarSeries*>(s);
      if (barseries) barseries->clear();
      break;
    }
  }
  if (pts.size() == 0)
    return false;
  QXYSeries* xyseries(0);
  QBarSeries* barseries(0);
  if (!series) {
    eStyle style = m_style[ (item->getAxisType() == Y1AXIS ? 0 : 1) ];
    if (style == PLOT){
      if (item->isMarker()) series = new QScatterSeries();
      else series = new QLineSeries();
    }else if (style == DOTS)
      series = new QScatterSeries();
    else if (style == AREA)
      series = new QAreaSeries();
    else if (style == BAR){
      barseries = new QBarSeries();
      series = barseries;
    }
    else
      series = new QSplineSeries();
    xyseries = dynamic_cast<QXYSeries*>(series);

    // add data
    if (barseries){
      QBarSet* barset = new QBarSet(series->name());
      for (auto pt: pts) {
        barset->append(pt.y);
      }
      barseries->append(barset);
      m_chart->createDefaultAxes();
    }else{
      for (auto pt: pts) {
        if (xyseries){
          xyseries->append(pt.x, pt.y);
          xyseries->append(pt.x, pt.y);
        }
      }
      QObject::connect(xyseries, &QLineSeries::pressed, this, &GuiQt2dPlot::clickPoint);
    }
    m_chart->addSeries(series);
    series->attachAxis(m_xaxis);
    if (item->getAxisType() == Y1AXIS){
      series->attachAxis(m_y1axis);
    }else{
      series->attachAxis(m_y2axis);
    }
  }
  else{
    xyseries = dynamic_cast<QXYSeries*>(series);
    barseries = dynamic_cast<QBarSeries*>(series);
    QBarSet* barset(0);
    if (barseries){
      barset = barseries->barSets()[0];//new QBarSet(series->name());
    }
    for (auto pt: pts) {
      if (xyseries)
        xyseries->append(pt.x, pt.y);
      if (barseries)
        barset->append(pt.y);
    }
  }

  // bounding box for autoscale
  if (xyseries){
    QRectF bbox = getBoundingBox(xyseries);
    setBoundingBox(item->getAxisType(), bbox.y(), bbox.y() + bbox.height());
    setBoundingBox(XAXIS, bbox.x(), bbox.x() + bbox.width());
  }

  //
  ///  series->setSelectedLightMarker(roundrect(6.0, Qt::red));
  
  // legend
  series->setName(qTitle);
  item->setPlotCurve(cycle, xWildcardIndex, yWildcardIndex, series);
  if (m_legendVis.count(qTitle)) {
    series->setVisible(m_legendVis[qTitle]);
  }

  // marker
  if(xyseries && item->isMarker()){
    // backward compatibility: markers used to be red dots
    if ( curvePalette->symbolStyle == -1 ) {
      // marker symbolStyle not set -> red dot
      curvePalette->symbolStyle = 0;//QwtSymbol::Ellipse;
      curvePalette->symbolColor = std::string("red");
      curvePalette->symbolPenColor = std::string("black");
      curvePalette->symbolSize = 6;
      int pIdx = curvePalette->index;
      if (pIdx < 0) {
        // not found, should not be happen
        pIdx = 0;
      }
      m_curveAttrs.m_curveSymbols[pIdx % m_curveAttrs.m_curveSymbols.size()] = curvePalette->symbolStyle;
      m_curveAttrs.m_curveSymbolsSize[pIdx % m_curveAttrs.m_curveSymbolsSize.size()] = curvePalette->symbolSize;
      m_curveAttrs.m_symbolColors[pIdx % m_curveAttrs.m_symbolColors.size()] = curvePalette->symbolColor;
      m_curveAttrs.m_symbolPenColors[pIdx % m_curveAttrs.m_symbolPenColors.size()] = curvePalette->symbolPenColor;
    }

    qreal marker_size = curvePalette->symbolSize;
    xyseries->setMarkerSize(marker_size);
    xyseries->setLightMarker(circle(curvePalette->symbolSize, QString::fromStdString(curvePalette->symbolColor)));
    xyseries->setSelectedLightMarker(roundrect(marker_size, Qt::magenta));
    ///    xyseries->setColor(QString::fromStdString(curvePalette->symbolColor));
    ///    dynamic_cast<QLineSeries*>(series)->setLineStyle(QLineSeries::StepLeft);
  }
  if (xyseries){
    xyseries->setColor(QString::fromStdString(curvePalette->lineColor));
  }
  //   dynamic_cast<GuiQwtScaleDraw*>(m_chart->axisScaleDraw( getAnnotationAxis() ))->setAnnotationType(m_showAnnotationLabels);

//   int overrideSymbolSize = getOverrideSymbolSize();
//   // symbol
//   QwtSymbol::Style sym = (QwtSymbol::Style)curvePalette->symbolStyle;
//   int symSize = overrideSymbolSize > 0 ? overrideSymbolSize :
//     curvePalette->symbolSize ? curvePalette->symbolSize : 6;
//   QString symCol = QString::fromStdString(curvePalette->symbolColor);
//   QString symPenCol = QString::fromStdString(curvePalette->symbolPenColor);
//   // single point is not drawn in many styles -> use a symbol in these cases
//   if( validCnt == 1  && // single point
//       ( style == GuiQwtPlotCurve::Lines || style == GuiQwtPlotCurve::Steps ) && // curve is drawn as a line
//       sym == QwtSymbol::NoSymbol // no symbol used -> point would not be shown at all
//       ){
//     sym = QwtSymbol::Ellipse; // use Ellipse
//   } else if( style == GuiQwtPlotCurve::Dots && sym == QwtSymbol::NoSymbol){ // dots but no symbol
//     sym = QwtSymbol::XCross; // use XCross
//   }
//   if (sym != QwtSymbol::NoSymbol) {
//     QPen pen = getSymbolPen(sym, symCol, symPenCol);
// #if QWT_VERSION < 0x060000
//     plotCurve->setSymbol(QwtSymbol(sym, QBrush(QColor(symCol)), pen, QSize(symSize, symSize)));
// #else
//     if (plotCurve) plotCurve->setSymbol(new QwtSymbol(sym, QBrush(QColor(symCol)), pen, QSize(symSize, symSize)));
// #endif
//   }

//     if (xItem) {
//       GuiQwtScaleDraw::Type type = GuiQwtScaleDraw::type_real;
//       bool bDate = false;
//       if ( (xItem->Attr()->StringType() == UserAttr::string_kind_date) ) {
// 	type = GuiQwtScaleDraw::type_date;
// 	bDate = true;
//       }
//       if ( (xItem->Attr()->StringType() == UserAttr::string_kind_time) ) {
// 	type = GuiQwtScaleDraw::type_time;
// 	bDate = true;
//       }
//       if ( (xItem->Attr()->StringType() == UserAttr::string_kind_datetime) ) {
// 	type = GuiQwtScaleDraw::type_datetime;
// 	bDate = true;
//       }

//       dynamic_cast<GuiQwtScaleDraw*>(m_chart->axisScaleDraw(AXIS_X_BOTTOM))->setType(type);
//       if ( dynamic_cast<GuiQwtScaleEngine*>(m_chart->axisScaleEngine(AXIS_X_BOTTOM)) )
//         dynamic_cast<GuiQwtScaleEngine*>(m_chart->axisScaleEngine(AXIS_X_BOTTOM))->setDateType(bDate);
//     } else {
//       GuiQwtScaleDraw::Type type = GuiQwtScaleDraw::type_real;
//       dynamic_cast<GuiQwtScaleDraw*>(m_chart->axisScaleDraw( getAnnotationAxis() ))->setType( type );
//       if ( dynamic_cast<GuiQwtScaleEngine*>(m_chart->axisScaleEngine( getAnnotationAxis() )) )
//         dynamic_cast<GuiQwtScaleEngine*>(m_chart->axisScaleEngine( getAnnotationAxis() ))->setDateType( false );
//     }
// #if QWT_VERSION < 0x060000
//     // falls am Ende nur ungueltige Werte sind, wird nur die
//     // Anzahl der zuvor gueltigen Werte uebergeben
//     plotCurve->setData( _curveData->getXArray(), _curveData->getYArray(),
// 			(firstInvalidIdx != validCnt) ? npoints : validCnt);
//     delete _curveData;
// #else
//     if (plotCurve) plotCurve->setData( _curveData );
// #endif
// #if QWT_VERSION >= 0x060100
//     if (plotBarItem) {
//       dynamic_cast<GuiQwtPlotBarChart*>(plotBarItem)->setSamples( _curveData );
//       dynamic_cast<GuiQwtPlotBarChart*>(plotBarItem)->addScaleFactor( xItem ? xItem->getScaleFactor() : 1,
// 								      yItem ? yItem->getScaleFactor() : 1);
//     }
// #endif

// #if HAVE_QPOLAR
//   QString qTitle = plotItem ? plotItem->title().text() : polarCurve->name();
// #else
//   QString qTitle = plotItem ? plotItem->title().text() : "";
// #endif
//   if (!m_legendVis.count(qTitle)) {
//     m_legendVis[qTitle] = true;
//   }

//   // hide legend ?
//   if (item->isLegendHide()) {
//     if (plotItem) {
//       plotItem->setItemAttribute(QwtPlotItem::Legend, false );
//       m_chart->updateLegend(plotItem);
//     }
//   }
// #if HAVE_QPOLAR
//   if (polarCurve) return polarCurve->isVisible();
// #endif
// return  plotItem ? plotItem->isVisible() : false;
  return true;
}

//----------------------------------------------------
// setAxisTitles
//----------------------------------------------------
void GuiQt2dPlot::setAxisTitles(){
  QString t;
  if( getTitleStream(XAXIS) != 0  )
     t = QString::fromStdString(convertToString( getTitleStream(XAXIS) ));
  else if( !getTitle(XAXIS).empty() ){
    t = QString::fromStdString(getTitle(XAXIS));
  }
  if(t.isEmpty() || m_showAnnotationLabels){  // !!! annotation with plotItemTitle
    std::ostringstream os;
    for(std::vector<std::string>::iterator iter = xAxisPlotItemTitles.begin();
        iter != xAxisPlotItemTitles.end(); ++iter ){
      if( os.str().size() )
        os << "  ";
      os << *iter;
    }
    if( os.str().size() )
      t = QString::fromStdString(os.str());
  }
  // if (m_chart->title() != t){
  //   m_chart->setTitle(t);
  // }
  // x axis
  auto axisX = m_chart->axes(Qt::Horizontal);
  for (auto axis: axisX){
    if (axis->titleText() != t){
      axis->setTitleText(t);
    }
  }
  t.clear();
//   if( getTitleStream(XAXIS2) != 0  )
//     t = QString::fromStdString(convertToString( getTitleStream(XAXIS2) ));
//   else if( !getTitle(XAXIS2).empty() ){
//     t = QString::fromStdString(getTitle(XAXIS2));
//   }
//   if (m_chart->axisTitle(AXIS_X_TOP) != t)
//     m_chart->setAxisTitle(AXIS_X_TOP, t );

  // y1 axis
  t.clear();
  if( getTitleStream(Y1AXIS) != 0  )
    t = QString::fromStdString(convertToString( getTitleStream(Y1AXIS) ));
  else if( !getTitle(Y1AXIS).empty() )
    t = QString::fromStdString(getTitle(Y1AXIS));
  auto axisY = m_chart->axes(Qt::Vertical);
  if (axisY.size() > 0 && axisY[0]->titleText() != t){
      axisY[0]->setTitleText(t);
  }

  // y2 axis
  t.clear();
  if( getTitleStream(Y2AXIS) != 0  )
    t = QString::fromStdString(convertToString( getTitleStream(Y2AXIS) ));
  else if( !getTitle(Y2AXIS).empty() )
    t = QString::fromStdString(getTitle(Y2AXIS));
  if (axisY.size() > 1 && axisY[1]->titleText() != t){
    axisY[1]->setTitleText(t);
  }
}

// //----------------------------------------------------
// // updateMarker
// //----------------------------------------------------
// bool GuiQt2dPlot::updateMarker(PlotItem *item, int xWildcardIndex, int yWildcardIndex){
//   if( !item->isMarker() )
//     assert( false );
//   if( item->getAxisType() == XAXIS )
//     return false;

//   GuiPlotDataItem *yItem = item->plotDataItem();
//   GuiPlotDataItem *xItem = item->xPlotDataItem();

//   assert( yItem->XferData() != 0 );

//   // yAxisType
//   int yAxisType;
//   if( item->getAxisType() == Y1AXIS || item->getAxisType() == HIDDEN ) {
//     // HIDDEN = Y1AXIS bis wir was schlaueres haben....
//     yAxisType = AXIS_Y_LEFT;
//   } else {
//     yAxisType = AXIS_Y_RIGHT;
//   }

//   //
//   QwtPlotCurve* plotCurve = 0;
//   QwtPlotItem* plotItem = 0;
//   void* voidItem = 0;
//   if (!(voidItem = item->getPlotCurve( dpi().currentCycle(), xWildcardIndex, yWildcardIndex))) {
//     // plotCurve = dynamic_cast<QwtPlotCurve*>(plotItem);
//     std::string label;
//     yItem->getLabelAndUnit( label );

//     // HIDDEN = Y1AXIS bis wir was schlaueres haben....
//     plotCurve = new GuiQwtPlotCurve( QString::fromStdString(label) );
// #if QWT_VERSION < 0x060000
//     plotCurve->setAxis( QwtPlot::xBottom, QwtPlot::yLeft );
// #else
//     plotCurve->setAxes( AXIS_X_BOTTOM, AXIS_Y_LEFT );
// #endif
//     plotCurve->attach(m_plot);
//     item->setPlotCurve( dpi().currentCycle(), xWildcardIndex, yWildcardIndex, plotCurve );
//     m_chart->enableAxis(yAxisType, isAxisShow(item->getAxisType()));

// #if QWT_VERSION < 0x060000
//     plotCurve->setAxis( QwtPlot::xBottom, yAxisType );
// #else
//     plotCurve->setAxes( AXIS_X_BOTTOM, yAxisType );
// #endif

//     if ( m_chart->legend() )
//       m_chart->legend()->setEnabled( false );

//     // hide marker legend ?
//     if (item->isLegendHide()) {
//       plotCurve->setItemAttribute(QwtPlotItem::Legend, false );
//       // hide from legend if Style is NoCurve
//       plotCurve->setStyle( QwtPlotCurve::NoCurve );
// #if QWT_VERSION >= 0x060100
//       m_chart->updateLegend(plotCurve);
// #endif
//     }
//   }
//   if (!plotCurve) return false;

//   bool cyclePlotMode( hasCyclePlotMode() );
//   Plot2dCurvePalette* curvePalette = m_curveAttrs.getPalette(yItem->getUniqueName(xWildcardIndex, yWildcardIndex, cyclePlotMode));
//   QColor color( QString::fromStdString(curvePalette->lineColor) );
//   QPen pen( plotCurve->pen() );
//   pen.setWidth(curvePalette->lineWidth);
//   pen.setColor( color  );
//   plotCurve->setPen( pen );

//   // backward compatibility: markers used to be red dots
//   if ( curvePalette->symbolStyle == QwtSymbol::NoSymbol ) {
//     // marker symbolStyle not set -> red dot
//     curvePalette->symbolStyle = QwtSymbol::Ellipse;
//     curvePalette->symbolColor = std::string("red");
//     curvePalette->symbolPenColor = std::string("black");
//     curvePalette->symbolSize = 6;
// 		int pIdx = curvePalette->index;
// 		if (pIdx < 0) {
//       // not found, should not be happen
//       pIdx = 0;
//     }
//     m_curveAttrs.m_curveSymbols[pIdx % m_curveAttrs.m_curveSymbols.size()] = curvePalette->symbolStyle;
//     m_curveAttrs.m_curveSymbolsSize[pIdx % m_curveAttrs.m_curveSymbolsSize.size()] = curvePalette->symbolSize;
//     m_curveAttrs.m_symbolColors[pIdx % m_curveAttrs.m_symbolColors.size()] = curvePalette->symbolColor;
//     m_curveAttrs.m_symbolPenColors[pIdx % m_curveAttrs.m_symbolPenColors.size()] = curvePalette->symbolPenColor;
//   }

//   int overrideSymbolSize = getOverrideSymbolSize();
//   QString symCol = QString::fromStdString(curvePalette->symbolColor);
//   QString symPenCol = QString::fromStdString(curvePalette->symbolPenColor);
//   QwtSymbol::Style sym = (QwtSymbol::Style)  curvePalette->symbolStyle;
//   int symSize = overrideSymbolSize > 0 ? overrideSymbolSize :
//     curvePalette->symbolSize ? curvePalette->symbolSize : 6;
//   QPen penMarker = getSymbolPen(sym, symCol, symPenCol);
// #if QWT_VERSION < 0x060000
//   plotCurve->setSymbol( QwtSymbol( sym, QBrush( QColor(symCol)),
//                                    penMarker, QSize(symSize, symSize)) );
// #else
//   plotCurve->setSymbol( new QwtSymbol(sym, QBrush(QColor(symCol)),
//                                       penMarker, QSize(symSize, symSize)) );
// #endif

//   int npoints = item->getNPoints();

//   XferDataItemIndex *xIndex = 0;
//   if( xItem ){
//     assert( xItem->XferData() != 0 );
//     xIndex = xItem->getDataItemIndexWildcard( 1 );
//   }
//   XferDataItemIndex *yIndex = yItem->getDataItemIndexWildcard( item->getDataPlotItemIndex() );
//   XferDataItemIndex *markerIndex = 0;
//   if( item->getMarker() ){
//     markerIndex = item->getMarker()->getDataItemIndexWildcard( item->getDataPlotItemIndex() );
//   }

//   GuiQwtPlotCurveData*  _markerData = new GuiQwtPlotCurveData(npoints);

//   std::vector<QwtPlotMarker*>& markers = item->getMarkers();
//   std::vector<QwtPlotMarker*>::iterator iter;
//   for( iter = markers.begin();iter != markers.end(); ++iter ){
//     delete (*iter);
//   }
//   markers.clear();
//   // get plotitem data
//   std::vector<Point> pts;
//   tMarkerDataVector markerLabels;
//   int np = readData(item, xWildcardIndex, yWildcardIndex, pts, markerLabels);
//   int j = 0;
//   for (tPointVector::iterator pit=pts.begin();
//        pit!=pts.end(); ++pit, ++j) {
//     _markerData->append( (*pit).x, (*pit).y);

//     if(!std::isnan((*pit).y)) {
//       std::string label(j < markerLabels.size() ? markerLabels[j].label : "");
//       std::string background(j < markerLabels.size() ? markerLabels[j].background : "");
//       std::string foreground(j < markerLabels.size() ? markerLabels[j].foreground : "");

//       // !!! only draw an label if exists !!!
//       if (label.size() > 0) {
//         QwtPlotMarker *marker = new QwtPlotMarker();
//         marker->attach(m_plot);

//         // override symbolColor
//         if (foreground.size()) {
//           QwtSymbol::Style sym = (QwtSymbol::Style) curvePalette->symbolStyle;
//           QString symCol = QString::fromStdString(foreground);
//           QString symPenCol = QString::fromStdString(curvePalette->symbolPenColor);
//           int overrideSymbolSize = getOverrideSymbolSize();
//           int symSize = overrideSymbolSize > 0 ? overrideSymbolSize :
//             curvePalette->symbolSize ? curvePalette->symbolSize : 6;
//           QPen pen = getSymbolPen(sym, symCol, symPenCol);
//           marker->setSymbol(new QwtSymbol(sym, QBrush(QColor(symCol)),
//                                           pen, QSize(symSize, symSize)));
//         } else {
//           // set label
//           marker->setLabel( QwtText( QString::fromStdString(label) ) );
//           marker->setLabelAlignment( Qt::AlignLeft );

//           QFont font = marker->label().font();
//           font =  QtMultiFontString::getQFont( "@plotMarker@", font );
//           marker->label().setFont( font );
//         }

//         marker->setXValue( (*pit).x );
//         marker->setYValue( (*pit).y );

//         if( item->getAxisType() == Y2AXIS ){
//           marker->setYAxis(AXIS_Y_RIGHT);
//         }
//         markers.push_back( marker );
//       }

//     }
//   }

//   if (plotCurve) {
// #if QWT_VERSION < 0x060000
//     plotCurve->setData( _markerData->getXArray(), _markerData->getYArray(), np);
//     delete _markerData;
// #else
//     plotCurve->setData( _markerData);
// #endif
//   }

//   // update checkable legend items
//   if (!m_legendVis.count(plotCurve->title().text() ) ) {
//     m_legendVis[ plotCurve->title().text() ] = true;
//   }
//   if (m_legendVis.count(plotCurve->title().text() ) ) {
//     showCurve(plotCurve, m_legendVis[plotCurve->title().text()], false  );
//   }
//   return true;
// }

//----------------------------------------------------
// myWidget
//----------------------------------------------------
QWidget *GuiQt2dPlot::myWidget(){
  return m_widgetStack;
}

// //----------------------------------------------------
// // myCurrentWidget
// //----------------------------------------------------
// QWidget *GuiQt2dPlot::myCurrentWidget(){
//   if(!m_widgetStack) {
//     return NULL;
//   }
//   return m_widgetStack->currentWidget();
// }

// //----------------------------------------------------
// // manage
// //----------------------------------------------------
// void GuiQt2dPlot::manage(){
//   if(myWidget()){
//     myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
//   }
//   std::vector<Gui2dPlot*>::iterator iter = m_clonedList.begin();
//   for (; iter != m_clonedList.end(); ++iter)
//     (*iter)->getElement()->manage();
// }

// //----------------------------------------------------
// // unmanage
// //----------------------------------------------------
// void GuiQt2dPlot::unmanage(){
//   if(myWidget()){
//     myWidget()->hide();
//   }
// }

// /* --------------------------------------------------------------------------- */
// /* acceptIndex --                                                              */
// /* --------------------------------------------------------------------------- */
// bool GuiQt2dPlot::acceptIndex( const std::string &name, int inx ) {
//   tPlotItemIterator it;
//   for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it ){
//     if( !(*it)->acceptIndex( name, inx ) ){
//       return false;
//     }
//   }

//   // axis
//   for( int n = 0; n < AXIS_TYPE_COUNT; ++n ){
//     if ( !getAxis(n).acceptIndex( name, inx ) )
//       return false;
//   }
//   return true;
// }

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */
GuiElement* GuiQt2dPlot::clone() {
  GuiElement* baseElem = findElement( getElement()->getName() );
  if (baseElem == this->getElement())
    m_clonedList.push_back( new GuiQt2dPlot( *this ) );
  else
    return baseElem->clone();
  return m_clonedList.back()->getElement();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  std::vector<Gui2dPlot*>::const_iterator iter = m_clonedList.begin();
  for (; iter != m_clonedList.end(); ++iter)
    cList.push_back( (*iter)->getElement() );
}

// /* --------------------------------------------------------------------------- */
// /* setIndex --                                                                 */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::setIndex( const std::string &name, int inx ) {
//   tPlotItemIterator it;
//   for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it ){
//     (*it)->setIndex( name, inx );
//   }

//   std::vector<Gui2dPlot*>::iterator iter = m_clonedList.begin();
//   for (; iter != m_clonedList.end(); ++iter)
//     (*iter)->getElement()->setIndex(name, inx);

//   // axis
//   for( int n = 0; n < AXIS_TYPE_COUNT; ++n ){
//     getAxis(n).setIndex( name, inx );
//   }
//   update( reason_Always );
// }

//---------------------------------------------------------------------------
// serializeXML
//---------------------------------------------------------------------------

void GuiQt2dPlot::serializeXML(std::ostream &os, bool recursive) {
  return Gui2dPlot::serializeXML(os, recursive);
}

// /* --------------------------------------------------------------------------- */
// /* serializeJson --                                                            */
// /* --------------------------------------------------------------------------- */
// bool GuiQt2dPlot::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
//   std::string s;
//   join(m_curveAttrs.m_lineColors, ' ', s);
//   jsonObj["colorList"] = s;
//   join(m_curveAttrs.m_symbolColors, ' ', s);
//   jsonObj["symbolColorList"] = s;
//   join(m_curveAttrs.m_symbolPenColors, ' ', s);
//   jsonObj["symbolPenColorList"] = s;
//   return Gui2dPlot::serializeJson(jsonObj, onlyUpdated);
// }

// /* --------------------------------------------------------------------------- */
// /* serializeProtobuf --                                                            */
// /* --------------------------------------------------------------------------- */
// #if HAVE_PROTOBUF
// bool GuiQt2dPlot::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
//   std::string s;
//   // join(m_curveAttrs.m_lineColors, ' ', s);
//   // jsonObj["colorList"] = s;
//   // join(m_curveAttrs.m_symbolColors, ' ', s);
//   // jsonObj["symbolColorList"] = s;
//   // join(m_curveAttrs.m_symbolPenColors, ' ', s);
//   // jsonObj["symbolPenColorList"] = s;
//   return Gui2dPlot::serializeProtobuf(eles, onlyUpdated);
// }
// #endif

// /* --------------------------------------------------------------------------- */
// /* closeEvent --                                                               */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::closeEvent( GuiEventData *event ) {

//   Gui2dPlot::closeEvent(event);
//   bool changes = false;
//   if (changes)
// 	update( reason_Always );
// }

// /* --------------------------------------------------------------------------- */
// /* resetEvent --                                                               */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::resetEvent( GuiEventData *event ) {
//   Gui2dPlot::resetEvent(event);
//   getConfigDialog()->getForm()->getElement()->update( GuiElement::reason_Always );
// }

// /* --------------------------------------------------------------------------- */
// /* configureIndexedMenu --                                                     */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::configureIndexedMenu( PlotItem *item, int idx ){
//   XferDataItem *varXfer[5];
//   DataReference *ref[6];
//   ref[0] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefLineStyleComboboxValue();
//   ref[1] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefSymbolStyleComboboxValue();
//   ref[2] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefSymbolSizeComboboxValue();
//   ref[3] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefLineColor();
//   ref[4] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefSymbolColor();
//   ref[5] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefUnitComboboxValue();
//   for (int i=0; i < 6; ++i) {
//     varXfer[i] = new XferDataItem( DataPool::newDataReference( *ref[i] ) );

//     XferDataItemIndex *inx = 0;
//     if( ( inx = varXfer[i]->getIndex( 0 ) ) == 0 ){
//       inx = varXfer[i]->newDataItemIndex();
//       inx->setIndex( varXfer[i]->Data(), idx );
//     }
//     inx->setIndex( varXfer[i]->Data(), idx);
//     varXfer[i]->setDimensionIndizes();

//     // PlotItem set xfers
//     switch(i) {
//     case 0:
//       item->setLineStyleXfer( varXfer[i] );
//       break;
//     case 1:
//       item->setSymbolStyleXfer( varXfer[i] );
//       break;
//     case 2:
//       item->setSymbolSizeXfer( varXfer[i] );
//       break;
//     case 3:
//       item->setLineColorXfer( varXfer[i] );
//       break;
//     case 4:
//       item->setSymbolColorXfer( varXfer[i] );
//       break;
//     case 5:
//       item->setUnitXfer( varXfer[i] );
//       break;
//     }
//   }
// }

// /* --------------------------------------------------------------------------- */
// /* configureMenu --                                                            */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::configureMenu( PlotItem *item, int idx ){
//   XferDataItem *varXfer[7];
//   XferDataItem *y1Xfer = 0;
//   XferDataItem *y2Xfer = 0;
//   XferDataItem *lineStyleXfer = 0, *symbolStyleXfer = 0;
//   DataReference *ref[7];

//   ref[0] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefXComboboxValue();
//   ref[1] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefLineStyleComboboxValue();
//   ref[2] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefSymbolStyleComboboxValue();
//   ref[3] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefSymbolSizeComboboxValue();
//   ref[4] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefLineColor();
//   ref[5] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefSymbolColor();
//   ref[6] = static_cast<QtPlot2dConfigDialog*>( getConfigDialog() )->getDrefUnitComboboxValue();
//   for (int i=0; i < 7; ++i) {
//     varXfer[i] = new XferDataItem( DataPool::newDataReference( *ref[i] ) );

//     XferDataItemIndex *inx = 0;
//     if( ( inx = varXfer[i]->getIndex( 0 ) ) == 0 ){
//       inx = varXfer[i]->newDataItemIndex();
//     }
//     inx->setIndex( varXfer[i]->Data(), i ? idx : idx-1 ); // !!! idx
//     varXfer[i]->setDimensionIndizes();
//     assert( varXfer[i] != 0 );
//   }
//   y1Xfer = initXfer( "y1axis", idx );
//   assert( y1Xfer != 0 );
//   y2Xfer = initXfer( "y2axis", idx );
//   assert( y2Xfer != 0 );
//   // colorXfer = initXfer( "color", idx );
//   // assert( colorXfer != 0 );
//   // symbolColorXfer = initXfer( "symbolColor", idx );
//   // assert( symbolColorXfer != 0 );
//   item->setAllXfers( varXfer[0], y1Xfer, y2Xfer, varXfer[4], varXfer[5], varXfer[1], varXfer[2], varXfer[3], varXfer[6] );
//   BUG_DEBUG("Set PlotItem Xfers: VM y1Xfer : " << y1Xfer->getFullName(true)
//             << ", xferColor: " << varXfer[3]->getFullName(true));
// }

/* --------------------------------------------------------------------------- */
/* drawHeaderText --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::drawHeaderText() {
  Gui2dPlot::drawHeaderText();
  char** text =  pheaderText();
  if( text != 0 ) {
    std::stringstream os;
    int size = 0;
    while(text[size] != 0) {
      os << pheaderText()[size] << std::endl;
      size++;
    }
    if (m_chart) m_chart->setTitle(QString::fromStdString(UnitManager::extractValue(os.str())).trimmed());
  }
}

/* --------------------------------------------------------------------------- */
/* drawFooterText --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::drawFooterText() {
  Gui2dPlot::drawFooterText();
  char** text =  pfooterText();
  if( text != 0 ) {
    std::stringstream os;
    if (!m_chart->title().isEmpty()){
      os << m_chart->title().toStdString() << " : ";
    }
    int size = 0;
    while(text[size] != 0) {
      os << UnitManager::extractValue(pfooterText()[size]) << std::endl;
      size++;
    }
    if (m_chart) m_chart->setTitle(QString::fromStdString(UnitManager::extractValue(os.str())).trimmed());
  }
}
/* --------------------------------------------------------------------------- */
/* zoomReset --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::zoomReset(){
  m_chart->zoomReset();
}

/* --------------------------------------------------------------------------- */
/* setLogX --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::setLogX( bool value ) {
  getAxis(2).setLogarithmic( value );
  getAxis(3).setLogarithmic( value );

  if( m_chart != 0 ){
    QAbstractAxis*  _xaxis = createAxis(0, getAxis(2));
    m_chart->addAxis(_xaxis, Qt::AlignBottom);
    for (auto series: m_chart->series()){
      if (series->detachAxis(m_xaxis)){
        series->attachAxis(_xaxis);
      }
    }
    m_chart->removeAxis(m_xaxis);
    m_xaxis = _xaxis;
    setAnnotationLabels();
    //setXAxisAnnotation(m_showAnnotationLabels);

    m_chart->update();
  }
}

/* --------------------------------------------------------------------------- */
/* setLogY --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::setLogY( bool value ) {
  getAxis(0).setLogarithmic( value );
  getAxis(1).setLogarithmic( value );

  if( m_chart != 0 ){
    QAbstractAxis*  _y1axis = createAxis(0, getAxis(0));
    QAbstractAxis*  _y2axis = createAxis(0, getAxis(1));
    m_chart->addAxis(_y1axis, Qt::AlignLeft);
    m_chart->addAxis(_y2axis, Qt::AlignRight);
    for (auto series: m_chart->series()){
      if (series->detachAxis(m_y1axis)){
        series->attachAxis(_y1axis);
      }
      if (series->detachAxis(m_y2axis)){
        series->attachAxis(_y2axis);
      }
    }
    m_chart->removeAxis(m_y1axis);
    m_y1axis = _y1axis;
    m_chart->removeAxis(m_y2axis);
    m_y2axis = _y2axis;

    m_chart->update();
  }
}

/* --------------------------------------------------------------------------- */
/* setOriginXAxis --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::setOriginXAxis( const double origin ) {
//   if (!getAxis(2).isScaleEnabled())
//     getAxis(2).setOrigin( origin );
//   if (!getAxis(3).isScaleEnabled())
//     getAxis(3).setOrigin( origin );
}

/* --------------------------------------------------------------------------- */
/* setOriginYAxis --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::setOriginYAxis( const double origin ) {
//   getAxis(0).setOrigin( origin );
//   getAxis(1).setOrigin( origin );
}

/* --------------------------------------------------------------------------- */
/* PrintType --                                                                */
/* --------------------------------------------------------------------------- */
HardCopyListener::FileFormat GuiQt2dPlot::getFileFormat() {
  return m_currentFileFormat;
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported --                                                    */
/* --------------------------------------------------------------------------- */
bool GuiQt2dPlot::isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ){
//   HardCopyListener::FileFormats2::iterator it;
  bool result = false;
//   for( it = m_supportedFileFormats.begin();
//        it != m_supportedFileFormats.end() && !result;
//        ++it ){
//     if( (*it).first == fileFormat )
//       result = true;
//   }
  return result;
}

// /* --------------------------------------------------------------------------- */
// /* isExportPrintFormat --                                                             */
// /* --------------------------------------------------------------------------- */
// bool GuiQt2dPlot::isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat ){
//   HardCopyListener::PrintType  ptype = getPrintType(fileFormat);
//   if (ptype==HardCopyListener::FILE_EXPORT ||
//       ptype==HardCopyListener::OWN_CONTROL   )
//     return true;
//   return false;
// }

// /* --------------------------------------------------------------------------- */
// /* getPrintType --                                                             */
// /* --------------------------------------------------------------------------- */
// HardCopyListener::PrintType GuiQt2dPlot::getPrintType( const HardCopyListener::FileFormat &fileFormat ){
//   HardCopyListener::FileFormats2::iterator it = m_supportedFileFormats.find( fileFormat );
//   if ( it == m_supportedFileFormats.end() ) {
//     return HardCopyListener::NOT_SUPPORTED;
//   } else
//     return (*it).second;

// }

/* --------------------------------------------------------------------------- */
/* write( InputChannelEvent &event ) --                                        */
/* --------------------------------------------------------------------------- */
bool GuiQt2dPlot::write( InputChannelEvent &event ) {
  BUG_DEBUG("write( InputChannelEvent &event )");

//   std::string msg;
//   GuiQtPrinterDialog& hardcopy = GuiQtPrinterDialog::Instance();
//   switch( hardcopy.FileFormat() ){
//   case HardCopyListener::Postscript :
//   case HardCopyListener::PNG :
//   case HardCopyListener::GIF :
//   case HardCopyListener::PDF :
//     m_currentFileFormat = HardCopyListener::Postscript;
//     break;
//   case HardCopyListener::JPEG :
//     m_currentFileFormat = HardCopyListener::JPEG;
//     break;
//   default :
//     msg = compose(_("%1: Selected file format is not supported"),getName() );
//     printMessage( msg, GuiElement::msg_Error );
//     //    return false;
//   }

//   if( m_plot == 0 && getMyForm()){
//     getMyForm()->getElement()->getQtElement()->create();
//   }
//   update(reason_Always);
//   std::string tmp_eps_name("/tmp/xxxx.eps");
//   if (AppData::Instance().disableFeatureSVG()) {
//     QTemporaryFile tmp_file( QString::fromStdString(compose("%1%2XXXXXX.eps", QDir::tempPath().toStdString(),QDir::separator().row())) );
//     if (tmp_file.open()){
//       tmp_eps_name =  tmp_file.fileName().toStdString();
//       tmp_file.remove();
//     }
//     QSize s(m_chart->sizeHint());
//     QPixmap pm(s.width(), s.height());
//     pm.fill();
//     m_chart->print( pm, true );
//     pm.save( QString::fromStdString(tmp_eps_name) );
//   } else {
//     generateFileWithSvgGenerator( tmp_eps_name, true );
//   }
//   QFile tmp_eps(QString::fromStdString(tmp_eps_name));

//   std::ostringstream os;
//   if (tmp_eps.open(QIODevice::ReadOnly))
//     os << tmp_eps.readAll().data() << std::flush;
//   event.write( os );

  return true;
}

// /* --------------------------------------------------------------------------- */
// /* generateFileWithSvgGenerator( const std::string &fileName ) --              */
// /* --------------------------------------------------------------------------- */

// bool  GuiQt2dPlot::generateFileWithSvgGenerator(std::string& outFilename, bool bPrinter) {
//   BUG_PARA(BugGui, "GuiQt2dPlot::generateFileWithSvgGenerator File: ", outFilename);

//   if (!m_plot) return false;

//   QSvgGenerator svg;
//   QTemporaryFile tmp_svg(QString::fromStdString(compose("%1%2XXXXXXXX.svg",  QDir::tempPath().toStdString(),QDir::separator().toLatin1())));
//   if (!tmp_svg.open())
//     return false;
//   ReportGen::Instance().newTmpFile( tmp_svg.fileName().toStdString() );

//   // set svg properties
//   svg.setFileName( tmp_svg.fileName() );
//   if (m_printSize.isEmpty()) {
//     if (!m_chart->size().isNull()) {
//       // At least 400 pixels big to prevent crowding of plot with text
//       svg.setSize(QSize(std::max(m_chart->size().width(), 400), std::max(m_chart->size().height(), 400)));
//     } else {
//       svg.setSize( m_chart->sizeHint());
//     }
//   } else {
//     svg.setSize( m_printSize );
//   }
//   BUG_MSG("Size width '"<<svg.size().width()<<"'  height '"<<svg.size().height()<<"'");

//   // render svg
//   m_chart->print( svg, bPrinter );

//   // only generating svg file => return
//   if ( FileUtilities::getSuffix(outFilename) == "svg") {
//     // weil das copy den bestehenden File nicht überschreibt, löschen wir ihn zuerst
//     QFile rf(QString::fromStdString(outFilename));
//     rf.remove();
//     return tmp_svg.copy( QString::fromStdString(outFilename) );
//   }

//   return processConvert(tmp_svg.fileName().toStdString(), outFilename);
// }

// /* --------------------------------------------------------------------------- */
// /* writeFile( const std::string &fileName ) -- via GuiElement                  */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::writeFile(QIODevice* ioDevice, const std::string &fileName, bool bPrinter ) {
//   bool widget_hide(!isShown());
//   if( m_plot == 0 && getMyForm()){
//     getMyForm()->getElement()->getQtElement()->create();
//   }
//   if (widget_hide) {
//     if (!myWidget()) { // !!! not used in a Form
//       create();
//     }
//     manage();
//     myWidget()->show();
//   }

//   update(reason_Always);

//   // only generating svg file => return
//   std::string suffix = FileUtilities::getSuffix(fileName);
//   if (suffix == "eps" || suffix == "svg" || suffix == "pdf") {  // vector graphic
//     std::string fn(fileName);
//     bool ret = generateFileWithSvgGenerator( fn, bPrinter );
//   } else {

//     QSize s(m_chart->sizeHint());
//     QPixmap pm(s.width(), s.height());
//     pm.fill();
//     m_chart->print( pm, bPrinter );
//     assert( !pm.isNull() );
//     if (ioDevice) {
//       pm.save(ioDevice, "png");
//     } else {
//       pm.save( QString::fromStdString(fileName) );
//     }
//   }

//   if (widget_hide) {
//     myWidget()->hide();
//   }

//   // finish job
//   if (!ioDevice) {
//     endFileStream( JobAction::job_Ok );
//   }
// }

/* --------------------------------------------------------------------------- */
/* write( const std::string &fileName ) --  via HardcopyListener               */
/* --------------------------------------------------------------------------- */
bool GuiQt2dPlot::write( const std::string &fileName ) {
  BUG_DEBUG("write filename: " << fileName);
//   std::string msg;
//   GuiQtPrinterDialog& hardcopy = GuiQtPrinterDialog::Instance();
//   switch( hardcopy.FileFormat() ){
//   case HardCopyListener::Postscript :
//   case HardCopyListener::PDF :
//     m_currentFileFormat = HardCopyListener::Postscript;
//     break;
//   case HardCopyListener::ASCII :
//     return true;
//     break;
//   default :
//     m_currentFileFormat = hardcopy.FileFormat();
//     msg = compose(_("%1: Selected file format is not supported"),getName() );
//     printMessage( msg, GuiElement::msg_Information );
//     //    return false;
//   }

//   if( m_plot == 0 && getMyForm()){
//     getMyForm()->getElement()->getQtElement()->create();
//   }
//   update(reason_Always);

//   // EPS is expected, we must make a SVG detour
//   std::string ext;
//   switch ( hardcopy.FileFormat()) {
//     case HardCopyListener::Postscript:
//     case HardCopyListener::PDF:
//     case HardCopyListener::SVG:
//       if (AppData::Instance().disableFeatureSVG() &&
// 	  hardcopy.FileFormat() == HardCopyListener::PDF) {
// 	ext = "PNG";
// 	break;
//       } else {
// 	std::string filename(fileName);
// 	generateFileWithSvgGenerator( filename, true );
// 	return true;
//       }
//     case HardCopyListener::JPEG:
//       ext = "JPG";
//       break;
//     case HardCopyListener::PNG:
//       ext = "PNG";
//       break;
//     case HardCopyListener::GIF:
//       ext = "GIF";
//       break;
//     case HardCopyListener::BMP:
//       ext = "BMP";
//       break;
//     case HardCopyListener::PPM:
//       ext = "PPM";
//       break;
//     case HardCopyListener::TIFF:
//       ext = "TIF";
//       break;
//     case HardCopyListener::XBM:
//       ext = "XBM";
//       break;
//     case HardCopyListener::XPM:
//       ext = "XPM";
//       break;
//   default:
//     // ignore 'HPGL', 'XML', 'JSON'...
//     ;
//   }
//   if (ext.size()) {
//     QSize s(m_chart->sizeHint());
//     QPixmap pm(s.width(), s.height());
//     pm.fill();
//     m_chart->print( pm, true );
//     assert( !pm.isNull() );
//     std::string fn = fileName;
//     std::string::size_type  npos = fn.rfind("."+lower(ext));
//     if (npos != (fn.size()-4))
//       fn +lower(ext);
//     pm.save( QString::fromStdString(fn) );
//     return true;
//   }
  return false;
}

/* --------------------------------------------------------------------------- */
/* saveFile --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQt2dPlot::saveFile( GuiElement *e ){
  GuiQtElement::saveFile(NULL);
  return true;
}

// /* --------------------------------------------------------------------------- */
// /* newPopupMenuDialogButton --                                                 */
// /* --------------------------------------------------------------------------- */

// GuiQtMenuButton* GuiQt2dPlot::newPopupMenuDialogButton( std::string label,
// 					   GuiMenuButtonListener* listener ){
//   GuiQtMenuButton *button =  new GuiQtMenuButton( m_popupMenu, listener );
//   m_popupMenu->attach( button );
//   button->setDialogLabel( label );
//   return button;
// }

// /* --------------------------------------------------------------------------- */
// /* newPopupMenuToggle --                                                       */
// /* --------------------------------------------------------------------------- */

// void GuiQt2dPlot::newPopupMenuToggle( std::string label, bool status,
// 				     GuiToggleListener* listener ){
//   GuiQtMenuToggle *button =  new GuiQtMenuToggle( m_popupMenu, listener );
//   m_popupMenu->attach( button );
//   button->setToggleStatus( status );
//   button->setDialogLabel( label );
// }

// /* --------------------------------------------------------------------------- */
// /* createUserInteractionModeMenu --                                            */
// /* --------------------------------------------------------------------------- */

// void GuiQt2dPlot::createUserInteractionModeMenu(  GuiQtPopupMenu *menu ) {
//   GuiQtPulldownMenu *uimMenu = new GuiQtPulldownMenu( menu, "User Interaction Mode" );
//   uimMenu->setLabel( _("UI Mode") );
//   menu->attach( uimMenu );
//   uimMenu->setAlways();
//   uimMenu->setTearOff( false );

//   GuiEventData *event = new GuiEventData();
//   GuiQtMenuToggle *toggle =
//     new GuiQtMenuToggle( uimMenu, &m_userInteractionMenu, event );
//   uimMenu->attach( toggle );
//   toggle->setToggleStatus( m_zoomer[0]->isEnabled() );
//   toggle->setLabel( _("Zoom") );
//   toggle->setAccelerator( "Ctrl<Key>S", "Ctrl-S" );  // S: Scale
//   m_userInteractionMenu.m_zoomButton = toggle;

//   event = new GuiEventData();
//   toggle = new GuiQtMenuToggle( uimMenu, &m_userInteractionMenu, event );
//   uimMenu->attach( toggle );
//   toggle->setToggleStatus( m_picker && m_picker->isEnabled() && !m_selectionRectMode );
//   toggle->setLabel( _("Select Point") );
//   toggle->setAccelerator( "Ctrl<Key>D", "Ctrl-D" );  // D: Dot
//   m_userInteractionMenu.m_selectPointButton = toggle;

//   if (getFunction()) {
//     event = new GuiEventData();
//     toggle = new GuiQtMenuToggle( uimMenu, &m_userInteractionMenu, event );
//     uimMenu->attach( toggle );
//     toggle->setToggleStatus( m_picker && m_picker->isEnabled() && m_selectionRectMode );
//     toggle->setLabel( _("Select Rectangle") );
//     toggle->setAccelerator( "Ctrl<Key>A", "Ctrl-A" );  // A: Area
//     m_userInteractionMenu.m_selectRectButton = toggle;
//   }
// }

/* --------------------------------------------------------------------------- */
/* popupMenu --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQt2dPlot::popupMenu(const QContextMenuEvent* event){
  auto popupMenu = getPopupMenu();
  if (popupMenu){
    popupMenu->popup();
  }
}

// /* --------------------------------------------------------------------------- */
// /* print --                                                                    */
// /* --------------------------------------------------------------------------- */

// void GuiQt2dPlot::print(QPrinter* qprinter) {
//   try {
//     if( m_plot == 0 && getMyForm()){
//       getMyForm()->getElement()->getQtElement()->create();
//     }
//     QPrinter printer(QPrinter::HighResolution);

//     // open own QPrintDialog
//     if (!qprinter) {
//       QString docName = m_chart->titleLabel()->text().text();
//       if ( docName.isEmpty() ) {
// 	  docName.replace (QRegularExpression(QString::fromLatin1 ("\n")), (" -- "));
// 	  printer.setDocName (docName);
// 	}

//       printer.setPageOrientation(QPageLayout::Landscape);

//       QPrintDialog dialog(&printer);
//       if (dialog.exec())
// 	qprinter = &printer;  // for post processing
//     }

//     // do post processing => print
//     if ( qprinter) {
//       m_chart->print(*qprinter, true);
//     }
//   } catch ( ... ) {
//     std::cerr << "  <<GuiQt2dPlot::print()>> Exception catched \n"<<std::flush;
//     GuiQtFactory::Instance()->showDialogWarning( NULL, "Exception catched", "unknown failure, maybe extern library crashed ..." );
//   }

// }

// /* --------------------------------------------------------------------------- */
// /* copy --                                                                     */
// /* --------------------------------------------------------------------------- */

void GuiQt2dPlot::copy() {
  QSize s(m_chartView->sizeHint());
  QPixmap pm(s.width(), s.height());
  pm.fill();
  QPainter painter;
  painter.begin(&pm);
  m_chartView->render(&painter);
      painter.end();
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setPixmap(pm);
  clipboard->setPixmap(pm, QClipboard::Selection);
}

// /* --------------------------------------------------------------------------- */
// /* openConfigDialog --                                                         */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::openConfigDialog() {
//   if (getName().size() == 0) // funktion in popupMenu nicht nutzbar
//     return;

//   bool cyclePlotMode( hasCyclePlotMode() );
//   QtPlot2dConfigDialog* configDialog = dynamic_cast<QtPlot2dConfigDialog*>(getConfigDialog());
//   tPlotItemIterator pit;

//   // if always created and plotitems has no wildcards nothing we do nothing
//   if (configDialog->myWidget()) {
// 	for( pit = m_plotItems.begin(); pit != m_plotItems.end(); ++pit){
// 	  if ((*pit)->hasAxisWildcardIndex()) {
// 		break; // WC found => dynamic (must rebuild) Dialog
// 	  }
// 	}

// 	// has cycle mode changed?
// 	if (!configDialog->setCycleMode( cyclePlotMode, m_showCycleVector)) {
// 	  // static Config Dialog, no layout changes
// 	  if (pit == m_plotItems.end()) {
// 		configDialogSetCurvePalette();
// 		getConfigDialog()->getForm()->getElement()->manage();
//  		getConfigDialog()->getForm()->getElement()->update( GuiElement::reason_Always );
// 		return;
// 	  }
// 	}
//   } else {
// 	configDialog->setCycleMode( cyclePlotMode, m_showCycleVector);
//   }

//   configDialog->recreateFieldgroup();
//   buildConfigDialog();
//   getConfigDialog()->replaceFieldgroup();
//   configDialogSetCurvePalette();
//   getConfigDialog()->getForm()->getElement()->manage();
//   getConfigDialog()->getForm()->getElement()->update( GuiElement::reason_Always );
// }


// /* --------------------------------------------------------------------------- */
// /* getConfigDialogItemStyle --                                                 */
// /* --------------------------------------------------------------------------- */
// ConfigDialog::eItemStyle GuiQt2dPlot::getConfigDialogItemStyle(PlotItem* plotItem) {
//   ConfigDialog::eItemStyle itemStyle(ConfigDialog::SHOW_NONE);
//   if  ( plotItem->getAxisType() == Gui2dPlot::Y1AXIS ||
// 		plotItem->getAxisType() == Gui2dPlot::Y2AXIS ) {
// 	if (plotItem->isMarker()) {
// 	  itemStyle = ConfigDialog::SHOW_AXIS_SYMBOL;
// 	} else {
// 	  GuiQwtPlotCurve::CurveStyleExt curveStyle = getQwtCurveStyle( plotItem->getAxisType() );
// 	  if (curveStyle == GuiQwtPlotCurve::Bar || curveStyle == GuiQwtPlotCurve::StackingBar) {
// 		itemStyle = ConfigDialog::SHOW_AXIS_LINE_COLOR;
// 	  } else {
// 		itemStyle = ConfigDialog::SHOW_ALL;
// 	  }
// 	}
//   }
//   return itemStyle;
// }

// /* --------------------------------------------------------------------------- */
// /* buildConfigDialog --                                                        */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::buildConfigDialog() {

//   // save last Fieldgroup and create a new one
//   QtPlot2dConfigDialog *configDialog = dynamic_cast<QtPlot2dConfigDialog*>(getConfigDialog());

//   tPlotItemIterator it;
//   std::vector<XferDataItem *> xfers;
//   int offset=1;
//   for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it, ++offset ) {
//     // maybe separator line
//     bool new_group(false);

//     if (configDialog->getStyle() == ConfigDialog::LIST &&
//         offset != 1 && (*it)->getAxisType() ==XAXIS) {
//       getConfigDialog()->addSeparator();
//       new_group = true;
//     }

//     // WildcardIndex loop, if no wildcards only one
//     int cnt = (*it)->hasAxisWildcardIndex() ? (*it)->getAxisWildcardIndexDimension() : 1;
//     for (int i=0; i < cnt; ++i) {
//       (*it)->setAxisWildcardIndexValue(i);  // TODO nur WC auf yAxis
//       int idx =  i + offset;
//       if ((*it)->hasAxisWildcardIndex() && i >= 0)
//         configureIndexedMenu( (*it), idx );

//       // config dialog line
//       std::vector<XferDataItem *> xfers;
//       std::string label;
//       (*it)->plotDataItem()->getLabelAndUnit( label );
//       XferDataItem *xXfer = 0, *y1Xfer = 0, *y2Xfer = 0;
//       XferDataItem *colorXfer = 0, *symbolColorXfer = 0,
//         *lineStyleXfer = 0, *symbolColorStyleXfer = 0, *symbolColorSizeXfer = 0, *unitXfer = 0;
//       (*it)->getAllXfers( xXfer, y1Xfer, y2Xfer,
//                           colorXfer, symbolColorXfer, lineStyleXfer, symbolColorStyleXfer, symbolColorSizeXfer, unitXfer);
//       BUG_DEBUG("buildConfigDialog VM y1Xfer : " << y1Xfer->getFullName(true)
//                 << ", xferColor: " << colorXfer->getFullName(true));
//       if (i == 0) {
//         xfers.push_back( y1Xfer );
//         xfers.push_back( y2Xfer );
//       }
//       // add config item
//       ConfigDialog::eItemStyle itemStyle = getConfigDialogItemStyle((*it));
//       unitXfer->setValue((*it)->plotDataItem()->Attr()->Unit(true));
//       bool done = getConfigDialog()->addItem(label, xfers, colorXfer, symbolColorXfer,
//                                              lineStyleXfer, symbolColorStyleXfer,
//                                              symbolColorSizeXfer, unitXfer,
//                                              itemStyle, new_group );
//       if (!(*it)->hasAxisWildcardIndex())
//         break;
//     }

//     xfers.clear();
//   }
//   getConfigDialog()->getForm()->getElement()->getQtElement()->create();
// }

// /* --------------------------------------------------------------------------- */
// /* configDialogSetCurvePalette --                                               */
// /* --------------------------------------------------------------------------- */

// void GuiQt2dPlot::configDialogSetCurvePalette() {
//   // set color columns
//   int offset =  1;
//   bool cyclePlotMode( hasCyclePlotMode() );
//   bool validSymbolColorExists(false);
//   bool validSymbolStyleExists(false);
//   bool validLineStyleExists(false);

//   for(tPlotItemIterator it = m_plotItems.begin(); it != m_plotItems.end(); ++it){

//     // WildcardIndex loop, if no wildcards only one
//     std::vector<int>::const_iterator showIt = m_showCycleVector.begin();
//     int cnt  = (*it)->hasAxisWildcardIndex() ? (*it)->getAxisWildcardIndexDimension() : 1;
//     for (int i=0; i < cnt; ++i, ++showIt) {
//       if (cyclePlotMode && *showIt != 1) continue;  // this cycle is not shown

//       int idx =  i + offset;
//       if ((*it)->hasAxisWildcardIndex() && i >= 0)
//         configureIndexedMenu( (*it), idx );
//       (*it)->setAxisWildcardIndexValue(i);  // TODO nur WC auf yAxis

//       XferDataItem *xXfer = 0, *y1Xfer = 0, *y2Xfer = 0, *colorXfer = 0, *symbolColorXfer = 0;
//       XferDataItem *lineStyleXfer = 0, *symbolStyleXfer = 0, *symbolSizeXfer = 0, *unitXfer = 0;
//       (*it)->getAllXfers(xXfer, y1Xfer, y2Xfer, colorXfer, symbolColorXfer, lineStyleXfer,
//                          symbolStyleXfer, symbolSizeXfer, unitXfer);

//       if( (*it)->getAxisType() ==XAXIS || (*it)->getAxisType() ==HIDDEN ) {
//         colorXfer->lockValue(); // Invalid
//         symbolColorXfer->lockValue(); // Invalid
//         lineStyleXfer->lockValue(); // Invalid
//         symbolStyleXfer->lockValue(); // Invalid
//         symbolSizeXfer->lockValue(); // Invalid
//         unitXfer->lockValue(); // Invalid
//         continue;
//       }

//       // check column visibility
//       ConfigDialog::eItemStyle itemStyle = getConfigDialogItemStyle((*it));
//       if (!validSymbolColorExists && itemStyle & ConfigDialog::SHOW_SYMBOL_COLOR) {
//         validSymbolColorExists = true;
//       }
//       if (!validLineStyleExists && itemStyle & ConfigDialog::SHOW_LINE_STYLE) {
//         validLineStyleExists = true;
//       }
//       if (!validSymbolStyleExists && itemStyle & ConfigDialog::SHOW_SYMBOL_STYLE) {
//         validSymbolStyleExists = true;
//       }

//       // cycle loop
//       int currentCycle = dpi().currentCycle();
//       int numCycles = cyclePlotMode ? dpi().numCycles() : 1;
//       DataVector lineColor, symbolColor, lineStyle, symbolStyle, symbolSize, unit;

//       for( int cycle = 0; cycle < numCycles; ++cycle ) {
//         GuiPlotDataItem *yItem = (*it)->plotDataItem();
//         if (cyclePlotMode)
//           dpi().goCycle( cycle, false ); // do not clear undo stack
//         Plot2dCurvePalette* curvePalette = m_curveAttrs.getPalette(yItem->getUniqueName((*it)->xPlotItem() ? (*it)->xPlotItem()->getAxisWildcardIndexValue() : 0,
//                                                                                        (*it)->getAxisWildcardIndexValue(),
//                                                                                        cyclePlotMode));
//         if (cyclePlotMode)
//           dpi().goCycle( currentCycle, false ); // do not clear undo stack

//         // line color
//         QString s = QString::fromStdString(curvePalette->lineColor);
//         if (!cyclePlotMode) {
//           colorXfer->setValue( QColor(s).name(QColor::HexArgb).toStdString() );
//         } else {
//           DataValue::Ptr ptr = new DataStringValue( QColor(s).name(QColor::HexArgb).toStdString() );
//           lineColor.appendValue( ptr );
//         }

//         // line style
//         Qt::PenStyle penStyle = (Qt::PenStyle) curvePalette->lineStyle;
//         if (!cyclePlotMode) {
//           lineStyleXfer->setValue( penStyle );
//         } else {
//           DataValue::Ptr ptr = new DataIntegerValue( (int) penStyle );
//           lineStyle.appendValue( ptr );
//         }

//         // symbol color
//         QString ss = QString::fromStdString(curvePalette->symbolColor);
//         if (!cyclePlotMode) {
//           symbolColorXfer->setValue( QColor(ss).name(QColor::HexArgb).toStdString());
//         } else {
//           DataValue::Ptr ptr = new DataStringValue( QColor(ss).name(QColor::HexArgb).toStdString() );
//           symbolColor.appendValue( ptr );
//         }

//         // symbol style
//         QwtSymbol::Style sym = (QwtSymbol::Style) curvePalette->symbolStyle;
//         if (!cyclePlotMode) {
//           symbolStyleXfer->setValue( sym );
//         } else {
//           DataValue::Ptr ptr = new DataIntegerValue( (int) sym );
//           symbolStyle.appendValue( ptr );
//         }

//         // symbol size
//         int size = curvePalette->symbolSize;
//         if (!cyclePlotMode) {
//           symbolSizeXfer->setValue( size );
//         } else {
//           DataValue::Ptr ptr = new DataIntegerValue( size );
//           symbolSize.appendValue( ptr );
//         }

//         // unit
//         std::string unitValue = yItem->Attr()->Unit(true);
//         if (!cyclePlotMode) {
//           unitXfer->setValue( unitValue );
//         } else {
//           DataValue::Ptr ptr = new DataStringValue( unitValue );
//           unit.appendValue( ptr );
//         }
//       }

//       // set line, symbol color dataVectors
//       if (cyclePlotMode) {
//         int id[2] = { idx, -1};
//         configureCycleXfer(colorXfer, 1);
//         configureCycleXfer(lineStyleXfer, 1);
//         configureCycleXfer(symbolColorXfer, 1);
//         configureCycleXfer(symbolStyleXfer, 1);
//         configureCycleXfer(symbolSizeXfer, 1);
//         configureCycleXfer(unitXfer, 1);

//         BUG_DEBUG("setDataVector LineColorVN: " << colorXfer->getFullName(true) << " idx: " << idx);
//         colorXfer->Data()->setDataVector( lineColor, 2, id );
//         lineStyleXfer->Data()->setDataVector( lineStyle, 2, id );
//         symbolColorXfer->Data()->setDataVector( symbolColor, 2, id );
//         symbolStyleXfer->Data()->setDataVector( symbolStyle, 2, id );
//         symbolSizeXfer->Data()->setDataVector( symbolSize, 2, id );
//         unitXfer->Data()->setDataVector( unit, 2, id );
//       }
//     }
//     ++offset;
//   }

//   // hide symbol color column
//   getConfigDialog()->showSymbolColorColumn(validSymbolColorExists);
//   getConfigDialog()->showSymbolColorColumn(validLineStyleExists,1);
//   getConfigDialog()->showSymbolColorColumn(validSymbolStyleExists,2);
// }

// /* --------------------------------------------------------------------------- */
// /* configDialogGetCurvePalette --                                               */
// /* --------------------------------------------------------------------------- */
// bool  GuiQt2dPlot::configDialogGetCurvePalette() {
//   bool changes(false);
//   int offset =  1;
//   bool cyclePlotMode( hasCyclePlotMode() );

//   // replace changed colors
//   for(tPlotItemIterator it = m_plotItems.begin(); it != m_plotItems.end(); ++it ){

//     // WildcardIndex loop, if no wildcards only one
//     std::vector<int>::const_iterator showIt = m_showCycleVector.begin();
//     int cnt  = (*it)->hasAxisWildcardIndex() ? (*it)->getAxisWildcardIndexDimension() : 1;
//     for (int i=0; i < cnt; ++i, ++showIt) {
//       int idx =  i + offset;
//       if( (*it)->getAxisType() ==XAXIS || (*it)->getAxisType() ==HIDDEN ) {
//         continue;
//       }
//       if (cyclePlotMode && *showIt != 1) continue;  // this cycle is not shown

//       if ((*it)->hasAxisWildcardIndex() && i >= 0)
//         configureIndexedMenu( (*it), i + offset );
//       (*it)->setAxisWildcardIndexValue(i);  // TODO nur WC auf yAxis

//       XferDataItem *xXfer = 0, *y1Xfer = 0, *y2Xfer = 0, *colorXfer = 0, *symbolColorXfer = 0;
//       XferDataItem *lineStyleXfer = 0, *symbolStyleXfer = 0, *symbolSizeXfer = 0, *unitXfer = 0;
//       (*it)->getAllXfers(xXfer, y1Xfer, y2Xfer, colorXfer, symbolColorXfer,
//                          lineStyleXfer, symbolStyleXfer, symbolSizeXfer, unitXfer);

//       // get new color
//       std::string newColorStr, newSymbolColorStr, newUnit;
//       int newSymbol(QwtSymbol::NoSymbol);
//       int newSize(6);
//       int newLineStyle(Qt::SolidLine);
//       DataVector lineColor, symbolColor, lineStyle, symbolStyle, symbolSize, unit;
//       if (cyclePlotMode) {
//         int id[2] = { idx, -1};

//         configureCycleXfer(colorXfer, 1);
//         configureCycleXfer(lineStyleXfer, 1);
//         configureCycleXfer(symbolColorXfer, 1);
//         configureCycleXfer(symbolStyleXfer, 1);
//         configureCycleXfer(symbolSizeXfer, 1);
//         configureCycleXfer(unitXfer, 1);

//         //BUG_DEBUG("getDataVector LineColor  VN: " << colorXfer->getFullName(true) << " idx: " << idx);
//         colorXfer->Data()->getDataVector( lineColor, 2, id );
//         lineStyleXfer->Data()->getDataVector( lineStyle, 2, id );
//         symbolColorXfer->Data()->getDataVector( symbolColor, 2, id );
//         symbolStyleXfer->Data()->getDataVector( symbolStyle, 2, id );
//         symbolSizeXfer->Data()->getDataVector( symbolSize, 2, id );
//         unitXfer->Data()->getDataVector( unit, 2, id );
//       } else {
//         colorXfer->getValue(newColorStr);
//         lineStyleXfer->getValue(newLineStyle);
//         symbolColorXfer->getValue(newSymbolColorStr);
//         symbolStyleXfer->getValue(newSymbol);
//         symbolSizeXfer->getValue(newSize);
//         unitXfer->getValue(newUnit);
//         getConfigDialog()->getForm()->getElement()->update(reason_Unit);
//       }

//       // cycle loop
//       int currentCycle = dpi().currentCycle();
//       int numCycles = cyclePlotMode ? dpi().numCycles() : 1;

//       for( int cycle = 0; cycle < numCycles; ++cycle ) {
//         if (!(*it)->xPlotItem()) continue;

//         if (cyclePlotMode) {
//           if (lineColor.getValue(cycle).is_valid())
//             lineColor.getValue(cycle)->getValue(newColorStr);
//           if (lineStyle.getValue(cycle).is_valid())
//             lineStyle.getValue(cycle)->getValue(newLineStyle);
//           if (symbolColor.getValue(cycle).is_valid())
//             symbolColor.getValue(cycle)->getValue(newSymbolColorStr);
//           if (symbolStyle.getValue(cycle).is_valid())
//             symbolStyle.getValue(cycle)->getValue(newSymbol);
//           if (symbolSize.getValue(cycle).is_valid())
//             symbolSize.getValue(cycle)->getValue(newSize);
//         }

//         GuiPlotDataItem *yItem = (*it)->plotDataItem();

//         // get old color
//         if (cyclePlotMode)
//           dpi().goCycle( cycle, false ); // do not clear undo stack
//         std::string curveName = yItem->getUniqueName((*it)->xPlotItem() ? (*it)->xPlotItem()->getAxisWildcardIndexValue() : 0,
//                                                      (*it)->getAxisWildcardIndexValue(),
//                                                      cyclePlotMode);
//         Plot2dCurvePalette* curvePalette = m_curveAttrs.getPalette(curveName);

//         if (cyclePlotMode)
//           dpi().goCycle( currentCycle, false ); // do not clear undo stack

//         // replace line color
//         int pIdx = curvePalette->index;
//         if (pIdx < 0) {
//           BUG_ERROR("Color not found inside color list.");
//           continue; // not found, should not be happen
//         }
//         if (newColorStr.size() > 0 && curvePalette->lineColor != newColorStr) {
//           BUG_DEBUG("Set new lineColor: " << newColorStr << " ("
//                     << curvePalette->lineColor << ")  CurveName: " << curveName
//                     << "  Idx: " << pIdx << "  VN : "<< colorXfer->getFullName(true));
//           curvePalette->lineColor = newColorStr;
//           m_curveAttrs.m_lineColors[pIdx % m_curveAttrs.m_lineColors.size()] = newColorStr;
//         }
//         // replace symbol color
//         if (newSymbolColorStr.size() > 0 && curvePalette->symbolColor != newSymbolColorStr) {
//           BUG_DEBUG("Set new symbolColor: " << newSymbolColorStr << " ("
//                     << curvePalette->symbolColor <<")  CurveName: " << curveName
//                     << "  Idx: " << pIdx << "  VN : "<< colorXfer->getFullName(true));
//           curvePalette->symbolColor = newSymbolColorStr;
//           m_curveAttrs.m_symbolColors[pIdx % m_curveAttrs.m_symbolColors.size()] = newSymbolColorStr;
//         }

//         // replace line style
//         if (curvePalette->lineStyle != newLineStyle) {
//           BUG_DEBUG("Set new lineStyle: " << newLineStyle << " (" << curvePalette->lineStyle << ")  CurveName: " << curveName);
//           curvePalette->lineStyle = (Qt::PenStyle) newLineStyle;
//           m_curveAttrs.m_lineStyles[pIdx % m_curveAttrs.m_lineStyles.size()] = (Qt::PenStyle) newLineStyle;
//         }

//         // replace symbol style
//         if (curvePalette->symbolStyle != newSymbol) {
//           BUG_DEBUG("Set new symbolStyle: " << newSymbol << " (" << curvePalette->symbolStyle << ")  CurveName: " << curveName);
//           curvePalette->symbolStyle = (QwtSymbol::Style) newSymbol;
//           m_curveAttrs.m_curveSymbols[pIdx % m_curveAttrs.m_curveSymbols.size()] = (QwtSymbol::Style) newSymbol;
//         }

//         // replace symbol size
//         if (curvePalette->symbolSize != newSize) {
//           BUG_DEBUG("Set new symbolSize: " << newSymbol << " (" << curvePalette->symbolSize << ")  CurveName: " << curveName);
//           curvePalette->symbolSize = newSize;
//           m_curveAttrs.m_curveSymbolsSize[pIdx % m_curveAttrs.m_curveSymbolsSize.size()] = newSize;
//         }
//         changes = true;

//       }
//     }
//     ++offset;
//   }
//   return changes;
// }

/* --------------------------------------------------------------------------- */
/* setConfigDialogStyle --                                                     */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::setConfigDialogStyle( ConfigDialog::eStyle style, int length ) {
//   assert( getConfigDialog() != 0 );
//   getConfigDialog()->setStyle( style, length );
}

// /* --------------------------------------------------------------------------- */
// /* configDialogResetEvent --                                                   */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::configDialogResetEvent() {
//   tPlotItemIterator it;
//   for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it )
//     (*it)->resetAxisTypeValues();
//   readSettings();
//   m_curveAttrs.reset();
//   configDialogSetCurvePalette();
//   getConfigDialog()->getForm()->getElement()->update( reason_Always );
//   update( reason_FieldInput );
// }

/* --------------------------------------------------------------------------- */
/* getSelectionPoints --                                                       */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::getSelectionPoints(tPointVector& pts,
				    std::vector<int>& axisType, std::vector<std::string>& titles) {
//   if (m_picker)
//     m_picker->getSelectionPoints(pts, axisType, titles);
}


// /* --------------------------------------------------------------------------- */
// /* hasFunction --                                                              */
// /* --------------------------------------------------------------------------- */
// bool GuiQt2dPlot::hasFunction() {
//   return getFunction() != 0;
// }

// /* --------------------------------------------------------------------------- */
// /* openScaleDialog --                                                          */
// /* --------------------------------------------------------------------------- */

// void GuiQt2dPlot::openScaleDialog() {
//   if (getName().size() == 0) // funktion in popupMenu nicht nutzbar
//     return;
//   if( getScaleDialog() == 0 )
//     buildScaleDialog();
//   else {
//     // update xfer for scale dialog
//     int qwtAxisType[AXIS_TYPE_COUNT - 1] = {2, 0, 1};
//     PlotAxis *axis[AXIS_TYPE_COUNT - 1] = { &getAxis(2), &(getAxis(0)), &(getAxis(1)) };
//     for( int y = 0; y < AXIS_TYPE_COUNT-1; ++y ){
//       XferDataItem *xferScaleMin = axis[y]->getXferScaleMin();
//       XferDataItem *xferScaleMax = axis[y]->getXferScaleMax();
//       XferDataItem *xferScaleEnable = axis[y]->getXferScaleEnable();
//       XferDataItem *xferAspectRatio = axis[y]->getXferAspectRatio();
//       // get axis interval
//       QwtInterval val;
//       if (!getAxis(qwtAxisType[y]).isLogarithmic()) {
//         val = m_chart->axisInterval(qwtAxisType[y]);
//       } else {
//         // logarithmic scale
//         // - Zero Value are ignored
//         // - min Value: min Value of logarithmic scale
//         // - max Value: max Value of none logarithmic scale
//         QwtInterval val0 = m_chart->axisInterval(qwtAxisType[y]);
//         val = m_chart->axisInterval(qwtAxisType[y]);
//         if (y == 0)
//           setLogX(false);
//         else
//           setLogY(false);
//         m_chart->setAxisAutoScale( AXIS_X_BOTTOM );
//         val = m_chart->axisInterval(qwtAxisType[y]);
//         if (y == 0)
//           setLogX(true);
//         else
//           setLogY(true);
//         val.setMinValue(val0.minValue());
//       }
//       xferScaleMin->setValue( val.minValue() );
//       xferScaleMax->setValue( val.maxValue() );
//       xferScaleEnable->setValue( axis[y]->isScaleEnabled() );
//       xferAspectRatio->setValue( m_chart->getPlotRescaler()->aspectRatio(qwtAxisType[y]) );
//       axis[y]->setAllXferScale(xferScaleMin, xferScaleMax, xferScaleEnable, xferAspectRatio);
//     }
//   }
//   getScaleDialog()->getForm()->getElement()->manage();
// }

// /* --------------------------------------------------------------------------- */
// /* buildScaleDialog --                                                         */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::buildScaleDialog() {
//   int status = 0;

//   GuiEventData *event = new GuiEventData();
//   ScaleDialog *dialog = new QtScaleDialog( this, this, "Scale Plot", "Axis", event );
//   setScaleDialog( dialog );
//   std::string name = m_isCloned ? compose("%1_%2", getName(), this) : getName();
//   dialog->initialize( name, initXfer( "aspectRatioType", 0 ) );

//   std::string axisString[AXIS_TYPE_COUNT - 1] = { "Xaxis", "Y1Axis", "Y2Axis" };
//   int qwtAxisType[AXIS_TYPE_COUNT - 1] = {2, 0, 1};
//   PlotAxis *axis[AXIS_TYPE_COUNT - 1] = { &getAxis(2), &(getAxis(0)), &(getAxis(1)) };
//   for( int n = 0; n < AXIS_TYPE_COUNT - 1; ++n ){
//     XferDataItem *xferScaleMin = axis[n]->getXferScaleMin();
//     Scale *min_scale = axis[n]->getMinScale();
//     XferDataItem *xferScaleMax = axis[n]->getXferScaleMax();
//     Scale *max_scale = axis[n]->getMaxScale();
//     XferDataItem *xferScaleEnable = 0;
//     XferDataItem* xferAspectRatio = axis[n]->getXferAspectRatio();
//     QwtInterval val = m_chart->axisInterval(qwtAxisType[n]);

//     if( !xferScaleMin ){
//       xferScaleMin = initXfer( "min", n );
//     }
//     assert( xferScaleMin != 0 );
//     xferScaleMin->setValue( val.minValue() );

//     if( !xferScaleMax ){
//       xferScaleMax = initXfer( "max", n );
//     }
//     assert( xferScaleMax != 0 );
//     xferScaleMax->setValue( val.maxValue() );

//     xferScaleEnable = initXfer( "scaleEnable", n );
//     assert( xferScaleEnable != 0 );
//     xferScaleEnable->setValue( axis[n]->isScaleEnabled() );

//     if( !xferAspectRatio ){
//       xferAspectRatio = initXfer( "aspectRatio", n );
//     }
//     assert( xferAspectRatio != 0 );
//     xferAspectRatio->setValue( m_chart->getPlotRescaler()->aspectRatio(qwtAxisType[n]) );

//     axis[n]->setAllXferScale( xferScaleMin, xferScaleMax, xferScaleEnable, xferAspectRatio);
//     dialog->addItem( axisString[n], xferScaleMin, min_scale, xferScaleMax, max_scale, xferScaleEnable, xferAspectRatio );
//   }
//   dialog->getElement()->getQtElement()->create();
// }

// /* --------------------------------------------------------------------------- */
// /* openCyclesDialog --                                                         */
// /* --------------------------------------------------------------------------- */

// void GuiQt2dPlot::openCyclesDialog() {
//   if (getName().size() == 0) // funktion in popupMenu nicht nutzbar
//     return;
//   std::vector<int> intVector;
//   int value = getAllCycles();

//   if( getCyclesDialog() != 0 ){
//     getCyclesDialog()->getValues( intVector, value );
//     delete getCyclesDialog();
//     setCyclesDialog(0);
//   }

//   buildCyclesDialog();
//   getCyclesDialog()->setValues( intVector, value );
//   getCyclesDialog()->getElement()->manage();
// }

/* --------------------------------------------------------------------------- */
/* buildCyclesDialog --                                                        */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::buildCyclesDialog() {
//   double value;
//   int status = 0;

//   GuiEventData *event = new GuiEventData();
//   QtCyclesDialog*  _cyclesDialog = new QtCyclesDialog( this, this, _("CaseDialog"), event );
//   setCyclesDialog( _cyclesDialog );
//   std::string name = m_isCloned ? compose("%1_%2", getName(), this) : getName();
//   _cyclesDialog->initialize( name );

//   int numCycles = dpi().numCycles();
//   int currentCycle = dpi().currentCycle();
//   for( int i = 0; i < numCycles; ++i ){
//     if( i != currentCycle ){
//       std::ostringstream os;
//       XferDataItem *xfer = initXfer( "showCycle", i );
//       xfer->setValue( 0 );
//       os << _("Case") << " " << i+1;
//       std::string cycleName;
//       dpi().getCycleName( i, cycleName );
//       if( cycleName.empty() )
// 	cycleName = "<no name>";
//       _cyclesDialog->addItem( os.str(),cycleName, xfer );
//     }
//   }
//   _cyclesDialog->create( );
}

/* --------------------------------------------------------------------------- */
/* clearPlots --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::clearPlots(bool always) {
  TransactionNumber  trans = GuiQtManager::Instance().LastGuiUpdate();
  tPlotVector rem_plot;
  BUG_DEBUG("ClearPlots m_plot["<<m_plots.size()<<"] m_plotItems["<<m_plotItems.size()<<"]");
  for(auto plot: m_plots) {
    bool remove(false);

    // delete unused plotItems
    for(auto plotItem: m_plotItems) {
      if( plot->cycle() < dpi().numCycles() &&
          plotItem && plotItem->xPlotDataItem() &&
          !plotItem->isDataItemUpdated(trans) && !always &&
          !plotItem->xPlotDataItem()->isDataItemUpdated(trans)) {
        continue;
      }
      QLineSeries *lineCurve = reinterpret_cast<QLineSeries*>(plotItem->getPlotCurve(plot->cycle(), plot->xIndex(), plot->yIndex()));

      BUG_DEBUG("ClearPlots lineCurve["<<lineCurve<<"]  getAxisType["<<plotItem->getAxisType()<<"]");
        if (lineCurve) {
          // remove legend entry
          // m_picker->clearSelection( dynamic_cast<QwtPlotCurve*>(plotItem));
          m_legendVis[ lineCurve->name() ] = lineCurve->isVisible();
          // plotItem->setItemAttribute(QwtPlotItem::Legend, false);
          // m_chart->updateLegend(plotItem);
          if (m_lastSelectedCurve == lineCurve) {
            m_lastSelectedCurve = 0;
          }
          delete lineCurve;
        }

      // clear entry
      remove=true;
      plotItem->setPlotCurve(plot->cycle(), plot->xIndex(), plot->yIndex(), 0);
      if (lineCurve && m_chart->series().contains(lineCurve))
        lineCurve->hide(); // m_chart->removeSeries(lineCurve);
    }

    if (remove || always || plot->cycle() >= dpi().numCycles() ) {
      rem_plot.push_back(plot);
    }
  }

  // delete unused plots
  for (auto plot: rem_plot) {
    m_plots.erase( std::find(m_plots.begin(), m_plots.end(), plot));
    delete plot;
  }
  BUG_DEBUG("ClearPlots Done remove: [" << rem_plot.size() << "] => m_plots["<<m_plots.size()<<"]");
}
// /* --------------------------------------------------------------------------- */
// /* writeSettings --                                                            */
// /* --------------------------------------------------------------------------- */

// void GuiQt2dPlot::writeSettings() {
//   if( !m_plot )
//     return;
//   if (!m_configButton)
//     return;

//   QSettings *settings = GuiQtManager::Settings();

//   // SettingPlot2DLevel
//   //    0 => Weder Applikations- noch User-Settings werden verwendet
//   //    1 => (Nur) Applikations-Settings werden verwendet
//   // >= 2 => (Default) Alle Settings werden geschrieben und nach einem Restart wieder eingelesen
//   int dlevel = settings->value("Intens/SettingPlot2DLevel", std::numeric_limits<int>::max()).toInt();
//   if (dlevel < 2)
//     return;

//   // write settings colors
//   settings->beginGroup( QString::fromStdString("Plot2d") );
//   settings->setValue("drawXAxisPlotItemTitles", drawXAxisPlotItemTitles);

//   QString _tmp = QString::fromStdString(getName().size() ? getName() : "GuiQt2dPlot");
//   settings->setValue(_tmp + ".background", m_background);
//   settings->setValue(_tmp + ".lineWidth", m_lineWidth);
//   settings->setValue(_tmp + ".maxMajorTicks",  m_majorTicks);
//   settings->setValue(_tmp + ".maxMinorTicks",  m_minorTicks);
//   settings->setValue(_tmp + ".annotationAngle",  m_annoAngle );
//   settings->setValue(_tmp + ".barOrientation", m_bcSettings.orientation);
//   settings->setValue(_tmp + ".barLayoutPolicy", m_bcSettings.layoutPolicy);
//   settings->setValue(_tmp + ".barLayoutHint", m_bcSettings.layoutHint);
//   settings->setValue(_tmp + ".barDataTipFlag", m_bcSettings.dataTipAlignment);
//   settings->setValue(_tmp + ".barDataTipLineStyleFlag", m_bcSettings.dataTipLineStyle);
//   settings->setValue(_tmp + ".barDataTipBackgroundColor", QString::fromStdString(m_bcSettings.dataTipBackgroundColor));
//   settings->setValue(_tmp + ".barDataTipBorderColor", QString::fromStdString(m_bcSettings.dataTipBorderColor));
//   settings->setValue(_tmp + ".barDataTipMinimumThreshold", m_bcSettings.dataTipMinThreshold);

//   if ( m_curveAttrs.m_lineColors.size() ) {
//     QStringList sl;
//     fromStdVector(m_curveAttrs.m_lineColors, sl);
//     settings->setValue(_tmp + ".colorList", sl);
//   }

//   // write settings symbol
//   if ( m_curveAttrs.m_curveSymbols.size() ) {
//     std::ostringstream os;
//     std::vector<int>::iterator symbolIterator;
//     for (symbolIterator = m_curveAttrs.m_curveSymbols.begin();
//          symbolIterator != m_curveAttrs.m_curveSymbols.end(); ++symbolIterator) {
//       if (os.str().size())
//         os << ";";
//       os <<  *symbolIterator;
//     }
//     settings->setValue(_tmp + ".symbolList", QString::fromStdString( os.str() ).split(";"));
//   }

//   // write settings symbol size
//   if ( m_curveAttrs.m_curveSymbolsSize.size() ) {
//     std::ostringstream os;
//     std::vector<int>::iterator symbolIterator;
//     for (symbolIterator = m_curveAttrs.m_curveSymbolsSize.begin();
//          symbolIterator != m_curveAttrs.m_curveSymbolsSize.end(); ++symbolIterator) {
//       if (os.str().size())
//         os << ";";
//       os <<  *symbolIterator;
//     }
//     settings->setValue(_tmp + ".symbolSizeList", QString::fromStdString( os.str() ).split(";"));
//   }

//   // write settings pen width
//   if ( m_curveAttrs.m_lineWidths.size() ) {
//     std::ostringstream os;
//     std::vector<int>::iterator styleIterator;
//     for (styleIterator = m_curveAttrs.m_lineWidths.begin();
//          styleIterator != m_curveAttrs.m_lineWidths.end(); ++styleIterator) {
//       if (os.str().size())
//         os << ";";
//       os <<  *styleIterator;
//     }
//     settings->setValue(_tmp + ".lineWidthList", QString::fromStdString( os.str() ).split(";"));
//   }

//   // write settings pen style
//   if ( m_curveAttrs.m_lineStyles.size() ) {
//     std::ostringstream os;
//     std::vector<int>::iterator styleIterator;
//     for (styleIterator = m_curveAttrs.m_lineStyles.begin();
//          styleIterator != m_curveAttrs.m_lineStyles.end(); ++styleIterator) {
//       if (os.str().size())
//         os << ";";
//       os <<  *styleIterator;
//     }
//     settings->setValue(_tmp + ".penStyleList", QString::fromStdString( os.str() ).split(";"));
//   }

//   // write settings symbol color
//   if ( m_curveAttrs.m_symbolColors.size() ) {
//     QStringList sl;
//     fromStdVector(m_curveAttrs.m_symbolColors, sl);
//     settings->setValue(_tmp + ".symbolColorList", sl);
//   }

//   // write settings symbol pen color
//   if ( m_curveAttrs.m_symbolPenColors.size() ) {
//     QStringList sl;
//     fromStdVector(m_curveAttrs.m_symbolPenColors, sl);
//     settings->setValue(_tmp + ".symbolPenColorList", sl);
//   }

//   settings->endGroup();
// }

// /* --------------------------------------------------------------------------- */
// /* readSettings --                                                             */
// /* --------------------------------------------------------------------------- */

// void GuiQt2dPlot::readSettings()
// {
//   QSettings *settings = GuiQtManager::Settings();

//   // SettingPlot2DLevel
//   //    0 => Weder Applikations- noch User-Settings werden verwendet
//   //    1 => (Nur) Applikations-Settings werden verwendet
//   // >= 2 => (Default) Alle Settings werden geschrieben und nach einem Restart wieder eingelesen
//   int dlevel = settings->value("Intens/SettingPlot2DLevel", std::numeric_limits<int>::max()).toInt();
//   if (dlevel < 1)
//     return;

//   settings->beginGroup( QString::fromStdString("Plot2d") );
//   drawXAxisPlotItemTitles = settings->value("drawXAxisPlotItemTitles",
// 					     drawXAxisPlotItemTitles).toBool();
//   QStringList slist;
//   QString _tmp = QString::fromStdString(getName().size() ? getName() : "GuiQt2dPlot");

//   m_background = settings->value(_tmp + ".background", m_background ).toString();
//   m_lineWidth  = settings->value(_tmp + ".lineWidth", m_lineWidth ).toInt();
//   m_majorTicks = settings->value(_tmp + ".maxMajorTicks",  m_majorTicks).toInt();
//   m_minorTicks = settings->value(_tmp + ".maxMinorTicks",  m_minorTicks).toInt();
//   m_annoAngle = settings->value(_tmp + ".annotationAngle", m_annoAngle).toInt();

//   // barplot Attributes
//   m_bcSettings.orientation =  settings->value(_tmp + ".barOrientation", m_bcSettings.orientation).toInt();
//   m_bcSettings.layoutPolicy = settings->value(_tmp + ".barLayoutPolicy", m_bcSettings.layoutPolicy).toInt();
//   m_bcSettings.layoutHint = settings->value(_tmp + ".barLayoutHint", m_bcSettings.layoutHint).toDouble();
//   m_bcSettings.dataTipAlignment = settings->value(_tmp + ".barDataTipFlag", m_bcSettings.dataTipAlignment).toInt();
//   m_bcSettings.dataTipLineStyle = settings->value(_tmp + ".barDataTipLineStyleFlag", m_bcSettings.dataTipLineStyle).toInt();
//   m_bcSettings.dataTipBackgroundColor = settings->value(_tmp + ".barDataTipBackgroundColor",
// 						    QString::fromStdString(m_bcSettings.dataTipBackgroundColor)).toString().toStdString();
//   m_bcSettings.dataTipBorderColor = settings->value(_tmp + ".barDataTipBorderColor",
// 						    QString::fromStdString(m_bcSettings.dataTipBorderColor)).toString().toStdString();
//   m_bcSettings.dataTipMinThreshold = settings->value(_tmp + ".barDataTipMinimumThreshold", m_bcSettings.dataTipMinThreshold).toDouble();

//   // printSize
//   m_printSize = settings->value(_tmp + ".printSize", QSize() ).toSize();

//   // read settings colors
//   QStringList sl;
//   fromStdVector(m_curveAttrs.m_lineColors, sl);

//   slist = settings->value(_tmp + ".colorList", sl).toStringList();
//   if (slist.size()) {
//     std::vector<std::string> vec;
//     toStdVector(slist, vec);
//     m_curveAttrs.m_lineColors = vec;
//   }
//   //std::cerr << _tmp.toStdString() <<".colorList: " << slist.join(" ").toStdString()<<std::endl;

//   // read settings symbol
//   slist = settings->value(_tmp + ".symbolList", QStringList() ).toStringList();
//   if (slist.size()) {
//     m_curveAttrs.m_curveSymbols.clear();
//     QStringList::iterator it= slist.begin();
//     for (; it != slist.end(); ++it)
//       m_curveAttrs.m_curveSymbols.push_back( (QwtSymbol::Style) (*it).toInt() );
//   }

//   // read settings symbol size
//   slist = settings->value(_tmp + ".symbolSizeList", QStringList() ).toStringList();
//   if (slist.size()) {
//     m_curveAttrs.m_curveSymbolsSize.clear();
//     QStringList::iterator it= slist.begin();
//     for (; it != slist.end(); ++it)
//       m_curveAttrs.m_curveSymbolsSize.push_back( (*it).toInt() );
//   }

//   // read settings line width
//   slist = settings->value(_tmp + ".lineWidthList", QStringList() ).toStringList();
//   if (slist.size()) {
//     m_curveAttrs.m_lineWidths.clear();
//     QStringList::iterator it= slist.begin();
//     for (; it != slist.end(); ++it)
//       m_curveAttrs.m_lineWidths.push_back( (Qt::PenStyle) (*it).toInt() );
//   }

//   // read settings symbol
//   slist = settings->value(_tmp + ".penStyleList", QStringList() ).toStringList();
//   if (slist.size()) {
//     m_curveAttrs.m_lineStyles.clear();
//     QStringList::iterator it= slist.begin();
//     for (; it != slist.end(); ++it)
//       m_curveAttrs.m_lineStyles.push_back( (Qt::PenStyle) (*it).toInt() );
//   }

//   // read settings symbol color
//   slist = settings->value(_tmp + ".symbolColorList", QStringList() ).toStringList();
//   if (slist.size()) {
//     toStdVector(slist, m_curveAttrs.m_symbolColors);
//   }

//   // read settings symbol pen color
//   slist = settings->value(_tmp + ".symbolPenColorList", QStringList() ).toStringList();
//   if (slist.size()) {
//     toStdVector(slist, m_curveAttrs.m_symbolPenColors);
//   }

//   settings->endGroup();
// }

// /* --------------------------------------------------------------------------- */
// /* popup menu listener --                                                      */
// /* --------------------------------------------------------------------------- */

// /* --------------------------------------------------------------------------- */
// /*  setUserInteractionMode --                                                  */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::setUserInteractionMode( eUserInteractionMode mode ){
//   // Plots without FUNC don't have the rectangle mode
//   // keep current mode
//   if (mode == SELECT_RECTANGLE && getFunction() == 0)
//     return;

//   // mode is set already -> nothing to do
//   if (m_userInteractionMode == mode)
//     return;

//   // new mode => set local setting
//   m_userInteractionMode = mode;

//   // new global ui mode?
//   bool newUiMode(mode != s_userInteractionMode);

//   bool bZoom = false;
//   UserInteractionModeMenu& menu = getUserInteractionModeMenu();
//   m_selectionRectMode = false;

//   if( mode == ZOOM ) {
//     if (menu.m_zoomButton)        menu.m_zoomButton->setToggleStatus( true );
//     if (menu.m_selectPointButton) menu.m_selectPointButton->setToggleStatus( false );
//     if (menu.m_selectRectButton)  menu.m_selectRectButton->setToggleStatus( false );
//     bZoom = true;
//     if (newUiMode)
//       setXferUiMode("ZOOM");
//   }
//   else if( mode == SELECT_POINT ) {
//     if (menu.m_zoomButton)        menu.m_zoomButton->setToggleStatus( false );
//     if (menu.m_selectPointButton) menu.m_selectPointButton->setToggleStatus( true );
//     if (menu.m_selectRectButton)  menu.m_selectRectButton->setToggleStatus( false );
//     m_picker->setRubberBandPen( QColor(Qt::red) );
//     m_picker->setTrackerPen(QColor(Qt::darkRed));
//     if (newUiMode)
//       setXferUiMode("SELECT_POINT");
//   }
//   else if( mode == SELECT_RECTANGLE  && menu.m_selectRectButton ) {
//     if (menu.m_zoomButton)        menu.m_zoomButton->setToggleStatus( false );
//     if (menu.m_selectPointButton) menu.m_selectPointButton->setToggleStatus( false );
//     if (menu.m_selectRectButton)  menu.m_selectRectButton->setToggleStatus( true );
//     m_picker->setRubberBandPen( QColor(Qt::green) );
//     m_picker->setTrackerPen(QColor(Qt::darkGreen));
//     m_selectionRectMode = true;
//     if (newUiMode)
//       setXferUiMode("SELECT_RECTANGLE");
//   }

//   m_zoomer[0]->setEnabled( bZoom );
//   m_zoomer[1]->setEnabled( bZoom );

//   // create bzw. recreate Picker (Alles neu initialisieren ist am Besten)
//   createPicker( !bZoom );

//   // datapool not changed, avoid recursive function call/gui update
//   if ( !newUiMode )
//     return;

//   // new mode => set global setting
//   s_userInteractionMode = mode;

//   // call function PLOT2D_UIMODE_FUNC
//   std::string funcname("PLOT2D_UIMODE_FUNC");
//   JobFunction *func = JobManager::Instance().getFunction( funcname );
//   if( func != 0 ){
// 	JobStarter *starter = new Trigger( func, this );
// 	starter->setReason( JobElement::cll_Input );
// 	starter->startJob();
//   } else {
// 	GuiQtManager::Instance().update(GuiElement::reason_Always);
//   }
// }

// /* --------------------------------------------------------------------------- */
// /*  UserInteractionModeMenu::ToggleStatusChanged --                            */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::UserInteractionModeMenu::ToggleStatusChanged( GuiEventData *event ){
//   eUserInteractionMode mode = ZOOM;
//   if( event == 0 )
//     return;
//   bool bZoom = false;
//   GuiElement *element = event->m_element;
//   m_chart->m_selectionRectMode = false;
//   if( element != 0 ){
//     if( element == m_zoomButton ) {
//       mode = ZOOM;
//     }
//     else if( element == m_selectPointButton ) {
//       mode = SELECT_POINT;
//     }
//     else if( element == m_selectRectButton ) {
//       mode = SELECT_RECTANGLE;
//     }
//   }

//   m_chart->setUserInteractionMode(mode);
// }

// /* --------------------------------------------------------------------------- */
// /* GuiQt2dPlot::PrintListener::ButtonPressed                                    */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::PrintListener::ButtonPressed() {
//   GuiPrinterDialog::MyEventData event( ReportGen::PRINT );
//   GuiQtPrinterDialog::Instance().showDialog( (HardCopyListener*) m_plot,
//                                              m_plot, &event );
// }

// /* --------------------------------------------------------------------------- */
// /* GuiQt2dPlot::OpenScaleListener::ButtonPressed                                */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::OpenScaleListener::ButtonPressed() {
//   m_chart->openScaleDialog();
// }

// /* --------------------------------------------------------------------------- */
// /* GuiQt2dPlot::OpenCycleListener::ButtonPressed                                */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::OpenCycleListener::ButtonPressed() {
//   m_chart->openCyclesDialog();
// }

// /* --------------------------------------------------------------------------- */
// /* GuiQt2dPlot::OpenConfigListener::ButtonPressed                                */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::OpenConfigListener::ButtonPressed() {
//   m_chart->openConfigDialog();
// }

// /* --------------------------------------------------------------------------- */
// /* GuiQt2dPlot::PropertyListener::ButtonPressed                                 */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::PropertyListener::ButtonPressed() {

//   // create
//   if (!m_propDlg) {
//     QWidget* pw = m_chart->getQtDialog() ? m_chart->getQtDialog()->getDialogWidget() : NULL;
//     m_propDlg = new GuiQwtPropertyDialog( pw );

//     connect( m_propDlg, SIGNAL( edited() ), m_plot, SLOT( updatePlot() ) );
//   }
//   m_propDlg->setSettings( m_chart->m_plot );
//   m_propDlg->exec();
// }

// /* --------------------------------------------------------------------------- */
// /* GuiQt2dPlot::AnnotationListener::ToggleStatusChanged                         */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::AnnotationListener::ToggleStatusChanged( bool state) {
//   m_chart->showAnnotationLabels( state );
//   // GuiUpdate
//   m_chart->update( reason_Always );
// }

/* --------------------------------------------------------------------------- */
/* showAnnotationLabels --                                                     */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::showAnnotationLabels( bool state ){
 //   bool refresh(false);

//   if( m_plot == 0 ){
//      m_showAnnotationLabels = state;
//      return;
//    }
//   if (1)  {
//     std::vector<QwtPlotMarker*>::iterator it =  m_xannoMarker.begin();
//     for (; it != m_xannoMarker.end(); ++it) {
//       if (state)
// 	(*it)->show();
//       else
// 	(*it)->hide();
//     }
//   }

//   setXAxisAnnotation(state);
//   if( m_showAnnotationLabels != state ){
//     m_showAnnotationLabels = state;
//     m_chart->replot();
//   }
}

/* --------------------------------------------------------------------------- */
/* setAnnotationLabels --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiQt2dPlot::setAnnotationLabels(){

//   // clear old marker
//   std::vector<QwtPlotMarker*>::iterator it =  m_xannoMarker.begin();
//   for (; it != m_xannoMarker.end(); ++it) {
//     delete (*it);
//   }
//   m_xannoMarker.clear();
//   GuiQwtScaleDraw* xScaleDraw = dynamic_cast<GuiQwtScaleDraw*>(m_chart->axisScaleDraw( getAnnotationAxis() ));
//   if (xScaleDraw)
//     xScaleDraw->clearAnnotationLabels();

//   int currentCycle = dpi().currentCycle();
//   tPlotIterator itp;
//   for( itp = m_plots.begin(); itp != m_plots.end(); ++itp ){

//     std::set<GuiPlotDataItem *> items;
//     (*itp)->getXPlotDataItems( items );
//     std::set<GuiPlotDataItem *>::iterator itemIter;
//     for( itemIter = items.begin(); itemIter != items.end(); ++itemIter ){
//       if( (*itemIter) != 0 ){
// 	GuiPlotDataItem::AnnotationLabelsMap annoMap;
// 	if( (*itemIter)->getAnnotationLabelsMap( annoMap ) ){
// 	  GuiPlotDataItem::AnnotationLabelsMap::iterator it;
// 	  for( it = annoMap.begin(); it != annoMap.end(); ++it ){
// 	    QwtPlotMarker *marker = new QwtPlotMarker();
// 	    marker->attach(m_plot);
// 		marker->setZ(1);

// 	    marker->setLinePen(QPen(QColor(200,150,0), 1, Qt::DashDotLine));
// 	    if (getAnnotationAxis() == AXIS_X_BOTTOM)
// 	      marker->setLineStyle(QwtPlotMarker::VLine);
// 	    else
// 	      marker->setLineStyle(QwtPlotMarker::HLine);
// 	    ///	    marker->setLabel( QwtText( QString::fromStdString((*it).second.c_str()) ) );
// 	    marker->setXValue( (*it).first );
// 	    ///	    marker->setLabelAlignment( Qt::AlignVCenter|Qt::AlignBottom );
// 	    m_xannoMarker.push_back( marker );
// 	    xScaleDraw->setAnnotationLabel((*it).first, (*it).second);
// 	  }
// 	}
//       }
//     }
//   }
//   GuiQwtScaleEngine* xScaleEngine = dynamic_cast<GuiQwtScaleEngine*>(m_chart->axisScaleEngine( getAnnotationAxis() ));
//   if (xScaleEngine) {
//     std::vector<double> lblValues = xScaleDraw->getAnnotationLabelValues();
//     xScaleEngine->setAnnotationLabelValues(lblValues);
//     xScaleEngine->setAnnotationType(m_showAnnotationLabels);
//   }
//   xScaleDraw->setAnnotationType(m_showAnnotationLabels);

  return true;
}

// /* --------------------------------------------------------------------------- */
// /* setAxisLabelRotation --                                                     */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::setAxisLabelRotation(double rotation) {
//   m_annoAngle = rotation;
//   tPlotIterator itp;
//   for( itp = m_plots.begin(); itp != m_plots.end(); ++itp ){
//     std::set<GuiPlotDataItem *> items;
//     (*itp)->getXPlotDataItems( items );
//     std::set<GuiPlotDataItem *>::iterator itemIter;
//     for( itemIter = items.begin(); itemIter != items.end(); ++itemIter ){
//       if( (*itemIter) != 0 ){
// 	//	  GuiPlotDataItem::AnnotationLabelsMap annoMap;
// 	//	  if( (*itemIter)->getAnnotationLabelsMap( annoMap ) ){
// 	(*itemIter)->setAnnotationAngle(rotation);
// 	//	  }
//       }
//     }
//   }
// }
// /* --------------------------------------------------------------------------- */
// /* setXAxisAnnotation --                                                       */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::setXAxisAnnotation(bool bAnno) {

//   GuiQwtScaleDraw* xScaleDraw;
//   GuiQwtScaleEngine* xScaleEngine;
//   xScaleDraw = dynamic_cast<GuiQwtScaleDraw*>(m_chart->axisScaleDraw( getAnnotationAxis() ));
//   xScaleEngine = dynamic_cast<GuiQwtScaleEngine*>(m_chart->axisScaleEngine( getAnnotationAxis() ));
//   xScaleDraw->setAnnotationType(bAnno);

//   // set label rotation
//   if ( bAnno ) { // get angle
//     tPlotIterator itp;
//     for( itp = m_plots.begin(); itp != m_plots.end(); ++itp ){

//       std::set<GuiPlotDataItem *> items;
//       (*itp)->getXPlotDataItems( items );
//       std::set<GuiPlotDataItem *>::iterator itemIter;
//       for( itemIter = items.begin(); itemIter != items.end(); ++itemIter ){
//         if( (*itemIter) != 0 ){
//           GuiPlotDataItem::AnnotationLabelsMap annoMap;
//           if( (*itemIter)->getAnnotationLabelsMap( annoMap ) ){
//             if (m_annoAngle == 0.0)
//               m_annoAngle = (*itemIter)->getAnnotationAngle();
//           }
//         }
//       }
//     }
//     m_chart->setAxisLabelRotation(AXIS_X_BOTTOM, m_annoAngle);
//   } else {
//     // reste label rotation
//     m_chart->setAxisLabelRotation(AXIS_X_BOTTOM, m_annoAngle);
//   }

//   // set label alignment
//   if (bAnno) {
//     m_chart->setAxisLabelAlignment(AXIS_X_BOTTOM, Qt::AlignCenter|Qt::AlignBottom);
//     if (fabs(m_annoAngle) > 0 && m_annoAngle != 0) {
//       m_chart->setAxisLabelAlignment(AXIS_X_BOTTOM,
//                                     m_annoAngle < 0 ? Qt::AlignVCenter|Qt::AlignLeft :
//                                     Qt::AlignBottom|Qt::AlignRight);
//     }
//   }
//   else
//     m_chart->setAxisLabelAlignment(AXIS_X_BOTTOM, Qt::AlignHCenter|Qt::AlignBottom );
//   if (xScaleEngine)
//     xScaleEngine->setAnnotationType(bAnno);

//   std::vector<double> lblValues = xScaleDraw->getAnnotationLabelValues();
//   if (lblValues.size() && bAnno)  {
//     m_chart->setAxisMaxMajor( getAnnotationAxis(), lblValues.size());
//     m_chart->setAxisMaxMinor( getAnnotationAxis(), 0);
//   } else {
//     m_chart->setAxisMaxMajor( getAnnotationAxis(), m_majorTicks);
//     m_chart->setAxisMaxMinor( getAnnotationAxis(), m_minorTicks);
//   }

//   m_chart->updateAxes();
// }

// //----------------------------------------------------
// // getAnnotationAxis
// //----------------------------------------------------
// AXIS_POSITION GuiQt2dPlot::getAnnotationAxis() {
//   // spezialfall barplot
//   if (getQwtCurveStyle(Gui2dPlot::Y1AXIS)  == GuiQwtPlotCurve::Bar ||
//       getQwtCurveStyle(Gui2dPlot::Y1AXIS)  == GuiQwtPlotCurve::StackingBar) {
//     return m_bcSettings.orientation == Qt::Vertical ? AXIS_X_BOTTOM : AXIS_Y_LEFT;
//   } else
//     if (getQwtCurveStyle(Gui2dPlot::Y2AXIS)  == GuiQwtPlotCurve::Bar ||
// 	getQwtCurveStyle(Gui2dPlot::Y2AXIS)  == GuiQwtPlotCurve::StackingBar   ) {
//       return m_bcSettings.orientation == Qt::Vertical ? AXIS_X_BOTTOM : AXIS_Y_RIGHT;
//   }

//   // default ist xBottom
//   return AXIS_X_BOTTOM;
// }

// //----------------------------------------------------
// // setBarChartLayoutSettings
// //----------------------------------------------------
// void GuiQt2dPlot::setBarChartLayoutSettings(const GuiQwtPropertyDialog::Settings::BarChart& settings) {
//   m_bcSettings = settings;
//   if (!getName().size()) writeSettings();
// }

// /* --------------------------------------------------------------------------- */
// /* setCursor --                                                                */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::setCursor() {
//   m_chart->canvas()->setCursor( (m_picker && m_picker->isEnabled() && !m_selectionRectMode) ?
//                                Qt::PointingHandCursor : Qt::CrossCursor );
// }

void GuiQt2dPlot::toStdVector(const QStringList& list, std::vector<std::string>& vec) {
  vec.clear();
  foreach( QString str, list) {
    vec.push_back(str.toStdString());
  }
}

//  void GuiQt2dPlot::fromStdVector(const std::vector<std::string>& vec, QStringList& list) {
//   list.clear();
//   for(std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
//     list.push_back(QString::fromStdString(*it));
//   }
// }


// /* --------------------------------------------------------------------------- */
// /* setRescaleMode --                                                           */
// /* --------------------------------------------------------------------------- */
// void GuiQt2dPlot::setRescaleMode() {
//   QwtPlotRescaler* rescaler = m_chart->getPlotRescaler();

//   if (!AspectRatioMode()) {
//     rescaler->setEnabled(false);
//     //     m_chart->replot();
//     return;
//   }

//   // set axis aspect ratio
//   PlotAxis *axis[AXIS_TYPE_COUNT - 1] = { &getAxis(0), &getAxis(1), &getAxis(2) };
//   double value;
//   for( int y = 0; y < AXIS_TYPE_COUNT-1; ++y ){
//     XferDataItem *xferAspectRatio = axis[y]->getXferAspectRatio();
//     if (xferAspectRatio && xferAspectRatio->getValue(value)) {
//       rescaler->setAspectRatio(y, value);
//       getAxis(y).setAspectRatio(value);
//       BUG_DEBUG("setAspectRatio (PlotAxis) axis: " << y << ", " << value);
//     } else {
//       rescaler->setAspectRatio(y, getAxis(y).getAspectRatio() );
//     }
//   }

//   // others
//   rescaler->setRescalePolicy( QwtPlotRescaler::Fixed );
//   rescaler->setEnabled(true);
//   for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
//     rescaler->setExpandingDirection(QwtPlotRescaler::ExpandUp);

//   rescaler->rescale();
//   return;
// }

/* --------------------------------------------------------------------------- */
/* connectMarkers --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::connectMarkers()
{
  // Connect all markers to handler
  const auto markers = m_chart->legend()->markers();
  for (QLegendMarker *marker : markers){
    // Disconnect possible existing connection to avoid multiple connections
    QObject::disconnect(marker, &QLegendMarker::clicked,
                        this, &GuiQt2dPlot::handleMarkerClicked);
    QObject::connect(marker, &QLegendMarker::clicked,
                     this, &GuiQt2dPlot::handleMarkerClicked);
    ///marker->setVisible(true);
    handleMarker(marker);
  }
}

/* --------------------------------------------------------------------------- */
/* handleMarkerClicked --                                                      */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::handleMarkerClicked(){
  auto marker = qobject_cast<QLegendMarker *>(sender());
  Q_ASSERT(marker);
  // Toggle visibility of series
  marker->series()->setVisible(!marker->series()->isVisible());

  handleMarker(marker);
}

/* --------------------------------------------------------------------------- */
/* handleMarker --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::handleMarker(QLegendMarker* marker){
  switch (marker->type()){
  case QLegendMarker::LegendMarkerTypeBar:
  case QLegendMarker::LegendMarkerTypeXY:{
    // Turn legend marker back to visible, since hiding series also hides the marker
    // and we don't want it to happen now.
    marker->setVisible(true);
    // Dim the marker, if series is not visible
    qreal alpha = 1.0;

    if (!marker->series()->isVisible())
      alpha = 0.4;

    QColor color;
    QBrush brush = marker->labelBrush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setLabelBrush(brush);

    brush = marker->brush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setBrush(brush);

    QPen pen = marker->pen();
    color = pen.color();
    color.setAlphaF(alpha);
    pen.setColor(color);
    marker->setPen(pen);
    break;
  }
  default:{
    qDebug("Unknown marker");
    std::cout << "Unknown marker type: " << marker->type() << std::endl;
    break;
  }
  }
}

/* --------------------------------------------------------------------------- */
/* rectangle --                                                                */
/* --------------------------------------------------------------------------- */
QImage GuiQt2dPlot::rectangle(qreal imageSize, const QColor &color)
{
    QImage image(imageSize, imageSize, QImage::Format_RGB32);
    QPainter painter;
    painter.begin(&image);
    painter.fillRect(0, 0, imageSize, imageSize, color);
    painter.end();
    return image;
}

/* --------------------------------------------------------------------------- */
/* circle --                                                                   */
/* --------------------------------------------------------------------------- */
QImage GuiQt2dPlot::circle(qreal imageSize, const QColor &color)
{
    QImage image(imageSize, imageSize, QImage::Format_ARGB32);
    image.fill(QColor(0, 0, 0, 0));
    QPainter paint;
    paint.begin(&image);
    paint.setBrush(color);
    QPen pen = paint.pen();
    pen.setWidth(0);
    paint.setPen(pen);
    paint.drawEllipse(0, 0, imageSize * 0.9, imageSize * 0.9);
    paint.end();
    return image;
}

/* --------------------------------------------------------------------------- */
/* imageSize --                                                                */
/* --------------------------------------------------------------------------- */
QImage GuiQt2dPlot::roundrect(qreal imageSize, const QColor &color)
{
    QImage image(imageSize, imageSize, QImage::Format_ARGB32);
    image.fill(QColor(0, 0, 0, 0));
    QPainter paint;
    paint.begin(&image);
    paint.setBrush(color);
    QPen pen = paint.pen();
    pen.setWidth(0);
    paint.setPen(pen);
    paint.drawRoundedRect(0, 0, imageSize, imageSize, 4, 4);
    paint.end();
    return image;
}

/* --------------------------------------------------------------------------- */
/* getBoundingBox --                                                           */
/* --------------------------------------------------------------------------- */
QRectF GuiQt2dPlot::getBoundingBox(QXYSeries *series) {
    if (series->count() == 0) {
        return QRectF(); // Return an empty rect if there are no points
    }

    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();

    for (const QPointF &point : series->points()) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }

    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

/* --------------------------------------------------------------------------- */
/* clearBoundingBox --                                                        */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::clearBoundingBox(){
  for( int axis = 0; axis < 4; ++axis ){
    m_ranges[axis][0] = std::numeric_limits<double>::max();
    m_ranges[axis][1] = -std::numeric_limits<double>::max();
  }
}

/* --------------------------------------------------------------------------- */
/* setBoundingBox --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::setBoundingBox(eAxisType axis, double min, double max){
  if (axis < 4){
    m_ranges[axis][0] = std::min(m_ranges[axis][0], min);
    m_ranges[axis][1] = std::max(m_ranges[axis][1], max);
   }
}

/* --------------------------------------------------------------------------- */
/* getBoundingBox --                                                           */
/* --------------------------------------------------------------------------- */
bool GuiQt2dPlot::getBoundingBox(eAxisType axis, double& min, double& max){
  if (axis < 4){
    min = m_ranges[axis][0];
    max = m_ranges[axis][1];
    return true;
  }
  min = std::numeric_limits<double>::max();
  max = std::numeric_limits<double>::min();
  return false;
}

/* --------------------------------------------------------------------------- */
/* createAxis --                                                               */
/* --------------------------------------------------------------------------- */
QAbstractAxis* GuiQt2dPlot::createAxis(GuiPlotDataItem* plotDataItem, PlotAxis& plotAxis){
  if (plotAxis.isLogarithmic())
    return  new QLogValueAxis;
  if (plotDataItem){
    const char* format(0);
    if (plotDataItem->Attr()->StringType() == UserAttr::string_kind_datetime){
      format = "dd-MM-yyyy h:mm";
    }
    if (plotDataItem->Attr()->StringType() == UserAttr::string_kind_date){
      format = "dd-MM-yyyy";
    }
    if (plotDataItem->Attr()->StringType() == UserAttr::string_kind_time){
      format = "h:mm";
    }
    if (format){
      QDateTimeAxis *axis = new QDateTimeAxis;
      axis->setFormat(format);
      return axis;
    }
  }
  return  new QValueAxis;
}

/* --------------------------------------------------------------------------- */
/* distance --                                                                 */
/* --------------------------------------------------------------------------- */
qreal GuiQt2dPlot::distance(const QPointF &p1, const QPointF &p2)
{
    return qSqrt((p1.x() - p2.x()) * (p1.x() - p2.x())
                 + (p1.y() - p2.y()) * (p1.y() - p2.y()));
}

/* --------------------------------------------------------------------------- */
/* clickPoint --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQt2dPlot::clickPoint(const QPointF &point)
{
  if (std::isnan(point.x()) || std::isnan(point.y())){
    return;
  }
  // Find the closest data point
  auto _clicked = false;
  QXYSeries* _series(0);
  int index = -1;
  qreal dist = std::numeric_limits<qreal>::max();
  auto m_movingPoint = QPointF();
  for(auto series: m_chart->series()){
    const auto xyseries = dynamic_cast<QXYSeries*>(series);
    const auto points = dynamic_cast<QXYSeries*>(series)->points();
    xyseries->deselectAllPoints();
    int i(0);
    for (QPointF p : points) {
      if (distance(p, point) < dist) {
        m_movingPoint = p;
        dist = distance(m_movingPoint, point);
        _clicked = true;
        _series =  dynamic_cast<QXYSeries*>(series);
        index = i;
      }
      ++i;
    }
  }
  if (index != -1) {
    QList<int> selPts = _series->selectedPoints();
    if (selPts.contains(index)){
      selPts.removeAll(index);
    }else{
      selPts.insert(0, index);
    }
    _series->selectPoints(selPts);
  }
}
#endif
