
#if !defined(__DP_DATA_ALTER_FUNCTOR__)
#define __DP_DATA_ALTER_FUNCTOR__

#include "datapool/DataGlobals.h"
#include "datapool/DataDebugger.h"

class DataReference;
class DataPool;
class DataItem;
class DataElement;
class DataStructElement;
class DataInx;
class DataContainer;
class DataDimension;

/** Mit dem DataAlterFunctor-Objekt werden alle Zugriffe auf Datapool-Strukturen
    bearbeitet. Die entsprechenden Functor-Implementationen werden auf dem Weg durch
    den Datenbaum aufgerufen.
 */
class DataAlterFunctor : public DataGlobals
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Der Konstruktor
   */
  DataAlterFunctor();

  /** Destruktor
   */
  virtual ~DataAlterFunctor();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// Debug-Funktion
  BUG_DECL_SHOW_COUNT("DataAlterFunctor");

  /** Diese Funktion wird vor der Verarbeitung aufgerufen. Der Funktor hat die
      Möglichkeit zu prüfen, ob er die Arbeit ausführen kann.
      @param ref Referenz auf das für den Zugriff vorgesehene DataReference-Objekt.
      @return true => Die Kontrolle ist nicht zufrieden => Abbruch
  */
  virtual bool check( DataReference &ref );

  /** Bei jedem in der Indexliste vorkommenden DataItem wird diese Funktion aufgerufen.
      @param item Referenz auf das DataItem
      @param status Falls Daten verändert wurden, Status des durchgeführten Updates.
      @param inx Referenz auf die aktuellen Index-Verhältnisse
      @param final true -> Die Arbeit auf diesem Datenast geht nicht mehr weiter, da die
      indizierten Elemente nicht vorhanden sind.
      @return false => die Arbeit soll auf diesem Datenast abgebrochen werden.
  */
  virtual bool alterItem( DataItem &item,
                          UpdateStatus &status,
                          DataInx &inx,
                          bool final=false ) = 0;

  /** Für jedes aufgefundene DataStructElement wird diese Funktion aufgerufen.
      @param el Referenz auf ein indiziertes DataStructElement.
  */
  virtual void alterStructData( DataStructElement &el ){}

  /** Für jedes aufgefundene DataElement wird diese Funktion aufgerufen.
      @param el Referenz auf ein indiziertes DataElement.
      @return Status des durchgeführten Updates.
              - NoUpdate      Keine Änderung
              - DataUpdated   Ein Attribut wurde verändert
              - ValueUpdated  Der Wert wurde verändert
  */
  virtual UpdateStatus alterData( DataElement &el ) = 0;

  /** Für jedes nicht vorhandene DataElement (Löcher) in einer DataDimension wird
      diese Funktion aufgerufen. Ein Update ist nicht möglich.
      @param inx Referenz auf die aktuellen Index-Verhältnisse
      @param num_dim Nummer der Dimension, in welcher das leere Element vorkommt.
  */
  virtual void nullElement( DataInx &inx, int num_dim ){}

  /** Die Funktion teilt mit, ob nach einem Zugriff die Datenelemente als
      'updated' markiert werden sollen.
      @result true -> markieren
  */
  virtual bool updateElements() const = 0;

  /** Die Funktion teilt mit, ob nicht vorhandene Datenelemente bei Bedarf
      angelegt werden sollen.
      @return true -> erstellen
  */
  virtual bool createElements() const = 0;

  /** Die Funktion teilt mit, ob der alte Zustand des DataItems in den TransactionTrail
      kopiert werden soll.
      @return true -> TTrail schreiben
  */
  virtual bool writeTTrail() const;

  /** Die Funktion teilt mit, ob nicht vorhandene Datenelemente bei Bedarf
      auch bei Iterationen über Wildcard-Indizes angelegt werden sollen.
      Achtung: Wenn der Funktor immer true zurück gibt, endet die Geschichte in
      einem Desaster !
      @param wildcard_num Nummer der betroffenen Wildcard
      @return true -> erstellen
  */
  virtual bool createWildcardElements( int wildcard_num ) { return false; }

  /** Dem Funktor wird angezeigt, wenn auf einer Wildcard mit dem nächsten
      Index-Wert gestartet wird.
      @param wildcard_num Nummer der Wildcard
      @param max_elements Anzahl der vorhandenen Elemente auf dieser Wildcard-Ebene
  */
  virtual void startWithWildcard( int wildcard_num, int max_elements ){}

  /** Dem Funktor wird angezeigt, wenn auf einer Wildcard das letzte Element
      erreicht wurde und die Liste zu Ende ist.
      @param wildcard_num Nummer der Wildcard
  */
  virtual void endWithWildcard( int wildcard_num ){}

  /** Dem Funktor wird angezeigt, dass genau ein Element in der Dimension
      indiziert wird.
  */
  virtual void startOneElement(){}

  /** Dem Funktor wird angezeigt, dass genau ein Element in der Dimension
      bearbeitet wurde und die Dimension verlassen wird.
  */
  virtual void endOneElement(){}

  /** Die Funktion hat die Möglichkeit, vor der Bearbeitung einer Dimension noch etwas
      zun tun bevor es losgeht, sofern es sich um ein DataDimension-Objekt handelt.
      @param dim Referenz auf das DataDimension-Objekt
      @return true -> Die Dimension soll normal bearbeitet werden.
   */
  bool startDimension( const DataContainer &cont, DataInx &inx );

  /** Die Funktion hat die Möglichkeit, vor der Bearbeitung einer Dimension noch etwas
      zun tun bevor es losgeht.
      @param dim Referenz auf das DataDimension-Objekt
      @return true -> Die Dimension soll normal bearbeitet werden.
   */
  virtual bool startDimension( const DataDimension &dim, DataInx &inx ){ return true; }

  /** Die Funktion zeigt an, dass auf der letzten Ebene im Falle eines Struktur-Items
      für alle vorhandenen DataItems in der Struktur ein Aufruf erfolgen soll.
      @return true -> Alle Items in der Struktur werden mit alterItem() bedient.
   */
  virtual bool allStructItems() const { return false; }

  /** Die Funktion zeigt an, ob ohne Indexliste auf einer Ebene alle Elemente durch
      alle Dimension durchlaufen werden sollen.
      @return true -> alle Elemente durchlaufen
   */
  virtual bool setWildcardWithoutIndices() const { return false; }

  /** Die Funktion zeigt an, ob führende Indizes mit Wert 0 ignoriert werden sollen.
   */
  virtual bool ignoreLeadingZeroIndices() const { return true; }

  /** Bei der Abfrage nach gültigen Elementen werden auch die DataItem-Attribute
      berücksichtigt, so dass u.U. auch ein Aufruf auf einem DataItem ohne gültige
      Elemente erfolgt.
  */
  virtual bool inclusiveValidItemAttributes() const { return false; }

  /** Diese Funktion wird nach einer Verarbeitung als letzte Aktion aufgerufen.
      Sie kann als Debug-Funktion implementiert werden.
      @param result true -> Die Verarbeitung hat kein update durchgeführt
   */
  virtual void epilog( bool result ){}

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;

};

#endif
/** \file */
