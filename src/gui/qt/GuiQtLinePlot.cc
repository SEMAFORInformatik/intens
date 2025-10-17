
#include <numeric>
#include <limits>

#include <QMouseEvent>
#include <QPen>

#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_text_label.h>
#include <qwt_text.h>
#include <qwt_picker_machine.h>

#include "gui/GuiForm.h"
#include "gui/qt/GuiMFMSettings.h"
#include "gui/qt/GuiQtLinePlot.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/DataProcessing.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQwtScaleDraw.h"

#include "LinePlotPicker.h"

LinePlotPicker::LinePlotPicker(GuiQtLinePlot *p, QWidget *c):
/*	QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,

		      QwtPlotPicker::CrossRubberBand,
		      QwtPicker::AlwaysOn,c),lineplot(p)*/
#if QWT_VERSION < 0x060100
	QwtPlotPicker(dynamic_cast<QwtPlotCanvas*>(c))
#else
	QwtPlotPicker(c)
#endif
{
    connect(this,
	    SIGNAL(selected(const QPolygon &)), p,
	    SLOT(selected(const QPolygon &)));
    setRubberBandPen(QColor(Qt::green));
    setTrackerPen(QColor(Qt::red));
#if QWT_VERSION < 0x060000
    setSelectionFlags( QwtPicker::PointSelection|QwtPicker::ClickSelection);
#else
    setStateMachine(new QwtPickerClickPointMachine());
#endif
}

void GuiQtLinePlot::selected (const QPolygon &pos){
    focusInEvent( 0 );
//     std::cout << "Picker::selected" << std::endl;
//     std::cout << " x: " << pos.x() << std::endl;
//     std::cout << " y: " << pos.y() << std::endl;
}

class MyLinePlot : public QwtPlot{
public:
    MyLinePlot( GuiQtLinePlot *plot );

    virtual ~MyLinePlot(){}

private:

  void contextMenuEvent ( QContextMenuEvent* event ){
    m_plot->popupMenu(event);
  }

  GuiQtLinePlot *m_plot;
  LinePlotPicker picker;
};


MyLinePlot::MyLinePlot( GuiQtLinePlot *plot )
    : QwtPlot()
    , m_plot( plot )
      , picker( plot, this->canvas() ){
}


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtLinePlot::GuiQtLinePlot( GuiElement *parent, const std::string &name )
  : GuiQtElement( parent, name ), GuiImage(3)
  , m_plot(0)
  , m_current_line( 0 )
  , m_size(0)
  , m_settingsListener( this ){
  setSize( 500,200 );
  m_type = e_Plot;
  m_xArray[0]=0;
  m_yArray[0]=0;
  m_xArray[1]=0;
  m_yArray[1]=0;
 }

GuiQtLinePlot::~GuiQtLinePlot(){
  delete m_plot;
}

