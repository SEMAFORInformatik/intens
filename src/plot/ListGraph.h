/* $Id: ListGraph.h,v 1.16 2009/02/11 12:38:44 amg Exp $
 * Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *
 */
#ifndef LISTGRAPH_H
#define LISTGRAPH_H

#include <string>
#include <deque>
#include "plot/Frame.h"

class Stream;
class GuiPlotDataItem;
class ListPlot;
class GuiVerticalContainer;
class GuiDataField;
class GuiHorizontalContainer;

/** ListGraph
    Mit der Klasse 'ListGraph' k\"onnen vorhandene Daten in einem x/y-Diagramm
    grafisch dargestellt werden.
    @version $Id: ListGraph.h,v 1.16 2009/02/11 12:38:44 amg Exp $
*/
class ListGraph {
 public:
  enum Options
  { Invalid
  , Dynamic
  , Static
  , Invisible
  };
  /** Konstruktor
      @param name Name 
  */
  ListGraph( const std::string &name, ListPlot *plot, const std::string &id );
  /** Destruktor
   */
  virtual ~ListGraph();
  /** Setzt den Titel
      @param title neuer Text
  */
  bool setStringTitle( const std::string &title );
  /** Setzt den Titel 
      @param title Stream der den neuen Text enth\"aelt
  */
  bool setStreamTitle( const std::string &titlestream );
  /** Liefert den Titel als String
      @return Titel
  */
  std::string getTitle();
  /** F\"ugt ein (weitere) Y-Achse hinzu
      @param name GuiPlotDataItem
  */
  void addGraphItem( GuiPlotDataItem *dataitem );
  /** Ordnet der x-Achse ein DataItem zu
      @param name GuiPlotDataItem
  */
  void setXaxis( GuiPlotDataItem *dataitem );
  /** Die Einteilung der x-Achse erfolgt logarithmisch
   */
  void setLogX();
  /** Prüft, ob bei logx auch die X-Achse definiert ist
   */
  bool checkGraph();
  /** Setzt die Einteilung der Y-Achse logarithmisch
   */
  void setLogY();
  /** Legt den Ursprung der x-Achsen fest
      @param orig Ursprung
  */
  void setAxesOriginX( double orig );
  /** Legt den Ursprung der y-Achse(n) fest
      @param orig Ursprung
  */
  void setAxesOriginY( double orig );
  /** Setzt eine geminsame y-Achsenunterteilung fuere alle y-Achsen
   */
  void setSameYRange();
  /** Wandelt einen Stream in einen String um
      @return String
   */
  std::string StreamToString( Stream *s );
  /** Liefert die Anzahl der y-Achsenelemente
      @return Anzahl der Y-Achsen
   */
  int nyaxis();
  /** Pr\"uft ob sich eine Dataitem geaendert hat
      @return true wenn sich ein Dataitem ge\"andert hat
   */
  bool isDataItemUpdated( TransactionNumber trans );

  bool acceptIndex( const std::string &, int );

  void setIndex( const std::string &name, int inx );

  /** Werte im Datenpool aktualisieren
      @param reset Flag zum Zuruecksetzen der von Hand eingestellten Werten
   */ 
  void writeValues( bool reset=false );
  /** Lokale Werte aktualisieren
   */
  void readValues();

