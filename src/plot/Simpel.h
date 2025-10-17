
#ifndef SIMPEL_UNIPLOT_H
#define SIMPEL_UNIPLOT_H

/** Simpel
    Die Klasse Simpel sorgt für die Ausgabe von BBC-Uniplot Grafikdaten
    auf ein Grafikdevice
*/

#include "plot/ListPlot.h"

/* class Simpel : public MotifListPlot */
/*              , public BasicStream */
class Simpel// : public ListPlot, public BasicStream
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param name Name des Simpelplots
      @param dref Datenreferenz
  */
  Simpel( GuiElement *parent, const std::string &name ){}//: ListPlot( name ), BasicStream( name ) {}
  Simpel(){}//: ListPlot( "" ), BasicStream( "" ) {}

  virtual ~Simpel() {}

 private:
/*   Simpel( const Simpel &s ); */

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

  virtual ListPlot *getListPlot() = 0;
  virtual BasicStream *getBasicStream() = 0;

    virtual void setTitle( const std::string &title ) = 0;

};
#endif