bool GuiQtLinePlot::destroy(){
  return true;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

void GuiQtLinePlot::create(){
  initChannel();
  createLinePlotAndPainter();
  createPopupMenu();
}

void GuiQtLinePlot::createLinePlotAndPainter(){
  if( !m_plot ){
    QWidget *parent = getParent()->getQtElement()->myWidget();
    m_plot = new MyLinePlot(this);

    QwtPlotGrid *_grid = new QwtPlotGrid;
    _grid->setPen( QPen(Qt::DotLine) );
    _grid->enableX(true);
    _grid->enableY(true);
    _grid->attach( m_plot );

    int w, h;
    getSize(w,h);
    m_plot->setMinimumSize( w, h );
    m_plot->enableAxis(QwtPlot::yRight, false);
    m_plot->enableAxis(QwtPlot::yLeft, true);
//    m_plot->enableYRightAxis(false);
//    m_plot->enableYLeftAxis(true);

    m_plot->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);//, QwtPlot::RightLegend);
#if QWT_VERSION < 0x060100
    m_plot->legend()->setItemMode(QwtLegend::ReadOnlyItem); // setReadOnly(true);
#else
    QwtLegend* legend = dynamic_cast<QwtLegend*>(m_plot->legend());
    if (legend) {
      legend->setDefaultItemMode(QwtLegendData::ReadOnly); // setReadOnly(true);
    }
#endif

// m_plot->titleLabel()->setText("cccCCCC");
    QFont font = m_plot->titleLabel()->font();
    m_plot->titleLabel()->setFont( QtMultiFontString::getQFont( "@plotTitle@", font ) );

    font = m_plot->axisFont( QwtPlot::yLeft );
    font =  QtMultiFontString::getQFont( "@plotAxis@", font );
    m_plot->setAxisFont( QwtPlot::yLeft, font );
    m_plot->setAxisScaleDraw( QwtPlot::yLeft, new GuiQwtScaleDraw( GuiQwtScaleDraw::type_real,
								   'g', 10, -1 ) );
    m_plot->setAxisFont( QwtPlot::yRight, font );
    m_plot->setAxisFont( QwtPlot::xBottom, font );
    m_plot->setAxisFont( QwtPlot::xTop, font );

    font = m_plot->axisTitle( QwtPlot::yLeft ).font();
    font =  QtMultiFontString::getQFont( "@plotAxisTitle@", font );
    QwtText paL, paR, paB, paT;
    paL.setFont( font ), paR.setFont( font ), paB.setFont( font ), paT.setFont( font ) ;
    m_plot->setAxisTitle( QwtPlot::yLeft, paL );
    m_plot->setAxisTitle( QwtPlot::yRight, paR );
    m_plot->setAxisTitle( QwtPlot::xBottom, paB );
    m_plot->setAxisTitle( QwtPlot::xTop, paT );

    const QColor &color = GuiQtManager::foregroundColor();
    m_plot->axisTitle( QwtPlot::yLeft ).setColor( color );

    font = m_plot->legend()->font();
    m_plot->legend()->setFont( QtMultiFontString::getQFont( "@plotLegend@", font ) );

#if QWT_VERSION < 0x060100
    m_plot->canvas()->setFocusIndicator( QwtPlotCanvas::CanvasFocusIndicator );
#endif
    m_defaultTitleBackgroundColor =
      m_plot -> titleLabel() -> palette().color(QPalette::Window);
    //    m_defaultTitleForegroundColor =
    //      m_plot -> titleLabel() -> paletteForegroundColor();

//     connect(m_plot,
// 	    SIGNAL(plotMousePressed(const QMouseEvent &)),
// 	    SLOT(mousePressed( const QMouseEvent &)));
    updateLabel();
    updateUnit();

    if( hasFocus() ){
      GuiImage::setFocus(); // its a bit silly isn't it?
    }
  }

  // add curves
  m_curve[0] = new QwtPlotCurve( "forward" );
  m_curve[0]->attach(m_plot);
  m_curve[1] = new QwtPlotCurve( "backward" );
  m_curve[1]->attach(m_plot);
  m_curve[0]->setPen(  QPen( "red" ) );
  m_curve[1]->setPen(  QPen( "blue" ) );
  m_plot->legend()->setEnabled( true );
//   m_plot->enableLegend( true, m_curve[0] );
//   m_plot->enableLegend( true, m_curve[1] );
}

