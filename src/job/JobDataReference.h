
#if !defined(JOB_DATAREFERENCE_INCLUDED_H)
#define JOB_DATAREFERENCE_INCLUDED_H

#include "job/JobElement.h"
#include "app/DataSet.h"
#include "app/UserAttr.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataReference.h"

class JobEngine;
class JobStackData;
class JobStackDataPtr;

/** \file
    Die Datei enhält die Beschreibung der Klasse JobDataReference.
*/
/** Die JobDataReference enthält eine DataReference. Sie enthält zusätzlich eine
    Liste aller Indizes, welche zur Laufzeit eines Jobs neu gesetzt werden. alle
    Zugriffe auf den Datapool aus Jobs werden über Objekte dieser Klasse getätigt.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
*/
class JobDataReference : public JobElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// Konstruktor
  JobDataReference();
  /// Destruktor
  virtual ~JobDataReference();

protected:
  JobDataReference( JobDataReference & );

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  /** Diese Funktion liefert den Typ der Datenvariablen.
      @return Typ
  */
  DataDictionary::DataType getDataType();

  /** Die Funktion liefert eine Referenz auf sich selber (cast)
      \return Referenz auf this
   */
  JobDataReference &referencexxxxx() { return *this; }

  virtual bool isVariable() { return false; }
  void allowWildcards() { m_allow_wildcards = true; }
  bool WildcardsAreAllowed() { return m_allow_wildcards; }
  int numWildcards() { return m_wildcards; }

  /** Diese Funktion liefert den Wert der Variablen in Form eines JobStackData-Objekts.
      Der Aufrufer muss das Objekt wieder löschen.
      @return Pointer auf JobStackData-Objekt mit dem gewünschten Wert
  */
  JobStackData *getJobStackData();

  DataReference &Data() { return *m_data; }
  DataReference *getDataReference() { return m_data; }

  /** Diese Funktion weist den Wert im JobStackData-Objekt dat der Datenvariablen im
      lokalen JobDataReference-Objekt zu.
      @param dat Pointer auf JobStackData-Objekt
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  OpStatus assign( JobStackDataPtr &dat );
  OpStatus assignCorresponding( JobStackDataPtr &dat );
  OpStatus getTimestamp(  JobEngine *eng );
  OpStatus getDbModified( JobEngine *eng );
  OpStatus getClassname( JobEngine *eng );
  OpStatus getNodename( JobEngine *eng );

  OpStatus increment();
  OpStatus decrement();

  OpStatus setEditable( JobEngine *eng, bool yes );
  OpStatus setLocked( JobEngine *eng, bool yes );
  OpStatus setColor(  JobEngine *eng, int color );
  OpStatus setColorBit(  JobEngine *eng, int color, bool unset );
  OpStatus setStylesheet(JobEngine *eng, const std::string& stylesheet);

  /** \todo Die Beschreibung der Funktion setDataTimestamp fehlt.
  */
  OpStatus setDataTimestamp(  JobEngine *eng, bool yes );

  /** \todo Die Beschreibung der Funktion setValueTimestamp fehlt.
  */
  OpStatus setValueTimestamp(  JobEngine *eng, bool yes );

  /** \todo Die Beschreibung der Funktion setDbTimestamp fehlt.
  */
  OpStatus setDbTimestamp( JobEngine *eng );

  void setValue( int i, int inx );
  void setRealValue( double d, int inx );
  void setStringValue( const std::string& s, int inx );

  /** Die Funktion löscht den gewünschten Datenbereich einer Variablen.
   */
  bool clearValues();

  /** Die Funktion löscht den gewünschten Datenbereich einer Variablen.
   */
  bool eraseValues();

  /** Diese Funktion löscht den gesamten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  bool clearAllValues();

  /** Diese Funktion packt den gewünschten Datenbereich einer Variablen.
      @return True: => Die Werte konnten gelöscht werden.
   */
  bool packValues( bool );

  /** Die Funktion meldet, ob der Wert der Datenvariablen gültig ist.
      @return True: => der Wert ist gültig
  */
  bool isValid();

  /** Die Funktion meldet, ob der Wert der Datenvariablen ungültig ist.
      @return True: => der Wert ist ungültig
   */
  bool isInvalid();

  /** Mit dieser Funktion wird eine DataReference innerhalb des Objekts angelegt.
      Gesucht wird ausschliesslich im aktuellen Namespace (lokale Variable).
      @return True: => erfolgreich
  */
  bool newDataReferenceInNamespace( const std::string &name, bool dataset );

  /** Mit dieser Funktion wird eine DataReference innerhalb des Objekts angelegt.
      Sie wird vom Parser aufgerufen.
      @return True: => erfolgreich
  */
  bool newDataReference( const std::string &name, bool dataset );

  /** Mit dieser Funktion wird eine DataReference innerhalb des Objekts angelegt.
      @param dref DataReference
      @return True: => erfolgreich
  */
  bool newDataReference( DataReference *dref );

  /** Mit dieser Funktion wird die DataReference erweitert. Dies ist nur möglich,
      wenn die bereits vorhandene DataReference eine Struktur ist.
      @return True: => erfolgreich
  */
  bool extendDataReference( const std::string &name );

  /** Mit Dieser Funktion wird mitgeteilt, dass auf dem angegebenen Level mit
      einem variablen Index auf die Daten zugegriffen wird. Mehrere Indizes auf
      demselben Level sind natürlich möglich. Vor dem Zugriff auf die Daten werden
      die nötigen Indizes vom Stack abgerufen.
      @param level Zugriffsebene (Level) des Index
  */
  void addIndex( int level );
  void addWildcard( int level );

  /** Mit Dieser Funktion wird mitgeteilt, dass an dieser Stelle ein variabler Teil
      des Itemnames auf dem Stack liegt. Die DataReference wird erst zur Laufzeit
      vollständig erstellt.
  */
  void addVariableName();

  /** Mit dieser Funktion werden zur Laufzeit eines Jobs die Indizes neu gesetzt.
      Die Funktion holt sich vom Stack der JobEngine die nötigen Daten.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus updateDataReference( JobEngine *eng );

  virtual void setDataRoot( DataReference *ref ) {}

  void setCycleNumber( int cyclenum );
  void initCycleNumber();
  void setReferenceInvalid() { m_valid = false; }
  bool isReferenceValid() { return m_valid; }

  OpStatus size( JobEngine *eng, JobStackDataPtr &data );
  OpStatus index( JobEngine *eng, JobStackDataPtr &data );
  OpStatus compare( JobEngine *eng, JobDataReference *data );
  OpStatus compare( JobEngine *eng, JobDataReference *dataLeft, JobDataReference *dataRight );
  OpStatus diff( JobEngine *eng, JobDataReference *data, int mode );

  void printFullName( std::ostream &ostr, bool withIndizes );
  bool getUnit( std::string & units );
  bool getLabel( std::string & label );

  /** Sollte die DataReference ein String sein, kann der
      String(ISO-Date) als lokales Datum interpretiert werden.
  */
  void setStringType(UserAttr::STRINGtype type);

  static int getRefCount();

/*=============================================================================*/
/* private functions                                                           */
/*=============================================================================*/
private:
  DATAAttributeMask setColorBit( int color );
  DataSet *m_dataset;
  bool m_isDataset;

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  typedef std::vector< int >  IndexList;

  IndexList       m_index_list;
  bool            m_allow_wildcards;
  int             m_wildcards;
  DataReference  *m_data;
  bool            m_valid; /* bereit für Datenzugriff */
  UserAttr::STRINGtype  m_stringType;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

#if defined(_DEBUG)
  static int   s_ref_count;
#endif
};

#endif
