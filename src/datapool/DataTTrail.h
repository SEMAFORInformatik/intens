
#if !defined(__DP_DATA_TTRAIL__)
#define __DP_DATA_TTRAIL__

#include <list>

#include "datapool/DataTTrailUndo.h"

class DataPool;
class DataItemContainer;
class DataItem;
class DataTTrailElement;
class TransactionOwner;

/** Das DataTTrail-Objekt kontrolliert alle Transactions im Datapool. Eine
    Transaction wird immer von einem TransactionOwner ausgelöst und auch wieder
    beendet. Anhand der TTrail-Daten wird bei einem Abort der Datapool wieder restauriert.
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataTTrail
{
  friend class DataPool;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:

  /** Konstruktor
      \param datapool
      \todo Die Beschreibung fehlt
   */
  DataTTrail( DataPool &datapool );

  /** Destruktor
   */
  virtual ~DataTTrail();

/*=============================================================================*/
/* Definitions                                                                 */
/*=============================================================================*/
public:
  typedef std::list<DataTTrailElement *> TTrail;

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion teilt mit, ob der Datapool in einer laufenden Transaktion ist.
      @return true -> im Transaction-Status
   */
  bool inTransaction() const;

  /** Die Funktion teilt mit, ob der Datapool in einer laufenden Transaktion ist.
      @return true -> im Transaction-Status
   */
  bool isInTransaction() const;

  /** Die Funktion teilt mit, ob der Datapool in einer laufenden Transaktion ist.
      @return true -> nicht im Transaction-Status
   */
  bool isntInTransaction() const;

  /** Eine neue Transaktion wird gestartet.
      @param owner Besitzer der Transaktion
   */
  void startTransaction( TransactionOwner *owner );

  /** Eine Transaktion wird erfolgreich beendet.
      @param owner Besitzer der Transaktion
   */
  void commitTransaction( TransactionOwner *owner );

  /** Eine Transaktion wird abgebrochen. Der alte Zustand des Datapool wird
      anhand der Datenaufzeichnungen im TTrail wieder hergestellt.
      @param owner Besitzer der Transaktion
   */
  void abortTransaction( TransactionOwner *owner );

  /** Alle noch offenen Transaktionen werden erfolgreich beendet.
   */
  void commitAllTransactions();

  /** Alle noch offenen Transaktionen werden abgebrochen. Der alte Zustand
      des Datapool wird anhand der Datenaufzeichnungen im TTrail wieder hergestellt.
   */
  void abortAllTransactions();

  /** Ein DataItem wird kopiert und im TTrail gesichert für eine eventuell
      notwendige Rekonstruktion bei einem abort().
      @param container Hauptcontainer mit allen Variablen
      @param itemNum Interne Nummer des DataItems in der Struktur.
   */
  void saveDataItem( DataItemContainer *container, int itemNum );

  /** Die Funktion sichert ein DataTTrailElement-Objekt im TTrail.
      @param el Pointer auf ein DataTTrailElement
   */
  void saveActionElement( DataTTrailElement *el );

  /** Die Funktion sichert bei einem Cycle-Switch die alte Cyclenummer.
      @param cyclenum Cyclenummer
   */
  void saveCycleNumber( int cyclenum );

  /** Die Funktion sichert bei einem Erstellen eines neuen Cycles die alte und die
      neue Cyclenummer.
      @param old_cyclenum alte Cyclenummer
      @param new_cyclenum neue Cyclenummer
   */
  void saveCycleBeforeNew( int old_cyclenum, int new_cyclenum );

  /** Die Funktion sichert beim Löschen eines Cycles die Daten.
      @param cycle Root-DataItem des Cycles
      @param cyclenum Cyclenummer
   */
  void saveDeletedCycle( DataItem *cycle, int cyclenum );

  /** Die Funktion übergibt eine Referenz auf das Undo-Objekt.
      @return Referenz auf das DataTTrailUndo-Objekt
   */
  DataTTrailUndo &TTrailUndo();

  /** Debug-Funktion: to find memory leaks
      @param ostr Outputstream
   */
  static void showMemoryUsage( std::ostream &ostr );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

  /** Beschreibung der Funktion isInInnerTransaction
      \return
      \todo Die Beschreibung der Funktion isInInnerTransaction fehlt
   */
  bool isInInnerTransaction() const;

  /** Beschreibung der Funktion isInFinalTransaction
      \return
      \todo Die Beschreibung der Funktion isInFinalTransaction fehlt
   */
  bool isInFinalTransaction() const;

  /** Beschreibung der Funktion commit
      \return
      \todo Die Beschreibung der Funktion commit fehlt
   */
  void commit();

  /** Beschreibung der Funktion commitInnerTransaction
      \return
      \todo Die Beschreibung der Funktion commitInnerTransaction fehlt
   */
  void commitInnerTransaction();

  /** Beschreibung der Funktion commitFinalTransaction
      \return
      \todo Die Beschreibung der Funktion commitFinalTransaction fehlt
   */
  void commitFinalTransaction();

  /** Beschreibung der Funktion abort
      \return
      \todo Die Beschreibung der Funktion abort fehlt
   */
  void abort();

  /** Beschreibung der Funktion ownerIsInTransaction
      \param owner
      \return
      \todo Die Beschreibung der Funktion ownerIsInTransaction fehlt
   */
  bool ownerIsInTransaction( TransactionOwner *owner );

  /** Beschreibung der Funktion clearTrail
      \return
      \todo Die Beschreibung der Funktion clearTrail fehlt
   */
  void clearTrail();

  /** Beschreibung der Funktion searchDataItem
      \param item
      \return
      \todo Die Beschreibung der Funktion searchDataItem fehlt
   */
  bool searchDataItem( DataItem *item );

  /** Beschreibung der Funktion resident
      \param DataTTrailElement
      \param iterator
      \param iterator
      \return
      \todo Die Beschreibung der Funktion resident fehlt
   */
  bool resident( DataTTrailElement *, TTrail::iterator, const TTrail::iterator );

  /** Beschreibung der Funktion isntActive
      \return
      \todo Die Beschreibung der Funktion isntActive fehlt
   */
  bool isntActive() const;

  /** Beschreibung der Funktion destroy
      \return
      \todo Die Beschreibung der Funktion destroy fehlt
   */
  void destroy();

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  typedef std::list<TTrail::iterator>    TTrailPoints;

  DataPool       &m_datapool;
  TTrail          m_trail;
  TTrailPoints    m_trail_points;
  int             m_transactions;
  bool            m_recovering;

  DataTTrailUndo  m_undo_data;
};

/** Beschreibung der Funktion operator<<
    \param ostr
    \param el
    \return
    \todo Die Beschreibung der Funktion operator<< fehlt
 */
std::ostream &operator << ( std::ostream &ostr, const DataTTrailElement &el );

#endif
/** \file */
