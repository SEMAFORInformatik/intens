
#if !defined GUI_3DPLOT_INCLUDED_H
#define GUI_3DPLOT_INCLUDED_H

#include "jsoncpp/json/json.h"
#include "app/HardCopyListener.h"

class GuiPlotDataItem;

/**
   'GuiPlot3d' manages 3d-Plots
*/
class Gui3dPlot : public HardCopyListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
 public:
  Gui3dPlot();
  virtual ~Gui3dPlot();
  Gui3dPlot( const Gui3dPlot &plot);
private:
  Gui3dPlot &operator=(const Gui3dPlot&);

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
 public:
  enum eAxisType { XAXIS, YAXIS };
  enum Style { BAR, SURFACE, CONTOUR };

  virtual GuiElement* getElement() = 0;

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  void getData(std::vector<double>& xvalues,
                        std::vector<double>& yvalues,
                        std::vector< std::vector<double> >& zmatrix);

  /// Setzt die Stilparameter Bar, Surface, Mesh, Shaded, Contour und Zones
  virtual void setPlotStyleBar() = 0;
  /// Setzt die Stilparameter Bar, Surface, Mesh, Shaded, Contour und Zones
  virtual void setPlotStyleSurface() = 0;
  /// Setzt die Stilparameter Bar, Surface, Mesh, Shaded, Contour und Zones
  virtual void setPlotStyleContour() = 0;
  /// Setzt den Titeltext
  virtual bool setHeaderText( const std::string &headerText );
  /// Setzt den Titeltext
  virtual bool setHeaderStream( const std::string &headerStream );
  /// Setzt den Fusstext
  virtual bool setFooterText( const std::string &footerText );
  /// Setzt den Fusstext aud den uebergebenen Text
  virtual bool setFooterStream( const std::string &footerStream );
  /// Setzt den Menuetext
  virtual bool setMenuText( const std::string &menuText ) = 0;
  /** Ermoeglicht das setzen der x, y und z-Achse. Die z-Achse enthaelt die zu
      plotende Werte in Form einer Matrix. */
  virtual bool putDataItem( const std::string &name, GuiPlotDataItem *item ) = 0;
  /** getDataItemType */
  virtual GuiPlotDataItem* getPlotDataItem(std::string axis_name, int num=-1) = 0;

  void setSize(int w, int h) {
    m_width  = w;
    m_height = h;
    m_isInitialSize = false;
  }
  void getPlotSize(int& w, int& h) const{
    w = m_width;
    h = m_height;
  }
  bool isInitialSize() { return m_isInitialSize; }

  virtual void withAnnotationOption( GuiPlotDataItem* plotAxis, bool state ) = 0;
  virtual bool hasAnnotationOption( bool bXAxis ) = 0;

  virtual void showAnnotationLabels( bool bXAxis, bool status ) = 0;
  virtual bool isShownAnnotationLabels( bool bXAxis ) = 0;
  Style getPlotStyle() { return m_plotStyle.getStyle(); }

/*=============================================================================*/
/* protected Definitions                                                       */
/*=============================================================================*/
 protected:
  /** Zeichnet den Headertext */
  void drawHeaderText();
  /** Zeichnet den Footertext */
  void drawFooterText();

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  virtual void setTitle(const std::vector<std::string>& title) = 0;
  virtual bool isDataItemUpdated( TransactionNumber trans ) = 0;

  static std::string convertToString( Stream *s );
  static void separateLines(std::vector<std::string>& result, const std::string &str);
  static void separateStreamLines(std::vector<std::string>& result, const std::string &str);
  double getZValue(int ix, int iy,
                   std::vector<double>& xvalues, std::vector<double>& yvalues);

  /// Die Klasse 'PlotStyle' kapselt alle Parameter des XRT/3d Wideget ab
  class PlotStyle {
  public:
    PlotStyle() :
      m_style(SURFACE),
      m_flagDrawMesh(true),
      m_flagDrawShaded(true),
      m_flagDrawContours(true),
      m_flagDrawZones(true),
      m_flagDrawHiddenLines(false),
      m_flagDrawPerspective(true),
      m_perspectiveDepth(2.5),
      m_flagStyleValid(false) {
    }
    PlotStyle(const PlotStyle& p) :
      m_style(p.m_style),
      m_flagDrawMesh( p.m_flagDrawMesh ),
      m_flagDrawShaded( p.m_flagDrawShaded ),
      m_flagDrawContours( p.m_flagDrawContours ),
      m_flagDrawZones( p.m_flagDrawZones ),
      m_flagDrawHiddenLines( p.m_flagDrawHiddenLines ),
      m_flagDrawPerspective( p.m_flagDrawPerspective ),
      m_perspectiveDepth(  p.m_perspectiveDepth ),
      m_flagStyleValid( p.m_flagStyleValid ) {
    }

    /// Gibt den Style
    Style getStyle() {return m_style;}
    bool getFlagDrawMesh() { return m_flagDrawMesh; }
    bool getFlagDrawShaded() { return m_flagDrawShaded; }
    bool getFlagDrawContours() { return m_flagDrawContours; }
    bool getFlagDrawZones() { return m_flagDrawZones; }
    bool getFlagDrawHiddenLines() { return m_flagDrawHiddenLines; }
    bool getFlagDrawPerspective() { return m_flagDrawPerspective; }
    double getPerspectiveDepth() { return m_perspectiveDepth; }
    bool getFlagStyleValid() { return m_flagStyleValid; }

    void setStyle( Style style ) {m_style = style;}
    void setFlagDrawMesh( bool flagDrawMesh ) {
      m_flagDrawMesh = flagDrawMesh; }
    void setFlagDrawShaded( bool flagDrawShaded ) {
      m_flagDrawShaded = flagDrawShaded; }
    void setFlagDrawContours( bool flagDrawContours ) {
      m_flagDrawContours = flagDrawContours; }
    void setFlagDrawZones( bool flagDrawZones ) {
      m_flagDrawZones = flagDrawZones; }
    void setFlagDrawHiddenLines( bool flagDrawHiddenLines ) {
      m_flagDrawHiddenLines = flagDrawHiddenLines; }
    void setFlagDrawPerspective( bool flagDrawPerspective ) {
      m_flagDrawPerspective = flagDrawPerspective; }
    void setPerspectiveDepth( double perspectiveDepth ) {
      m_perspectiveDepth = perspectiveDepth; }
    void setFlagStyleValid( bool flagStyleValid ) {
      m_flagStyleValid = flagStyleValid; }

  private:
    Style            m_style;
    bool             m_flagDrawMesh;
    bool             m_flagDrawShaded;
    bool             m_flagDrawContours;
    bool             m_flagDrawZones;
    bool             m_flagDrawHiddenLines;
    bool             m_flagDrawPerspective;
    double           m_perspectiveDepth;
    bool             m_flagStyleValid;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
protected:
  PlotStyle m_plotStyle;
private:
  int    m_width;
  int    m_height;
  bool   m_isInitialSize;
  bool   m_isCloned;

  std::vector<std::string> m_pheaderText;
  Stream                  *m_pheaderStream;
  std::vector<std::string> m_pfooterText;
  Stream                  *m_pfooterStream;
};

#endif
