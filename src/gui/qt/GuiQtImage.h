
#if !defined(GUI_QT_IMAGE_H)
#define GUI_QT_IMAGE_H

#include <qwidget.h>
#include <qobject.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qlabel.h>

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiImage.h"
#include "gui/qt/GuiMFMSettings.h"
#include "gui/FileSelectListener.h"

class QImage;
class GuiMFMSettings;
class QScrollArea;
class GuiQtImage;
class CanvasWidget;
class QLineEdit;

// ******************************************************************
// Class ImageView
// ******************************************************************

class ImageView{
public:
  enum view_type{
    e_none
    , e_tip
    , e_center
    , e_select_area
    , e_select_line
    , e_rotate_fast
    , e_rotate_slow
    , e_add_point
    , e_show_point
  };

  // -----------------------------------------------------
  // private definitions
  // -----------------------------------------------------
  class Trigger : public JobStarter
  {
  public:
  Trigger( JobFunction *f ) : JobStarter( f ) {}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt ){}
  };

  // ------------------------
  // Constructor / Destructor
  // ------------------------
  ImageView(){
    clear();
  }
  virtual ~ImageView(){}

  // ------------------------
  // public functions
  // ------------------------
public:
  void clear(){
    m_type = e_none;
    m_move_type = e_none;
    m_zoom_type = e_none;
    m_rotate_type = e_none;
  }
  void setType( view_type vt ){
    m_type=vt;
  }

  view_type getType() {
    return m_type;
  }

  void start( const QPoint &p );
  void setLine( const QPoint &p ){
    m_rotate_p2 = p;
  }
  static void setZoom( const QPoint &p ){
    m_zoom_p2 = p;
  }
  void paint( GuiQtImage *, QPainter& p );
  void mousePressEvent ( QMouseEvent * e, GuiQtImage *img );
  void mouseMoveEvent ( QMouseEvent * e, GuiQtImage *img );
  void mouseReleaseEvent ( QMouseEvent * e );

  // -------------------------
  // private members
  // -------------------------
private:
  void drawZoom( GuiQtImage *, QPainter& p );
  void drawPoint( GuiQtImage *, QPainter& p);
  void drawLine( GuiQtImage *, QPainter& p);
  void drawRectangle( GuiQtImage *, QPainter& p);

  QPoint m_point;
  static QPoint m_zoom_p1;
  static QPoint m_zoom_p2;
  QPoint m_rotate_p1;
  QPoint m_rotate_p2;
  view_type m_type;
  view_type m_zoom_type;
  view_type m_rotate_type;
  view_type m_move_type;
};

// *********************************************************************
// Class GuiQtImage
// *********************************************************************

