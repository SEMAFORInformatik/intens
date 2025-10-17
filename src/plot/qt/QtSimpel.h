
#ifndef MOTIF_SIMPEL_UNIPLOT_H
#define MOTIF_SIMPEL_UNIPLOT_H

/** Simpel
    Die Klasse Simpel sorgt für die Ausgabe von BBC-Uniplot Grafikdaten
    auf ein Grafikdevice
*/

#include <stdio.h>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <assert.h>

#include "datapool/DataReference.h"
#include "gui/GuiElement.h"
#include "app/HardCopyListener.h"
#include "gui/GuiPrinterDialog.h"
#include "plot/RedisplayListener.h"
#include "plot/qt/QtListPlot.h"
#include "streamer/Stream.h"
#include "gui/GuiMenuButtonListener.h"
#include "plot/Simpel.h"

// 2011-01-13 amg von tracomo kommt es in als 4-byte
// int ist auf 32 & 64 Bit OS immer 4-byte
// waehrend
// long ist auf 32 OS 4-byte UND auf 32 OS 8-byte
#define FLONG int
//long

class iCanvas;
class Gdev;
class OutputChannelEvent;
class InputChannelEvent;
class GuiEventData;
class QWidget;

class QtSimpel : public Simpel, public QtListPlot, public BasicStream
{
 public:
  struct Frame {
    Frame( double a0, double a1, double b0, double b1 ) {
      x0 = a0; x1 = a1; y0 = b0; y1 = b1; }
    void setValues( double a0, double a1, double b0, double b1 ) {
      x0 = a0; x1 = a1; y0 = b0; y1 = b1; }
    double x0,x1,y0,y1;
  };

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class PopUpMenu : public GuiMenuButtonListener
    {
  public:
    PopUpMenu( QtSimpel *simpel )
      : m_simpel( simpel )
      , m_popUpMenu( 0 )
      , m_nextPageBtn( 0 )
      , m_prevPageBtn( 0 ){}
    virtual ~PopUpMenu() {}
    virtual void ButtonPressed( GuiEventData *event );
    void create();
    virtual JobAction* getAction() { return 0; }
 private:
    GuiQtPopupMenu  *m_popUpMenu;
    GuiQtMenuButton *m_nextPageBtn;
    GuiQtMenuButton *m_prevPageBtn;
    GuiQtMenuButton *m_printPageBtn;
    QtSimpel             *m_simpel;
  };

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param name Name des Simpelplots
      @param dref Datenreferenz
  */
  QtSimpel( GuiElement *parent, const std::string &name );
  /// Destruktor
  virtual ~QtSimpel();

 private:
/*   QtSimpel( const QtSimpel &s ); */

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

  virtual ListPlot *getListPlot() { return this; }
  virtual BasicStream *getBasicStream() { return this; }

  /// Basisfunktion fuer den Gui-Manager
  virtual GuiElement::ElementType Type();
  /// Basisfunktion fuer den Gui-Manager
  virtual void manage();
  /** Basisfunktion fuer den Gui-Manager
      @param parent Widget
  */
  virtual void create();
  /** Basisfunktion fuer den Gui-Manager
      @param width,height Abmessungen vom Ausgabebereich
  */
  virtual void getSize( int &width, int &height );
  /** sorgt für die Anzeige der Grafikdaten
      @param override Override Ereignis
  */
  virtual void update( UpdateReason override );
  /** Liefert einen Pointer auf das benutzte Widget
  */
  virtual QWidget* myWidget();

  // Ueberladene Funktionen der Klasse 'HardCopyListener'
  virtual HardCopyListener::FileFormat getFileFormat();
  virtual const std::string &getName(){ return m_name; }
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &MenuLabel();

  /** Basisfunktion für den ChannelListener
   */
  virtual bool write( InputChannelEvent &event );
  virtual bool read( OutputChannelEvent & );
  virtual void resetContinuousUpdate(){};
  virtual bool setContinuousUpdate( bool flag ){ return false; };

