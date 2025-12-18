
#ifndef GUI_QWT_PLOT_H
#define GUI_QWT_PLOT_H

#include <qwt_plot.h>
#if QWT_VERSION >= 0x060000 // unter 6.0
#include <qwt_interval.h>
#include <qwt_point_3d.h>
#endif
#include <qstringlist.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_rescaler.h>
#include <qwt_symbol.h>
#include <QSize>
#include <QMouseEvent>

#include "gui/qt/GuiQwtPlotCurveData.h"
#include "gui/qt/GuiQwtPropertyDialog.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/Gui2dPlot.h"
#include "gui/Plot2dCurveAttributes.h"
#include "gui/GuiMenuButtonListener.h"
#include "gui/GuiToggleListener.h"

class QStackedWidget;
class GuiQtPolarChart;
class GuiQtChartView;
class GuiQwtPolarPlot;
class QwtPolarCurve;
#if HAVE_QPOLAR
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#endif
#if QT_VERSION >= 0x060000
QT_USE_NAMESPACE
#else
QT_CHARTS_USE_NAMESPACE
#endif

class QwtPlotZoomer;
class GuiQtPopupMenu;
class GuiQtPulldownMenu;
class GuiQtMenuToggle;
class GuiQtMenuButton;
class QwtPlotCanvas;
class GuiQwtPlotPicker;
class GuiQwtPlotZoomer;
class GuiQwtPlotBarChart;
class GuiQtForm;
//---------------------------------------------------
// Klasse GuiQwtPlotCurve
//---------------------------------------------------
class GuiQwtPlotCurve : public QwtPlotCurve{
public:
 GuiQwtPlotCurve( const QString& name )
   : QwtPlotCurve(name) {}

  /** QwtPlotCurve::CurveStyle Extending
   */
  enum CurveStyleExt
  {
    NoCurve = QwtPlotCurve::NoCurve,
    Lines = QwtPlotCurve::Lines,
    Sticks = QwtPlotCurve::Sticks,
    Steps = QwtPlotCurve::Steps,
    Dots = QwtPlotCurve::Dots,
    UserCurve = QwtPlotCurve::UserCurve,
    Bar = QwtPlotCurve::UserCurve + 1,
    StackingBar = QwtPlotCurve::UserCurve + 2,
    Polar = QwtPlotCurve::UserCurve + 3
  };

 private:
#if QWT_VERSION >= 0x060000
  /**
    draw curve in pieces if y-values are 'nan'-values
  */
  void drawCurve( QPainter *painter, int style,
		  const QwtScaleMap &xMap, const QwtScaleMap &yMap,
		  const QRectF &canvasRect, int from, int to ) const;
#endif
};

class GuiQWTPlot : public GuiQtElement, public Gui2dPlot {
public:

  //------------------------------------
  // CurvePalette Class
  //------------------------------------
  struct CurvePalette {
  CurvePalette(int idx, QString& lineColor, QString& symColor,
               Qt::PenStyle lineStyle, QwtSymbol::Style symStyle, int symSize)
  : index(idx)
  , lineColor(lineColor)
  , symbolColor(symColor)
  , lineStyle(lineStyle)
  , symbolStyle(symStyle)
  , symbolSize(symSize)
  {}
    int index;
    QString lineColor;
    QString symbolColor;
    Qt::PenStyle     lineStyle;
    QwtSymbol::Style symbolStyle;
    int symbolSize;
  };

  //------------------------------------
  // MyQwtPlot Class
  //------------------------------------
  class MyQwtPlot : public QwtPlot {
  public:
    MyQwtPlot(QWidget *parent, GuiQWTPlot *plot);
    virtual ~MyQwtPlot() {}

    /** overload print function and to own print attributes */
    virtual void print(QPaintDevice& pd, bool bPrinter);

    /** get Barplot Orientation */
    Qt::Orientation getBarChartOrientation();

    /** get Barplot Layout Settings */
    void setBarChartLayoutSettings(GuiQwtPropertyDialog::Settings::BarChart& settings);

    /** apply Settings from QwtPlot Setting Dialog */
    void applySettings( const GuiQwtPropertyDialog::Settings &settings );

    /** get Plot Rescaler (Aspect Ratio) */
    QwtPlotRescaler* getPlotRescaler();

