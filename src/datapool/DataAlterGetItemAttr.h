
#if !defined(__DP_DATA_ALTER_GET_ITEM_ATTR__)
#define __DP_DATA_ALTER_GET_ITEM_ATTR__

#include "datapool/DataAlterFunctor.h"

/** Mit einem DataAlterGetItemAttr-Objekt wird das DataItemAttr-Objekt eines DataItems
    im Datenbaum gesucht.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetItemAttr : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param update true -> Das DataItemAttr-Objekt wird für eine Änderung
             gelesen. Wenn nötig, wird ein neues Element angelegt.
   */
  DataAlterGetItemAttr( bool update );

  /** Destruktor
   */
  virtual ~DataAlterGetItemAttr();

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

  /** Diese Funktion wird nicht aufgerufen. Nur das DataItem ist von Interesse.
      @param el Referenz auf das DataElement
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataElement &el );

  /** Die Funktion zeigt an, ob im Datapool das Item als 'updated' markiert werden muss.
      @return false -> Keine 'update'-Markierungen nötig.
    */
  virtual bool updateElements() const;

  /** Die Funktion zeigt an, ob ein neues Item angelegt werden muss wenn es noch
      nicht vorhanden ist.
      @return false -> Kein Element anlegen
   */
  virtual bool createElements() const;

  /** Die Funktion zeigt an, dass keine Aufzeichnung im TTrail gemacht werden soll.
      @return false -> Kein Eintrag im TTrail erstellen
   */
  virtual bool writeTTrail() const { return false; }

  /** Die Funktion liefert den Pointer auf das DataItemAttr-Objekt des DataItems.
      Wenn im Konstruktor der update-Parameter 'true' übergeben wurde führt dieser
      aufruf zu einem Abbruch der Verarbeitung.
      @return Pointer auf das DataItemAttr-Objekt
   */
  const DataItemAttr *getConstItemAttr() const;

  /** Die Funktion liefert den Pointer auf das DataItemAttr-Objekt des DataItems.
      Wenn im Konstruktor der update-Parameter 'false' übergeben wurde führt dieser
      aufruf zu einem Abbruch der Verarbeitung.
      @return Pointer auf das DataItemAttr-Objekt
   */
  DataItemAttr *getItemAttr() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  bool          m_update;
  DataItem     *m_item;
};

#endif
/** \file */
