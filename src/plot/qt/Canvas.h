
#ifndef CANVAS_H
#define CANVAS_H
/** Canvas
    Die Klasse Canvas stellt ein Widget zur grafischen Ausgabe
    zur Verfügung.
*/

#include <string>
#include <QObject>

#include "plot/Gdev.h"

class RedisplayListener;
class QWidget;
class QPixmap;
class GuiPopupMenu;
class QContextMenuEvent;
class QtCanvas;
class PlotCanvasView;

class iCanvas : public QObject {
    Q_OBJECT
 public:
  /**@name methods */
  //@{
  /** Konstruktor
      @param name Name
      @param rlistener RedisplayListener-Objekt
  */
  iCanvas( const std::string &name, RedisplayListener *rlistener, int width = 297, int height = 210 );
  /// Destruktor
  virtual ~iCanvas();
  /** erzeugt das Widget und ein GdevQt-Objekt und setzt
      die Redisplay/Resize-Callbacks
      @param parent Parent-Widget
  */
  Gdev &gdev(){ return *m_gdev; }
  void create();
  void getSize( int &w, int &h );
  Gdev::Orientation getOrientation();
  QWidget* myWidget();
  /** aendert die Grösse der Zeichenfläche
      @param w,h Breite und Höhe in Pixel
  */
  void setDrawingAreaSize( int w, int h );
  void setTitle( const std::string &title ) { m_title = title; }
  //@}
  void repaint();
  /** setzt ein popup menu
      @param menu popup memu
  */
  void setPopupMenu(GuiPopupMenu* menu);
  /** popup menu */
  GuiPopupMenu* getPopupMenu();

 public slots:
  void resizedSlot();

 private:
  RedisplayListener *m_rlistener;
  Gdev *m_gdev;
  QPixmap *m_pixmap;
  int m_width, m_height;
  int m_xmin, m_ymin;
  int m_xmax, m_ymax;
  std::string m_title;

  QtCanvas   *m_darea;
  PlotCanvasView *m_canvasView;
  QWidget*    m_form;

};

#include "qtcanvas.h"
class PlotCanvasView : public QtCanvasView {

 public:
  PlotCanvasView(QtCanvas *canvas, QWidget* parent, bool resizeable);

  /** setzt ein popup menu
      @param menu popup memu
  */
  void setPopupMenu(GuiPopupMenu* menu);
  /** popup menu */
  GuiPopupMenu* getPopupMenu();

 private:
  void contextMenuEvent(QContextMenuEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  GuiPopupMenu  *m_menu;
};


#endif