class GuiQtImage : public GuiQtElement , public GuiImage
 {
   Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
 public:
   GuiQtImage( GuiElement *parent, const std::string &name, int frame_size );
   virtual ~GuiQtImage();

 private:
   GuiQtImage( const GuiQtImage &i );
   GuiQtImage &operator=( const GuiQtImage &i );

  // -----------------------------------------------------
  // private definitions
  // -----------------------------------------------------
   class Trigger : public JobStarter
   {
   public:
     Trigger( JobFunction *f )
       : JobStarter( f ){}
     virtual ~Trigger() {}
     virtual void backFromJobStarter( JobAction::JobResult rslt ){}
   };

   class CanvasWidget : public QWidget {
   public:
     CanvasWidget(GuiQtImage *img, QWidget* par, const char* name=0);

     virtual ~CanvasWidget();

     void setViewType( ImageView::view_type vt ){
       if ( vt == ImageView::e_none ) {
         unsetCursor();
       } else {
         setCursor( Qt::CrossCursor );
       }
       s_view.setType( vt );
     }
     ImageView::view_type getViewType(){
       return s_view.getType();
     }
     void paintView( GuiQtImage *img, QPainter& p){
       s_view.paint( img, p );
     }
     void clearView(){
       s_view.clear();
     }

   protected:
     virtual void paintEvent( QPaintEvent *e ) {
       //       QWidget::paintEvent( e );
	 m_img->paintEvent( e );
     }
     virtual void focusInEvent ( QFocusEvent *e ){
       m_img->focusInEvent( e );
       QWidget::focusInEvent( e );
     }
     virtual void contextMenuEvent ( QContextMenuEvent * e );

     virtual void mousePressEvent ( QMouseEvent * e );
     virtual void mouseMoveEvent ( QMouseEvent * e );
     virtual void mouseReleaseEvent ( QMouseEvent * e );
	 virtual void keyPressEvent(QKeyEvent* e);
  private:
     GuiQtImage        *m_img;
     static ImageView   s_view;

   };

  class SaveFileListener : public FileSelectListener{
  public:
    SaveFileListener( GuiQtImage *image ): m_image( image ) {}
    virtual void FileSelected( const std::string &filename
			       , const HardCopyListener::FileFormat format
			       , const std::string dir );
  private:
    GuiQtImage  *m_image;
  };

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
   virtual GuiElement::ElementType Type() { return GuiElement::type_Image; }
   virtual void create();
   virtual void manage();
   virtual void unmanage();
   virtual bool destroy();
   virtual void resize( int w, int h );
   virtual void resize();
   virtual QWidget* myWidget();

   virtual GuiElement * getElement() { return this; }

   virtual void getSize( int &x, int &y );
   virtual void update( UpdateReason );
   /** change indicator */
   virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
   virtual void serializeXML(std::ostream &os, bool recursive = false);
/*=============================================================================*/
/* public slots                                                                */
/*=============================================================================*/
  public slots:
   void slot_Settings();
   void slot_move_tip();
   void slot_move_center();
   void slot_rotate_fast();
   void slot_rotate_slow();
   void slot_select_area();
   void slot_select_line();
   void slot_reset_selection();
   void slot_Set_master();
   void slot_Get_master();
   void slot_saveImage();
   void slot_contentsMoving( int x, int y );
   void slot_add_point();
   void slot_show_point();
   void slot_delete_point();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
 public:
   virtual void setDialogImage(){
     GuiMFMSettings::getDialog().setImage( this, getTitle() );
   }
   virtual void rebuildDialogChannelBox(){
     GuiMFMSettings::getDialog().rebuildChannelBox();
   }
   virtual void updateDialogUnit(){
     GuiMFMSettings::getDialog().setUnits( getChannelUnit() );
   }
   virtual std::string getTitle();
   virtual void setRange( bool, double, double );
   virtual void setFocus( bool );
   virtual void clear( bool clearImages );
   void focusInEvent( QFocusEvent * );
   void paintEvent( QPaintEvent * );
//   virtual void paint();
   virtual void setLine( int id, int line,
			const MFMImageMapper::ImageData::Line &data,
			 const MFMImageMapper::ImageData *img );
   virtual void showImage( const MFMImageMapper::ImageData *img, int id );
   void setViewType( ImageView::view_type vt ){
     m_canvas->setViewType( vt );
   }
   ImageView::view_type getViewType(){
     return m_canvas->getViewType();
   }
   QWidget *canvas(){ return m_canvas; }
   QScrollArea *scroll_view(){ return m_scroll_view; }
   virtual void clearView(){
     if( m_canvas ){
       m_canvas->clearView();
     }
   }
   virtual void updateLabel();
   virtual void updateUnit(){}
   virtual void contextMenuEvent ( QContextMenuEvent * e );
   virtual void setContentsPos( double x, double y );
   void drawMarker();
   static const int getMarkerAreaWidth(){
     return s_markerWidth + s_markerSpacing;
   };

   static void setPoint(const QPoint& p, bool allowDuplicate);

   static std::list<QPoint>& getPoints() {
     return points;
   }

   static void clearPoints() {
     points.clear();
   }

   static void clearLastPoint() {
	 if (!points.empty())
	   points.pop_back();
   }

   static void updateAll();
   QString trackerText(const QPoint& pos );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
 private:
   void show( const MFMImageMapper::ImageData *img );
   virtual bool updateImageStatistics( const MFMImageMapper::ImageData *img );
   void createImageAndPainter();
	 bool saveImage ( const std::string &filename, const std::string &dir );

 private:
   QWidget          *m_frame;
   QWidget          *m_sep;
   QLineEdit           *m_label;
   CanvasWidget     *m_canvas;
   QScrollArea      *m_scroll_view;
   QImage	    *m_image;	    // the displayed image
   QPainter          m_painter;

   int               m_bottom;
   int               m_current_line;
   int               m_y; // y of line last drawn

   static bool       s_hasMaster;
   static const int  s_markerWidth;
   static const int  s_markerSpacing;

   QColor            m_defaultTitleBackgroundColor;
   QColor            m_defaultTitleForegroundColor;

   SaveFileListener  m_save_image_listener;
	 static std::string s_dir;

   static std::list<QPoint> points;
};

#endif
