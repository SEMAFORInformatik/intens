
#ifndef GUI_2DPLOT_INCLUDED_H
#define GUI_2DPLOT_INCLUDED_H

#include <vector>
#include <set>
#include <map>

#include "gui/GuiElement.h"
#include "app/HardCopyListener.h"
#include "app/DataPoolIntens.h"
#include "plot/ConfigDialog.h"
#include "plot/PlotAxis.h"
#include "plot/ScaleDialogListener.h"
#include "plot/ConfigDialogListener.h"
#include "plot/CyclesDialogListener.h"

class QRectF;
class GuiPlotDataItem;
class XferDataItem;
class Stream;
class ConfigDialog;
class ScaleDialog;
class CyclesDialog;
class Scale;
class QwtPlotMarker;
class QwtPlotItem;
class QwtPolarItem;
class Plot2dCurvePalette;

typedef long TransactionNumber;

/** Die Klasse 'Gui2dPlot' ermoeglicht das Darstellen von 2d-Plots

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: Gui2dPlot.h,v 1.25 2008/10/01 08:08:08 amg Exp $
*/
class Gui2dPlot :   public HardCopyListener,
		    public ScaleDialogListener,
		    public ConfigDialogListener,
		    public CyclesDialogListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  Gui2dPlot( const std::string &name );
  virtual ~Gui2dPlot();
  Gui2dPlot( const Gui2dPlot& );
private:
  Gui2dPlot &operator=(const Gui2dPlot&);
public:
  virtual GuiElement* getElement() = 0;

public:
  enum eStyle {
    PLOT,
    BAR,
    AREA,
    POLAR,
    STACKING_BAR,
    STEP,
    DOTS           // used by 2d plot
  };

  enum eAxisType {
    XAXIS,
    XAXIS2,
    Y1AXIS,
    Y2AXIS,
    HIDDEN
  };
  enum eUserInteractionMode { ZOOM, SELECT_POINT, SELECT_RECTANGLE };

  struct Point {
  Point(double _x, double _y)
  : x(_x), y(_y) {}
    double x, y;
  };
  typedef std::vector<Point> tPointVector;
  struct MarkerData {
    MarkerData(const std::string& _label,
               const std::string& _background, const std::string& _foreground)
      : label(_label), background(_background), foreground(_foreground) {}
    std::string label, background, foreground;
  };
  typedef std::vector<MarkerData> tMarkerDataVector;


protected:
  typedef std::set<eAxisType> AXISTYPESET;
  typedef AXISTYPESET::const_iterator AXISTYPEITERATOR;
  AXISTYPESET m_axisTypeSet;

  /*=============================================================================*/
  /* public function of ScaleDialogListener & ConfigDialogListener               */
  /*=============================================================================*/
public:

