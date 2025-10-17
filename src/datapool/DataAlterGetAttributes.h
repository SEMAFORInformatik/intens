
#if !defined(__DP_DATA_ALTER_GET_ATTRIBUTES__)
#define __DP_DATA_ALTER_GET_ATTRIBUTES__

#include "datapool/DataAlterFunctor.h"
#include "datapool/DataAttributes.h"

/** Mit einem DataAlterGetAttributes-Objekt wird die Attribut-Bitmaske eines
    DataElement-Objekts gesucht.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetAttributes : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param t Timestamp zum Setzen des 'Updated'-Bits in der Maske
   */
  DataAlterGetAttributes( TransactionNumber t );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterGetAttributes fehlt
   */
  virtual ~DataAlterGetAttributes();

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

  /** Für jedes aufgefundene DataStructElement wird diese Funktion aufgerufen.
      @param el Referenz auf ein indiziertes DataStructElement.
  */
  virtual void alterStructData( DataStructElement &el );

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

  /** Diese Funktion wird nach einer Verarbeitung als letzte Aktion aufgerufen.
      @param result true -> Die Verarbeitung hat kein update durchgeführt
   */
  virtual void epilog( bool result );

  /** Die Funktion liefert die Attribut-Bit-Maske des DataElements.
      @return Bit-Mask
   */
  DATAAttributeMask get() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  TransactionNumber  m_transactionnumber;
  TransactionNumber  m_element_gui_timestamp;
  TransactionNumber  m_element_back_timestamp; // last Rollback
  DATAAttributeMask  m_mask;
  bool               m_editable;
};

#endif
/** \file */
