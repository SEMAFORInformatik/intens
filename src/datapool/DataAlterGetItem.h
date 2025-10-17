
#if !defined(__DP_DATA_ALTER_GET_ITEM__)
#define __DP_DATA_ALTER_GET_ITEM__

#include "datapool/DataAlterFunctor.h"

/** Mit einem DataAlterGetItem-Objekt wird genau ein DataItem-Objekt im
    Datenbaum gesucht.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetItem : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \param create
      \todo Die Beschreibung der Funktion DataAlterGetItem fehlt
   */
  DataAlterGetItem( bool create=false );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterGetItem fehlt
   */
  virtual ~DataAlterGetItem();

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

  /** Die Funktion zeigt an, dass im Datapool kein Item als 'updated' markiert werden muss.
      @return false -> Keine 'update'-Markierungen nötig.
    */
  virtual bool updateElements() const { return m_create; }

  /** Die Funktion zeigt an, dass kein neues Element angelegt werden muss.
      @return false -> Kein Element anlegen
   */
  virtual bool createElements() const { return m_create; }

  /** Die Funktion liefert den Pointer auf das gesuchte DataItem-Objekt.
      @return Pointer auf das DataItem-Objekt
   */
  const DataItem *getItem() const;

  /** Die Funktion liefert den Pointer auf das gesuchte DataItem-Objekt.
      ACHTUNG: Das DataItem darf nicht mutiert werden!
      @return Pointer auf das DataItem-Objekt
   */
  DataItem *getItemNoUpdate() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  DataItem  *m_item;
  bool       m_create;
};

#endif
/** \file */
