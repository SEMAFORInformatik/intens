#ifndef GDEV_H
#define GDEV_H
/** Gdev is a abstract base class for diplaying
    simple graphical elements
*/

#include <string>
#include <vector>

class Gdev {
 public:
  typedef enum {
    ORIENT_PORTRAIT,
    ORIENT_LANDSCAPE }
  Orientation;

  typedef enum {
    HLEFT = 1, HCENTER = 2, HRIGHT = 3 }
  HJust;

  typedef enum {
    VBOTTOM = 1, VCENTER = 2, VTOP = 3 }
  VJust;

  /**@name Methoden */
  //@{
  /// Konstruktor
  /** Der Konstruktor legt den Window- und Viewport-Bereich fest
      @param wx0,wx1,wy0,wy1 Koordinaten der Window-Eckpunkte
      @param vx0,vx1,vy0,vy1 Koordinaten der Viewport-Eckpunkte
  */
  Gdev( double wx0=0.0, double wx1=1.0
	, double wy0=0.0, double wy1 =1.0
	, double vx0=-1.0, double vx1=1.0
	, double vy0=-1.0, double vy1=1.0 );
  virtual ~Gdev(){}
  virtual void copyPixmapIntoDarea(){}
  virtual void clearPixmap(){}
  enum GdevType
    { type_Base = 0
    , type_Svg = 1
    };
  virtual GdevType type() { return type_Base; }

  /** Window-Bereich festlegen
      @param wx0,wy0,wx1,wy1 Koordinaten der Eckpunkte
      (Punkte, die ausserhalb liegen, werden abgeschnitten) */
  void setWindow( double wx0, double wy0
		, double wx1, double wy1 );
  /** Viewport-Bereich festlegen
      @param vx0,vy0,vx1,vy1 Koordinaten der Eckpunkte */
  void setViewport( double vx0, double vy0
		  , double vx1, double vy1 );
  /** Standardeinstellungen setzen
    */
  void setDefaultSettings();
  /** Aktuelle Abmessungen des Ausgabebereichs setzen
      @param w,y Breite und Hoehe vom Ausgabebereich */
  void setCurrentArea( double w, double h );
  /** Linienbreite setzen.
      @param lwidth definiert die Linienbreite
    */
  virtual void setLinewidth( int lwidth )=0;
  /** Linienart festlegen.
      @param lstyle
      \begin{tabular}{ll}
      0 & ----------------\\
      1 &  -\ \ -\ \ -\ \ -\ \ - \\
      2 &  ----\ \ \ \ ----    \\
      3 &  --\ -\ --\ --\ -\ -- \\
      4 &  --\ -\ -\ -\ --\ -\ -\ -\\
      5 &  -\ --\ -\ -\ -\ --\ -\ -\\
      6 &  -\ -\ -\ -\ -\ -\ -\ -\ \\
      \end{tabular}
  */
  virtual void setLinestyle ( int lstyle )=0;
  ///  setzt die Zeichengr\"osse
  virtual void setTextSize( double size )=0;
  /** setzt die horizontale Ausrichtung:
      @param hjust
      \begin{tabular}{ll}
      1 & linksbündig \\
      2 & zentriert \\
      3 & rechtsbündig \\
      \end{tabular}
  */
  virtual void setTextHjust( HJust hjust )=0;
  /** setzt die vertikale Ausrichtung:
      @param vjust
      \begin{tabular}{ll}
      1 & kopfbündig \\
      2 & zentriert \\
      3 & fussbündig \\
      \end{tabular}
  */
  virtual void setTextVjust( VJust vjust )=0;
  /** setzt den Drehwinkel von Texten.
      @param angle Drehwinkel
  */
  virtual void setTextAngle( double angle )=0;
  /**  setzt den Schrifttyp
      @param fontno Index des Schrifttyps
  */
  virtual void setTextFont( int fontno )=0;
  /** setzt die folgenden Textoptionen:
      @param vjust
      \begin{tabular}{ll}
      size  & Schriftgroesse \\
      hjust & horizontale Ausrichtung \\
      vjust & vertikale Ausrichtung \\
      angle & Drehwinkel \\
      fno   & Index des Schrifttyps \\
      \end{tabular}
  */
  virtual void setTextAttributes( double size, HJust hjust, VJust vjust
				, double angle, int fno )=0;
  /** setzt das Format des Markersymbols
      @param msymbol
      \begin{tabular}{ll}
      0 & $\bullet$ \\
      1 & $+$ \\
      2 & $\ast$ \\
      3 & $\circ$\\
      4 & $\times$\\
      5 & $\bigtriangleup$\\
      6 & $\square$ \\
      7 & $\blacksquare$ \\
      \end{tabular}
  */
  virtual void setMarkerSymbol( int msymbol )=0;
  /// setzt die Hintergrundfarbe
  virtual void setBackgroundColor( int bcolor )=0;
  /** setzt ein Markersymbol.
      @param x,y Koordinaten des Mittelpunktes
  */
  virtual void mark( double x, double y )=0;
  /** setzt ein Markersymbol.
      @param x,y Koordinaten des Mittelpunktes
      @param msymbol
      \begin{tabular}{ll}
      0 & $\bullet$ \\
      1 & $+$ \\
      2 & $\ast$ \\
      3 & $\circ$\\
      4 & $\times$\\
      5 & $\bigtriangleup$\\
      6 & $\square$ \\
      7 & $\blacksquare$ \\
      \end{tabular}
  */
  virtual void mark( double x, double y, int msymbol)=0;
  /** gibt Text an der aktuellen Position aus.
      @param text auszugebender Textstring
  */
  virtual void putText( const std::string &text ) =0;
  /** zeichnet ein Polygon.
      @param x,y Koordinaten der Eckpunkte
  */
  virtual void drawPolygone( const std::vector<double>&x
			     , const std::vector<double>&y )=0;
  /** setzt die Füllfarbe von Polygonen
      @param col Fülfarbe (für transparente Darstellung -1 angeben)
  */
  virtual void setFillColor( int col )=0;
  /** setzt den Begrenzungstyp von Polygonen.
      @param edge zeichnet Kanten falls true
  */
  virtual void setEdgeStyle( bool edge )=0;
  /**  zeichnet einen Ellipsenbogen.
      @param xc,yc Mittelpunktkoordinaten
      @param a,b Achsenlänge
      @param a0,a1 Anfang- und Endwinkel
  */
  virtual void drawArc( double xc, double yc
			, double a, double b
			, double a0, double a1)=0;
  /// Vordergrund setzen
  virtual void setColor( int icol )=0;
  /// endpage setzen
  virtual void endPage()=0;
  /** verschiebt die aktuelle Position.
       @param x,y Koordinaten der neuen Position
  */
  virtual void moveTo( double x, double y )=0;
  /** Zeichnet eine Linie mit der aktuellen
      Position als Anfangspunkt.
      @param x,y Koordinaten des Endpunktes
  */
  virtual void drawTo( double x, double y ) =0;
  /** zeichnet mehrere Linien
      @param x,y Koordinaten der Eckpunkte
  */
  virtual void drawLines( const std::vector<double>& x
	     , const std::vector<double>&y )=0;
  /// Grafikpuffer leeren
  virtual void flush()=0;
  /// Zeichenfeld löschen
  virtual void clear()=0;
  /// Abschliessende Arbeiten
  virtual void finallyWork()=0;
  /// gibt transformierten X-Koordinatenwert zurück
  virtual double getX( double x );
  /// gibt transformierten Y-Koordinatenwert zurück
  virtual double getY( double y );
  /// setzt den kleinsten und grössten Wert
  void setXMinMax( const double x );
  void setYMinMax( const double y );
  /// gibt transformierte Laenge (auf X-Achse) zurück
  double getTX( double l ) const;
  /// gibt Streckungsfaktor fur Text zurueck
  double getTText() const;
  /// gibt transformierte Laenge (auf Y-Achse) zurück
  double getTY( double l ) const;
  /// gibt Streckungsfaktor (auf X-Achse) zurück
  double getTX() const;
  /// gibt Streckungsfaktor (auf Y-Achse) zurück
  double getTY() const;
  /** prueft ob sich der Punkt innerhalb der Zeichenflaeche befindet
      @param x,y Koordinaten
  */
  bool pointIsVisible( int x, int y );
  /** prueft ob sich die Linie innerhalb der Zeichenflaeche befindet
      @param x0,y0,x1,y1 Anfangs- und Endpunktkoordinate der Linie
  */
  bool lineIsVisible( int x0, int y0, int x1, int y1 );
  double getWidth() { return m_width; }
  double getHeight() { return m_height; }
  void invert( bool status );
   //@}
  void stretchText( double stretch ){ m_textStretch = stretch; }
  double textStretch(){ return m_textStretch; }
  virtual double pixelsize() const = 0;
  double tx(){ return m_tx; }
  double qx(){ return m_qx; }
  double ty(){ return m_ty; }
  double qy(){ return m_qy; }
  double xMax() const { return m_xMax; }
  double xMin() const { return m_xMin; }
  double yMax() const { return m_yMax; }
  double yMin() const { return m_yMin; }