QWidget* GuiQtLinePlot::myWidget(){
  return m_plot;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::update( UpdateReason reason ){
  TransactionNumber trans = GuiQtManager::Instance().LastGuiUpdate();
  GuiImage::update( trans );
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtLinePlot::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  return GuiImage::hasChanged( trans, xfer, show );
}

/* --------------------------------------------------------------------------- */
/* getTitle --                                                                 */
/* --------------------------------------------------------------------------- */

std::string GuiQtLinePlot::getTitle(){
  std::string title;
  GuiForm * myform( getMyForm() );
  if( myform ){
    title = myform->getTitle();
  }
  return title;
}

/* --------------------------------------------------------------------------- */
/* setFocus --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::setFocus( bool focus ){
  QwtText tl (m_plot->titleLabel()->text());
  if( focus ){
    tl.setBackgroundBrush(  GuiQtManager::imageHighlightColor() ) ;
  }
  else {
    tl.setBackgroundBrush( m_defaultTitleBackgroundColor ) ;
  }
  m_plot->titleLabel()->setText( tl );

}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::clear( bool clearImages ){
  if( m_plot ){
#if QWT_VERSION < 0x060000
    m_plot->clear();
    m_curve[0] = new QwtPlotCurve( "forward" );
    m_curve[0]->attach(m_plot);
    m_curve[1] = new QwtPlotCurve( "backward" );
    m_curve[1]->attach(m_plot);
    m_curve[0]->setPen(  QPen( "red" ) );
    m_curve[1]->setPen(  QPen( "blue" ) );
    m_plot->legend()->setEnabled( true );
#elif QWT_VERSION < 0x060200
    m_curve[0]->setSamples( 0, 0, 0 );
    m_curve[1]->setSamples( 0, 0, 0 );
#else
    QVector<double> d;
    m_curve[0]->setSamples(d);
    m_curve[1]->setSamples(d);
#endif
    delete [] m_xArray[0];
    delete [] m_yArray[0];
    m_xArray[0]=0;
    m_yArray[0]=0;
    delete [] m_xArray[1];
    delete [] m_yArray[1];
    m_xArray[1]=0;
    m_yArray[1]=0;
    m_size=0;
  }
}

void GuiQtLinePlot::updateLabel(){
  if( m_plot ){
    m_plot->titleLabel()->setText( QString::fromStdString(getChannelName()) );
  }
}

void GuiQtLinePlot::updateUnit(){
  if( m_plot ){
    m_plot->setAxisTitle( QwtPlot::yLeft, QString::fromStdString(getChannelUnit()) );
  }
}

/* --------------------------------------------------------------------------- */
/* focusIn --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::focusInEvent( QFocusEvent* e ){
  if( !hasFocus() ){
    GuiImage::setFocus();
  }
#ifdef Q_OS_UNIX
  GuiMFMSettings::getDialog().setActiveWindow();
#endif
  GuiForm *myform = getMyForm();
  std::string title;
  if( myform ){
    title = myform->getTitle();
  }
  GuiMFMSettings::getDialog().setImage( this, title );
}

//-------------------------------------------------------------
// redraw
//-------------------------------------------------------------

void GuiQtLinePlot::redraw( bool _clear ){
  clear( _clear );
  resetMinMax();
  for( int dir=0; dir<=1; ++dir ){
    const MFMImageMapper::ImageData *image_data =
      MFMImageMapper::getImageData( this, dir * 100 + getChannel() );
    showImage( image_data, dir * 100 + getChannel()  );
  }
}

bool GuiQtLinePlot::updateImageStatistics( const MFMImageMapper::ImageData *img
					   , int direction ){

    bool new_range = GuiImage::updateImageStatistics( img, direction );

  if( hasFocus() ){
    GuiMFMSettings::getDialog().setAverage( getAverage() );
    GuiMFMSettings::getDialog().setStdDev( getStdDev() );
    if( new_range ){
      GuiMFMSettings::getDialog().setCurrentRange( getMinValue(),
						   getMaxValue() );
    }
    if( getAutoRange() ){
      GuiMFMSettings::getDialog().setMappingRange( getRangeMin(),
						   getRangeMax() );
    }
  }

  return new_range;

}

/* --------------------------------------------------------------------------- */
/* setLine --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::setLine( int id, int line,
			     const MFMImageMapper::ImageData::Line &data,
			     const MFMImageMapper::ImageData *img ){

  int channel = id % 100;
  int dir =  ( id - channel ) / 100;

  if( id != -1 ){
    if( channel != getChannel() ){
      return;
    }
    if( dir != 0 && dir != 1 ){
      return;
    }
  }

  if( m_plot == 0 ){
    return;
  }

  // delete backward curve
  if( dir == 0 ){
#if QWT_VERSION < 0x060000
    m_curve[1]->setData( 0, 0, 0 );
#elif QWT_VERSION < 0x060200
    m_curve[1]->setSamples( 0, 0, 0 );
#else
    QVector<double> d;
    m_curve[1]->setSamples(d);
#endif
  }

  // set data
  if( !data.empty() ){
    if( m_size < data.size() ){
      // we only want to reallocate when size has changed
      delete [] m_xArray[dir];
      delete [] m_yArray[dir];
      m_xArray[dir]=0;
      m_yArray[dir]=0;
    }
    if( m_xArray[dir]==0 ){
      m_xArray[dir]=new double [ data.size() ];
      m_yArray[dir]=new double [ data.size() ];
      m_size=data.size();
    }
    bool new_range = updateImageStatistics( img, dir );

    DataProcessing *dproc=getProcessing( img );
    dproc -> setLine( line );


    double f=getChannelUnitFactor();

    int i;
    double y;
    for( i=0; i<data.size(); ++i ){
      m_xArray[dir][i] = i;
      y = (*dproc)( data[i] );
      m_yArray[dir][i] = f * y;
    }

    if( new_range && getAutoRange() ){
      m_plot -> setAxisScale( QwtPlot::yLeft,
			      f*getRangeMin(), f*getRangeMax() );
    }
    // copy the data into the curve
    if( m_xArray[dir] ){
#if QWT_VERSION < 0x060000
      m_curve[dir]->setData( m_xArray[dir], m_yArray[dir], m_size );
#else
      m_curve[dir]->setSamples( m_xArray[dir], m_yArray[dir], m_size );
#endif
    }
  }
  // finally, refresh the plot
  m_plot->replot();
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::manage(){
  myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::unmanage(){
  //  GuiMFMSettings::getDialog().hide();
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::getSize( int &w, int &h ){
  GuiImage::getSize( w, h );
}

/* --------------------------------------------------------------------------- */
/* setRange --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::setRange( bool manual, double min, double max ){
    setManualRange(min,max);
    setAutoRange( !manual );

  double f = getChannelUnitFactor();
  double y0 = f*getRangeMin();
  double y1 = f*getRangeMax();
  m_plot->setAxisScale( QwtPlot::yLeft, y0, y1);
  redraw( false );
}

void GuiQtLinePlot::showImage( const MFMImageMapper::ImageData *img, int id ){
  int channel = id % 100;
  int dir =  ( id - channel ) / 100;

  if( id != -1 ){
    if( channel != getChannel() ){
      return;
    }
    if( dir != 0 && dir != 1 ){
      return;
    }
  }

  if( m_plot == 0 ) return; //createLinePlotAndPainter();
  if( img ){
      if( currentImage() == this ){
	  GuiForm *myform = getMyForm();
	  std::string title;
	  if( myform ){
	      title = myform->getTitle();
	  }
	  GuiMFMSettings::getDialog().setImage( this, title );
	  GuiMFMSettings::getDialog().setMapping( getMapping() );
      }
      bool new_range = updateImageStatistics( img, dir );

    double f = getChannelUnitFactor();
    m_plot -> setAxisScale( QwtPlot::yLeft,
			    f*getRangeMin(), f*getRangeMax() );

    MFMImageMapper::ImageData::Image::size_type len=(*img).getHeight();
    MFMImageMapper::ImageData::Image::const_iterator i=img -> find(len-1);
    if( i != img -> end() ){
      setLine( dir * 100 + getChannel(), len-1, (*i).second, img );
    }
  }
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::serializeXML(std::ostream &os, bool recursive){
}

/* --------------------------------------------------------------------------- */
/* createPopupMenu --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtLinePlot::createPopupMenu (  ){
  m_popupMenu = new GuiQtPopupMenu( this );
  m_popupMenu->setTearOff( false );

  GuiQtMenuButton *button =  new GuiQtMenuButton( m_popupMenu,
						  &m_settingsListener );
  m_popupMenu->attach( button );
  button->setDialogLabel( _("View settings") );

  m_popupMenu->create();
}

void GuiQtLinePlot::popupMenu(const QContextMenuEvent* event){
  m_popupMenu->myWidget()->move(event->globalX(), event->globalY());
  m_popupMenu->myWidget()->show();
}
void GuiQtLinePlot::createSettingsDialog(){
  GuiMFMSettings &dialog = GuiMFMSettings::getDialog();

  dialog.openDialog( 0 ); //m_plot);

  GuiForm *myform = getMyForm();
  std::string title;
  if( myform ){
    title = myform->getTitle();
  }
  dialog.setImage( this, title );
}

void GuiQtLinePlot::SettingsListener::ButtonPressed(){
  m_plot->createSettingsDialog();
}
