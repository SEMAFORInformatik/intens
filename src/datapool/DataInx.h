
#if !defined(__DP_DATA_INX_ITERATOR__)
#define __DP_DATA_INX_ITERATOR__

#include <iostream>

#include "datapool/DataDebugger.h"
#include "datapool/DataIndexList.h"
#include "datapool/DataContainer.h"

class DataIndices;
class DataIndex;
class DataDimension;

/** Das Objekt DataInx dient als temporärer Index-Container dazu, durch den
    Datenstrukturbaum zu navigieren.
 */
class DataInx
{
  friend class DataDimension;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \param indices
      \param level
      \todo Die Beschreibung der Funktion DataInx fehlt
   */
  DataInx( DataIndices &indices, int level=0 );

  /** Copy-Konstruktor
      \param DataInx
      \todo Die Beschreibung der Funktion DataInx fehlt
   */
  DataInx( const DataInx & );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataInx fehlt
   */
  virtual ~DataInx();

private:

  /** Der Konstruktor ohne Parameter wird nicht verwendet und ist daher nicht implementiert.
   */
  DataInx(); // unused

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataInx");

  /** Die Funktion zeigt an, ob die Indexliste auf dem aktuellen Level
      (siehe getLevel()) leer ist.
      @return true -> die Indexliste ist leer.
  */
  bool isIndexListEmpty() const;

  /** Die Funktion zeigt an, ob das DataInx-Objekt gültig ist. Dies ist nicht
      der Fall, wenn DataInx nicht auf einen Index zeigt, z.B. am Ende einer
      Indexliste angelangt ist.
      @return true -> Das Objekt ist gültig.
   */
  bool isIndexListValid() const;

  /** (Siehe Funktion isIndexListValid())
   */
  bool isntIndexListValid() const;

  /** Die Funktion zeigt an, wieviele Indizes in der Indexliste auf dem aktuellen
      Level noch verbleiben (inkl. aktueller Index).
      @return verbleibende Indizes.
  */
  int remainingIndicesInIndexList() const;

  /** newFrontIndex
      \todo Die Beschreibung der Funktion newFrontIndex fehlt
   */
  void newFrontIndex();

  /** newFrontWildcard
      \todo Die Beschreibung der Funktion newFrontWildcard fehlt
   */
  void newFrontWildcard();

  /** Das Objekt wird auf den ersten Index der Indexliste auf dem aktuellen
      Level gesetzt.
      @return true -> Die Indexliste ist nicht leer und das Objekt ist
      gültig (siehe isIndexListValid()).
  */
  bool firstIndex();

  /** Das Objekt wird auf den nächsten Index der Indexliste auf dem aktuellen
      Level gesetzt.
      @return true -> Ein weiterer Index war vorhanden und das Objekt ist
      gültig (siehe isIndexListValid()).
  */
  bool nextIndex();

  /** Die aktuelle Indexliste wird kopiert und der Pointer ausgegeben.
   */
  DataIndexList *cloneIndexList() const;

  /** Das Objekt wird auf den ersten Index des ersten Levels gesetzt.
      @return true -> Die Indexliste auf dem ersten Level ist nicht leer
      und das Objekt ist gültig (siehe isIndexListValid()).
  */
  bool firstLevel();

  /** Das Objekt wird auf den ersten Index des nächsten Levels gesetzt.
      @return true -> Die Indexliste auf dem nächsten Level ist vorhanden
      und nicht leer und das Objekt ist gültig (siehe isIndexListValid()).
  */
  bool nextLevel();

  /** Das Objekt wird auf den ersten Index des letzten Levels gesetzt.
      @return true -> Die Indexliste auf dem letzten Level ist nicht leer
      und das Objekt ist gültig (siehe isIndexListValid()).
  */
  bool lastLevel();

  /** Die Funktion darf nur auf dem letzten Level aufgerufen werden. Sie sorgt
      dafür, dass das Objekt auf dem letzten Level bleibt, aber ohne Indexliste.
      Dies ist nötig wenn der Funktor mit der Funktion allStructItems() anzeigt,
      dass er über den adressierten Bereich gehen will.
   */
  void freezeLastLevel();

  /** Die Funktion liefert die 0-relative Nummer des aktuellen Levels. Falls
      das Objekt nicht auf einem vorhandenen Level zeigt wird -1 zurück
      gegeben.
      @return aktuelle Levelnummer.
  */
  int getLevel() const;

  /** Die Funktion zeigt an, ob sie auf dem letzten Level steht.
      @return true -> der letzte Level ist erreicht.
   */
  bool isLastLevel() const;

  /** Die Funktion zeigt an, ob das Index-Objekt auf der letzten Datenebene (Level)
      steht und ein Freeze durchgeführt wurde (siehe freezeLastLevel() ).
      @return true -> der letzte Level ist erreicht und mit freeze fixiert.
   */
  bool isLastLevelFrozen() const;

  /** Die Funktion zeigt an, wieviele Levels nach dem Aktuellen noch verbleiben
      zum Bearbeiten.
      @return noch verbleibende Levels.
  */
  int remainingLevels() const;