  /** RedisplayListener-Interface.
      schreibt die Grafik auf das Device
      @param g Grafikdevice
  */
  virtual void repaint( Gdev &gdev, bool allPages = false );
  /** RedisplayListener-Interface.
      schreibt das Pixmap auf das Device
      @param g Grafikdevice
  */
  virtual void refresh( Gdev &gdev );
  /** Gibt alle Seiten aus
  */
  void printAllPages( Gdev &gdev );
  /** Gibt den Simpel-Plot auf dem duch den Dateiname spezifizierten
      Geraet aus
      @param filename Name der zu druckender Datei
  */
  void plotSimpelFile( Gdev &gdev, std::string &s );
  void plotSimpelFile( std::string &s );
  void setTitle( const std::string &title ) { m_title = title; }

  virtual int getParameters( std::ostream &os ) const { return 0; }
  virtual void getValues( StreamSource *src ) {}
  virtual void clearRange();
  void prevPage();
  void nextPage();
  virtual int pages(){ return m_pageCount; }
  void stretchText( double stretch );

 private:
  bool streamRead( char *c, int size, int n, std::string &s );

  bool get( float *, std::string &s );
  bool get( int *, std::string &s );
  bool get( char *, int len, std::string &s );

  void endPlot( Gdev &gdev );
  void executePlot( Gdev &gdev, std::string &s );
  void executePlot( std::string &s );
  void setXMinMax( double x );
  void setYMinMax( double y );

  int plotSimpel( Gdev &gdev, std::string &s );
  int plotSimpel( std::string &s );
  void newPage( Gdev &gdev );
  bool setPage( Gdev &gdev );
  void putString( Gdev &gdev, double x, double y
		, double angle, double hs
		, int len, char* buf );

   void siopen();
   int sihead( std::string &s, FLONG *type, FLONG *num );
   int siscale( std::string &s, float *x0, float *y0, float *amx, float *amy
		    ,float *xmax, float *ymax, FLONG *mr, FLONG *jr );
   int sipoly( std::string &s, FLONG *ks, FLONG *kp, FLONG *ir
		   , FLONG *m, float *x, float *y );
   int sipolyf( std::string &s, FLONG *ks, FLONG *kf, FLONG *kp
		    , FLONG *kd, FLONG *ir, FLONG *m, float *x, float *y );
   int sinum( std::string &s, float *xt, float *yt, float *ra, FLONG *ia
		  , FLONG *j, FLONG *n, FLONG *m, float *hs, float *ws, FLONG *kp );
   int sitext( std::string &s, float *xt, float *yt
		   , float *hs, float *ws, FLONG *kp, FLONG *m, char *text );
   int siarc( std::string &s, float *xm, float *ym, float *r, float *ag, float *eg
		  , FLONG *ks, FLONG *kp );
   int siarcf( std::string &s, float *xm, float *ym, float *r, float *ag, float *eg
		   , FLONG *kp, FLONG *kf, FLONG *kl, FLONG *kd );
   int siend( std::string &s, FLONG *n );
   int sigreek( std::string &s, float *xk, float *yk, FLONG *kl, float *hs, FLONG *kp );
   int sirot( std::string &s, float *dx, float *dy, float *wd
		  , float *sfx, float *sfy );
   int sichar( std::string &s, float *xt, float *yt, float *hs, float *bs
		   , float *as, float *ws, FLONG *kp, FLONG *m, char *text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  std::string       m_name;
  std::string       m_title;

  bool   m_endPage;
  double m_xoffset;
  double m_xmax, m_xmin, m_ymax, m_ymin;

  float m_tx0, m_ty0, m_tamx, m_tamy, m_txmax, m_tymax;

  std::string::iterator m_plotIter;
  std::string::iterator m_startPlotIter;

  int    m_height;
  int    m_width;

  PopUpMenu           m_popUpMenu;

  HardCopyListener::FileFormat m_currentFileFormat;
  HardCopyListener::FileFormats m_supportedFileFormats;

  int m_pageCount;
  int m_currentPage;

  double m_textStretch;
};
#endif