/*=============================================================================*/
/* public Definitions                                                          */
/*=============================================================================*/
public:
  //---------
  // PlotItem
  //---------
  class PlotItem {
  public:
    PlotItem( GuiPlotDataItem *item );
    virtual ~PlotItem();
    PlotItem( const PlotItem & );
  private:
    PlotItem &operator=( const PlotItem & );

    enum eLegendDisplay { SHOW, HIDE, DEFAULT };

  public:
    void setIsMarker(){
      m_isMarker=true;
    }
    void setColorsetName(const std::string& name);
    ColorSet *colorset(){ return m_colorset; }
    void setLegendHide(bool hide=true){
      m_legendHide = hide ? HIDE : SHOW;
    }
    void setMarker( XferDataItem *xfer );
    XferDataItem *getMarker(){ return m_marker; }
    // Marker X-Axis (only used in 3d ContourPlot)
    bool isMarker(){ return m_isMarker; }
    bool isLegendHide(){ return m_legendHide == DEFAULT ? m_isMarker : m_legendHide==HIDE; }
    void initAxisTypeValues( bool plot2d, int posOfX );
    void resetAxisTypeValues();
    bool isDataItemUpdated(TransactionNumber trans);
    GuiPlotDataItem *plotDataItem() const { return m_plotDataItem; }
    GuiPlotDataItem *xPlotDataItem() const;

    void updateAxisTypes( bool plot2d, PlotItem *xPlotItem );
    void setAllXfers(XferDataItem *xXfer
                     , XferDataItem *y1Xfer
                     , XferDataItem *y2Xfer
                     , XferDataItem *colorXfer
                     , XferDataItem *symbolColorXfer
                     , XferDataItem *lineStyleXfer
                     , XferDataItem *symbolStyleXfer
                     , XferDataItem *symbolSizeXfer
                     , XferDataItem *unitXfer);
    void setLineColorXfer(XferDataItem *colorXfer);
    void setSymbolColorXfer(XferDataItem *symbolColorXfer);
    void setLineStyleXfer(XferDataItem *lineStyleXfer);
    void setSymbolStyleXfer(XferDataItem *symbolStyleXfer);
    void setSymbolSizeXfer(XferDataItem *symbolSizeXfer);
    void setUnitXfer(XferDataItem *unitXfer);
    XferDataItem* getLineColorXfer();
    void getAllXfers(XferDataItem *&xXfer
                     , XferDataItem *&y1Xfer
                     , XferDataItem *&y2Xfer
                     , XferDataItem *&colorXfer
                     , XferDataItem *&symbolColorXfer
                     , XferDataItem *&lineStyleXfer
                     , XferDataItem *&symbolStyleXfer
                     , XferDataItem *&symbolSizeXfer
                     , XferDataItem *&unitXfer);
    eAxisType getAxisType(){ return m_axisType; }
    void setAxisType( eAxisType type ){ m_axisType = type; }
    void setXPlotItem( PlotItem *item ){ m_xPlotItem = item; }
    PlotItem *xPlotItem() const { return m_xPlotItem; }
    int getXPos();
    bool isXAxisDefined();
    int getNPoints();
    void* getPlotCurve(int cycle, int xIndex, int yIndex);
    void setPlotCurve(int cycle, int xIndex, int yIndex, void* pc);
    std::vector<QwtPlotMarker*> &getMarkers(){ return m_markers; }

    /**
       set Axis Wildcard Index
       normally plotItem has only 1 wildcard
       if a yplotitem have also a wildcard this wildcard must be set here
       @param wildcardIdx  wildcard index of y plotitem, index starts with 1
     */
    void setAxisWildcardIndex( int wildcardIdx );

    /** get Data Plot Item Index */
    int getDataPlotItemIndex();

    /** has wildcard indexes */
    bool hasAxisWildcardIndex();

    /** set Axis wildcard index */
    void setAxisWildcardIndexValue( int idxValue );

    /** get value of Y Axis wildcard index */
    int getAxisWildcardIndexValue();

    /** get dimension of axis wildcard index */
    int getAxisWildcardIndexDimension();

    bool acceptIndex( const std::string &, int );
    void setIndex( const std::string &name, int inx );

    void serializeXML(std::ostream&, const std::string &label, const std::string &axis,
		      tPointVector& pts, tMarkerDataVector& markerLabels);
    bool serializeJson(Json::Value& jsonObj, const std::string &label, const std::string &axis,
                       tPointVector& pts, tMarkerDataVector& markerLabels);
#if HAVE_PROTOBUF
    bool serializeProtobuf(in_proto::Plot2D::PlotItem* plotItem, const std::string &label, const std::string &axis,
                            tPointVector& pts, tMarkerDataVector& markerLabels);
#endif
    tPointVector& DataCache_PointVector() { return m_cachePts; }
    /** webtens, data cache marker labels vector */
    tMarkerDataVector& DataCache_MarkerLabelsVector() { return m_cacheMarkerLabels; }
    /** webtens, data cache clear */
    void clearDataCache();
  private:
    typedef std::vector<void*> QwtPlotItemVector;
    typedef std::vector<QwtPlotItemVector> QwtPlotItemMatrix;
    typedef std::vector<QwtPlotItemMatrix> PlotItemData;
    PlotItemData m_plotCurve;
    GuiPlotDataItem     *m_plotDataItem;
    PlotItem            *m_xPlotItem;
    XferDataItem        *m_xferXAxisType;
    int                  m_xResetValue;
    XferDataItem        *m_xferY1AxisType;
    int                  m_y1ResetValue;
    XferDataItem        *m_xferY2AxisType;
    int                  m_y2ResetValue;
    XferDataItem        *m_xferLineColor;
    XferDataItem        *m_xferSymbolColor;
    XferDataItem        *m_xferLineStyle;
    XferDataItem        *m_xferSymbolStyle;
    XferDataItem        *m_xferSymbolSize;
    XferDataItem        *m_xferUnit;
    eAxisType            m_axisType;
    XferDataItem        *m_marker;
    bool                 m_isMarker;
    eLegendDisplay       m_legendHide;
    std::vector<QwtPlotMarker*>     m_markers;
    int                  m_axisWildcardIndex;
    std::string          m_colorsetName;
    ColorSet            *m_colorset;

    // cache only used by zoom mode in webtens
    tMarkerDataVector m_cacheMarkerLabels;
    tPointVector m_cachePts;
  };

  typedef std::vector<Gui2dPlot::PlotItem *> tPlotItemVector;
  typedef std::map<int, Gui2dPlot::PlotItem *> tPlotItemMap;
  typedef tPlotItemVector::iterator tPlotItemIterator;

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
protected:
  //---------
  // Plot
  //---------
  class Plot{
  public:
  Plot( int cycle, int yGuiIndex, int xGuiIndex )
    : m_cycle( cycle ), m_yGuiIndex(yGuiIndex), m_xGuiIndex(xGuiIndex) {}
  private:
    Plot &operator=(const Plot &);
    Plot( const Plot & );
  public:
    bool isAxisDefined( eAxisType type );
    //    bool setAnnotationLabels( Widget w );
    bool isAnnotationLabelsUpdated( TransactionNumber trans );
    tPlotItemVector &plotItems( eAxisType type );
    int cycle(){ return m_cycle; }
    int yIndex(){ return m_yGuiIndex; }
    int xIndex(){ return m_xGuiIndex; }
    bool hasAxisWildcardIndex();
    eAxisType addPlotItem( PlotItem *item );
    void setAllXPlotItems( PlotItem *item);
    void getXPlotDataItems( std::set<GuiPlotDataItem *> &plotDataItems );
    /* tPlotItemVector getYPlotGuiIndexItems(); */
    //    XrtDataType getXrtDataType();
  private:
    tPlotItemVector       m_yPlotItems[2];
    int                   m_cycle;
    int                   m_yGuiIndex;
    int                   m_xGuiIndex;
  };

  typedef std::vector<Plot *> tPlotVector;
  typedef tPlotVector::iterator tPlotIterator;

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
 public:
  void setSize( int width, int height ){
    m_initial_width = width;
    m_initial_height = height;
    m_initial_size = true;
  }
  /// Setzt den Titeltext
  bool setHeaderText( const std::string &headerText );
  /// Setzt den Titeltext
  bool setHeaderStream( const std::string &headerStream );
  /// Setzt den Fusstext
  bool setFooterText( const std::string &footerText );
  /// Setzt den Fusstext
  bool setFooterStream( const std::string &footerStream );
  /// Setzt den Menuetext
  bool setMenuText( const std::string &menuText );
  void deleteMultiLineString( char **multiline );
  virtual void drawHeaderText()=0;
  virtual void drawFooterText()=0;
  void setLabel( const std::string &title, eAxisType axis );
  const std::string &MenuLabel();
  const std::string PlotItemLabel(GuiPlotDataItem* yItem);
  static char** separateLines( const std::string &str );
  static char** separateStreamLines( const std::string &str );
  static std::string convertToString( Stream *s );
  static char** copyCharArray( char** text );
  virtual void showAnnotationLabels( bool status ) = 0;
  virtual void setWithScrollBar( bool flag ) = 0;
  void setAllCycles( bool flag ){ m_allCycles = flag; }
  virtual void setLabelStream( const std::string &stream, eAxisType axis );
  virtual void setYPlotStyle( eStyle style, eAxisType axis ) = 0;
  void setBarStyleOption_PlotItemGrouped() {  m_barStyleOption_plotItemGrouped = true; }

  PlotItem *addDataItem( GuiPlotDataItem *dataitem, PlotItem *xPlotItem );
  /// Logarithmische x-Skala
  virtual void setLogX( bool value = true ) = 0;
  /// Logarithmische y-Skala
  virtual void setLogY( bool value = true ) = 0;
  /// x-Achsenursprung festsetzen
  virtual void setOriginXAxis( const double origin ) = 0;
  /// y-Achsenursprung festsetzen
  virtual void setOriginYAxis( const double origin )  = 0;

  virtual void withAnnotationOption( bool state ) = 0;
  virtual bool hasAnnotationOption() = 0;
  /// Stil von Konfigurationsdialog festlegen
  virtual void setConfigDialogStyle( ConfigDialog::eStyle style, int length ) = 0;
  virtual void setPrintStyle( bool status )           = 0;
  virtual void showMINMAX( bool show, int precision ){ m_showMINMAX = show; m_precMINMAX = precision; }
  virtual void showAVG( bool show, int precision )   { m_showAVG = show; m_precAVG = precision; }
  virtual void showRMS( bool show, int precision )   { m_showRMS = show; m_precRMS = precision; }
  /** get all selected points
   */
  virtual void getSelectionPoints(tPointVector& pts,
				  std::vector<int>& axisType, std::vector<std::string>& axisTitle) = 0;

  void setFunction(JobFunction* func);
  void setColorsetName( const std::string &name );
  ColorSet *colorset(){ return m_colorset; }
  char **pheaderText(){ return m_pheaderText; }
  char **pfooterText(){ return m_pfooterText; }
  const std::string &getName(){ return m_name; }
  /// Ereignis vom Button 'Reset' des Konfigurationsdialoges
  virtual void configDialogResetEvent(){}
  void scaleDialogResetEvent();
  void setPlotAxisScale( double min, double max, eAxisType axistype );
  void setPlotAxisScale( XferDataItem * min, Scale *min_scale, XferDataItem * max, Scale *max_scale, eAxisType axistype );
  void setPlotAxisFormat( eAxisType axistype, const std::string& format );
  bool isAxisUpdated( TransactionNumber trans );
  bool isScaleUnitUpdated();
  void setAxisHide(Gui2dPlot::eAxisType axistype){
    m_axisHiddenSet.insert(axistype);
  }
  bool isAxisShow(Gui2dPlot::eAxisType axistype){
    return m_axisHiddenSet.count(axistype) == 0;
  }
  void setAxisAspectRatio(Gui2dPlot::eAxisType axistype, double value);
  void setAxisAspectRatioXfer(Gui2dPlot::eAxisType axistype, XferDataItem* xfer);
  void setAxisAspectRatioReferenceAxis(Gui2dPlot::eAxisType axistype);
  int getInitialHeight(){ return m_initial_height; }
  int getInitialWidth(){ return m_initial_width; }
  bool AspectRatioMode();
  eAxisType AspectRatioReferenceAxis();

