
#if !defined(JOB_ENGINE_INCLUDED_H)
#define JOB_ENGINE_INCLUDED_H

#include "job/JobElement.h"
#include "job/JobCodeItem.h"
#include "job/JobFunction.h"
#include "job/JobStack.h"
#include "datapool/TransactionOwner.h"

class JobStackItem;
class JobStackItemPtr;
class JobStackDataPtr;
class JobStackReturn;

/** Mit einem Objekt der Klasse JobEngine wird eine Funktion ausgeführt.
    Sie verwaltet den Stack.
    Sie kann nur einmal verwendet werden. Ist eine Funktion beendet, so
    soll das Objekt gelöscht werden. Zum Ausführen der Funktion wird die
    Procedure executeCode() verwendet.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: JobEngine.h,v 1.18 2005/04/05 12:37:41 ked Exp $
*/
class JobEngine : public JobElement
                , public TransactionOwner
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobEngine( JobFunction * );
  virtual ~JobEngine();

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Die Funktion liefert die eindeutige Identifikationsnummer des Objekts.
      @return Identifikationsnummer
  */
  int get_id() const;

  /** Dies ist die Hauptfunktion der Job Engine. Sie wird immer wieder
      aufgerufen und liefert den jeweiligen Status des Jobs.
  */
  OpStatus execute();

  /** Bei einem vorzeitigen Abbruch der Verarbeitung erhält die JobEngine die
      Gelegenheit, allfällige Hängepartien (Threads) sauber abzubrechen.
  */
  void cancel();

  /** Diese Funktion liefert den MessageStream für die Message für die
      Messagebar.
  */
  std::ostream &MessageStream();

  /** Mit dieser Funktion wird die Message gesetzt, welche am Ende des Jobs
      auf der Messagebar der aufrufenden Form erscheinen soll. Der Text
      muss vorher über den MessageStream() eingelesen worden sein.
  */
  void setMessage();

  /** Mit dieser Funktion wird die Message abgerufen.
   */
  inline std::string &Message() { return m_message; }

  /** Mit dieser Funktion wird das Errorflag gesetzt
   */
  void setError( bool error = true ) { m_error = error; }

  /** Diese Funktion liefert den Wert des Errorflags. Sie zeigt somit an, ob
      während der Ausführung Fehler aufgetaucht sind.
  */
  bool getError() { return m_error; }

  /** Diese Funktion liefert einen Pointer auf die aufgerufene Funktion.
   */
  JobFunction *getFunction() { return m_function; }

  /** Diese Funktion stellt am Ende der Ausführung eines JobCodes den Zustand
      vor dem Aufruf des Codes wieder her. Der alte Code wird installiert mit
      der Addresse des nächsten auszuführenden Operators.
      @param ret Referenz auf ein JobStackReturn-Objekt mit dem altem Code
                 und der korrekten Addresse.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  OpStatus restoreCode( JobStackReturn &ret );

  /** Diese Funktion stellt ein StackItem-Objekt auf den Stack.
      @param item Pointer auf das für den Stack bestimmten Stackitems
  */
  void push( const JobStackItemPtr &item );

  /** Diese Funktion stellt ein StackData-Objekt auf den Stack.
      @param item Pointer auf das für den Stack bestimmten Stackitems
  */
  void pushData( const JobStackDataPtr &item );

  /** Diese Funktion stellt ein Stack-Objekt mit dem Wert 1 für True oder
      0 für False auf den Stack.
      @param yes True => push True.
  */
  void pushTrue( bool yes );

  /** Diese Funktion stellt ein Address-Objekt mit dem entsprechenen Wert
      auf den Stack.
      /param addr Wert der Addresse
  */
  void pushAddress( int addr );

  /** Diese Funktion ruft pushTrue(). Als Parameter erhält sie die Aussage,
      ob der CallReason zutrifft.
      @param reason CallReason der Function oder Task.
  */
  void pushReason( CallReason reason );

  /** Diese Funktion stellt ein Stack-Objekt mit einem ungültigen Wert auf den Stack.
  */
  void pushInvalid();

  /** Diese Funktion stellt ein StackDataInteger-Objekt mit dem gewünschten Index
      auf den Stack. Wenn die kontrollierende JobFunction die notwendigen
      Informationen nicht besitzt, ist der Wert im Objekt ungültig.
      @param level Daten-Level der Inputvariable
      @param index Indexnummer innerhalb des Daten-Levels.
  */
  void pushIndex( int level, int index );

  /** Diese Funktion stellt ein StackDataInteger-Objekt mit der gewünschten Index-
      Nummer auf den Stack. Wenn die kontrollierende JobFunction die notwendigen
      Informationen nicht besitzt, ist der Wert im Objekt ungültig.
      @param level Daten-Level der Inputvariable
      @param index Indexnummer innerhalb des Daten-Levels.
  */
  void pushIndexNumber();

  /** Diese Funktion stellt ein StackDataInteger-Objekt mit der gewünschten Diagram
      x-Position auf den Stack. Wenn die kontrollierende JobFunction die notwendigen
      Informationen nicht besitzt, ist der Wert im Objekt ungültig.
      @param level Daten-Level der Inputvariable
      @param index Indexnummer innerhalb des Daten-Levels.
  */
  void pushDiagramXPos();

  /** Diese Funktion stellt ein StackDataInteger-Objekt mit der gewünschten Diagram
      y-Position auf den Stack. Wenn die kontrollierende JobFunction die notwendigen
      Informationen nicht besitzt, ist der Wert im Objekt ungültig.
      @param level Daten-Level der Inputvariable
      @param index Indexnummer innerhalb des Daten-Levels.
  */
  void pushDiagramYPos();

  /** Diese Funktion stellt ein StackDataInteger-Objekt mit der gewünschten Sort Criteria
      auf den Stack. Wenn die kontrollierende JobFunction die notwendigen
      Informationen nicht besitzt, ist der Wert im Objekt ungültig.
  */
  void pushSortCriteria();

  /** Diese Funktion holt den Pointer eines Stack-Items vom Stack. Der Aufrufer
      ist verantwortlich für die weitere Zukunft dieses Objekts.
      \return Pointer auf ein JobStackItem-Objekt
  */
  const JobStackItemPtr pop();

    /** Diese Funktion liefert einen Pointer auf das oberste Item auf dem Stack
      ohne es von dort zu eliminieren.
      \attention Das Objekt darf nicht gelöscht werden
      \return Pointer auf ein JobStackItem-Objekt
  */
  const JobStackItemPtr get();

  /** Die Funktion liefert die aktuelle Grösse des Stacks.
      \return Anzahl StackItems
  */
  int sizeOfStack();

  /** Mit dieser Funktion wird eine JobFunction in der JobEngine installiert.
      Der Rücksprung nach deren Ende wird sichergestellt (siehe restoreCode).
  */
  void callFunction( JobFunction *func );

  /** Mit dieser Funktion wird ein Branch durchgeführt (z.B durch IF-
      WHILE- und Branch-Operatoren).
      @param addr Neue Code-Adresse.
  */
  void setCurrentAddr( int addr ) { m_current_addr = addr; }

  /** Diese Funktion liefert eine Referenz auf das ostream-Objekt für normale
      Meldungen (PRINT).
      @return Output Stream für Meldung
  */
  std::ostream &PrintStream() { return m_function->PrintStream(); }

  /** Eine hängige Meldung wird ausgegeben.
   */
  void flushPrintStream() { m_function->flushPrintStream(); }

  /** Diese Funktion liefert eine Referenz auf das ostream-Objekt für
      Fehlermeldungen (SET_ERROR).
      @return Output Stream für Fehlermeldung
  */
  std::ostream &ErrorStream() { return m_function->ErrorStream(); }

  /** Eine hängige Fehlermeldung wird ausgegeben.
   */
  void flushErrorStream() { m_function->flushErrorStream(); }

  /** Diese Funktion liefert eine Referenz auf das ostringstream-Objekt für
      Logmeldungen.
      @return Output Stream für Fehlermeldung
  */
  std::ostringstream &LogStream() { return m_function->LogStream(); }
  /** Stream für Logmeldung löschen. */
  void clearLogStream() { m_function->clearLogStream(); }

  /** Diese Funktion liefert einen Pointer auf ein neues JobStackData-Objekt
      mit dem alten Wert eines GuiElements. Wenn die kontrollierende JobFunction
      die notwendigen Informationen nicht besitzt, ist der Wert im Objekt ungültig.
      @return Pointer auf ein neues JobStackData-Objekt.
  */
  JobStackData *getOldData() { return m_function->getOldData(); }

  /** Diese Funktion liefert einen Pointer auf ein neues JobStackData-Objekt
      mit dem neuen Wert eines GuiElements. Wenn die kontrollierende JobFunction
      die notwendigen Informationen nicht besitzt, ist der Wert im Objekt ungültig.
      @return Pointer auf ein neues JobStackData-Objekt.
  */
  JobStackData *getNewData() { return m_function->getNewData(); }

  /** Diese Funktion liefert einen Pointer auf ein neues DataReference-Objekt
      mit dem neuen Wert eines GuiElements. Wenn die kontrollierende JobFunction
      die notwendigen Informationen nicht besitzt, ist das Objekt ungültig.
      @return Pointer auf ein neues DataReference-Objekt.
  */
  DataReference *getNewDataReference() {return m_function->getNewDataReference(); }
  DataReference *getBaseDataReference() {return m_function->getBaseDataReference(); }
  DataReference *getSourceDataReference() {return m_function->getSourceDataReference(); }
  std::vector<DataReference*> getSourceListDataReference() {return m_function->getSourceListDataReference(); }
  DataReference *getSource2DataReference() {return m_function->getSource2DataReference(); }

  /** Diese Funktion liefert einen Pointer auf ein neues DataReference-Objekt auf die
      Structvariable des Items eines GuiElements. Wenn die kontrollierende JobFunction
      die notwendigen Informationen nicht besitzt, ist das Objekt ungültig.
      @return Pointer auf ein neues DataReference-Objekt.
  */
  DataReference *getNewThisReference() {return m_function->getNewThisReference(); }

  /** Die Funktion liefert eine eindeutige Identifaktion der Engine
      als TransactionOwner.
  */
  virtual std::string ownerId() const;

  /** Die Funktion schreibt den Stack in den Outputstream.
   */
  void printStack( std::ostream &ostr ){ m_stack.print( ostr ); }

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  void pushReturnControl( const std::string &name );
  int  popAddress();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  static int         s_id_counter;

  // ----------------------------------------------------------------------------
  // Jede Engine hat eine eindeutige Identifikationsnummer. Diese kann von
  // OpCode-Objekten genutzt werden.
  // ----------------------------------------------------------------------------
  int                m_id;

  // ----------------------------------------------------------------------------
  // Die Engine hat immer einen Pointer der aufrufenden Funktion. Dies ist nötig
  // um diverse Aufgaben weiter zu reichen.
  // ----------------------------------------------------------------------------
  JobFunction       *m_function;

  // ----------------------------------------------------------------------------
  // Die Engine hat immer nur den Pointer auf den im Moment auszuführenden Code.
  // Werden aus einer Funktion andere Funktionen aufgerufen, so wird der auf-
  // rufende Code zeitweilig auf dem Stack gespeichert.
  // ----------------------------------------------------------------------------
  JobCodeStream     *m_code;

  JobStack           m_stack;          // Stack des Jobs
  int                m_current_addr;   // Aktuelle Position im Code
  bool               m_error;          // Fehlerflag des laufenden Jobs

  std::ostringstream *m_messagestream;  // stream für das Einlesen der Message
  std::string         m_message;        // ev. Ausgabemeldung auf einer Messagebar
};

#endif
