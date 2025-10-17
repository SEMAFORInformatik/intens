
#if !defined(__DP_DATA_ALTER_GET_ELEMENT_UPDATED__)
#define __DP_DATA_ALTER_GET_ELEMENT_UPDATED__

#include "datapool/DataAlterFunctor.h"

/** Mit einem DataAlterGetElementUpdated-Objekt werden die Timestamps für das
    gewünschte DataElement-Objekt eingesammelt. Mit den entsprechenden Funktionen
    isUpdated() und getUpdated() kann die gewünschte Information abgefragt werden.
    ACHTUNG: Es dürfen auch Wildcards auf der letzten Datenebene verwendet werden.
    In diesem Fall wird der jüngste Timestamp über alle referenzierten DataElemente
    verwendet resp. geliefert.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetElementUpdated : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param t Referenz-Timestamp für die Funktion isUpdated()
   */
  DataAlterGetElementUpdated( TransactionNumber t );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterGetElementUpdated fehlt
   */
  virtual ~DataAlterGetElementUpdated();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion prüft, ob eine Aktion möglich ist. Es können mehrere Elemente
      geprüft werden. Die Wildcards sind aber nur auf der letzten Ebene erlaubt.
      @param ref Referenz auf DataReference-Objekt
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
  virtual bool updateElements() const { return false; }

  /** Die Funktion zeigt an, dass kein neues Element angelegt werden muss.
      @return false -> Kein Element anlegen
   */
  virtual bool createElements() const { return false; }

  /** Die Funktion teilt mit, ob das untersuchte DataElement gegenüber dem Referenz-Timestamp
      verändert wurde.
      @param status Art der gewünschten Update-Information.
   */
  bool isUpdated( UpdateStatus status ) const;

  /** Die Funktion liefert den gewünschten Update-Timestamp des untersuchten DataElements.
      @param status Art der gewünschten Update-Information.
   */
  TransactionNumber getUpdated( UpdateStatus status ) const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  TransactionNumber  m_transactionnumber;
  TransactionNumber  m_element_data_timestamp;
  TransactionNumber  m_element_value_timestamp;
  TransactionNumber  m_element_full_timestamp;
  TransactionNumber  m_element_gui_timestamp;
  TransactionNumber  m_element_back_timestamp; // last Rollback
};

#endif
/** \file */
