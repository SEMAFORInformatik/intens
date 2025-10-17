
#ifndef GDEVQT_H
#define GDEVQT_H

/** GdevQt.
    Die Klasse GdevQt sorgt für die Ausgabe von Grafikelementen
    auf dem Bildschirm
*/

#include <string>
#include <vector>
#include <QPen>

#include "plot/Gdev.h"

class QWidget;
class QtCanvas;
class QColor;

class GdevQt: public Gdev {
public:
  /**@name methods */
  //@{
  /// Konstruktor
  /** Der Konstruktor hat die folgenden Parameter:
      @param widget QWidget
  */
  GdevQt( QtCanvas *w, const std::string &xfontlist="" );
  /// Destruktor
  virtual ~GdevQt();
  /** Linienbreite setzen.
      @param lwidth definiert die Linienbreite
    */
  virtual void setLinewidth( int lwidth );
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
  virtual void setLinestyle ( int lstyle );
  ///  setzt die Zeichengr\"osse
  virtual void setTextSize( double size );
  /** setzt die horizontale Ausrichtung:
      @param hjust
      \begin{tabular}{ll}
      1 & linksbündig \\
      2 & zentriert \\
      3 & rechtsbündig \\
      \end{tabular}
  */
  virtual void setTextHjust( HJust hjust );
  /** setzt die vertikale Ausrichtung:
      @param vjust
      \begin{tabular}{ll}
      0 & kopfbündig \\
      1 & zentriert \\
      2 & fussbündig \\
      \end{tabular}
  */
  virtual void setTextVjust( VJust vjust );
  /** setzt den Drehwinkel von Texten.
      @param angle Drehwinkel
  */
  virtual void setTextAngle( double angle );
  /**  setzt den Schrifttyp
      @param fontno Index des Schrifttyps
  */
  virtual void setTextFont( int fontno );

  virtual void setTextAttributes( double size, HJust hjust, VJust vjust
				, double angle, int fno );
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
  virtual void setMarkerSymbol( int msymbol );
  /// setzt die Hintergrundfarbe
  virtual void setBackgroundColor( int bcolor );
  /** setzt ein Markersymbol.
      @param x,y Koordinaten des Mittelpunktes
  */
  virtual void mark( double x, double y );
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
  virtual void mark( double x, double y, int msymbol);
  /** gibt Text an der aktuellen Position aus.
      @param text auszugebender Textstring
  */
  virtual void putText( const std::string &text );
  /** zeichnet ein Polygon.
      @param x,y Koordinaten der Eckpunkte
  */
  virtual void drawPolygone( const std::vector<double>&x
			     , const std::vector<double>&y );
  /** setzt die Füllfarbe von Polygonen
      @param col Fülfarbe (für transparente Darstellung -1 angeben)
  */
  virtual void setFillColor( int col );
  /** setzt den Begrenzungstyp von Polygonen.
      @param edge zeichnet Kanten falls true
  */
  virtual void setEdgeStyle( bool edge );
  /**  zeichnet einen Ellipsenbogen.
      @param xc,yc Mittelpunktkoordinaten
      @param a,b Achsenlänge
      @param a0,a1 Anfang- und Endwinkel
  */
  virtual void drawArc( double xc, double yc
			, double a, double b
			, double a0, double a1);
  /// Vordergrund setzen
  virtual void setColor( int icol );
  /// endpage setzen
  virtual void endPage();
  /** verschiebt die aktuelle Position.
       @param x,y Koordinaten der neuen Position
  */
  virtual void moveTo( double x, double y );
  /** Zeichnet eine Linie mit der aktuellen
      Position als Anfangspunkt.
      @param x,y Koordinaten des Endpunktes
  */
  virtual void drawTo( double x, double y ) ;
  /** zeichnet mehrere Linien
      @param x,y Koordinaten der Eckpunkte
  */
  virtual void drawLines( const std::vector<double>& x
	     , const std::vector<double>&y );
  /// Grafikpuffer leeren
  virtual void flush();
  /// Zeichenfeld löschen
  virtual void clear();
  /// Abschliessende Arbeiten
  virtual void finallyWork();
  //@}
  virtual double pixelsize() const{ return 1. ; }

 private:
  void setBoundingBox();

 private:
#define MAXFONTS 20
#define MAXCHARSETS 5

  std::string FONTPATTERN;

/*   struct{ */
/*     int          nfonts; */
/*     char 	 **fontlist; */
/*     XFontStruct  *font[MAXFONTS]; */
/*     int          size[MAXFONTS]; */
/*   } DAfont[MAXCHARSETS]; */
  int GDncharsets;

/*   GC m_gc, m_fillgc; */
  std::vector<QColor> m_pixcol;
  QtCanvas *m_widget;

  int m_currx, m_curry;
  int m_xmin, m_xmax;
  int m_ymin, m_ymax;

  double m_size, m_angle;
  HJust  m_hjust;
  VJust  m_vjust;
  int    m_align;
  int    m_fontno, m_charset;
  int    m_fillcolor, m_ncolors;
  bool   m_edge;


  QColor* m_currColor;
  int     m_currLineWidth;
  Qt::PenStyle    m_currLineStyle;
};
#endif