  private:
    bool event(QEvent * event);
    void timerEvent(QTimerEvent * event);
    void enterEvent(QEnterEvent * event);
    void leaveEvent(QEvent * event);
    void contextMenuEvent(QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent *event);

  private:
    GuiQWTPlot      *m_plot;
    Qt::Orientation  m_barChartOrientation;
    QwtPlotRescaler *m_rescaler;
  };

  //------------------------------------
  // Constructor / Destructor
  //------------------------------------
  GuiQWTPlot( const std::string &name );
  virtual ~GuiQWTPlot();
 private:
  GuiQWTPlot( const GuiQWTPlot &plot);

  //------------------------------------
  // public functions of GuiQtElement
  //------------------------------------
public:
  virtual GuiElement::ElementType Type(){
    return GuiElement::type_QWTPlot;
  }
  virtual void create();
  virtual void manage();
  virtual void unmanage();
  virtual void update(GuiElement::UpdateReason);
  virtual bool acceptIndex( const std::string &, int );
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;
  virtual void setIndex( const std::string &name, int inx );
  virtual void serializeXML(std::ostream&, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual QWidget* myWidget();
  virtual QWidget* myCurrentWidget();
  virtual Gui2dPlot* get2dPlot() { return this; }
  virtual void writeSettings();
  void readSettings();
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) {
    return Gui2dPlot::variantMethod(method, jsonArgs, eng);
  }

  //------------------------------------
  // public functions of Gui2dPlot
  //------------------------------------
public:
  virtual void drawHeaderText();
  virtual void drawFooterText();

  virtual GuiElement* getElement(){ return this; }
  virtual void showAnnotationLabels( bool status );
  virtual void setWithScrollBar( bool flag ){}
  virtual void setYPlotStyle( eStyle style, eAxisType axis );

  /// Logarithmische x-Skala
  virtual void setLogX( bool value = true );
  /// Logarithmische y-Skala
  virtual void setLogY( bool value = true );
  /// x-Achsenursprung festsetzen
  virtual void setOriginXAxis( const double origin );
  /// y-Achsenursprung festsetzen
  virtual void setOriginYAxis( const double origin );
  virtual Plot2dCurvePalette* getPalette(const std::string& curveName) { return m_curveAttrs.getPalette(curveName); }
  void doPlot(bool clear_always);

  virtual void withAnnotationOption( bool state ){ m_withAnnotationOption = state; }
  virtual bool hasAnnotationOption() { return m_withAnnotationOption; }
  /// Stil von Konfigurationsdialog festlegen
  virtual void setConfigDialogStyle( ConfigDialog::eStyle style, int length );
  /// Ereignis vom Button 'Reset' des Konfigurationsdialoges
  virtual void configDialogResetEvent();
  virtual void setPrintStyle( bool status )          {}
  virtual void getSelectionPoints(tPointVector& pts,
				  std::vector<int>& axisType, std::vector<std::string>& axisTitle);
  virtual bool hasFunction();