 protected:
  /// Transformationsparameter berrechnen
  void calcTransParam();

 private:
  /// Viewport-Koordinaten berrechnen
  void calcViewportCoord();
 private:
  double m_qx, m_qy, m_tx, m_ty; // Transformationsparameter
  double m_tText;                // Transformationsparameter fuer Text
  double m_width, m_height;      // Ausgabebereich
  double m_currx, m_curry;       // current position
  double m_wx0, m_wy0;           // linker untere Ecke vom Window
  double m_wx1, m_wy1;           // rechte obere Ecke vom Window
  double m_vx0, m_vy0;           // linker untere Ecke vom Viewport
  double m_vx1, m_vy1;           // rechte obere Ecke vom Viewport
  bool   m_invert;
  double m_textStretch;
  double m_xMax;
  double m_xMin;
  double m_yMax;
  double m_yMin;

/*   double m_lmarg, m_rmarg; // Rechter und linker Seitenrand */
/*   double m_tmarg, m_bmarg; // Oberer und unterer Seitenrand */
/*   double m_wcap;   // Legendenbreite */
/*   int m_ncols;  // Anzahl Graphen horizontal */
/*   int m_nrows;  // Anzahl Graphen vertikal */
/*   int m_nitems; // Anzahl y-Achsen */
/*   int m_lsize;  // Fenstergroesse */
/*   int m_column; // Kolonne */
/*   int m_row;    // Zeile */
};
#endif