/*=============================================================================*/
/* public function of ScaleDialogListener & ConfigDialogListener               */
/*=============================================================================*/

  virtual void resetEvent( GuiEventData *event );
  virtual void closeEvent( GuiEventData *event );
  virtual void scaleUpdate(){ getElement()->update( GuiElement::reason_Always );}
  virtual void cyclesUpdate(){ getElement()->update( GuiElement::reason_Always );}

protected:
  /** get Curve Palette */
  virtual Plot2dCurvePalette* getPalette(const std::string& curveName) = 0;
  /** get Curve Info */
  std::string getCurveInfo(tPointVector& pts);
  bool getAllCycles() { return m_allCycles; }
  void updateAxisTypes();
  virtual void doPlot(bool clear_always) = 0;
  virtual void buildCyclesDialog() = 0;
  void updateAxisScaleValues();
  DataReference *getDRefStruct(){ return m_drefStruct; }
  virtual void configureMenu( PlotItem *, int ){}
  DataPoolIntens &dpi(){ return DataPoolIntens::Instance(); }
  /// Liefert True wenn sich ein DataItem geaendert hat
  bool isDataItemUpdated( TransactionNumber trans );
  XferDataItem *initXfer( const std::string &item, int idx );
  ConfigDialog *getConfigDialog() const { return m_configDialog; }
  void setConfigDialog( ConfigDialog *d ){ m_configDialog = d; }
  ScaleDialog *getScaleDialog(){ return m_scaleDialog; }
  CyclesDialog *getCyclesDialog(){ return m_cyclesDialog; }
  void setScaleDialog( ScaleDialog *d ){ m_scaleDialog = d; }
  void setCyclesDialog( CyclesDialog *d ){ m_cyclesDialog = d; }
  PlotAxis& getAxis(int idx){ return m_axis[idx]; }
  bool hasInitialSize() { return m_initial_size; }
  Stream* getTitleStream(eAxisType axis);
  const std::string getTitle(eAxisType axis);
  bool isBarStyleOptionPlotItemGrouped() { return m_barStyleOption_plotItemGrouped; }
  std::string getXferUiMode();
  void setXferUiMode(std::string mode);
  int getOverrideSymbolSize();
  void serializeXML(std::ostream&, bool recursive = false);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
  bool serializePlotCurveData(google::protobuf::RepeatedPtrField<in_proto::Plot2D::PlotItem>* jsonObj, QRectF* bRect);