  //---------------------------------------
  // public functions of HardCopyListener
  //---------------------------------------
  virtual bool write( InputChannelEvent &event );
  virtual bool write( const std::string &fileName );
  virtual void writeFile(QIODevice* ioDevice, const std::string &fileName, bool bPrinter );
  virtual FileFormat getFileFormat();
  virtual const std::string &getName() { return Gui2dPlot::getName(); }
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual bool isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat );
  virtual PrintType getPrintType( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &MenuLabel() { return Gui2dPlot::MenuLabel(); }
  virtual void print(QPrinter* print=0);
  virtual bool saveFile( GuiElement * );

  void zoomed();
  void openScaleDialog();
  void openCyclesDialog();
  bool hasCyclePlotMode();

  /// Oeffnet den Konfigurationsdialog
  void openConfigDialog();
  QSize sizeHint(){
    return QSize( getInitialWidth(), getInitialHeight() );
  }
  /// generates an svg, eps, pdf, ... file as output
  virtual bool generateFileWithSvgGenerator(std::string& filename, bool bPrinter);

  // convert julian date to local date string
  /* QString toLocalDate(long julianDate); */

  /** set user interaction mode */
  void setUserInteractionMode( eUserInteractionMode mode );

  /** for debugging reason */
  void printInfo(char* msg);

  /** set Cursor */
  void setCursor();
  /** set Axis Label Rotation (xBotton) */
  void setAxisLabelRotation(double rotation);

/*=============================================================================*/
/* public function of ConfigDialogListener                                     */
/*=============================================================================*/
  virtual void closeEvent( GuiEventData *event );
  virtual void resetEvent( GuiEventData *event );

/*=============================================================================*/
/* member functions of Gui2dPlot                                               */
/*=============================================================================*/
protected:
  virtual void configureMenu( PlotItem *, int );
  virtual void configureIndexedMenu( PlotItem *, int idx );

private:
  void toStdVector(const QStringList& list, std::vector<std::string>& vec);
  void fromStdVector(const std::vector<std::string>& vec, QStringList& list);
  bool setScale(TransactionNumber trans=0);
  void setAutoScale(TransactionNumber trans=0);
  void setStartScaleValues();
  void buildScaleDialog();
  void buildCyclesDialog();
  void buildConfigDialog();
  GuiQwtPlotCurve::CurveStyleExt getQwtCurveStyle(eAxisType axis);
  void setdrefAxis();
  bool updateMarker(PlotItem *item, int xWildcardIndex, int yWildcardIndex);
  void addLabel( std::vector<std::string> &labels, GuiPlotDataItem *item );
  QPen getSymbolPen(QwtSymbol::Style sym, QString symCol, QString symPenCol);
  bool updateCurve( PlotItem *item, int cycle, int xWildcardIndex, int yWildcardIndex);
  void clearPlots(bool always);
  bool setAnnotationLabels();
  bool isAnnotationLabelsUpdated( TransactionNumber trans );
  /// set annotation labels onto xaxis
  void setXAxisAnnotation(bool bAnno);
#if QWT_VERSION >= 0x060200
  QwtAxis::Position getAnnotationAxis();
#else
  QwtPlot::Axis getAnnotationAxis();
#endif
 ConfigDialog::eItemStyle getConfigDialogItemStyle(PlotItem* plotItem);

  // menu staff
  GuiQtMenuButton* newPopupMenuButton( std::string label, GuiMenuButtonListener* listener );
  GuiQtMenuButton* newPopupMenuDialogButton( std::string label, GuiMenuButtonListener* listener);
  void newPopupMenuToggle( std::string label, bool status, GuiToggleListener* listener );
  void createLogarithmicMenu( GuiQtPopupMenu *menu );
  void timerEvent ( QTimerEvent * event );
  void configDialogSetCurvePalette();
  bool configDialogGetCurvePalette();
  std::string getCurveInfo(const QwtPlotCurve& curve);
  void configureCycleXfer(XferDataItem* xfer, int pos);
  void getClassPointer(PlotItem* plotitem, void* pClass,
                       QwtPlotItem*& plotItem, QwtPolarCurve*& polarCurve);
#if HAVE_QPOLAR
  void getClassPointer(PlotItem* plotitem, void* pClass,
                       QwtPlotItem*& plotItem, QLineSeries*& polarCurve);
#endif
  void createPolarPlot();

  /*=============================================================================*/
  /* private class Listener Definitions                                          */
  /*=============================================================================*/
 private:
  class ZoomListener : public GuiToggleListener
  {
  public:
    ZoomListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state );
  private:
    GuiQWTPlot  *m_plot;
  };
  class ResetListener : public GuiMenuButtonListener
  {
  public:
    ResetListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQWTPlot  *m_plot;
  };
  class PrintListener : public GuiMenuButtonListener
  {
  public:
    PrintListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQWTPlot  *m_plot;
  };
  class CopyListener : public GuiMenuButtonListener
  {
  public:
    CopyListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQWTPlot  *m_plot;
  };
  class FullscreenListener : public GuiMenuButtonListener
  {
  public:
    FullscreenListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQWTPlot  *m_plot;
  };
  class RedrawListener : public GuiMenuButtonListener
  {
  public:
    RedrawListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQWTPlot  *m_plot;
  };
  class OpenScaleListener : public GuiMenuButtonListener
  {
  public:
    OpenScaleListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQWTPlot  *m_plot;
  };
  class OpenCycleListener : public GuiMenuButtonListener
  {
  public:
    OpenCycleListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQWTPlot  *m_plot;
  };
  class OpenConfigListener : public GuiMenuButtonListener
  {
  public:
    OpenConfigListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQWTPlot  *m_plot;
  };
  class AnnotationListener : public GuiToggleListener
  {
  public:
    AnnotationListener( GuiQWTPlot *plot ): m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state );
  private:
    GuiQWTPlot  *m_plot;
  };
  class PropertyListener : public GuiMenuButtonListener
  {
  public:
    PropertyListener( GuiQWTPlot *plot ): m_plot( plot ), m_propDlg(0) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
    GuiQwtPropertyDialog::Settings Settings() { return m_propDlg->settings(); }
  private:
    GuiQWTPlot           *m_plot;
    GuiQwtPropertyDialog *m_propDlg;
  };
  //----------
  // StyleMenu
  //----------
  class StyleMenu : public GuiToggleListener {
  public:
    class MyEvent : public GuiEventData{
    public:
      MyEvent( eAxisType axisType ) : m_axisType( axisType ){}
      eAxisType m_axisType;
    };
    StyleMenu( GuiQWTPlot *plot )
      : m_plot( plot )
      , m_buttonStylePlot( 0 )
      , m_buttonStyleBar( 0 )
      , m_buttonStyleStackingBar( 0 )
      , m_buttonStyleArea( 0 )
      , m_buttonStylePolar( 0 )
      , m_buttonStyleStep( 0 )
      , m_buttonStyleDots( 0 ){}
    virtual void ToggleStatusChanged( GuiEventData *event );
    void setStyleButtons( eStyle style );
    GuiQWTPlot      *m_plot;
    GuiQtMenuToggle *m_buttonStylePlot;
    GuiQtMenuToggle *m_buttonStyleBar;
    GuiQtMenuToggle *m_buttonStyleStackingBar;
    GuiQtMenuToggle *m_buttonStyleArea;
    GuiQtMenuToggle *m_buttonStylePolar;
    GuiQtMenuToggle *m_buttonStyleStep;
    GuiQtMenuToggle *m_buttonStyleDots;
  private:
    StyleMenu( const StyleMenu &s );
    void operator=(const StyleMenu &s );
    eStyle m_currentStyle;

  };
  //---------------
  //LogarithmicMenu
  //---------------
  class LogarithmicMenu : public GuiToggleListener {
  public:
    LogarithmicMenu( GuiQWTPlot *plot )
      : m_plot( plot )
      , m_xaxisLogButton( 0 )
      , m_yaxisLogButton( 0 ){}
    virtual void ToggleStatusChanged( GuiEventData *event );
    GuiQWTPlot      *m_plot;
    GuiQtMenuToggle *m_xaxisLogButton;
    GuiQtMenuToggle *m_yaxisLogButton;
  private:
    LogarithmicMenu( const LogarithmicMenu &l );
    void operator=( const LogarithmicMenu &l );
  };
  //---------------
  //UserInteractionModeMenu
  //---------------
  class UserInteractionModeMenu : public GuiToggleListener {
  public:
    UserInteractionModeMenu( GuiQWTPlot *plot )
      : m_plot( plot )
      , m_zoomButton( 0 )
      , m_selectPointButton( 0 )
      , m_selectRectButton( 0 ) {}
    virtual void ToggleStatusChanged( GuiEventData *event );
    GuiQWTPlot      *m_plot;
    GuiQtMenuToggle *m_zoomButton;
    GuiQtMenuToggle *m_selectPointButton;
    GuiQtMenuToggle *m_selectRectButton;
  private:
    UserInteractionModeMenu( const UserInteractionModeMenu &l );
    void operator=( const UserInteractionModeMenu &l );
  };

  //---------------------------------------
  // private functions for popupmenu
  //---------------------------------------
  void createPopupMenu();
  void createYStyleMenu( GuiQtPopupMenu *menu, eAxisType axis );
  void createStyleMenu( GuiQtPulldownMenu *pullDownMenu, eAxisType axis );
  void createUserInteractionModeMenu( GuiQtPopupMenu *menu );

  //---------------------------------------
  // private Trigger class
  //---------------------------------------
 private:
  class Trigger : public JobStarter{
 public:
 Trigger( JobFunction *f, GuiQWTPlot *plot )
   : JobStarter( f ), m_plot(plot) {}
   virtual ~Trigger() {}
   virtual void backFromJobStarter( JobAction::JobResult rslt ) {
     m_plot->setCursor();
     if( rslt == JobAction::job_Ok ){
       GuiManager::Instance().update( GuiElement::reason_Process );
     }
   }
 private:
   GuiQWTPlot *m_plot;
 };

