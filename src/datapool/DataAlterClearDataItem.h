
#if !defined(__DP_DATA_ALTER_CLEAR_DATAITEM__)
#define __DP_DATA_ALTER_CLEAR_DATAITEM__

#include "datapool/DataAlterFunctor.h"

/** Mit einem DataAlterClearDataItem-Objekt wird genau ein DataItem ungültig
    gemacht. Alle angehängten DataElemente werden ungültig.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterClearDataItem : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \todo Die Beschreibung der Funktion DataAlterClearDataItem fehlt
   */
  DataAlterClearDataItem( bool incl_itemattr=false );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterClearDataItem fehlt
   */
  virtual ~DataAlterClearDataItem();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
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
   */
  virtual UpdateStatus alterData( DataElement &el );

  /** Die Funktion zeigt an, dass im Datapool bei einer Änderung des Elements die
      betroffenen DataItems als 'updated' markiert werden müssen.
      @return true -> Bei Änderung des Elements die nötigen 'update'-Markierungen setzen.
   */
  virtual bool updateElements() const { return true; }

  /** Die Funktion zeigt an, dass kein neues Element angelegt werden muss.
      @return false -> Kein neues Element anlegen
   */
  virtual bool createElements() const { return false; }

  /** Bei der Abfrage nach gültigen Elementen werden auch die DataItem-Attribute
      berücksichtigt, so dass u.U. auch ein Aufruf auf einem DataItem ohne gültige
      Elemente erfolgt.
  */
  virtual bool inclusiveValidItemAttributes() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  bool    m_incl_itemattr;
};

#endif
/** \file */