#endif
  bool serializePlotCurveData(Json::Value& jsonObj, QRectF* bRect);
  bool serializePlotCurveInfo(Json::Value& jsonObj, int curveIndex);
  int readData(PlotItem* item, int xWildcardIndex, int yWildcardIndex,
               tPointVector& pts, tMarkerDataVector& markerLabels,
               QRectF* bRect=0);
  /** Diese Funktion ruft eine variante Methode auf.
      Die Argumente werden diese Methode übergeben.
      Das Result wird als Json-String zurückgeben.
  */
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng);
  virtual void fullscreen() = 0;
  void refreshAllDataItemIndexedList();
  JobFunction* getFunction() { return m_function; }
private:
  void createLocalData();
  std::string getAxisStyle(eStyle style);
  bool schrinkPlotCurveData(tPointVector& pts, tMarkerDataVector& markerLabels, QRectF* bRect, PlotItem* item);

  const std::string getXAxisPlotItemTitles() {
    std::ostringstream os;
    for(std::vector<std::string>::iterator iter = xAxisPlotItemTitles.begin();
        iter != xAxisPlotItemTitles.end(); ++iter ){
      if( os.str().size() )
        os << "  ";
      os << *iter;
    }
    return os.str();
  }

 protected:
  static const int     AXIS_TYPE_COUNT = 4;
  std::vector<std::string> xAxisPlotItemTitles;
  tPlotVector          m_plots;
  tPlotItemVector      m_plotItems;
  tPlotItemMap         m_dummyItems;
  std::vector<int>     m_showCycleVector;
  eStyle               m_style[2];
  bool                 m_isCloned;
  bool m_showMINMAX;
  bool m_showAVG;
  bool m_showRMS;
  int  m_precMINMAX;
  int  m_precAVG;
  int  m_precRMS;

private:
  std::string          m_title[AXIS_TYPE_COUNT];
  Stream              *m_titleStream[AXIS_TYPE_COUNT];
  PlotAxis             m_axis[AXIS_TYPE_COUNT];

  const std::string    m_name;
  JobFunction         *m_function;
  DataReference       *m_drefStruct;
  ConfigDialog        *m_configDialog;
  ScaleDialog         *m_scaleDialog;
  CyclesDialog        *m_cyclesDialog;

  char               **m_pheaderText;
  Stream              *m_pheaderStream;
  char               **m_pfooterText;
  Stream              *m_pfooterStream;
  std::string          m_menuText;

  int                  m_initial_height;
  int                  m_initial_width;
  bool                 m_initial_size;
  AXISTYPESET          m_axisHiddenSet;
  bool                 m_barStyleOption_plotItemGrouped;
  std::string          m_colorsetName;
  ColorSet            *m_colorset;
  XferDataItem        *m_uiMode;
  XferDataItem        *m_overrideSymbolSize;
  bool                 m_aspectRatioMode;
  eAxisType            m_aspectRatioReferenceAxis;

  bool                 m_allCycles;
  int                  m_maxCurveLen;
};
#endif