//---------------------------------------
  // other public
  //---------------------------------------
 public:
  void popupMenu(const QContextMenuEvent* event);
  /** copy to Clipboard */
  void copy();
  virtual void fullscreen();
  void createPicker(bool state);
  void setAxisTitles();
  /** set a new bar chart layout hint attribute */
  void setBarChartLayoutSettings(const GuiQwtPropertyDialog::Settings::BarChart& settings);
  UserInteractionModeMenu& getUserInteractionModeMenu() { return m_userInteractionMenu; }
  void setXAxisMajorTicks(int ticks) { m_majorTicks = ticks; }
  void setXAxisMinorTicks(int ticks) { m_minorTicks = ticks; }
  void setBarChartOrientation(Qt::Orientation o) { m_bcSettings.orientation = o; }

  //---------------------------------------
  // private slots
  //---------------------------------------
    Q_OBJECT
private slots:
    void legendChecked (const QVariant &itemInfo, bool on, int index);
    bool showCurve(QwtPlotItem *, bool on, bool refresh=true);
    void slot_selected(const QRectF&);
    void slot_selected(const QwtPlotCurve*, int, const QPointF&, const QPointF&);
    void slot_selectedCurve(QwtPlotCurve&);
public slots:
  void updatePlot();

private:
  void setRescaleMode();
  MyQwtPlot       *m_plot;