  /** Zeichnet den Graph 
      @param gdev Ger\"atedevice
      @param plotarea Plotbereich
      @param nhelmnts Anzahl horizontaler Elemente
   */
  void drawGraph( Gdev *gdev, const Frame &plotarea, int nhelmnts );
  /** Berechnet die einzelnen Plotbereiche
      @param plotarea Plotbereich
      @param titlearea Plotbereich f\"ur den Titel
      @param grapharea Plotbereich f\"ur den Graph
      @param xaxisarea Plotbereich f\"ur die X-Achse
      @param yaxisareas Plotbereich f\"ur die einzelnen Y-Achsen
      @param nhelmnts Anzahl horizontaler Elemente
   */
  void setSubdivisions( const Frame &plotarea,
		        Frame &titlearea,
			Frame &grapharea,
			Frame &xaxisarea,
			std::vector<Frame> &yaxisareas,
			int nhelmnts );
  /** Zeichnet ein Raster
      @param viewport Ausschnitt im Plotbereich
   */
  void drawGrid( const Frame &viewport );
  /** Zeichnet die X-Achse
      @param viewport Plotbereich
      @param values Skalierungen (Vektor)
      @param dataitem Achsenelement
   */
  void drawXAxis( const Frame &viewport, 
		  const std::deque<double> &values,
		  GuiPlotDataItem *dataitem );
  /** Zeichnet eine Y-Achse
      @param viewport Plotbereich
      @param values Skalierungen (Vektor)
      @param column Nummer der Spalte (erstes Element = 1)
   */
  void drawYAxis( const Frame &viewport, 
		  const std::vector<double> &values,
		  int column );
  /** Zeichnet ein Label einer Y-Achse
      @param viewport Plotbereich
      @param dataitem Achsenelement
      @param column Nummer der Spalte (erstes Element = 1)
   */
  void drawYAxisLabel( const Frame &viewport, 
		       GuiPlotDataItem *dataitem,
		       int column );
  /** Sucht Minimal- und Maximalwerte
      @param true wenn die letzten vorhanden Werte ber\"cksichtigt 
             werden sollen
      @return gr\"osste Dimension
   */
  int calculateMinMax( bool first );
  /** Korrigiert die Schrittweite der Achse so das alle Werte in dieser 
      dargestellt werden k\"onnen 
      @param steps Anzahl Unterteilungen
      @param min Minimalwert
      @param max Maximalwert
      @param yAxis is YAxis
      @return korrigierte Schrittweite
   */
  double calculateNewRange( int steps, double min, double max, bool yAxis=true );
  /** Formatiert die Y-Achse(n) fuer die Logarithmische Darstellung
      @param nyitems Anzahl der Y-Achsen
      @return true wenn der Vorgang erfolgreich abgeschlossen wurde
   */
  bool calculateNewRangeLog( int nyitems );
  /** Formatiert die X-Achse fuer die Logarithmische Darstellung
      @return true wenn der Vorgang erfolgreich abgeschlossen wurde
   */
  bool calculateNewRangeXLog();
  /** Zeichnet eine Kurve
      @param viewport Plotbereich
      @param windowsize Reale Abmessungen
      @param dataitem Achsenelement
      @param xvalues X-Achsenwerte (Vektor)
      @param column Nummer der Spalte (erstes Element = 1)
   */
  void drawValues( const Frame &viewport, 
		   Frame &windowsize,
		   GuiPlotDataItem *dataitem,
		   std::vector<double> &xvalues,
		   int column );
  /** Zeichnet den Titel
      @param viewport Plotbereich
   */
  void drawTitle( const Frame &viewport );
  /** Generiert den eigenen Anteil am  Konfigurationsdialog
      @param cont Parent
   */
  void buildConfigDialog( GuiElement *cont );
  /** Upadate der Felder im Konfigurationsdialog
   */
  void cDupdateDatafields();
  const std::string &getName(){ return m_name; }
 public:
  Frame viewport;

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  void createDataReference();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  const std::string      m_name;
  const std::string      m_id;
  std::string            m_title;
  Stream                *m_titleStream;
  GuiPlotDataItem       *m_xaxis;
  std::vector<GuiPlotDataItem*> m_yaxes;
  Gdev                  *m_gdev;
  ListPlot              *m_plot;
  DataReference         *m_listGraphStruct;
  DataReference         *m_drefname;
  DataReference         *m_drefmin;
  DataReference         *m_drefmax;
  DataReference         *m_drefstate;

  int                    m_numberOfMarks;
  bool                   m_sameyrange;        // Gleiche Y-Achsenskalierung fuer alle Kurven

  bool                   m_isOriginXDef;      // Nullpunkt der X-Achse wird manuell gesetzt
  double                 m_originX;           // Nullpunkt der X-Achse
  int                    m_ndivx;             // Anzahl der x-Achsenunterteilungen
  int                    m_nmaxdivx;          // Max. Unterteilungen der x-Achse
  bool                   m_logx;              // Logarithmische Einteilung der X-Achse

  bool                   m_isOriginYDef;      // Nullpunkt der Y-Achse wird manuell gesetzt
  double                 m_originY;           // Nullpunkt der X-Achse
  int                    m_ndivy;             // Anzahl der y-Achsenunterteilungen
  int                    m_nmaxdivy;          // Max. Unterteilungen der y-Achse
  bool                   m_logy;              // Logarithmische Einteilung der Y-Achse
  bool                   m_lessThanOriginY;   // Gibt es Werte kleiner als OriginY

  std::vector<int>         m_options;           // Achsen-Optionen (Definiert in Typ 'Options')
  std::vector<double>      m_orgmaxvalues;      // Maximalwerte der y-Achsen
  std::vector<double>      m_outmaxvalues;      // Maximalwerte der y-Achsen zur Ausgabe
  std::vector<double>      m_orgminvalues;      // Minimalwerte der y-Achsen
  std::vector<double>      m_outminvalues;      // Minimalwerte der y-Achsen zur Ausgabe
  std::vector<std::string> m_itemnames;         // Namen der Dataitems
  int                    m_optionsxaxis;
  double                 m_orgmaxxaxis;
  double                 m_outmaxxaxis;
  double                 m_orgminxaxis;
  double                 m_outminxaxis;
  std::string            m_xaxisname;

  std::vector<GuiDataField*>  m_cDminDataFields;  
  std::vector<GuiDataField*>  m_cDmaxDataFields;  
  std::vector<GuiDataField*>  m_cDstateDataFields;  
};

#endif
