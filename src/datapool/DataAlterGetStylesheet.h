
#if !defined(__DP_DATA_ALTER_GET_STYLESHEET__)
#define __DP_DATA_ALTER_GET_STYLESHEET__

#include "datapool/DataAlterFunctor.h"
#include "datapool/DataAttributes.h"

/** Mit einem DataAlterGetStylesheet-Objekt wird die Attribut-Bitmaske eines
    DataElement-Objekts gesucht.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetStylesheet : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param t Timestamp zum Setzen des 'Updated'-Bits in der Maske
   */
  DataAlterGetStylesheet( TransactionNumber t );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterGetStylesheet fehlt
   */
  virtual ~DataAlterGetStylesheet();

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

  /** Die Funktion liefert die Attribut-Bit-Maske des DataElements.
      @return Bit-Mask
   */
  const std::string& get(bool& updateGui) const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  TransactionNumber  m_transactionnumber;
  TransactionNumber  m_element_gui_timestamp;
  TransactionNumber  m_element_back_timestamp; // last Rollback
  DATAAttributeMask  m_mask;
  std::string        m_stylesheet;
};

#endif
/** \file */
