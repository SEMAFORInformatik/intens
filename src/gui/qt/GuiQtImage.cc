
#include <vector>
#include <algorithm>
#include <iomanip>
#include <math.h>

#include <qwidget.h>
#include <qpainter.h>
#include <QMenu>
#include <QScrollArea>
#include <QScrollBar>
#include <qlabel.h>
#include <qlayout.h>
#include <QMouseEvent>
#include <QImage>
#include <QLineEdit>
#include <QToolTip>

#include "gui/qt/GuiQtMinMaxColorMap.h"
#include "utils/Debugger.h"
INIT_LOGGER();
#include "datapool/DataVector.h"
#include "datapool/DataIntegerValue.h"
#include "xfer/XferDataItem.h"

#include "job/JobManager.h"
#include "gui/GuiFactory.h"

#include "gui/qt/QtMultiFontString.h"
#include "gui/GuiForm.h"
#include "gui/qt/GuiMFMSettings.h"
#include "gui/qt/GuiQtImage.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/DataProcessing.h"
#include "gui/qt/MarkerFrame.h"

//QRgb _red = qRgb(255,0,0);
//QRgb _highlight = qRgb(255,200,0);
//QRgb _blue = qRgb(0,0,255);
//QColor activeColor(0,0,255);
//QColor backgroundColor(255,255,255);

ImageView GuiQtImage::CanvasWidget::s_view;
bool GuiQtImage::s_hasMaster = false;
const int GuiQtImage::s_markerWidth = 4;
const int GuiQtImage::s_markerSpacing = 2;
std::list<QPoint> GuiQtImage::points;
QPoint ImageView::m_zoom_p1;
QPoint ImageView::m_zoom_p2;

// Functor for drawing pixels of line
//
class ImagePixel {
public:
  ImagePixel( int line, const ColorMap &cmap,
	      double scale_x, double scale_y, QImage *img,
	      DataProcessing *d ):
    _cmap(cmap), _x(0), _x_old(-1), _line(line), m_image(img),
    nx(1),ny(1),tx(1),ty(1), m_d(d){

      assert( m_image );
    _xmax = img->width();
    _ymax = img->height();
    tx=scale_x;
    if( scale_x >= 1 ){
      nx = (int)scale_x;
      if( scale_x > (int)scale_x ){
	// just to prevent empty lines
	++nx;
      }
    }
    if( scale_y >= 1 ){
      ny = (int)scale_y;
      if( scale_y > (int)scale_y ){
	// just to prevent empty lines
	++ny;
      }
    }
  }

  void operator()( double v ){
    if( _x >= 0 && _x < _xmax ){
      int x =(int)_x;
      // don't overwrite column
      if ( x != _x_old ) {
  	_x_old = x;
	double ss = (*m_d)(v);
	unsigned int color = _cmap( ss );
	for( int i=0; i<nx; ++i ){
	  for( int j=0; j<ny; ++j ){
	    int y=(_line-j);
	    //	  std::cerr << x << "," << y << std::endl;
	    if( x<_xmax && y>=0 && y<_ymax){
//		  std::cout << "  set Pixel x["<<x<<"] y["<<y<<"] color["<<color<<"] \n"<<std::flush;
	      m_image->setPixel( x, y , color );
	    }
	  }
	  ++x;
	}
      }
    }
    //    std::cerr << std::endl;
    _x += tx;
  }

private:
  DataProcessing *m_d;
  QImage  *m_image;
  double _x, _xmax, _ymax;
  int _line;
  int nx, ny;
  double tx, ty;
  const ColorMap &_cmap;
  int _x_old; // x of column last drawn
};

class MyScrollView : public QScrollArea{
public:
  MyScrollView( GuiQtImage *image, QWidget * parent = 0, const char * name = 0)
    : QScrollArea( parent )
    , m_image( image ){
  }
  virtual ~MyScrollView(){}
private:
  GuiQtImage *m_image;
};
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtImage::GuiQtImage( GuiElement *parent, const std::string &name,
			int frame_size )
  : GuiQtElement( parent, name ), GuiImage(frame_size)
  , m_frame( 0 )
  , m_label( 0 )
  , m_canvas(0)
  , m_scroll_view(0)
  , m_image(0)
  , m_current_line( -1 )
  , m_save_image_listener( this )
  , m_y( -1 ){
  m_type = e_Image;
  s_images.push_back( this );
}

GuiQtImage::~GuiQtImage(){
  delete m_image;
  delete m_scroll_view;
  delete m_canvas;
  delete m_frame;
  delete m_label;
}

