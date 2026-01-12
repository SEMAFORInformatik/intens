
#if !defined GUI_QT_3DPLOT_INCLUDED_H
#define GUI_QT_3DPLOT_INCLUDED_H

#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/GuiToggleListener.h"
#include "gui/Gui3dPlot.h"

class Stream;
class GuiPlotDataItem;
class GuiPopupMenu;
class GuiMenuToggle;
class GuiForm;
class GuiTextDialog;
class GuiDataField;
class GuiFieldgroupLine;
class InputChannelEvent;
class DataReference;
#if HAVE_QGRAPHS
class GuiQtSurfaceGraph;
class GuiQtBarGraph;
class GuiQt3dData;
#else
class GuiQwtContourPlot;
class GuiQwtContourPlotData;
class QwtPlotCurve;
class QwtLinearColorMap;
#endif
class GuiQt3dBasePlot;

class QStackedWidget;
class Plot3D;
class Plot;
class Plot3dData;

/**
   Display 3d-Plots
   @author Copyright (C) 2025 SEMAFOR Informatik & Energie AG, Basel, Switzerland
*/

class GuiQt3dPlot : public GuiQtElement, public Gui3dPlot
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
 public:
  GuiQt3dPlot( GuiElement *parent, const std::string& name );
  virtual ~GuiQt3dPlot();
 private:
  GuiQt3dPlot( const GuiQt3dPlot &plot);
  void init();

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
 public:
  GuiElement* getElement() { return this; }
  virtual void withAnnotationOption( GuiPlotDataItem* plotAxis, bool state );
  virtual bool hasAnnotationOption( bool bXAxis );
  virtual void showAnnotationLabels( bool bXAxis, bool status );
  virtual bool isShownAnnotationLabels( bool bXAxis );

  class RedrawButtonListener : public GuiButtonListener
  {
  public:
    RedrawButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() { m_plot->update( reason_Always ); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQt3dPlot *m_plot;
  };

  class PrintButtonListener : public GuiButtonListener
  {
  public:
    PrintButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed();
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQt3dPlot *m_plot;
  };

  class ResetScaleButtonListener : public GuiButtonListener
  {
  public:
    ResetScaleButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() { m_plot->resetScaleParameters(); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQt3dPlot *m_plot;
  };

  class ResetRotationButtonListener : public GuiButtonListener
  {
  public:
    ResetRotationButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() { m_plot->resetRotationParameters(); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQt3dPlot *m_plot;
  };

  class StyleBarsButtonListener : public GuiToggleListener
  {
  public:
    StyleBarsButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { if( state ) m_plot->setPlotStyleBar(); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class StyleSurfaceButtonListener : public GuiToggleListener
  {
  public:
    StyleSurfaceButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { if( state ) m_plot->setPlotStyleSurface(); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class StyleContourButtonListener : public GuiToggleListener
  {
  public:
    StyleContourButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { if( state ) m_plot->setPlotStyleContour(); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class DrawHiddenLinesButtonListener : public GuiToggleListener
  {
  public:
    DrawHiddenLinesButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) {  m_plot->drawHiddenLines(); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class PerspectiveButtonListener : public GuiToggleListener
  {
  public:
    PerspectiveButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { m_plot->drawPerspective(); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class DeleteTextButtonListener : public GuiButtonListener
  {
  public:
    DeleteTextButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() {  m_plot->deleteText(); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQt3dPlot *m_plot;
  };

  class ShowTextButtonListener : public GuiToggleListener
  {
  public:
    ShowTextButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { m_plot->showText( state ); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class DrawMeshButtonListener : public GuiToggleListener
  {
  public:
    DrawMeshButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { m_plot->drawMesh(); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class DrawShadedButtonListener : public GuiToggleListener
  {
  public:
    DrawShadedButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { m_plot->drawShaded(); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class DrawContoursButtonListener : public GuiToggleListener
  {
  public:
    DrawContoursButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { m_plot->drawContours(state); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class DrawZonesButtonListener : public GuiToggleListener
  {
  public:
    DrawZonesButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ToggleStatusChanged( bool state ) { m_plot->drawZones(state); }
  private:
    GuiQt3dPlot *m_plot;
  };

  class ConfigButtonListener : public GuiButtonListener
  {
  public:
    ConfigButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() { m_plot->openConfigDialog(); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQt3dPlot *m_plot;
  };

  class CDresetButtonListener : public GuiButtonListener
  {
  public:
    CDresetButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() { m_plot->cDresetEvent(); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQt3dPlot *m_plot;
  };

  class CDcloseButtonListener : public GuiButtonListener
  {
  public:
    CDcloseButtonListener( GuiQt3dPlot *plot ) : m_plot( plot ) {}
    virtual void ButtonPressed() { m_plot->cDcloseEvent(); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQt3dPlot *m_plot;
  };

 public:

  // Ueberladene Funktionen der Klasse 'GuiElement'
  virtual GuiElement::ElementType Type() { return GuiElement::type_3dPlot; }
  virtual void create();
  virtual void manage();
  virtual void update( UpdateReason );
  virtual QWidget* myWidget();
  virtual bool acceptIndex( const std::string &, int );
  virtual void setIndex( const std::string &name, int inx );
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual void writeSettings();
  void readSettings();

  /// generates an svg, eps, pdf, ... file as output
  virtual bool generateFileWithSvgGenerator(std::string& filename);

  // Ueberladene Funktionen der Klasse 'HardCopyListener'
  virtual FileFormat getFileFormat();
  virtual const std::string &getName(){ return m_name; }
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual bool isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat );
  virtual PrintType getPrintType( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &MenuLabel();
  virtual bool write( InputChannelEvent &event );
  virtual bool write( const std::string &fileName );
  virtual bool getDefaultSettings( HardCopyListener::PaperSize &size,
				   HardCopyListener::Orientation &orient,
				   int &quantity,
				   HardCopyListener::FileFormat &format,
				   double &lm, double &rm,
				   double &tm, double &bm );
  virtual void print(QPrinter* print=0);
  virtual bool saveFile( GuiElement * );


  /// Setzt den Toggle Zustand der Stilvorgabe Buttons (Bars, Surface, Contour )
  void setDrawButtons();
  void setStyleButtons();
  /// Setzt die Stilparameter Bar, Surface, Mesh, Shaded, Contour und Zones
  void setPlotStyleBar();
  /// Setzt die Stilparameter Bar, Surface, Mesh, Shaded, Contour und Zones
  void setPlotStyleSurface();
  /// Setzt die Stilparameter Bar, Surface, Mesh, Shaded, Contour und Zones
  void setPlotStyleContour();
  /// Setzt den Menuetext
  bool setMenuText( const std::string &menuText );
  /** Ermoeglicht das setzen der x, y und z-Achse. Die z-Achse enthaelt die zu
      plotende Werte in Form einer Matrix. */
  bool putDataItem( const std::string &name, GuiPlotDataItem *item );

  /// Oeffnet den Konfigurationsdialog
  void openConfigDialog();
  /// Ereignis vom Button 'Reset' des Konfigurationsdialoges
  void cDresetEvent();
  /// Ereignis vom Button 'Close' des Konfigurationsdialoges
  void cDcloseEvent();

  // Callback Funktionen
  void popupMenu(const QContextMenuEvent* event);

  void resetScaleParameters();
  void resetRotationParameters();
  void drawHiddenLines();
  void drawPerspective();
  void deleteText();
  void toggleStyle();
  void drawMesh();
  void drawShaded();
  void drawContours(bool flag);
  void drawZones(bool flag);
  void showText( bool state );
#if HAVE_QTPLOT3D
  /** get 3d plot data */
  Plot3dData* getPlot3dData() { return m_data;  }
#endif
  /** draw marker line(s) */
  void drawMarkerLine();

  /** get count of displayed countour levels */
  int getCountContourLevels() { return m_contourLevels; }
/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  virtual void setTitle(const std::vector<std::string>& title);

  void setDetailStyle( bool mesh, bool shaded, bool contours, bool zones );
  void setAxesData();
  void createPopupMenu();
  void createDetailMenu( GuiPopupMenu *menu );
  void buildConfigDialog();
  void createDataReference();
  bool isDataItemUpdated( TransactionNumber trans );

  void setDataGrid();

  /** getDataItemType */
  GuiPlotDataItem* getPlotDataItem(std::string axis_name, int num=-1);

  /** create base colorMap */
#if !HAVE_QGRAPHS
  QwtLinearColorMap* createColorMap();
#endif
  bool setAnnotationLabels();
  bool isSurfaceType();
  bool isBarType();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::map<std::string, GuiPlotDataItem*> DataItemType;
  typedef DataItemType::value_type DataItemPair;

  const std::string m_name;
  QStackedWidget   *m_widgetStack;
  FlagStatus        m_with_frame;

  HardCopyListener::FileFormat  m_currentFileFormat;
  HardCopyListener::FileFormats2 m_supportedFileFormats;

  std::string               m_menuText;
  DataItemType         m_dataitems;

  std::string          m_fileName;
  DataReference       *m_drefStruct;
  DataReference       *m_drefCameraRotationX;
  DataReference       *m_drefCameraRotationY;
  DataReference       *m_drefDistnMethod;
  DataReference       *m_drefDistnTable;

  bool                 m_keyShiftPressed;
  int                  m_xIndex;
  int                  m_yIndex;
  bool                 m_showText;

  RedrawButtonListener           m_redrawButtonListener;
  PrintButtonListener            m_printButtonListener;
  ResetScaleButtonListener       m_resetScaleButtonListener;
  ResetRotationButtonListener    m_resetRotationButtonListener;
  StyleBarsButtonListener        m_styleBarsButtonListener;
  StyleSurfaceButtonListener     m_styleSurfaceButtonListener;
  StyleContourButtonListener     m_styleContourButtonListener;
  DrawHiddenLinesButtonListener  m_drawHiddenLinesButtonListener;
  PerspectiveButtonListener      m_perspectiveButtonListener;
  DeleteTextButtonListener       m_deleteTextButtonListener;
  ShowTextButtonListener         m_showTextButtonListener;
  DrawMeshButtonListener         m_drawMeshButtonListener;
  DrawShadedButtonListener       m_drawShadedButtonListener;
  DrawContoursButtonListener     m_drawContoursButtonListener;
  DrawZonesButtonListener        m_drawZonesButtonListener;
  ConfigButtonListener           m_configButtonListener;

  GuiMenuToggle *m_buttonStyleBars;
  GuiMenuToggle *m_buttonStyleSurface;
  GuiMenuToggle *m_buttonStyleContour;
  GuiMenuToggle *m_buttonDrawHiddenLines;
  GuiMenuToggle *m_buttonPerspective;
  GuiMenuToggle *m_buttonShowText;
  GuiMenuToggle *m_buttonDrawMesh;
  GuiMenuToggle *m_buttonDrawShaded;
  GuiMenuToggle *m_buttonDrawContours;
  GuiMenuToggle *m_buttonDrawZones;
  bool                m_PopupMenuCreated;

  GuiForm       *m_configDialog;
  CDresetButtonListener       *m_cDresetButtonListener;
  CDcloseButtonListener       *m_cDcloseButtonListener;
  GuiMenuToggle *m_cDDistnMethodToggleButton;
  GuiDataField  *m_fieldNumDistnLevels;
  GuiDataField  *m_fieldDistnTable;
  GuiFieldgroupLine  *m_lineDistnTable;

  GuiQtPopupMenu     *m_popupMenu;

#if HAVE_QGRAPHS
  GuiQt3dData       *m_data;
  GuiQtSurfaceGraph *m_contourWidget;
  GuiQtBarGraph     *m_barWidget;
#else
  GuiQwtContourPlotData *m_data;
  GuiQwtContourPlot     *m_contourWidget;
  std::vector<QwtPlotCurve*> m_markerCurves;
#endif

  std::vector<GuiQt3dPlot*> m_clonedList;  // cloned plot3d list

  // value vector for colorMap
  std::vector<double>    m_valueList;
  // color name vector for colorMap
  std::vector<std::string> m_colorList;
  // color name vector for colorMap
  int m_contourLevels;
  bool                  m_showAnnotationLabels[2];
  typedef std::map<GuiPlotDataItem*, bool> WithAnnotationMap;
  WithAnnotationMap m_withAnnotationOption;
};

#endif
