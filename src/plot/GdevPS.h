/* $Id: GdevPS.h,v 1.13 2007/06/06 09:30:43 amg Exp $
 * Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef GDEVPS_H
#define GDEVPS_H
#include <sstream>
#include <string>

#include "plot/Gdev.h"

/** GdevPS.
    Die Klasse GdevPS sorgt für die Ausgabe von Grafikelementen
    auf einen Stream (iostream) in PostScript-Format (EPS Version 3.0)
    @version $Id: GdevPS.h,v 1.13 2007/06/06 09:30:43 amg Exp $
*/
class GdevPS: public Gdev {
public:
  /** Konstruktor
      @param filename File name (Std-Ausgabe bei leerem String)
      @param title    Titel des Dokumentes
      @param x0,x1,y0,y1 Bildkoordinaten (in cm)
      @param creator  Programmname
      @param user     Benutzername
      @param orient   Hoch- (Portrait) oder Querformat (Landscape)
  */
  GdevPS( std::ostream &ostr, const std::string& title
	  , double x0, double y0, double x1, double y1
	  , const std::string &creator
	  , const std::string &user
	  , int pages
	  , Gdev::Orientation orient=ORIENT_PORTRAIT );
  /** Destruktor
   */
  virtual ~GdevPS();
  /** Funktion zum setzen der Linienbreite
      @param lwidth Linienbreite */
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
  virtual void setLinestyle( int lstyle );
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
      1 & fussbündig \\
      2 & zentriert \\
      3 & kopfbündig \\
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
  /** setzt die Eigenschaften fuer Polygone.
      @param edge,style,col,pattern Attribute
  */
  void setPolygonAttr( int edge, int style, int col, int pattern );
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
  virtual void drawTo( double x, double y );
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
  /// schreibt EPS-Header Informationen
  void header( const std::string& title
	       , double x0, double y0, double x1, double y1
	       , const std::string &creator, const std::string &user);

  virtual double getY( double y );
  void close();
  void showpage();
  virtual double pixelsize() const;

 private:
  void putString( char *cp );
  void stroke();
  void openSegment();
  void closeSegment();
  void colorTable();

 private:
  std::ostream &m_ostr;

  double PicWidth, PicHeight, XOFF, YOFF;
  bool m_path;
  bool SegmentOpened;
  int m_lineWidth;
  double m_size;
  HJust m_hjust;
  VJust m_vjust;
  int m_fontno;
  int m_color;
  Gdev::Orientation m_orient;
  int m_numPages;
  int m_currentPage;
  int m_numCharSets;
};
#endif
