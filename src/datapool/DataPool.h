
#if !defined(__DP_DATAPOOL__)
#define __DP_DATAPOOL__

#include "datapool/DataDictionaryRoot.h"
#include "datapool/DataTTrail.h"
#include "datapool/DataGlobals.h"

class DataReference;
class DataStructElement;
class DataItem;
class DataUserAttr;
class DataItemAttr;
class TransactionOwner;
class DataEvents;

/** DataPool

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: DataPool.h,v 1.22 2005/06/21 06:35:08 ked Exp
 */
class DataPool
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:

  /** Konstruktor
      \param pUserAttr
      \param pItemAttr
      \todo Die Beschreibung der Funktion DataPool fehlt
   */
  DataPool(DataUserAttr *pUserAttr = 0, DataItemAttr *pItemAttr = 0);

  /** Destruktor
   */
  virtual ~DataPool();

protected:

  /** Beschreibung der Funktion DataPool
      \param ref
      \todo Die Beschreibung der Funktion DataPool fehlt
   */
  DataPool(const DataPool &ref);

  /** Beschreibung der Funktion operator=
      \param ref
      \return
      \todo Die Beschreibung der Funktion operator= fehlt
   */
  DataPool& operator=(const DataPool &ref);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Pointer auf das Singleton-Objekt DataPool.
   */
  static DataPool *Instance( DataUserAttr *pUserAttr = 0, DataItemAttr *pItemAttr = 0 );

  /** Die Funktion liefert die Referenz auf das Singleton-Objekt DataPool.
      Der Datapool muss zu diesem Zeitpunkt bereits vorhanden sein.
      @return Referenz auf den DataPool
   */
  static DataPool &getDatapool();

  /** Die Funktion liefert den Pointer auf das Singleton-Objekt DataPool.
   */
  static void Destroy();

  /** Die Funktion liefert ein neues DataReference-Objekt. Die Constructors
      sind nicht public.
      @param addr Adresse der neu anzulegenden DataReference
      @return Pointer auf das neue DataReference-Objekt
  */
  DataReference *newDataReference( const char *addr = 0 );

  /** Die Funktion liefert ein neues DataReference-Objekt als Kopie des als
      Parameter übergebenen Objekts. Die Constructors sind nicht public.
      @param ref Reference auf das zu kopierende DataReference-Objekt
      @param addr Adresse der neu anzulegenden DataReference
      @return Pointer auf das neue DataReference-Objekt
  */
  static DataReference *newDataReference( const DataReference &ref, const char *addr = 0 );

  /** Die Funktion erhöht die fortlaufende Transaktionsnummer.
   */
  TransactionNumber NewTransaction();

  /** Die Funktion liefert die aktuelle Transaktionsnummer.
   */
  TransactionNumber GetCurrentTransaction() const;

  /** Die Funktion liefert die aktuelle Transaktionsnummer, sofern der
      DataPool vorhanden ist.
      \return Aktuelle Transaktionsnummer
   */
  static TransactionNumber getTransactionNumber();

  /** Die Funktion liefert die Transaktionsnummer des letzten Aufrufs von ClearCycle().
      @return Transaktionsnummer des letzten ClearCycle().
   */
  TransactionNumber getLastClearTransaction() const;

  /** Die Funktion teilt mit, ob die übergebene Transaktionsnummer dem letzten
      ClearCycle()-Aufruf entspricht.
      \return true => letzter ClearCycle-Zeitpunkt.
   */
  static bool isLastClearTransaction( TransactionNumber num );

  /** Diese Funktion setzt die aktuelle TransactionsnNummer neu. Sie wird
      nur vom TTrail benötigt bei einem Rollback.
  */
  void RestoreCurrentTransaction( TransactionNumber num );

  /** Die Funktion liefert einen Pointer auf den Dictionary-Eintrag, der die
      Root-Struktur-Definition beschreibt. Dies ist die Wurzel des Datapools.
  */
  DataDictionary *GetRootDict();

  /** Die Funktion liefert einen Pointer auf den Dictionary-Eintrag, der die
      Root-Strukturvariable beschreibt.
  */
  DataDictionary *GetRootDictVar();

  /** Die Funktion liefert den Pointer auf die Schablone (Vorlage-Objekt)
      des DataItemAttr.
   */
  DataItemAttr *GetDataItemAttr();

  /** Die Funktion fügt ein neues Item zum DataDictionary. Wenn die Funktion erfolgreich war,
      gibt sie den Pointer auf das neu erstellte Dictionary-Item zurueck, sonst eine
      0 (Keine weiteren Fehlerinformationen). Dies ist ein Wrapper. Die genaue Beschreibung
      der Parameter kann der Klasse DataDictionaryRoot entnommen werden.
  */
  DataDictionary * AddToDictionary( const char                     * pStructMember,
                                    const std::string              & Name,
                                    const DataDictionary::DataType   Type,
                                    const char                     * pStuctVar = 0,
                                    bool                             ItemIsGlobal = false,
                                    bool                             ItemIsInternal = false );

  /** Beschreibung der Funktion AddToDictionary
      \param pStructMemberDict
      \param Name
      \param Type
      \param pStuctVarDict
      \param ItemIsGlobal
      \param ItemIsInternal
      \return
      \todo Die Beschreibung der Funktion AddToDictionary fehlt
   */
  DataDictionary * AddToDictionary( DataDictionary                 * pStructMemberDict,
                                    const std::string              & Name,
                                    const DataDictionary::DataType   Type,
                                    DataDictionary                 * pStuctVarDict = 0,
                                    bool                             ItemIsGlobal = false,
                                    bool                             ItemIsInternal = false );

  /** Die Funktion loescht einen Eintrag aus dem DataDictionary und aus dem Datapool
   */
  bool RemoveFromDictionary( const char *pName );

  /** Die Funktion loescht einen Eintrag aus dem DataDictionary und aus dem Datapool
   */
  bool RemoveFromDictionary( DataDictionary *pDict );

  /** Die Funktion sucht eine Definition im DataDictionary und liefert den Pointer
      darauf oder 0, wenn sie nicht existiert. Die Definition kann mehrstufig
      angegeben werden, getrennt durch einen Punkt. Also z.B. "A.B.C"
   */
  DataDictionary *FindDictEntry( const char *pAddr );

  /** Die Funktion druckt das Dictionary aus. Dies ist eine Debugging-Funktion die
       nur aus Bequemlichkeit hier auch noch vorhanden ist.
   */
  void PrintDataDictionary();

  /** Die Funktion liefert einen Pointer auf das Root-Dataitem. Dieser Pointer
      beruecksichtigt schon den aktuellen Cycle
   */
  DataItem * GetRootItem( int cyclenum = -1 );

  /** Die Funktion liefert einen Pointer auf das Root-StructElement. Dieses
      Element haengt als einziges am Root-Item.
   */
  DataStructElement *GetRootElement( int cyclenum = -1 );

  /** Die Funktion erstellt einen neuen Cycle und liefert dessen Index.
   */
  int NewCycle();

  /** Die Funktion setzt den aktuellen Cylcepointer. Der gewünschte Cycle muss
      vorgaengig schon angelegt worden sein.
      Wenn der Parameter clearUndoStack 'false' ist, wird der undo-stack nicht gelöscht.
      ACHTUNG: Allerdings sollte dann wirklich nur lesend auf den Cycle zugegriffen werden.
      ( Dies kann leider nicht validiert werden. )
      \param CycleIndex
      \param clearUndoStack
      \return
   */
  bool SetCycle( int CycleIndex, bool clearUndoStack=true );

  /** Die Funktion loescht den angegeben Cycle und erstellt einen neuen (leeren) Cycle
   */
  bool ClearCycle( int CycleIndex );

  /** Die Funktion eliminiert einen Cycle. Die folgenden Cycles rutschen eine
      Stelle nach vorne.
   */
  bool RemoveCycle( int CycleIndex );

  /** Die Funktion eliminiert einen Cycle. Die folgenden Cycles rutschen eine
      Stelle nach vorne.
      ACHTUNG: Diese Funktion bedient den TTrail nicht. Sie wird jedoch
      vom TTrail verwendet zum Restaurieren des DataPools.
  */
  bool TTRemoveCycle( int currentcycle, int newcycle );

  /** TTInsertCycle
   */
  void TTInsertCycle( DataItem *cycle, int cyclenum );

  /** Die Funktion liefert die aktuelle Anzahl Cycles.
   */
  int NumCycles() const;

  /** Die Funktion liefert die aktuelle Cycle-Nummer.
   */
  int GetCurrentCycle() const;

  /** Die Funktion teilt, ob der Datenpool bereits vorhanden ist.
   */
  bool DataPoolAllocated() const;

  /** Die Funktion markiert den Beginn einer geschuetzten Transaktion. Der augenblickliche Zustand
      des DataPools wird festgehalten.
   */
  void BeginDataPoolTransaction( TransactionOwner *owner );

  /** Die Funktion markiert das Ende einer geschuetzten Transaktion. Der gespeicherte Zustand
      zu Beginn der Transaktion wird weggeworfen.
   */
  void CommitDataPoolTransaction( TransactionOwner *owner, bool all = false );

  /** Die Funktion markiert das unruehmliche Ende einer geschuetzten Transaktion. Der gespeicherte Zustand
      zu Beginn der Transaktion wird wieder hergestellt.
   */
  void RollbackDataPoolTransaction( TransactionOwner *owner, bool all = false );

  /** Die Funktion speichert alle offenen Transaktion. Vorsicht: Ist das wirklich gewollt?
   */
  void commitAllTransactions();

  /** Die Funktion teilt mit, ob eine offene Transaktion vorhanden ist.
   */
  bool inDataPoolTransaction() const;

  /** Diese Funktion schreibt den TTrail.
   */
  void writeTransactionTrail( DataItemContainer *cont, int inx );

  /** Diese Funktion Addiert dynamisch einen Wert zum Datapool. Der Eintrag in
      den DatenDictionary muss vorgaengig gemacht worden sein. Der Param
      pDict muss also auf ein gueltig gelinktes Dictionary-Item deuten.
      Diese Funktion wird automatisch aufgerufen, wenn ein Item mit der
      Funktion AddToDictionary addiert wird, und der Datapool schon alloziert
      worden ist.
      Bei Erfolg gibt die Funktion true zurueck.
  */
  bool DynamicDatapoolAlloc(DataDictionary * pDict);

  /** Die Funktion loescht Eintraege aus dem Datapool. Der Parameter pDict
      deutet auf ein Dictentry. Dies MUSS gueltig sein. D.h. es darf nicht
      vorgaengig aus dem Datapool "entlinked" worden sein. Die Funktion
      DynamicDatapoolDelete besorgt dann auch gerade das "entfernen".
      Als primaere Einschraenkung gilt: Es ist niemals moeglich eine
      Struktur-Definition zu entfernen.

      Die Aufruf-Sequenz koennt ungefaer so aussehen:
      pDict = FindDictEntry("MeineVariable");
      DynamicDatapoolDelete(pDcit);

      Entfernt wird ein Dictionary-Entrag niemals. Er wird nur
      als ungueltig markiert.
  */
  bool DynamicDatapoolDelete( DataDictionary * pDict );

  /** Die Funktion aktiviert das Undo/Redo-Feature.
   */
  void activateUndo();

  /** Die Funktion zeigt an, ob das Undo/Redo-Feature aktiviert wurde.
   */
  bool isUndoActivated();

  /** Mit dieser Funktion wird der Undo-Listener angehängt. Er wird bei jeder
      Änderung des Undo-Status mit der Funktion undoStatus() aktiviert.
   */
  void attachUndoListener( DataTTrailUndoListener *lsnr );

  /** Mit dieser Funktion wird der Redo-Listener angehängt. Er wird bei jeder
      Änderung des Undo-Status mit der Funktion redoStatus() aktiviert.
   */
  void attachRedoListener( DataTTrailUndoListener *lsnr );

  /** Mit dieser Funktion können Aktionen in den TTrail aufgenommen werden
      für das Undo/Redo-Feature
   */
  void saveTTrailActionElement( DataTTrailElement *el );

  /** Falls Daten vorhanden sind wird ein undo durchgeführt.
   */
  void undo();

  /** Falls Daten vorhanden sind wird ein redo durchgeführt.
   */
  void redo();

  /** Die Funktion setzt das Undo-Feature in Kraft. Das Feature muss aktiviert
      sein, sonst geschieht nichts.
   */
  void enableUndo();

  /** Die Funktion setzt das Undo-Feature ausser Kraft. Alle Undo-Daten werden
      initialisiert.
   */
  void disableUndo();

  /** Die Funktion setzt das Undo-Feature wieder in Kraft ohne das die
      Listener sich melden. Sie soll nur im Zusammenhang mit disallowUndo()
      verwendet werden.
   */
  void allowUndo();

  /** Die Funktion setzt das Undo-Feature temporär ausser Kraft ohne das die
      Listener sich melden.
      @return true => Das Undo-Feature war vor dem Aufruf aktiv.
   */
  bool disallowUndo();

  /** Diese Funktion schreibt den Datapool in den ostream ostr.
   */
  bool write( std::ostream &ostr
            , std::vector<std::string> &attrs
            , int level
            , bool debug );

  /** Die Funktion schreibt den gesamten Datapool im XML-Format in den Stream ostr.
      Der Output wird nur für Tests verwendet.
      \param ostr Outputstream
      \param i Anzahl der einzurückenden Stellen
      \param cycle optionale Cyclenummer
   */
  void writeDP( std::ostream &ostr, int i, int cycle=-1 );

  /** Die Funktion schreibt das gesamte Dictionary im XML-Format in den Stream ostr.
      Der Output wird nur für Tests verwendet.
      \param ostr Outputstream
      \param i Anzahl der einzurückenden Stellen
   */
  void writeDD( std::ostream &ostr, int i );

  /** Beschreibung der Funktion operator==
      \param dp
      \return
      \todo Die Beschreibung der Funktion operator== fehlt
   */
  bool operator==( const DataPool &dp );

  /** Beschreibung der Funktion operator!=
      \param dp
      \return
      \todo Die Beschreibung der Funktion operator!= fehlt
   */
  bool operator!=( const DataPool &dp );

  /** Die Funktion schreibt die aktuellen Objektzähler in den Stream.
   */
  static void showMemoryUsage( std::ostream &ostr );

  /** Beschreibung der Funktion setDataEvents
      \param e
      \return
      \todo Die Beschreibung der Funktion setDataEvents fehlt
   */
  static void setDataEvents( DataEvents *e );

  /** Beschreibung der Funktion getDataEvents
      \return
      \todo Die Beschreibung der Funktion getDataEvents fehlt
   */
  static DataEvents &getDataEvents();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
protected:
  /** Die Funktion DataDictionaryModified() aktualisiert den Datapool.
   */
  void DataDictionaryModified( DataDictionary *pDict ,bool AddOrDelete );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
protected:
  typedef std::vector<DataItem *> CycleList;

  CycleList             m_Cycles;             // Liste der Cycles
  int                   m_ActualCycle;        // Aktueller Cycle
  DataDictionaryRoot    m_DataDict;           // Root useres Data-Dictionary
  TransactionNumber     m_ActualTransaction;  // the actual Transactionnumber
  TransactionNumber     m_ClearTransaction;   // the Transactionnumber of last ClearCycle-Action
  DataItemAttr *        m_pItemAttr;          // Masterkopie des ItemAttributes
  DataTTrail            m_ttrail;             // Transaction Trail

private:
  static DataPool      *s_instance;
  static DataEvents    *s_events;
};

#endif // !defined(__DP_DATAPOOL__)
/** \file */
