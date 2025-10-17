
#if !defined(__DP_DATA_INDEXLIST__)
#define __DP_DATA_INDEXLIST__

#include <iostream>
#include <list>

#include "datapool/DataDebugger.h"
#include "datapool/DataIndex.h"

class DataIndices;
class DataInx;
class DataDictionary;

/** Das Objekt ist die komplette Liste der Indizes auf einer bestimmten
    Daten-Ebene. Es ist Teil des Objekts <b>DataIndizes</b>.
 */

class DataIndexList : public std::list<DataIndex>
{
  friend class DataIndices;
  friend class DataInx;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataIndexList();

  /** Konstruktor
      \param item_idnr
      \todo Die Beschreibung der Funktion DataIndexList fehlt
   */
  DataIndexList( int item_idnr );

  /** Copy-Konstruktor
      \param indexlist
      \todo Die Beschreibung der Funktion DataIndexList fehlt
   */
  DataIndexList( const DataIndexList &indexlist );

  /** Destruktor
   */
  virtual ~DataIndexList();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataIndices");

  /** Die Funktion setzt die Nummer des DataItem-Objekts innerhalb der Struktur
      im DataStructElement-Objekt einer Datenebene.
      @param idnr Nummer des DataItem's im DataStructElement-Objekt.
   */
  void setItemIdnr( int idnr );

  /** Die Funktion liefert für den Datenzugriff im  DataStructElement-Objekt die
      Nummer des DataItem-Objekts.
      @return Nummer des DataItem's im DataStructElement-Objekt.
   */
  int getItemIdnr() const;

  /** Ein neuer Index wird in die Indexliste eingefügt.
      @param inx Wert des eingefügten Index
      @param front true -> das DataIndex soll vorne angefügt werden
      @return Referenz auf das neue DataIndex-Objekt
   */
  DataIndex &newIndex( int inx=0, bool front=false );

  /** Ein neuer Index als Wildcard wird in die Indexliste eingefügt.
      @param front true -> das DataIndex soll vorne angefügt werden
      @return Referenz auf das neue DataIndex-Objekt
   */
  DataIndex &newWildcard( bool front=false );

  /** Die Funktion löscht den ersten Index in der Liste.
   */
  void deleteFirstIndex();

  /** Die Funktion löscht den letzten Index in der Liste.
   */
  void deleteLastIndex();

  /** Die Funktion liefert das entsprechende Index-Objekt. Index 0 entspricht
      dem ersten Index in der Liste.<br>
      FATAL. Wenn die gewünschte Indexnummer nicht vorhanden ist, bricht das
      Programm die Verarbeitung ab!
      @param index Nummer des gewünschten DataIndex-Objekts.
      @return Referenz auf das gewünschte DataIndex-Objekt.
  */
  DataIndex &index( int index );

  /** Die Funktion liefert das erste Index-Objekt in der Liste.
      FATAL: Wenn die gewünschte Objekt nicht vorhanden ist, bricht das
      Programm die Verarbeitung ab!
      @return Referenz auf das gewünschte DataIndex-Objekt.
  */
  DataIndex &firstIndex();

  /** Die Funktion liefert das letzte Index-Objekt in der Liste.
      FATAL: Wenn die gewünschte Objekt nicht vorhanden ist, bricht das
      Programm die Verarbeitung ab!
      @return Referenz auf das gewünschte DataIndex-Objekt.
  */
  DataIndex &lastIndex();

  /** Die Indexliste wird auf die gewünschte Grösse gesetzt. Die neuen
      DataIndex-Objekte erhalten den Wert 0. Ist die Indexliste bereits gross
      genug, geschieht nichts. Die Liste wird nicht verkleinert.
      @param sz Gewünschte Grösse (size)
   */
  void resizeList( int sz );

  /** Die gelieferten Indexwerte werden als neue Indexliste installiert.
      @param indexlist Indexwerte im Objekt.
   */
  void setIndexList( const DataIndexList &indexlist );

  /** Die gelieferten Indexwerte werden als neue Indexliste installiert.
      @param num_inx Anzahl der Indexwerte.
      @param inx Liste der Indexwerte.
   */
  void setIndexList( int num_inx, const int *inx );

  /** Die Funktion liefert die Indexwerte der vorhandenen Indexliste.
      @param num_inx Grösse der Liste für die Indexwerte. Der Parameter erhält
             durch die Funktion den Wert der wirklichen Anzahl der vorhandenen Indizes.
      @param inx Liste der Indexwerte.
      @return true -> es sind Indizes vorhanden.
   */
  bool getIndexList( int &num_inx, int *inx ) const;

  /** Die Funktion zeigt an, ob Wildcards in der Indexliste vorhanden sind.
      @return true -> es sind Wildcards vorhanden.
   */
  bool hasWildcards() const;

  /** Die Funktion liefert die Anzahl der Wildcards in der Indexliste.
      @return Anzahl der Wildcards
   */
  int numberOfWildcards() const;

  /** Die Funktion zeigt an, ob keine Indizes vorhanden sind.
   */
  bool isEmpty() const;

  /** Die Funktion zeigt an, wieviele Indizes vorhanden sind resp. wieviele
      Dimensionen indiziert werden.
      @return Anzahl der Indizes resp. Dimensionen.
   */
  int getDimensions() const;

  /** Die Funktion schreibt das referenzierte Item mit Zusatzinfos
      in den Stream.
      @param ostr Stream für den Output.
      @param dict Pointer auf das entsprechende DataDictionary-Objekt
      @param level Datenebene
      @param withIndices true -> die Indexwerte werden ausgegeben.
  */
  void printItem( std::ostream &ostr
                , const DataDictionary *dict
                , int level
                , bool withIndices ) const;

  /** Die Funktion schreibt die Liste aller Indizes in den Stream.
      @param ostr Stream für den Output.
   */
  void printIndices( std::ostream &ostr ) const;

    /** Die Funktion gibt die Liste aller Indizes aus.
      @return Liste aller Indizes
   */

  std::string indices() const;

/*=============================================================================*/
/* public operators                                                            */
/*=============================================================================*/
public:
  /** Der Operator vergleicht zwei DataIndexList-Objekte.
      @return Die beiden Objekte sind identisch.
   */
  bool operator==( const DataIndexList &inxlist ) const;

  /** Der Operator vergleicht zwei DataIndexList-Objekte.
      @return Die beiden Objekte sind NICHT identisch.
   */
  bool operator!=( const DataIndexList &inxlist ) const;

/*=============================================================================*/
/* private member variables                                                    */
/*=============================================================================*/
private:
  int      m_item_idnr;

  BUG_DECLARE_COUNT;
};

#endif
/** \file */
