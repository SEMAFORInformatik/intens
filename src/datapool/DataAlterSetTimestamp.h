
#if !defined(__DP_DATA_ALTER_SET_TIMESTAMP__)
#define __DP_DATA_ALTER_SET_TIMESTAMP__

#include "datapool/DataAlterFunctor.h"

/** Mit einem DataAlterSetTimestamp-Objekt wird den referenzierten DataElements
    der gewünschte Timestamp gesetzt. Es sind Wildcards an beliebiger Stelle möglich.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterSetTimestamp : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \param t Timestamp für die referenzierten DataElements
      \param status Art des Timestamp-Updates
   */
  DataAlterSetTimestamp( TransactionNumber t, UpdateStatus status, bool update_items );

  /** Destruktor
   */
  virtual ~DataAlterSetTimestamp();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Für jedes DataItem auf dem Weg zum gewünschten DataElement wird diese Funktion
      aufgerufen.
      \param item Referenz auf das DataItem
      \param status Rückgabewert der vorgenommenen Mutation am DataItem-Objekt
      \param inx Referenz auf das Indexobjekt
      \param final true Das DataItem ist das physisch letzte vorhandene DataItem
             Im Datenbaum. Das gesuchte DataItem resp. DataElement ist (noch) nicht vorhanden.
      \retval false Die Verarbeitung wird abgebrochen.
      \retval true  Die Verarbeitung soll weitergehen.
   */
  virtual bool alterItem( DataItem &item, UpdateStatus &status, DataInx &inx, bool final );

  /** Diese Funktion setzt anhand des im Konstruktor gesetzten Status den gewünschten
      Timestamp des DataElements.
      \param el Referenz auf das DataElement
      \return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataElement &el );

  /** Die Funktion zeigt an, dass im Datapool die betroffenen DataItem. als 'updated'
      markiert werden müssen.
      \return false -> Keine 'update'-Markierungen nötig.
    */
  virtual bool updateElements() const { return m_update_items; }

  /** Die Funktion zeigt an, dass kein neues Element angelegt werden muss.
      \return false -> Kein Element anlegen
   */
  virtual bool createElements() const { return false; }

  /** Die Funktion zeigt an, ob ohne Indexliste auf einer Ebene alle Elemente durch
      alle Dimension durchlaufen werden sollen.
      @return true -> alle Elemente durchlaufen
   */
  virtual bool setWildcardWithoutIndices() const { return m_wildcards; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void setItemTimestamp( DataItem &item );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  TransactionNumber  m_timestamp;
  UpdateStatus       m_status;
  bool               m_wildcards;
  bool               m_update_items;
};

#endif
/** \file
    Die Datei enthält die Definition der Klasse <b>DataAlterSetTimestamp</b>.
*/
