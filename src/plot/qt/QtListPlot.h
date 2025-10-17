
#ifndef QT_LISTPLOT_H
#define QT_LISTPLOT_H

#include <math.h>
#include <vector>
#include <list>

#include "plot/ListPlot.h"
#include "plot/ListGraph.h"
#include "datapool/DataReference.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/GuiForm.h"
#include "gui/GuiFactory.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiToggleListener.h"
#include "gui/GuiPrinterDialog.h"
#include "plot/ScaleDialog.h"

#ifdef HAVE_QT
/* #include<QMutex> */
/* #include<QWaitCondition> */
#else
#include <pthread.h>
#endif

class iCanvas;
class Gdev;
class GuiPlotDataItem;
class GuiMenuToggle;
class Stream;
class InputChannelEvent;

/** QtListPlot
    Mit der Klasse QtListPlot k\oennen Objekte der Klasse 'ListGraph' horizontal
    und vertikal in einem Zeichnungsf\"ache angeordnet werden.
    @version $Id: QtListPlot.h,v 1.6 2009/02/11 12:07:27 amg Exp $
*/
class QtListPlot : public GuiQtElement, public ListPlot /*,
		     public RedisplayListener,
		     public HardCopyListener*/ {
 private:
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Redraw'
  class RedrawButtonListener : public GuiButtonListener {
  public:
    RedrawButtonListener( QtListPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() {
      if( m_plot != 0 ) m_plot->update( reason_Always );
    };
    virtual JobAction* getAction() { return 0; }
  private:
    QtListPlot *m_plot;
  };
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Print...'
  class PrintButtonListener : public GuiButtonListener {
  public:
    PrintButtonListener( QtListPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() {
      if( m_plot != 0 ){
	GuiPrinterDialog::MyEventData event( ReportGen::PRINT );
	GuiFactory::Instance()->createPrinterDialog()->showDialog( m_plot,  m_plot, &event );
      }
    };
    virtual JobAction* getAction() { return 0; }
  private:
    QtListPlot *m_plot;
  };
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'All cycles'
  class AllCyclesButtonListener : public GuiToggleListener {
  public:
    AllCyclesButtonListener( QtListPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) {
      if( m_plot != 0 ) {
	m_plot->allCycles( state );
	m_plot->update( reason_Always );
      }
    };
    virtual JobAction* getAction() { return 0; }
  private:
    QtListPlot *m_plot;
  };
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Detail grid'
  class DetailGridButtonListener : public GuiToggleListener {
  public:
    DetailGridButtonListener( QtListPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) {
      if( m_plot != 0 ) {
	m_plot->detailGrid( state );
	m_plot->update( reason_Always );
      }
    };
    virtual JobAction* getAction() { return 0; }
  private:
    QtListPlot *m_plot;
  };
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Same Y-Range'
  class SameYRangeButtonListener : public GuiToggleListener {
  public:
    SameYRangeButtonListener( QtListPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) {
      if( m_plot != 0 ) {
	m_plot->sameYRange( state );
	m_plot->update( reason_Always );
      }
    };
    virtual JobAction* getAction() { return 0; }
  private:
    QtListPlot *m_plot;
  };
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Scale Y-Range...'
  class ScaleYRangeButtonListener : public GuiButtonListener {
  public:
    ScaleYRangeButtonListener( QtListPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() {
      if( m_plot != 0 ) m_plot->openConfigDialog();
    };
    virtual JobAction* getAction() { return 0; }
  private:
    QtListPlot *m_plot;
  };
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Reset'
  class CDresetButtonListener : public GuiButtonListener {
  public:
    CDresetButtonListener( QtListPlot *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed() {
      if( m_dialog != 0 ) m_dialog->cDresetEvent(); };
    virtual JobAction* getAction() { return 0; }
  private:
    QtListPlot *m_dialog;
  };
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Close'
  class CDcloseButtonListener : public GuiButtonListener {
  public:
    CDcloseButtonListener( QtListPlot *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed() {
      if( m_dialog != 0 ) m_dialog->cDcloseEvent(); };
    virtual JobAction* getAction() { return 0; }
  private:
    QtListPlot *m_dialog;
  };

 public:
  /// Konstruktor
  QtListPlot( GuiElement *parent, const std::string &name, int width = 148, int height = 105 );
  /// Destruktor
  virtual ~QtListPlot();

  // Ueberladene Funktionen der Klasse 'GuiElement'
  virtual GuiElement* getElement() { return this; }
  virtual GuiElement::ElementType Type();
  virtual void manage();
  virtual void create();// QWidget* parent );
  virtual void getSize( int &width, int &height );
  virtual void update( UpdateReason reason );
  virtual QWidget* myWidget();
  virtual GuiPopupMenu* getPopupMenu();
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value &v, bool onlyUpdated = false) {
    return ListPlot::serializeJson(v, onlyUpdated);
  }
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false) {
    return ListPlot::serializeProtobuf(eles, onlyUpdated);
  }
#endif
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) {
    return ListPlot::variantMethod(method, jsonArgs, eng);
  }

  // GuiIndexListener
  virtual bool acceptIndex( const std::string &, int );
  virtual void setIndex( const std::string &name, int inx );

  // Ueberladene Funktionen der Klasse 'HardCopyListener'
  virtual HardCopyListener::FileFormat getFileFormat();
  virtual const std::string &getName(){ return m_name; }
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual bool isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat );
  virtual HardCopyListener::PrintType getPrintType( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &MenuLabel();
  virtual bool write( InputChannelEvent &event );
  virtual bool write( const std::string &s );
  virtual bool getDefaultSettings( HardCopyListener::PaperSize &size,
				   HardCopyListener::Orientation &orient,
				   int &quantity,
				   HardCopyListener::FileFormat &format,
				   double &lm, double &rm,
				   double &tm, double &bm );
  // Ueberladene Funktionen der Klassen 'RedisplayListener'
  virtual void repaint( Gdev &g, bool allPages = false );
  virtual void refresh( Gdev &g );
  /// Oeffnet den Konfigurationsdialog
  void openConfigDialog();
  /// Ereignis vom Button 'Reset' des Konfigurationsdialoges
  void cDresetEvent();
  /// Ereignis vom Button 'Close' des Konfigurationsdialoges
  void cDcloseEvent();

  /** Setzt den Plot-Titel
      @param title Titel-Text
   */
  void setTitle( const std::string &title );
  /** Liefert den Titeltext
   */
  const std::string &getTitle() const;
  /** Setzt die Plot-Groesse
      @param width Breite des Plots
      @param height Hoehe des Plots
   */
  void setSize( int width, int height ) { m_width=width; m_height=height; }
  /** Gibt einen Zeiger auf ein QtListPlot-Objekt zurück
      @param name Name des gesuchten QtListPlot-Objekts
   */
  static QtListPlot * getPlot( const std::string &name );
  /** F\"ugt einen Indexschalter hinzu
      @param title Beschriftung
   */
  bool addIndexButton( const std::string& title );
  /** F\"ugt eine neue (leere) ListGraph-Zeile ein
   */
  void addHGraph();
  /** F\"ugt ein neues ListGraph-Objekt in der aktuellen Zeile ein
      @param name Name des ListGraph-Objektes
  */
  ListGraph *addGraph( const std::string &name );
  /** Definiert die Legende
      @param streamname Name des Streams
   */
  void setCaptionStream( const std::string &streamname );
  /** Funktionswerte aller Zyklen gemeinsam darstellen
      @param state Zustand
   */
  void allCycles( bool state );
  /** Dichte der Rasterunterteilung setzen
      @param state Zustand
   */
  void detailGrid( bool state );
  /** Funktionswerte mit einer gemeinsamen Y-Achsenunterteilung darstellen
      @param state Zustand
   */
  void sameYRange( bool state );
  /** Liefert die Anzahl der Unterteilungen einer x-Achsenunterteilungen zurueck
      @return Anzahl der Unterteilungen einer x-Achsenunterteilung
   */
  int getNSubDivX() { return m_nsubdivx; }
  /** Liefert die Anzahl der Unterteilungen einer y-Achsenunterteilungen zurueck
      @return Anzahl der Unterteilungen einer y-Achsenunterteilung
   */
  int getNSubDivY() { return m_nsubdivy; }
  /** Liefert true zurueck, wenn eine gemeinsame y-Achsenunterteilung gewuenscht ist
   */
  bool isSameYRange() { return m_sameYRange; }
  /** Liefert true zurueck, wenn alle Zyklen gemeinsam dargestellt werden sollen
   */
  bool printAllCycles() { return m_allCycles; }
  /** Werte im Datenpool aktualisieren
      @param reset Flag zum Zuruecksetzen der von Hand eingestellten Werten
   */
  void writeValues( bool reset=false );
  /** Lokale Werte aktualisieren
   */
  void readValues();
  /** Liefert den gr\"ossten, im QtListPlot vorkommenden, y-Achsenwert
   */
  int maxYRange();
  GuiForm* getConfigDialog() { return m_configDialog; }
  int getId() { m_id++; return m_id; }
  void updateConfigDialog( UpdateReason reason ) {
    if( m_configDialog != 0 ) m_configDialog->getElement()->update( reason ); }
/*   static void doPrintAnyCallback( XtPointer data, XtIntervalId *id ); */
/*   static void doPrintPSCallback( XtPointer data, XtIntervalId *id ); */
  void doPrintPS();
  void doPrintAny();
  virtual int pages(){ return 1; };

 private:
  void createPopupMenu();
  void buildConfigDialog();
  void drawLegend( const Frame &viewport );
  bool isDataItemUpdated( TransactionNumber trans );
  std::string StreamToString( Stream *s );

protected:
  iCanvas *m_canvas;    // Zeichnungsflaeche

 private:
  const std::string m_name; // Name der im Descriptionfile definiert wurde
  int m_id;            // Id fuer ListGraphen
  Gdev *m_gdev;        // Geraetedevice
  std::string m_title;      // Titel fuer den QtListPlot
  int      m_width;         // Breite des Plots
  int      m_height;        // Hoehe  des Plots
  std::string m_legend;     // Legend string (not displayed if empty)
  Stream *m_legendStream;     // Legend stream
  std::vector<std::vector<ListGraph*> > m_graphs; // Matrix der ListGraph-Objekte

  std::ostream *m_ostrPrinter;
#ifdef HAVE_QT
  InputChannelEvent        *m_inEvent;
#else
  pthread_mutex_t m_mutex;
  pthread_cond_t m_printingDone;

#endif

  int m_nsubdivx;    // Anzahl der Unterteilungen einer x-Achsenunterteilung
  int m_nsubdivy;    // Anzahl der Unterteilungen einer y-Achsenunterteilung
  bool m_sameYRange; // Gemeinsame Skalierung der Y-Achsen
  bool m_allCycles;  // alle Cycles gleichzeitig ausgeben

  // Config-Dialog
  GuiForm                        *m_configDialog;
  QtListPlot::CDresetButtonListener     *m_cDresetButtonListener;
  QtListPlot::CDcloseButtonListener     *m_cDcloseButtonListener;

  // Popup-Menue
  QtListPlot::RedrawButtonListener      *m_redrawButtonListener;
  QtListPlot::PrintButtonListener       *m_printButtonListener;
  QtListPlot::AllCyclesButtonListener   *m_allCyclesButtonListener;
  QtListPlot::DetailGridButtonListener  *m_detailGridButtonListener;
  QtListPlot::SameYRangeButtonListener  *m_sameYRangeButtonListener;
  QtListPlot::ScaleYRangeButtonListener *m_scaleYRangeButtonListener;
  GuiMenuToggle                  *m_buttonAllCycles;
  GuiMenuToggle                  *m_buttonDetailGrid;
  GuiMenuToggle                  *m_buttonSameYRange;

  HardCopyListener::FileFormat         m_currentFileFormat;
  HardCopyListener::FileFormats2       m_supportedFileFormats;
};
#endif