bool GuiQtImage::destroy(){
  return true;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/


/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtImage::create(){
  initChannel();
  int width(0), height(0), img_width(0), img_height(0);
  GuiImage::getSize( width, height );
  getImageSize( img_width, img_height );

  QWidget *parent = getParent()->getQtElement()->myWidget();

  m_frame = new QWidget( parent );

  m_label = new QLineEdit( m_frame );
  updateLabel();
  QFont font = m_label->font();
  m_label->setFont( QtMultiFontString::getQFont( "@imageTitle@", font ) );
  m_label->setFrame( false );
  m_label->setFocusPolicy( Qt::NoFocus );
  m_label->setDisabled( true );

  QGridLayout *m_layout = new QGridLayout();
  m_layout->setSpacing( 2 );
  m_layout->setContentsMargins(3,3,3,3);
  m_frame->setLayout( m_layout );

  m_layout->addWidget( m_label,0,0 );

  m_scroll_view= new MyScrollView( this, m_frame,0);//,Qt::WA_NoBackground );
//   connect( m_scroll_view, SIGNAL( contentsMoving ( int, int ) ), this, SLOT( slot_contentsMoving (int, int) ) );

  m_scroll_view->setMinimumSize(width+s_markerWidth+s_markerSpacing+4,height+4); // 4 for frame around canvas
  m_scroll_view->setMaximumSize(width+s_markerWidth+s_markerSpacing+4,height+4); // 4 for frame around canvas

  m_frame->setMinimumSize(width+s_markerWidth+s_markerSpacing+4+2,height+4+m_label->height()); // 4 for frame around canvas
  m_frame->setMaximumSize(width+s_markerWidth+s_markerSpacing+4+2,height+4+m_label->height()); // 4 for frame around canvas

  m_layout->addWidget( m_scroll_view,1,0 );

  m_canvas = new CanvasWidget( this, m_scroll_view->viewport(), "canvas" );
  m_canvas->setMinimumSize(img_width+s_markerWidth+s_markerSpacing,img_height);
  m_canvas->setMaximumSize(img_width+s_markerWidth+s_markerSpacing,img_height);
  m_canvas->setFocusPolicy ( Qt::ClickFocus );

  m_scroll_view -> setWidget( m_canvas );

  QPalette pal=  m_frame->palette();
  m_defaultTitleBackgroundColor = pal.color(QPalette::Window);
  m_defaultTitleForegroundColor = pal.color(QPalette::WindowText);
  if( hasFocus() ){
    GuiImage::setFocus();
  }
  else
    setFocus(false);
  createImageAndPainter();
  redraw();
}

/* --------------------------------------------------------------------------- */
/* setFocus --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtImage::setFocus( bool focus ){
  if( !m_label ){
    return;
  }
  QPalette pal=  m_label->palette();
  if( focus ){
    pal.setColor(QPalette::Base,
		 GuiQtManager::imageHighlightColor() );
  }
  else{
    pal.setColor(QPalette::Disabled, QPalette::Base, m_defaultTitleBackgroundColor );
  }
  pal.setColor(QPalette::Disabled, QPalette::Text, m_defaultTitleForegroundColor );
  m_label->setPalette(pal);
}


/* --------------------------------------------------------------------------- */
/* resize --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtImage::resize( int w, int h ){
  BUG_DEBUG( "GuiQtImage::resize size " << w << "/" << h );
  if (m_image)
    if (w==m_image->width() && h==m_image->height())
      return;

  setImageSize( w, h );
  if( m_canvas ){
    m_canvas->setMinimumSize(w+s_markerWidth+s_markerSpacing,h);
    m_canvas->setMaximumSize(w+s_markerWidth+s_markerSpacing,h);
  }
  delete m_image;
  m_image=0;
  createImageAndPainter();
  redraw();
}

/* --------------------------------------------------------------------------- */
/* resize --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtImage::resize(){
  int w,h;
  GuiImage::getSize( w, h );
  resize( w, h );
}

void GuiQtImage::updateLabel(){
  if( m_label ){
    std::string label(getChannelName() + ( getDirection() == 0 ? "  -->" : "  <--" ));
    m_label->setText( QString::fromStdString(label) );
  }
}

/* --------------------------------------------------------------------------- */
/* createImageAndPainter --                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtImage::createImageAndPainter(){
  if( !m_image ){
    // Image Display Size (Number of Pixels)
    int width(0), height(0);
    getImageSize( width, height );
    BUG_DEBUG( "GuiQtImage::createImageAndPainter "
		   << width << "/" << height );

    m_image = new QImage ( width, height, QImage::Format_RGB32 );
    m_bottom = height - 1;
//     m_image->fill( QColor("white").rgb() );
    clear( true );
  }
}

/* --------------------------------------------------------------------------- */
/* saveImage --                                                                */
/* --------------------------------------------------------------------------- */
bool GuiQtImage::saveImage ( const std::string &filename, const std::string &dir ){
	s_dir = dir;
	if ( m_image ) {
		return m_image->save( QString::fromStdString(filename) );
	}
	return false;
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtImage::myWidget(){
  return m_frame;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtImage::update( UpdateReason reason ){
  TransactionNumber trans = GuiQtManager::Instance().LastGuiUpdate();
  GuiImage::update( trans );
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtImage::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  return GuiImage::hasChanged( trans, xfer, show );
}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtImage::clear( bool clearImages ){
  clearPoints();
  if( !clearImages ){
    return;
  }
  if( m_image )
    m_image->fill( QColor("white").rgb() );
  if( m_canvas )
    m_canvas->update();
}

/* --------------------------------------------------------------------------- */
/* focusInEvent --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtImage::focusInEvent( QFocusEvent * e ){
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

/* --------------------------------------------------------------------------- */
/* paintEvent --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtImage::paintEvent( QPaintEvent *e ){
  if( !m_canvas ){
    return;
  }
  m_painter.begin( m_canvas );
  int x=s_markerWidth+s_markerSpacing, y=0;
  m_painter.drawImage( QPoint(x,y)
		       , *m_image );
  //		       , QRect( x, y, w, h ) );
  m_canvas->paintView( this, m_painter );
  m_painter.end();
  drawMarker();
}

void GuiQtImage::drawMarker(){
  if( m_current_line == -1 ){
    return;
  }
  int l =  m_bottom - (int) floor(0.5+getScaleY()*m_current_line);
  m_painter.begin( m_canvas );
  m_painter.setPen( QColor(0,0,0) );
  m_painter.setBrush( QColor(0,0,0 ) );
  if( l  < getImageHeight() && l >= 0 ){
    int h=(int) floor(0.5+getScaleY());
    if( h<2 ) h=2;
    int y = l-h;
    if (y<0) y=0;
    if( y>=0 && y<getImageHeight() ){
      m_painter.drawRect( 0, y, s_markerWidth, h );
    }
  }
  m_painter.end();
}

/** calculate statistics values (min/max/avg/stddev)
 * @return true if range has changed
 */
bool GuiQtImage::updateImageStatistics( const MFMImageMapper::ImageData *img ){

    bool new_range = GuiImage::updateImageStatistics( img );

  if( hasFocus() ){
    GuiMFMSettings::getDialog().setAverage( getAverage() );
    GuiMFMSettings::getDialog().setStdDev( getStdDev() );
    if( new_range )
      GuiMFMSettings::getDialog().setCurrentRange( getMinValue(),
						   getMaxValue() );
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

void GuiQtImage::setLine( int id, int line,
			  const MFMImageMapper::ImageData::Line &data,
			  const MFMImageMapper::ImageData *img ){
  if( id != getId() || data.empty() ){
    return;
  }
//  BUG_DEBUG( "GuiQtImage::setLine " << line );
  m_current_line=line;

  if( m_image == 0 ) createImageAndPainter();

  bool new_range=updateImageStatistics( img );

  if( getAutoRange() && new_range ){
    // we need to redraw because it is likely that the color mapping has changed
    show( img );
    return;
  }

  if( line < 0 || line*getScaleY() > m_image->height()-1 )
    return;

  int y = m_bottom - (int)(getScaleY()*line);
  // don't overwrite line
  if ( y != m_y ) {
    m_y = y;
    DataProcessing *dproc = getProcessing( img );
    dproc->setLine( line );
    std::for_each( data.begin(), data.end(),
       ImagePixel( y,
             MinMaxColorMap(getRangeMin(),
                            getRangeMax(),
                            getChannel()),
             getScaleX(), getScaleY(),
             m_image, dproc ) );
  }

  if (m_canvas)
    m_canvas->update();
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtImage::manage(){
  myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtImage::unmanage(){
  //  GuiMFMSettings::getDialog().hide();
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtImage::getSize( int &w, int &h ){
  GuiImage::getSize( w, h );
  if (m_frame) {
    w = m_frame->sizeHint().width();
    h = m_frame->sizeHint().height();
  }
}

/* --------------------------------------------------------------------------- */
/* setRange --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtImage::setRange( bool manual, double min, double max ){
    setManualRange(min,max);

  if( manual == !getAutoRange() && !manual )
    return;

  setAutoRange( !manual );

  show( MFMImageMapper::getImageData( this ) );
}

void GuiQtImage::showImage( const MFMImageMapper::ImageData *img, int id ){
//    BUG_DEBUG( "GuiQtImage::showImage id="
//		   << getId() << " image id " << id );
  if( img == 0 || id != getId() || img->empty() ){
      return;
  }
  if( m_image == 0 ) createImageAndPainter();
  m_current_line = -1;
  m_y = -1;
  BUG_DEBUG( "GuiQtImage::showImage id="
	     << getId() << " image id " << id );

  if( currentImage() == this ){
      GuiForm *myform = getMyForm();
      std::string title;
      if( myform ){
	  title = myform->getTitle();
      }
      GuiMFMSettings::getDialog().setImage( this, title );
      GuiMFMSettings::getDialog().setMapping( getMapping() );
  }
  updateImageStatistics( img );
  show( img );
}

void GuiQtImage::show( const MFMImageMapper::ImageData *img ){
  if( !img )
    return;
  BUG_DEBUG( "GuiQtImage::show height:" << img->getHeight() );

  MFMImageMapper::ImageData::Image::const_iterator iter;

  int imageHeight = getImageHeight();
  int imageWidth = getImageWidth();
  BUG_DEBUG( "GuiQtImage::show " << imageWidth << "/" << imageHeight <<
		 "  ../" << img->getHeight() );
  DataProcessing *dproc=getProcessing( img );
  int line=0;
  int h_old=-1;
  for( iter = img->begin(); iter != img->end(); ++iter ){
    line = (*iter).first;
    int h = m_bottom - (int)(line*getScaleY());

    // don't overwrite line
    if ( h != h_old ) {
      h_old = h;

      std::vector<double>::const_iterator iiter;
      dproc->setLine( line );
      std::for_each( (*iter).second.begin(), (*iter).second.end(),
         ImagePixel( h,
         MinMaxColorMap(getRangeMin(),
                        getRangeMax(),
                        getChannel()),
         getScaleX(), getScaleY(),
         m_image, dproc ) );
    }
  }
  if (m_canvas){
    m_canvas->update();
  }
}

/* --------------------------------------------------------------------------- */
/* slot_contentsMoving --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_contentsMoving( int x, int y ){
  GuiMFMSettings &dialog = GuiMFMSettings::getDialog();
  if( dialog.isLinked() ){
    std::vector<GuiImage*>::iterator iter;
    int xx=0, yy=0;
    GuiImage::getSize( xx, yy );
    for( iter=s_images.begin();iter!=s_images.end();++iter ){
      if( *iter != this ){
	(*iter) -> setContentsPos ( double(x)/xx, double(y)/yy );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* slot_Set_master --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_Set_master(){
  JobFunction *f = getOnViewAction();
  if( f ){
    s_hasMaster=true;
    DataReference *ref_string = DataPoolIntens::Instance().getDataReference("string_arg");
    if( ref_string ){
      XferDataItem *xfer_string = new XferDataItem( ref_string );
      XferDataItemIndex *index_string = xfer_string->newDataItemIndex(0);
      index_string->setIndex( ref_string, 0 );
      xfer_string->setValue("set_master");
      delete xfer_string;
      Trigger *trigger = new Trigger( f );
      trigger->startJob();
      // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
    }
  }
}

/* --------------------------------------------------------------------------- */
/* slot_Get_master --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_Get_master(){
  JobFunction *f = getOnViewAction();
  if( f ){
    DataReference *ref_string = DataPoolIntens::Instance().getDataReference("string_arg");
    if( ref_string ){
      XferDataItem *xfer_string = new XferDataItem( ref_string );
      XferDataItemIndex *index_string = xfer_string->newDataItemIndex(0);
      index_string->setIndex( ref_string, 0 );
      xfer_string->setValue("get_master");
      delete xfer_string;
      Trigger *trigger = new Trigger( f );
      trigger->startJob();
      // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
    }
  }
}

/* --------------------------------------------------------------------------- */
/* slot_saveImage --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_saveImage(){
	GuiFactory::Instance()->showDialogFileSelection
		( this
			, _("Save image")
			, "PNG (*.png)"
			, s_dir
      , &m_save_image_listener
      , DialogFileSelection::Save
			);
}

/* --------------------------------------------------------------------------- */
/* slot_reset_selection --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_reset_selection(){
  JobFunction *f = getOnViewAction();
  if( f ){
    DataReference *ref_string = DataPoolIntens::Instance().getDataReference("string_arg");
    if( ref_string ){
      XferDataItem *xfer_string = new XferDataItem( ref_string );
      XferDataItemIndex *index_string = xfer_string->newDataItemIndex(0);
      index_string->setIndex( ref_string, 0 );
      xfer_string->setValue("reset_selection");
      delete xfer_string;
      Trigger *trigger = new Trigger( f );
      trigger->startJob();
      // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getTitle --                                                            */
/* --------------------------------------------------------------------------- */

std::string GuiQtImage::getTitle(){
  std::string title;
  GuiForm * myform( getMyForm() );
  if( myform ){
    title = myform->getTitle();
  }
  return title;
}

/* --------------------------------------------------------------------------- */
/* slot_move_tip --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_move_tip(){
  m_canvas->setViewType( ImageView::e_tip );
}

/* --------------------------------------------------------------------------- */
/* slot_move_center --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_move_center(){
  m_canvas->setViewType( ImageView::e_center );
}

/* --------------------------------------------------------------------------- */
/* slot_rotate_fast --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_rotate_fast(){
  m_canvas->setViewType( ImageView::e_rotate_fast );
}

/* --------------------------------------------------------------------------- */
/* slot_rotate_slow --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_rotate_slow(){
  m_canvas->setViewType( ImageView::e_rotate_slow );
}

/* --------------------------------------------------------------------------- */
/* slot_select_area --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_select_area(){
  m_canvas->setViewType( ImageView::e_select_area );
}

/* --------------------------------------------------------------------------- */
/* slot_select_line --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_select_line(){
  m_canvas->setViewType( ImageView::e_select_line );
}

/* --------------------------------------------------------------------------- */
/* slot_add_point --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_add_point(){
  m_canvas->setViewType( ImageView::e_add_point );
  if (getPoints().size() > 0) {
	clearPoints();
	updateAll();
  }
}

/* --------------------------------------------------------------------------- */
/* slot_show_point --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_show_point(){
  if (getViewType() == ImageView::e_show_point) {
    m_canvas->setViewType( ImageView::e_none );
  } else {
    m_canvas->setViewType( ImageView::e_show_point );
  }
}

/* --------------------------------------------------------------------------- */
/* slot_delete_point --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_delete_point(){
  clearPoints();
  updateAll();
}

/* --------------------------------------------------------------------------- */
/* slot_Settings --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtImage::slot_Settings(){
  GuiMFMSettings &dialog = GuiMFMSettings::getDialog();

  dialog.openDialog( 0 );
  GuiForm *myform = getMyForm();
  std::string title;
  if( myform ){
    title = myform->getTitle();
  }
  dialog.setImage( this, title );
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtImage::serializeXML(std::ostream &os, bool recursive){
}

GuiQtImage::CanvasWidget::CanvasWidget(GuiQtImage *img, QWidget* par, const char* name )
  : QWidget() //Qt::WA_NoBackground|Qt::WA_StaticContents )
  , m_img(img){
  setMouseTracking(true);  // for tooltip, we need tracking events
}

GuiQtImage::CanvasWidget::~CanvasWidget(){
}

/* --------------------------------------------------------------------------- */
/* setContentsPos --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtImage::setContentsPos( double x, double y ){
  if( m_scroll_view != 0 ){
    int xx, yy;
    GuiImage::getSize( xx, yy );

    m_scroll_view -> blockSignals( true );
    QRect rect = m_scroll_view -> viewport()->geometry();
    rect.setX( int(x*xx) );
    rect.setY( int(y*yy)  );
    m_scroll_view -> viewport()->setGeometry( rect );
    m_scroll_view -> blockSignals( false );
  }
}

/* --------------------------------------------------------------------------- */
/* mousePressEvent --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtImage::CanvasWidget::mousePressEvent ( QMouseEvent * e ){
  QWidget::mousePressEvent(e);
  s_view.mousePressEvent( e, m_img );
  update();
//   m_img->paintAll();
}

/* --------------------------------------------------------------------------- */
/* mouseReleaseEvent --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtImage::CanvasWidget::mouseReleaseEvent ( QMouseEvent * e ){
  QWidget::mouseReleaseEvent(e);
  s_view.mouseReleaseEvent( e );
  if (s_view.getType() != ImageView::e_add_point &&
      s_view.getType() != ImageView::e_show_point) {
    unsetCursor();
  }
  update();
  //  repaint();
}

/* --------------------------------------------------------------------------- */
/* mouseMoveEvent --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtImage::CanvasWidget::mouseMoveEvent ( QMouseEvent * e ){
  QWidget::mouseMoveEvent(e);
  // tracking events
  if (e->buttons() == 0){
 	QString qsTp = m_img->trackerText(e->pos());
#if QT_VERSION >= 0x060000
  QPoint pos(e->globalPosition().toPoint());
#else
  QPoint pos(e->pos());
#endif
	QToolTip::showText(pos, qsTp);

	// alle images
	std::ostringstream os;
	std::vector<GuiImage*>::iterator iter;
	for( iter=getImages().begin();iter!=getImages().end();++iter ){
	  if (iter!=getImages().begin())
		os << ", ";
	  os << (**iter).getChannelName() << ": "
		 << dynamic_cast<GuiQtImage*>(*iter)->trackerText(e->pos()).toStdString();
	}
	//	m_img->setHelptext(os.str());
	m_img->printMessage(os.str(), msg_Information, 10);
	return;
  }
#if QT_VERSION >= 0x060000
  QPoint pos(e->globalPosition().toPoint());
#else
  QPoint pos(e->pos());
#endif
  if (QToolTip::isVisible())
    QToolTip::showText(pos, "");

  s_view.mouseMoveEvent( e, m_img );
  update();
//  dynamic_cast<GuiQtImage*>( m_img )->myWidget()->repaint();
//   m_img->paintAll();
}

/* --------------------------------------------------------------------------- */
/* contextMenuEvent --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtImage::CanvasWidget::contextMenuEvent ( QContextMenuEvent * e ){
  if (s_view.getType() != ImageView::e_add_point) { // don't show menu in add point mode
    m_img->contextMenuEvent( e );
  }
}

void GuiQtImage::contextMenuEvent ( QContextMenuEvent * e ){
  QMenu* contextMenu = new QMenu( m_canvas );
  Q_CHECK_PTR( contextMenu );

  contextMenu->addAction( _("View settings"), this, SLOT(slot_Settings()) );

  if( getScanHeight() > 0.9 ){
    // add all actions, enabled or disabled
    contextMenu->addAction( _("Move tip"), this, SLOT(slot_move_tip()) )->setEnabled(eos());
    contextMenu->addAction( _("Move center"), this, SLOT(slot_move_center()) )->setEnabled(eos() || imageScan());
    contextMenu->addAction( _("Rotate to fast axis"), this, SLOT(slot_rotate_fast()) )->setEnabled(imageScan());
    contextMenu->addAction( _("Rotate to slow axis"), this, SLOT(slot_rotate_slow()) )->setEnabled(imageScan());
    contextMenu->addAction( _("Select area"), this, SLOT(slot_select_area()) )->setEnabled(eos() || imageScan());
    contextMenu->addAction( _("Reset selection"), this, SLOT(slot_reset_selection()) )->setEnabled(eos());
    contextMenu->addAction( _("Select line"), this, SLOT(slot_select_line()) )->setEnabled(eos() && imageScan());
    contextMenu->addAction( _("Set master"), this, SLOT(slot_Set_master()) )->setEnabled(eos());
    contextMenu->addAction( _("Get master"), this, SLOT(slot_Get_master()) )->setEnabled(eos() && s_hasMaster);
    contextMenu->addAction( _("Save image"), this, SLOT(slot_saveImage()) )->setEnabled(eos());
    // contextMenu->addSeparator();
    // separator does not show on windows in style plastique or cleanlooks
    // -> submenu could be used, but that requires two clicks
    contextMenu->addAction( _("Add points"), this, SLOT(slot_add_point()) )->setEnabled(eos() && imageScan());
    QAction *showPointsAction = contextMenu->addAction( _("Show points"), this, SLOT(slot_show_point()) );
    showPointsAction->setCheckable(true);
    showPointsAction->setChecked(getViewType() == ImageView::e_show_point);
    contextMenu->addAction( _("Delete points"), this, SLOT(slot_delete_point()) )->setEnabled(getPoints().size() > 0);
  }

  contextMenu->exec( QCursor::pos() );
  delete contextMenu;
}


/* --------------------------------------------------------------------------- */
/* setPoint --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtImage::setPoint(const QPoint& p, bool allowDuplicate) {
  if (allowDuplicate) {
	points.push_back(p);
  } else {
	// no duplicates
	std::list<QPoint>::iterator it = std::find(points.begin(), points.end(), p);
	if (it == points.end())
	  points.push_back(p);
  }
}

/* --------------------------------------------------------------------------- */
/* keyPressEvent --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtImage::CanvasWidget::keyPressEvent(QKeyEvent* e) {
  switch (e->key()) {
  case Qt::Key_Delete:
  case Qt::Key_Backspace:
	if (getViewType() == ImageView::e_add_point ||
		getViewType() == ImageView::e_show_point) {
	GuiQtImage::clearLastPoint();
	GuiQtImage::updateAll();
	}
	break;
  }
  QWidget::keyPressEvent(e);
}

/* --------------------------------------------------------------------------- */
/* updateAll --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtImage::updateAll() {
  std::vector<GuiImage*>::iterator iter;
  for( iter=getImages().begin();iter!=getImages().end();++iter ){
	if(! (*iter)->getElement()->getQtElement()->myWidget())
	  continue;  // widget not (yet) created, nothing to be done

	(*iter)->getElement()->getQtElement()->myWidget()->update();
#if defined Q_OS_WIN || defined Q_OS_CYGWIN
	// unter windows funktioniert kein update() oder repaint()
	// deshalb müssen wir ein updateGeometry erzwingen
	// damit ein repaint() gemacht wird
	QSize s((*iter)->getElement()->getQtElement()->myWidget()->size());
	(*iter)->getElement()->getQtElement()->myWidget()->resize(s.height()-1, s.width()-1);
	(*iter)->getElement()->getQtElement()->myWidget()->resize(s);
	(*iter)->getElement()->getQtElement()->myWidget()->updateGeometry();
#endif
  }
}

/* --------------------------------------------------------------------------- */
/* trackerText --                                                              */
/* --------------------------------------------------------------------------- */

QString GuiQtImage::trackerText(const QPoint& pos ) {
    const int markerAreaWidth = getMarkerAreaWidth();
	int imageHeight = int(ceil(getImageHeight()/getScaleY()));
	int ypos = imageHeight - int(floor(pos.y()/getScaleY()+1.01));
    int xpos = int(floor((pos.x()-markerAreaWidth)/getScaleX()+0.01));
	// outside ?
	if (xpos < 0 || xpos >= getScanWidth() ||
		ypos < 0 || ypos >= getImageHeight())
	  return QString();

	std::ostringstream os;
	// os << "X: " << xpos << "  Y: " << ypos << " ";
	const MFMImageMapper::ImageData *image_data = MFMImageMapper::getImageData(this);
	if (!image_data) {
	  return QString::fromStdString("no Data");
	}
	MFMImageMapper::ImageData::Image::const_iterator it = image_data->find(ypos);
	if(it != image_data->end() && xpos < it->second.size()){
	  os << std::setprecision(3) << std::fixed; // drei Nachkommastellen
	  os << it->second[xpos] * getChannelUnitFactor() << " " << getChannelUnit();
	}
	return QString::fromStdString(os.str());
}

/* --------------------------------------------------------------------------- */
/* ImageView::start --                                                         */
/* --------------------------------------------------------------------------- */

void ImageView::start( const QPoint &p ){
  switch( m_type ){
  case e_tip :
  case e_add_point:
  case e_show_point:
  case e_center :
    m_move_type = m_type;
    m_point = p;
    break;
  case e_select_area :
  case e_select_line :
    m_zoom_type = m_type;
    m_zoom_p1 = m_zoom_p2 = p;
    break;
  case e_rotate_fast :
  case e_rotate_slow :
    m_rotate_type = m_type;
    m_rotate_p1 = m_rotate_p2 = p;
  default:
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* ImageView::paint --                                                         */
/* --------------------------------------------------------------------------- */

void ImageView::paint( GuiQtImage *img, QPainter& p  ){
//    BUG_DEBUG( "ImageView::paint begin" );
  drawPoint( img, p );
  drawZoom( img, p );
  drawLine( img, p );
  drawRectangle(img, p);
//    BUG_DEBUG( "ImageView::paint end" );
}

/* --------------------------------------------------------------------------- */
/* ImageView::drawPoint --                                                     */
/* --------------------------------------------------------------------------- */

void ImageView::drawPoint( GuiQtImage *img, QPainter& painter  ){
  switch( m_move_type ){
  case e_tip :
  case e_center :{
    double scale_x = img->getScaleX();
    double scale_y = img->getScaleY();
    const int markerAreaWidth = img->getMarkerAreaWidth();
    QPoint point( int(m_point.x()*scale_x)+markerAreaWidth, int(m_point.y()*scale_y) );

    painter.setPen( GuiQtManager::imageHighlightColor() );
    painter.setBrush( GuiQtManager::imageHighlightColor() );
    painter.drawPoint( point );

    break;
  }
  default :
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* ImageView::drawRectangle                                                    */
/* Draw the selected point after add point was choosen in the context menu     */
/* --------------------------------------------------------------------------- */
void ImageView::drawRectangle( GuiQtImage *img, QPainter& painter ) {

  // Get the point list
  std::list<QPoint> &p = img->getPoints();
  // if the size is more than one
  if(p.size() > 0 ) {
     painter.setPen( GuiQtManager::imageHighlightColor() );
     painter.setBrush( GuiQtManager::imageHighlightColor() );
     double scale_x = img->getScaleX();
     double scale_y = img->getScaleY();
     const int markerAreaWidth = img->getMarkerAreaWidth();

     std::list<QPoint>::iterator it;
     for (it=p.begin(); it!=p.end(); it++) {
       // draw e recangle on position x, y, width, height
       painter.drawRect( (floor((it->x()+0.5)*scale_x+0.5))+markerAreaWidth-1,
						 (floor((it->y()-0.5)*scale_y+0.5)-1), 2, 2 );
     }
   }
}

/* --------------------------------------------------------------------------- */
/* ImageView::drawLine --                                                      */
/* --------------------------------------------------------------------------- */

void ImageView::drawLine( GuiQtImage *img, QPainter& painter  ){
  switch( m_rotate_type ){
  case e_rotate_fast :
  case e_rotate_slow :{
    double scale_x = img->getScaleX();
    double scale_y = img->getScaleY();
    const int markerAreaWidth = img->getMarkerAreaWidth();
    QPoint rotate_p1( int(m_rotate_p1.x()*scale_x)+markerAreaWidth, int(m_rotate_p1.y()*scale_y) );
    QPoint rotate_p2( int(m_rotate_p2.x()*scale_x)+markerAreaWidth, int(m_rotate_p2.y()*scale_y) );

    painter.setPen( GuiQtManager::imageHighlightColor() );
    painter.setBrush( GuiQtManager::imageHighlightColor() );
    painter.drawLine( rotate_p1, rotate_p2 );
    break;
  }
  default :
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* ImageView::drawZoom --                                                      */
/* --------------------------------------------------------------------------- */

void ImageView::drawZoom( GuiQtImage *img, QPainter& painter  ){
  double scale_x = img->getScaleX();
  double scale_y = img->getScaleY();
  const int markerAreaWidth = img->getMarkerAreaWidth();

  switch( m_zoom_type ){
  case e_select_area :{
    QPoint zoom_p1( int(floor((m_zoom_p1.x()+0.5)*scale_x)+0.5)+markerAreaWidth, int((m_zoom_p1.y()-0.5)*scale_y) );
    QPoint zoom_p2( int(m_zoom_p2.x()*scale_x)+markerAreaWidth, int(m_zoom_p2.y()*scale_y) );

    painter.setPen( GuiQtManager::imageHighlightColor() );
    painter.setBrush( GuiQtManager::imageHighlightColor() );
    QPoint p = zoom_p1 - zoom_p2;
    QPoint p1 = zoom_p1 + p;
    painter.drawLine( p1, QPoint( p1.x(), zoom_p2.y() ) );
    painter.drawLine( p1, QPoint( zoom_p2.x(), p1.y() ) );
    painter.drawLine( zoom_p2, QPoint( p1.x(), zoom_p2.y() ) );
    painter.drawLine( zoom_p2, QPoint( zoom_p2.x(), p1.y() ) );

    break;
  }
  case e_select_line :{
    QPoint zoom_p1( int(m_zoom_p1.x()*scale_x)+markerAreaWidth, int(m_zoom_p1.y()*scale_y) );
    QPoint zoom_p2( int(m_zoom_p2.x()*scale_x)+markerAreaWidth, int(m_zoom_p2.y()*scale_y) );

    painter.setPen( GuiQtManager::imageHighlightColor() );
    painter.setBrush( GuiQtManager::imageHighlightColor() );
    painter.drawLine( zoom_p1, zoom_p2 );

    break;
  }
  default :
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* mousePressEvent --                                                          */
/* --------------------------------------------------------------------------- */

void ImageView::mousePressEvent ( QMouseEvent * e, GuiQtImage *img ){
  if( e->button() == Qt::LeftButton ){
    double scale_x = img->getScaleX();
    double scale_y = img->getScaleY();
    int h, w, sh, sw;
    img->getImageSize(w, h);
    img->getScanSize(sw, sh);
    const int markerAreaWidth = img->getMarkerAreaWidth();
// y imageheight - scanheight
    int ypos = int(ceil(e->pos().y()/scale_y));

    BUG_DEBUG( "GuiQtImage::mousePressEvent ypos " << e->pos().y()
		   << " imageHeight " << h << " scanHeight " << sh
		   << " --> " << ypos -(h-sw));
    start( QPoint( int((e->pos().x()-markerAreaWidth)/scale_x), ypos) );
    // Add point to image list
    if( m_type ==  e_add_point || m_type ==  e_show_point) {
      img->setPoint( QPoint( int((e->pos().x()-markerAreaWidth)/scale_x), ypos),
					 m_type == ImageView::e_add_point );
      GuiQtImage::updateAll();
    }
  } else if (m_type !=  e_add_point && m_type !=  e_show_point) { // show context menu and clear
    clear();
  } else  if (m_type ==  e_add_point ) { // This must be a right click. And we are in 'add point' mode-> do not show the contextMenu
    JobFunction *f = img->getOnViewAction();
    if( f ) {
      DataReference *ref_string = DataPoolIntens::Instance().getDataReference("string_arg");
	  DataReference *ref_int = DataPoolIntens::Instance().getDataReference("int_arg");
	  DataVector vectorPoints;
      int imageHeight = int(ceil(img->getImageHeight()/img->getScaleY()));
	  int id[1] = { -1  };

      if( ref_string && ref_int){
        XferDataItem *xfer_string = new XferDataItem( ref_string );
        XferDataItemIndex *index_string = xfer_string->newDataItemIndex(0);
        index_string->setIndex( ref_string, 0 );
        xfer_string->setValue("save_points");

		// set points
		std::list<QPoint> &p = img->getPoints();
		std::list<QPoint>::iterator it;
		for (it=p.begin(); it!=p.end(); it++) {
		  DataValue::Ptr ptr = new DataIntegerValue( it->x());
		  vectorPoints.appendValue( ptr );
		  ptr = new DataIntegerValue( imageHeight-it->y() );
		  vectorPoints.appendValue( ptr );
		}
		ref_int->setDataVector( vectorPoints, 1, id );

        delete xfer_string;
        delete ref_int;
        Trigger *trigger = new Trigger( f );
        trigger->startJob();
        // Clear the selected points after save job is complete
        img->clearPoints();
        GuiQtImage::updateAll();
      }
    }
    clear();
    img->canvas()->unsetCursor();
  }
}

/* --------------------------------------------------------------------------- */
/* mouseReleaseEvent --                                                        */
/* --------------------------------------------------------------------------- */

void ImageView::mouseReleaseEvent ( QMouseEvent * e ){

  if( m_type == e_none ){
    return;
  }

  JobFunction *f = GuiImage::getOnViewAction();

  if( f ){
    DataReference *ref_string = DataPoolIntens::Instance().getDataReference("string_arg");
    DataReference *ref_int = DataPoolIntens::Instance().getDataReference("int_arg");
    DataReference *ref_real = DataPoolIntens::Instance().getDataReference("real_arg");
    if( ref_string != 0 && ref_int != 0 && ref_real != 0 ){
      XferDataItem *xfer_string = new XferDataItem( ref_string );
      XferDataItemIndex *index_string = xfer_string->newDataItemIndex(0);
      index_string->setIndex( ref_string, 0 );

      XferDataItem *xfer_int = new XferDataItem( ref_int );
      XferDataItemIndex *index_int = xfer_int->newDataItemIndex(0);
      index_int->setIndex( ref_int, 0 );

      XferDataItem *xfer_real = new XferDataItem( ref_real );
      XferDataItemIndex *index_real = xfer_real->newDataItemIndex(0);
      index_real->setIndex( ref_real, 0 );

      GuiImage *currentImage = GuiImage::currentImage();
      assert( currentImage != 0 );
      int imageHeight = int(ceil(currentImage->getImageHeight()/currentImage->getScaleY()));

      switch( m_type ){
      case e_tip :
	xfer_string->setValue("tip");
	index_int->setIndex(ref_int, 0);
	xfer_int->setValue( m_point.x() );
	index_int->setIndex(ref_int, 1);
	xfer_int->setValue( imageHeight-m_point.y() );
	break;
      case e_center :
	xfer_string->setValue("center");
	index_int->setIndex(ref_int, 0);
	xfer_int->setValue( m_point.x() );
	index_int->setIndex(ref_int, 1);
	xfer_int->setValue( imageHeight-m_point.y() );
	break;
      case e_select_area :
	xfer_string->setValue("select_area");
	index_int->setIndex(ref_int, 0);
	xfer_int->setValue( m_zoom_p1.x() );
	index_int->setIndex(ref_int, 1);
	xfer_int->setValue( imageHeight-m_zoom_p1.y() );
	index_int->setIndex(ref_int, 2);
	xfer_int->setValue( m_zoom_p2.x() );
	index_int->setIndex(ref_int, 3);
	xfer_int->setValue( imageHeight-m_zoom_p2.y() );
	GuiQtImage::updateAll();
	break;
      case e_select_line :
	xfer_string->setValue("select_line");
	index_int->setIndex(ref_int, 0);
	xfer_int->setValue( m_zoom_p1.x() );
	index_int->setIndex(ref_int, 1);
	xfer_int->setValue( imageHeight-m_zoom_p1.y() );
	index_int->setIndex(ref_int, 2);
	xfer_int->setValue( m_zoom_p2.x() );
	index_int->setIndex(ref_int, 3);
	xfer_int->setValue( imageHeight-m_zoom_p2.y() );
	GuiQtImage::updateAll();
	break;
      case e_rotate_fast :
	xfer_string->setValue("rotate_fast");
	index_int->setIndex(ref_int, 0);
	xfer_int->setValue( m_rotate_p1.x() );
	index_int->setIndex(ref_int, 1);
	xfer_int->setValue( imageHeight-m_rotate_p1.y() );
	index_int->setIndex(ref_int, 2);
	xfer_int->setValue( m_rotate_p2.x() );
	index_int->setIndex(ref_int, 3);
	xfer_int->setValue( imageHeight-m_rotate_p2.y() );
	break;
      case e_rotate_slow :{
	xfer_string->setValue("rotate_slow");
	index_int->setIndex(ref_int, 0);
	xfer_int->setValue( m_rotate_p1.x() );
	index_int->setIndex(ref_int, 1);
	xfer_int->setValue( imageHeight-m_rotate_p1.y() );
	index_int->setIndex(ref_int, 2);
	xfer_int->setValue( m_rotate_p2.x() );
	index_int->setIndex(ref_int, 3);
	xfer_int->setValue( imageHeight-m_rotate_p2.y() );
	break;
      }
      case e_add_point : {
	xfer_string->setValue("add_point");
	index_int->setIndex(ref_int, 0);
	xfer_int->setValue( m_point.x() );
	index_int->setIndex(ref_int, 1);
	xfer_int->setValue( imageHeight-m_point.y() );
  break;
	  }
      case e_show_point :
		break;
      default:
	assert( false );
      }
      delete xfer_string;
      delete xfer_int;
      delete xfer_real;
	  if (m_type != e_show_point) {
		Trigger *trigger = new Trigger( f );
		trigger->startJob();
		// Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
	  }
    }
  }
  if (  m_type != e_add_point && m_type != e_show_point) {
    clear();
  }

}

/* --------------------------------------------------------------------------- */
/* mouseMoveEvent --                                                           */
/* --------------------------------------------------------------------------- */

void ImageView::mouseMoveEvent ( QMouseEvent * e, GuiQtImage *img ){
  double scale_x = img->getScaleX();
  double scale_y = img->getScaleY();
  const int markerAreaWidth = img->getMarkerAreaWidth();

  int ypos = int(ceil(e->pos().y()/scale_y));

  switch( m_type ){
  case e_select_area :
  case e_select_line :
    setZoom( QPoint( int((e->pos().x()-markerAreaWidth)/scale_x), ypos ) );
    GuiQtImage::updateAll();
    break;
  case e_rotate_fast :
  case e_rotate_slow :
    setLine( QPoint( int((e->pos().x()-markerAreaWidth)/scale_x), ypos ) );
    break;
  default:
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* SaveFileListener --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtImage::SaveFileListener::FileSelected( const std::string &filename
							, const HardCopyListener::FileFormat format
							, const std::string dir ){
	if ( !m_image->saveImage( filename, dir ) ) {
		GuiFactory::Instance()->showDialogInformation
			( 0
				, "Save image failed"
				, "The image could not be saved as \n" + filename
				, 0
				);
	}
}

/* --------------------------------------------------------------------------- */
/* s_dir --                                                                    */
/* --------------------------------------------------------------------------- */
std::string GuiQtImage::s_dir(".");
