
#if !defined(JOB_STACK_DATA_INPUT_INCLUDED_H)
#define JOB_STACK_DATA_INPUT_INCLUDED_H

#include "job/JobStackData.h"
#include "xfer/XferDataItem.h"

class JobStackDataInput : public JobStackData
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackDataInput( XferDataItem& dataitem )
    : m_dataitem( dataitem ){
  }
  virtual ~JobStackDataInput(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Typ des Werts im Objekt.
      @return DataType (siehe DataDictionary.h)
  */
  virtual DataDictionary::DataType getDataType();

  /** Diese Funktion erstellt eine Kopie des Objekts.
      @return Pointer auf neues Objekt.
  */
  virtual JobStackDataInput *clone();

  /** Die Funktion meldet, ob der Wert des Objekts gültig ist.
      @return True: => der Wert ist gültig
  */
  virtual bool isValid();

  /** Die Funktion meldet, ob der Wert des Objekts ungültig ist.
      @return True: => der Wert ist ungültig
   */
  virtual bool isInvalid();

  /** Mit dieser Funktion wird der Wert des Objekts gültig oder ungültig gesetzt.
      @param valid Neuer Zustand des Werts.
   */
  //  void setValid( bool valid );

  /** Diese Funktion liefert den Wert als double. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getRealValue( double &d );

  /** Diese Funktion liefert Imaginär-Teil des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getImagValue( double &d );

  /** Diese Funktion liefert den Betrag des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getAbsValue( double &d );

  /** Diese Funktion liefert den Sinus des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getSinValue( double &d );

  /** Diese Funktion liefert den Tangens des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getTanValue( double &d );

  /** Diese Funktion liefert den Cosinus des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getCosValue( double &d );

  /** Diese Funktion liefert den ArcusSinus des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getASinValue( double &d );

  /** Diese Funktion liefert den ArcusTangens des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getATanValue( double &d );

  /** Diese Funktion liefert den ArcusCosinus des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getACosValue( double &d );

  /** Diese Funktion liefert den Log des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getLogValue( double &d );

  /** Diese Funktion liefert den Log10 des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getLog10Value( double &d );

  /** Diese Funktion liefert die Wurzel des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getSqrtValue( double &d );

  /** Diese Funktion liefert den Winkel des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getArgValue( double &d );

  /** Diese Funktion liefert den Wert als dComplex. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param c Wert als dComplex (complex<double> siehe DataPool.h).
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getComplexValue( dComplex &c );

  /** Diese Funktion liefert den Wert als int. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param i Wert als int.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getIntegerValue( int &i );

  /** Diese Funktion liefert den Wert als string. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param s Wert als string.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getStringValue( std::string &s );

  /** Diese Funktion liefert einen Pointer auf die entsprechende DataReference.
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param s Wert als string.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getStructureValue( DataReference * &rout );

  virtual bool getStructureValue(  DataReference * &rout, DataReference *rin );

  /** Diese Funktion wird verwendet um zu klären, ob das Resultat einer Expression
      True oder False, resp. ungleich Null oder Null ist.
      @return True: => Der Wert des Objekts ist gültig und nicht 0.
   */
  virtual bool isTrue();
  virtual OpStatus getNodename( JobEngine *eng );

  /** Diese Funktion schreibt den Wert in den Outputstream.
      @param ostr Stream für den Output.
  */
  virtual void print( std::ostream &ostr );

  /// Rundungsroutinen
  //@{
  virtual OpStatus round( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus round5( JobEngine *eng );
  virtual OpStatus round10( JobEngine *eng );
  //@}

  virtual OpStatus add( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus subtract( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus multiply( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus divide( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus power( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus negate( JobEngine *eng );
  virtual OpStatus greater( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus greaterEqual( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus less( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus lessEqual( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus equal( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus notEqual( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus assign( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus assignCorresponding( JobEngine *eng, JobStackDataPtr &dat );

  virtual OpStatus setColor( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus setColorBit( JobEngine *eng, JobStackDataPtr &dat, bool unset );

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  JobStackData *getJobStackData();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  XferDataItem m_dataitem;

};

#endif
