
#if !defined(JOB_STACK_DATA_VARIABLE_INCLUDED_H)
#define JOB_STACK_DATA_VARIABLE_INCLUDED_H

#include "job/JobStackData.h"

class JobStackDataReal;
class JobDataReference;
class JobStackDataPtr;

class JobStackDataVariable : public JobStackData
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackDataVariable( JobDataReference *data )
    : m_data( data ){
  }
  virtual ~JobStackDataVariable(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Typ des Werts im Objekt.
      @return DataType (siehe DataDictionary.h)
  */
  virtual DataDictionary::DataType getDataType();

  /** Die Funktion liefert eine Referenz auf das JobDataReference-Objekt.
      Falls kein Objekt verfügbar ist, endet das Programm mit einem assert().
      \return Referenz auf JobDataReference-Objekt.
  */
  virtual JobDataReference &reference();

  /** Die Funktion liefert die Units des Objektes.
      @return std::string
  */
  virtual bool getUnits( std::string & units );

  /** Die Funktion liefert das Label des Objektes.
      @return std::string
  */
  virtual bool getLabel( std::string & label );

  /** Diese Funktion erstellt eine Kopie des Objekts.
      @return Pointer auf neues Objekt.
  */
  virtual JobStackDataVariable *clone();

  /** Die Funktion meldet, ob der Wert des Objekts gültig ist.
      @return True: => der Wert ist gültig
  */
  virtual bool isValid();

  /** Die Funktion meldet, ob der Wert des Objekts ungültig ist.
      @return True: => der Wert ist ungültig
   */
  virtual bool isInvalid();

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

  /** Diese Funktion liefert den Pointer der DataReference des Objects, falls
      sie auf einen Wert derselben StructDefinition zeigt wie d.
      @param dat Resultat als Pointer auf DataReference
      @param d DataReference zum Vergleichen
      @return True: => Die gewünschte DataReference ist gültig.
  */
  virtual bool getStructureValue( DataReference * &dat, DataReference *d );

  virtual bool getStructureValue( DataReference * &dat );

  /** Die Funktion liefert einen Pointer auf das DataReference-Objekt.
      @return Pointer auf DataReference
  */
  virtual DataReference *getDataReference();

  virtual bool setValue( int i, int inx );
  virtual bool setRealValue( double d, int inx );
  virtual bool setStringValue( const std::string& s, int inx );

  /** Diese Funktion löscht den gewünschten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  virtual bool clearValues();

  /** Diese Funktion löscht den gesamten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  virtual bool clearAllValues();

  /** Diese Funktion löscht den gewünschten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  virtual bool eraseValues();

  /** Diese Funktion packt den gewünschten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  virtual bool packValues( bool packRow );

  /** Diese Funktion wird verwendet um zu klären, ob das Resultat einer Expression
      True oder False, resp. ungleich Null oder Null ist.
      @return True: => Der Wert des Objekts ist gültig und nicht 0.
   */
  virtual bool isTrue();

  /** Diese Funktion schreibt den Wert in den Outputstream.
      @param ostr Stream für den Output.
  */
  virtual void print( std::ostream &ostr );

  /** Diese Funktion setzt die Cyclenummer für den Zugriff auf den Datapool.
   */
  virtual void setCycleNumber( JobStackDataPtr &dat );

  /// Funktionen
  //@{
  virtual OpStatus round( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus round5( JobEngine *eng );
  virtual OpStatus round10( JobEngine *eng );
  virtual OpStatus size( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus index( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus compare( JobEngine *eng, JobStackDataPtr &data );
  virtual OpStatus compare( JobEngine *eng, JobStackDataPtr &dataLeft, JobStackDataPtr &dataRight );
  //@}
  virtual OpStatus add( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus subtract( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus multiply( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus divide( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus modulo( JobEngine *eng, JobStackDataPtr &dat );
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
  virtual OpStatus accumulate( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus increment( JobEngine *eng );
  virtual OpStatus decrement( JobEngine *eng );
  virtual OpStatus setEditable( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus setLocked( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus setColor( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus setColorBit( JobEngine *eng, JobStackDataPtr &dat, bool unset );
  virtual OpStatus setStylesheet(JobEngine *eng, const std::string& stylesheet);
  virtual OpStatus setDataTimestamp( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus setValueTimestamp( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus getTimestamp( JobEngine *eng );
  virtual OpStatus getDbModified( JobEngine *eng );
  virtual OpStatus setDbTimestamp( JobEngine *eng );
  /** Die Funktion liefert den Classname desObjekts. */
  virtual OpStatus getClassname( JobEngine *eng );
  /** Die Funktion liefert den Nodename desObjekts. */
  virtual OpStatus getNodename( JobEngine *eng );

  virtual void printFullName( std::ostream &ostr, bool withIndizes );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  JobDataReference   *m_data;

};

#endif