#if HAVE_QPOLAR
  GuiQtChartView*  m_polarPlot;
  GuiQtPolarChart* m_polarChart;
  QValueAxis*  m_angularAxis;
  QValueAxis*  m_radialAxis;
#endif
  /// last marked closest (Curve-)Point
  long m_lastClosestIdx;

  QStackedWidget            *m_widgetStack;
  GuiQwtPlotZoomer          *m_zoomer[2];
  GuiQwtPlotPicker          *m_picker;
  GuiQwtPlotBarChart        *m_barPlotChart[2];
  GuiQtPopupMenu            *m_popupMenu;
  LogarithmicMenu            m_logarithmicMenu;
  UserInteractionModeMenu    m_userInteractionMenu;
  StyleMenu                 *m_yStyleMenu[2];
  QString                    m_background;
  int                        m_lineWidth;
  QSize                      m_printSize;

  // barplot properties
  GuiQwtPropertyDialog::Settings::BarChart m_bcSettings;

  std::map<QString, bool>    m_legendVis;

  RedrawListener             m_redrawListener;
  OpenScaleListener          m_openScaleListener;
  OpenCycleListener          m_openCycleListener;
  OpenConfigListener         m_openConfigListener;
  ResetListener              m_resetListener;
  PrintListener              m_printListener;
  CopyListener               m_copyListener;
  FullscreenListener         m_fullscreenListener;
  AnnotationListener         m_annotationListener;
  PropertyListener           m_propertyListener;

  GuiQtForm*                 m_fullscreenDialog;
  HardCopyListener::FileFormat  m_currentFileFormat;
  HardCopyListener::FileFormats2 m_supportedFileFormats;
  static eUserInteractionMode s_userInteractionMode;
  eUserInteractionMode m_userInteractionMode;
  Plot2dCurveAttributes      m_curveAttrs;
  QwtPlotCurve              *m_lastSelectedCurve;
  GuiQtMenuButton           *m_configButton;
  GuiQtMenuButton           *m_cyclesButton;

  ///  tPlotVector          m_plots;
  bool                 m_selectionRectMode;

  // x annotion
  bool                   m_showAnnotationLabels;
  bool                   m_withAnnotationOption;
  std::vector<QwtPlotMarker*> m_xannoMarker;

  /// default count of major ticks
  static const int MAJOR_TICKS;
  /// default count of minor ticks
  static const int MINOR_TICKS;

  int m_majorTicks;
  int m_minorTicks;
  int m_annoAngle;

  std::vector<Gui2dPlot*> m_clonedList;  // cloned plot2d list

  /** draw flag for titles xAxis PlotItems */
  static bool drawXAxisPlotItemTitles;

  friend class GuiQwtPlotLayout;
};
#endif
