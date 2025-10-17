
#if !defined(JOB_STACK_DATA_COMPLEX_INCLUDED_H)
#define JOB_STACK_DATA_COMPLEX_INCLUDED_H

#include "job/JobStackData.h"

class JobStackDataComplex : public JobStackData
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackDataComplex( dComplex value )
    : JobStackData( true )
    , m_value( value ){
  }
  virtual ~JobStackDataComplex(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Typ des Werts im Objekt.
      @return DataType (siehe DataDictionary.h)
  */
  DataDictionary::DataType getDataType() { return DataDictionary::type_Complex; }

  /** Diese Funktion erstellt eine Kopie des Objekts.
      @return Pointer auf neues Objekt.
  */
  virtual JobStackDataComplex *clone();

  /** Diese Funktion liefert Real-Teil des Werts als double. Falls Der Wert
       ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
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

  /** Diese Funktion wird verwendet um zu klären, ob das Resultat einer Expression
      True oder False, resp. ungleich Null oder Null ist.
      @return True: => Der Wert des Objekts ist gültig und nicht 0.
   */
  virtual bool isTrue();

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

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  dComplex     m_value;

};

#endif
