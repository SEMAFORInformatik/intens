
#if !defined(_DATA_ALTER_SET_DB_MODIFIED_H_INCLUDED_)
#define _DATA_ALTER_SET_DB_MODIFIED_H_INCLUDED_

#include "datapool/DataAlterFunctor.h"

class DataAlterSetDbModified : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DataAlterSetDbModified( TransactionNumber trans=0, bool new_trans=true );
  virtual ~DataAlterSetDbModified();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion wird vor der Verarbeitung aufgerufen. Der Funktor hat die
      Möglichkeit zu prüfen, ob er die Arbeit ausführen kann.
      @param ref Referenz auf das für den Zugriff vorgesehene DataReference-Objekt.
      @return true => Die Kontrolle ist nicht zufrieden => Abbruch
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
   */
  virtual UpdateStatus alterData( DataElement &el );

  virtual bool updateElements() const { return true; }
  virtual bool createElements() const { return false; }
  virtual bool deleteElements() const { return false; }

  /** Die Funktion zeigt an, dass auf der letzten Ebene im Falle eines Struktur-Items
      für alle vorhandenen DataItems in der Struktur ein Aufruf erfolgen soll.
      @return true -> Alle Items in der Struktur werden mit alterItem() bedient.
   */
  virtual bool allStructItems() const { return true; }

  /** Bei der Abfrage nach gültigen Elementen werden auch die DataItem-Attribute
      berücksichtigt, so dass u.U. auch ein Aufruf auf einem DataItem ohne gültige
      Elemente erfolgt.
  */
  virtual bool inclusiveValidItemAttributes() const;

  /** Diese Funktion wird nach einer Verarbeitung als letzte Aktion aufgerufen.
      Sie kann als Debug-Funktion implementiert werden.
      @param result true -> Die Verarbeitung hat kein update durchgeführt
   */
  virtual void epilog( bool result );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  TransactionNumber m_db_timestamp;
  bool              m_new_transaction;
};

#endif
