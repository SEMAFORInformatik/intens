
#if !defined(JOB_STACK_DATA_INCLUDED_H)
#define JOB_STACK_DATA_INCLUDED_H

#include "job/JobStackItem.h"
#include "datapool/DataPool.h"

class JobEngine;
class JobStackDataReal;
class JobStackDataPtr;
class JobDataReference;
class JobStackAddress;
class DataReference;

/** JobStackData ist die Basisklasse aller Datenobjekte welche auf dem Stack eines
    laufenden Jobs (JobEngine) abgelegt werden müssen.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: JobStackData.h,v 1.21 2005/05/24 09:08:28 ked Exp $
*/
class JobStackData : public JobStackItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackData( bool valid = false );
  virtual ~JobStackData();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Typ des StackItems.
      \return Typ
  */
  virtual StackType Type() { return stck_Data; }

  /** Die Funktion liefert den Typ des Werts im Objekt.
      @return DataType (siehe DataDictionary.h)
  */
  virtual DataDictionary::DataType getDataType() = 0;

  /** Die Funktion liefert eine Referenz auf das JobDataReference-Objekt.
      Falls kein Objekt verfügbar ist, endet das Programm mit einem assert().
      \return Referenz auf JobDataReference-Objekt.
  */
  virtual JobDataReference &reference();

  /** Die Funktion liefert die Units im Objekt.
  */
  virtual bool getUnits( std::string & units ){ return false; };
  /** Die Funktion liefert das Label im Objekt.
  */
  virtual bool getLabel( std::string & label ){ return false; };
  /** Diese Funktion erstellt eine Kopie des Objekts.
      @return Pointer auf neues Objekt.
  */
  virtual JobStackData *clone() = 0;

  /** Die Funktion meldet, ob der Wert des Objekts gültig ist.
      @return True: => der Wert ist gültig
  */
  virtual bool isValid() { return m_valid; }

  /** Die Funktion meldet, ob der Wert des Objekts ungültig ist.
      @return True: => der Wert ist ungültig
   */
  virtual bool isInvalid() { return !m_valid; }

  /** Mit dieser Funktion wird der Wert des Objekts ungültig gemacht. In gewissen
      Situationen ist dies zweckmässig.
   */
  virtual void setInvalid() { m_valid = false; }

  /* Die Funktion meldet, ob es sich um ein EndOfLine handelt.
   */
  virtual bool isEndOfLine() { return false; }

  /** Diese Funktion liefert Real-Teil des Werts als double. Falls Der Wert
       ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getRealValue( double &d ) { return false; }

  /** Diese Funktion liefert Imaginär-Teil des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getImagValue( double &d ) { return false; }

  /** Diese Funktion liefert den Betrag des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getAbsValue( double &d ) { return false; }

  /** Diese Funktion liefert den Sinus des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getSinValue( double &d ) { return false; }

  /** Diese Funktion liefert den Tangens des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getTanValue( double &d ) { return false; }

  /** Diese Funktion liefert den Cosinus des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getCosValue( double &d ) { return false; }

  /** Diese Funktion liefert den ArcusSinus des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getASinValue( double &d ) { return false; }

  /** Diese Funktion liefert den ArcusTangens des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getATanValue( double &d ) { return false; }

  /** Diese Funktion liefert den ArcusCosinus des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getACosValue( double &d ) { return false; }

  /** Diese Funktion liefert den Log des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getLogValue( double &d ) { return false; }

  /** Diese Funktion liefert den Log10 des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getLog10Value( double &d ) { return false; }

  /** Diese Funktion liefert die Wurzel des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getSqrtValue( double &d ) { return false; }

  /** Diese Funktion liefert den Winkel des Werts als double. Falls Der Wert
      ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */

  virtual bool getArgValue( double &d ) { return false; }

  /** Diese Funktion liefert den Wert als dComplex. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param c Wert als dComplex (complex<double> siehe DataPool.h).
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getComplexValue( dComplex &c ) { return false; }

  /** Diese Funktion liefert den Wert als int. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param i Wert als int.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getIntegerValue( int &i ) { return false; }

  /** Diese Funktion liefert den Wert als string. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param s Wert als string.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getStringValue( std::string &s ) { return false; }

  virtual bool getStructureValue( DataReference * &dat ) { return false; }

  /** Diese Funktion liefert den Pointer der DataReference des Objects, falls
      sie auf einen Wert derselben StructDefinition zeigt wie d.
      @param dat Resultat als Pointer auf DataReference
      @param d DataReference zum Vergleichen
      @return True: => Die gewünschte DataReference ist gültig.
  */
  virtual bool getStructureValue( DataReference * &dat, DataReference *d ) { return false; }

  /** Die Funktion liefert einen Pointer auf das DataReference-Objekt.
      @return Pointer auf DataReference
  */
  virtual DataReference *getDataReference() { return 0; }

  ///
  virtual bool setValue( int i, int inx ) { return false; }
  ///
  virtual bool setRealValue( double d, int inx ) { return false; }
  ///
  virtual bool setStringValue( const std::string& s, int inx ) { return false; }

  /** Diese Funktion löscht den gewünschten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  virtual bool clearValues() { return false; }

  /** Diese Funktion löscht den gesamten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  virtual bool clearAllValues() { return false; }

  /** Diese Funktion löscht den gewünschten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  virtual bool eraseValues() { return false; }

  /** Diese Funktion packt den gewünschten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  virtual bool packValues( bool ) { return false; }

  /** Diese Funktion wird verwendet um zu klären, ob das Resultat einer Expression
      True oder False, resp. ungleich Null oder Null ist.
      @return True: => Der Wert des Objekts ist gültig und nicht Null.
   */
  virtual bool isTrue() { return false; }

  /** Diese Funktion setzt die Cyclenummer für den Zugriff auf den Datapool.
   */
  virtual void setCycleNumber( JobStackDataPtr &dat ) {}

  /// Funktionen
  //@{
  virtual OpStatus round( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus round5( JobEngine *eng );
  virtual OpStatus round10( JobEngine *eng );
  virtual OpStatus size( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus index( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus compare( JobEngine *eng, JobStackDataPtr &dat);
  virtual OpStatus compare( JobEngine *eng, JobStackDataPtr &datLeft, JobStackDataPtr &datRight);
  virtual OpStatus diff( JobEngine *eng, JobStackDataPtr &dat, int mode );
  virtual OpStatus checkFormat( JobEngine *eng, JobStackDataPtr &dat );
  //@}

  /// Operatoren
  //@{
  virtual OpStatus add( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus subtract( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus multiply( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus divide( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus modulo( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus power( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus negate( JobEngine *eng ) = 0;
  virtual OpStatus greater( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus greaterEqual( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus less( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus lessEqual( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus equal( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus notEqual( JobEngine *eng, JobStackDataPtr &dat ) = 0;
  virtual OpStatus assign( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus assignCorresponding( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus accumulate( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus increment( JobEngine *eng ) { return op_FatalError; }
  virtual OpStatus decrement( JobEngine *eng ) { return op_FatalError; }
  virtual OpStatus setEditable( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus setLocked( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus setColor( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus setColorBit( JobEngine *eng, JobStackDataPtr &dat, bool unset ) { return op_FatalError; }
  virtual OpStatus setStylesheet(JobEngine *eng, const std::string& stylesheet){return op_FatalError;}
  virtual OpStatus setDataTimestamp( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus setValueTimestamp( JobEngine *eng, JobStackDataPtr &dat ) { return op_FatalError; }
  virtual OpStatus getTimestamp( JobEngine *eng );
  virtual OpStatus getDbModified( JobEngine *eng );
  virtual OpStatus setDbTimestamp( JobEngine *eng );
  /** Die Funktion liefert den Classname desObjekts. */
  virtual OpStatus getClassname( JobEngine *eng );
  //@}
  /** Die Funktion liefert den Nodename desObjekts. */
  virtual OpStatus getNodename( JobEngine *eng );
  //@}

  virtual void printFullName( std::ostream &ostr, bool withIndizes );

  std::string fullName( bool withIndizes );

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
protected:
  double runden( double d, int i );
  double runden5( double d );
  double aufrunden( double d );
  double abrunden( double d );
  bool isInteger( double d );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  bool       m_valid;
};


class JobStackDataPtr
{
public:
  JobStackDataPtr();
  JobStackDataPtr( JobStackData *pt );
  JobStackDataPtr( const JobStackItemPtr & );
  JobStackDataPtr( const JobStackDataPtr & );
  virtual ~JobStackDataPtr();
public:
  bool is_valid() const;
  bool isnt_valid() const;

  virtual JobDataReference &reference();
  const JobStackItemPtr &itemPtr() const;
  JobStackData *get_ptr() const;
  JobStackData *operator->() const;
  virtual JobStackDataPtr& operator=( const JobStackDataPtr &p );
  virtual JobStackDataPtr& operator=( JobStackData * );
  virtual JobStackData& operator*();

private:
  JobStackItemPtr  m_item_ptr;
};

#endif