  /** Die Funktion liefert die Anzahl der Indizes auf dem aktuellen Level.
      @return Anzahl Indizes.
   */
  int getDimensions() const;

  /** Die Funktion zeigt an, ob der aktuelle Index eine Wildcard ist.
      @return true -> Es ist eine Wildcard.
   */
  bool isWildcard() const;

  /** Die Funktion zeigt an, ob auf dem aktuellen Level Wildcards vorhanden
      sind.
      @return true -> Es hat Wildcards.
  */
  bool hasWildcards() const;

  /** Die aktuelle 1-relative Wildcardnummer bei einer Schlaufe über eine Wildcard
      kann abgefragt werden.
      @return aktuelle Wildcardnummer.
  */
  int wildcardNumber() const;

  /** Der aktuelle 0-relative Index bei einer Schlaufe über eine Wildcard
      kann abgefragt werden.
      @return aktueller Index der Wildcard.
  */
  int wildcardIndex() const;

  /** Die Funktion hängt an die aktuelle Indexliste einen Index mit dem ensprechenden
      Wert an.
      @param inx Wert des neuen Index
  */
  void newIndex( int inx );

  /** Die Funktion zeigt an, ob ab dem aktuell in Arbeit befindlichen Index noch
      Wildcards vorhanden sind.
      @param anzahl Die Anzahl der zu untersuchenden verbleibenden Indizes
             in der Indexlist.
   */
  bool hasRemainingWildcards( int anzahl=9999 ) const;

  /** (siehe hasRemainingWildcards() )
      @param anzahl Die Anzahl der zu untersuchenden verbleibenden Indizes
             in der Indexlist.
   */
  bool hasntRemainingWildcards( int anzahl=9999 ) const;

  /** Beschreibung der Funktion positionOfRemainingWildcards
      \param wc_pos_list
      \return
      \todo Die Beschreibung der Funktion positionOfRemainingWildcards fehlt
   */
  void positionOfRemainingWildcards( DataContainer::SizeList &wc_pos_list ) const;

  /** Beschreibung der Funktion remainingWildcards
      \return
      \todo Die Beschreibung der Funktion remainingWildcards fehlt
   */
  int remainingWildcards() const;

  /** Beschreibung der Funktion getLowerbound
      \return
      \todo Die Beschreibung der Funktion getLowerbound fehlt
   */
  int getLowerbound() const;

  /** Beschreibung der Funktion getUpperbound
      \return
      \todo Die Beschreibung der Funktion getUpperbound fehlt
   */
  int getUpperbound() const;

  /** Beschreibung der Funktion getIndex
      \return
      \todo Die Beschreibung der Funktion getIndex fehlt
   */
  int getIndex() const;

  /** Beschreibung der Funktion isZero
      \return
      \todo Die Beschreibung der Funktion isZero fehlt
   */
  bool isZero() const;

  /** Beschreibung der Funktion getItemIdnr
      \return
      \todo Die Beschreibung der Funktion getItemIdnr fehlt
   */
  int getItemIdnr() const;

  /** Die Funktion wird nur zum Debuggen verwendet.
      Sie liefert alle DataIndex-Objekte im aktuellen
      DataIndexList-Objekt als String-Resultat.
   */
  const std::string printAllIndices() const;

  /** Die Funktion wird nur zum Debuggen verwendet.
      Sie liefert die verbleibenden DataIndex-Objekte im aktuellen
      DataIndexList-Objekt als String-Resultat.
   */
  const std::string printRemainingIndices() const;

  /** Die Funktion wird nur zum Debuggen verwendet.
      Sie liefert die verbleibenden DataIndexList-Objekte als String-Resultat.
   */
  const std::string printRemainingLevels() const;

  /** Die Funktion wird nur zum Debuggen verwendet.
      Sie liefert das DataIndices-Objekt als String-Resultat.
   */
  const std::string print() const;

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Diese Funktion wird ausschliesslich von der Funktion alterData() in
      DataDimension aufgerufen. Der aktuelle 0-relative Index bei einer
      Schlaufe mit Wildcard wird gesetzt. Mit wildcardIndex() ist der Funktor
      in der Lage den Index abzufragen.
      @param i Index
  */
  void setWildcardIndex( int i );

  /** Dem Objekt wird mitgeteilt, dass auf dem aktuellen Level nur mit Wildcards
      gearbeitet wird. Die aktuelle Indexliste muss leer sein. Dies wird benötigt,
      wenn bei fehlenden Indizes über alle Elemente iteriert werden soll.
   */
  void setWildcard();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  /** Beschreibung der Funktion setIndex
      \return
      \todo Die Beschreibung der Funktion setIndex fehlt
   */
  bool setIndex();

/*=============================================================================*/
/* private member variables                                                    */
/*=============================================================================*/
private:
  DataIndices                   &m_indices;
  int                            m_level;
  DataIndexList                 *m_indexlist;
  const DataIndex               *m_index;
  bool                           m_use_wildcard;
  int                            m_wildcard_nr;
  int                            m_wildcard_index;
  DataIndexList::const_iterator  m_iter;
  bool                           m_freeze_last_level;

  BUG_DECLARE_COUNT;
};

#endif
/** \file */
