
#if !defined(__DP_DATA_ALTER_SET_ELEMENT__)
#define __DP_DATA_ALTER_SET_ELEMENT__

#include "datapool/DataAlterFunctor.h"

class DataElement;

/** Dies ist die Basisklasse für Objekte, welche genau ein DataElement bearbeiten
    müssen. Ein neuer Wert wird gesetzt oder der alte wird gelöscht, abhängig vom
    valid-Parameter des Konstruktors.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland

 */
class DataAlterSetElement : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param valid true -> Der neue Wert des Element ist gültig
   */
  DataAlterSetElement( bool valid );

  /** Destruktor
   */
  virtual ~DataAlterSetElement();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion prüft, ob die gewünschte Bearbeitung durchgeführt werden kann.
      @param ref Refeerenz auf das DataReference-Objekt
      @return true -> Die Verarbeitung ist nicht möglich.
   */
  virtual bool check( DataReference &ref );

  /** Für jedes DataItem auf dem Weg zum gewünschten DataElement wird diese Funktion
      aufgerufen.
      @param item Referenz auf das DataItem
      @param status Rückgabewert der vorgenommenen Mutation am DataItem-Objekt
      @param inx Referenz auf das Indexobjekt
      @param final true Das DataItem ist das physisch letzte vorhandene DataItem
             Im Datenbaum. Das gesuchte DataItem resp. DataElement ist (noch) nicht vorhanden.
      @return true -> Die Verarbeitung soll weitergehen.
   */
  virtual bool alterItem( DataItem &item, UpdateStatus &status, DataInx &inx, bool final );

  /** Diese Funktion wird aufgerufen mit dem gesuchten DataElement zum Durchführen
      der nötigen Mutation.
      @param el Referenz auf das DataElement
      @return Status des durchgeführten Updates.
              - NoUpdate      Keine Änderung
              - DataUpdated   Ein Attribut wurde verändert
              - ValueUpdated  Der Wert wurde verändert
   */
  virtual UpdateStatus alterData( DataElement &el );

  /** Die Funktion zeigt an, dass im Datapool bei einer Änderung des Elements die
      betroffenen DataItems als 'updated' markiert werden müssen.
      @return true -> Bei Änderung des Elements die nötigen 'update'-Markierungen setzen.
    */
  virtual bool updateElements() const { return true; }

  /** Die Funktion zeigt an, ob bei Bedarf ein neues Element angelegt werden muss.
      @return true -> Neues Element anlegen, wenn nötig
   */
  virtual bool createElements() const { return m_valid; }

  /** Die Funktion zeigt an, ob führende Indizes mit Wert 0 in einer Indexliste
      ignoriert werden sollen.
   */
  virtual bool ignoreLeadingZeroIndices() const { return false; }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Diese Funktion wird aufgerufen zum Setzen des neuen Werts.
      @param el Referenz auf des DataElement
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus setValue( DataElement &el ) = 0;

  /** Die Funktion liefert das passende DataDictionary zum DataElement.
      @return Referenz auf das DataDictionary-Objekt
   */
  const DataDictionary &dictionary() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  bool              m_valid;
  DataDictionary   *m_dict;
};

#endif
/** \file */
