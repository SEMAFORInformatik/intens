
#if !defined(_DATA_ALTER_GET_DB_MODIFIED_H_INCLUDED_)
#define _DATA_ALTER_GET_DB_MODIFIED_H_INCLUDED_

#include "datapool/DataAlterFunctor.h"

class DataAlterGetDbModified : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DataAlterGetDbModified();
  virtual ~DataAlterGetDbModified();

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

  virtual bool updateElements() const { return false; }
  virtual bool createElements() const { return false; }

  /** Die Funktion zeigt an, dass auf der letzten Ebene im Falle eines Struktur-Items
      für alle vorhandenen DataItems in der Struktur ein Aufruf erfolgen soll.
      @return true -> Alle Items in der Struktur werden mit alterItem() bedient.
   */
  virtual bool allStructItems() const { return !m_db_modified; }

  /** Diese Funktion wird nach einer Verarbeitung als letzte Aktion aufgerufen.
      Sie kann als Debug-Funktion implementiert werden.
      @param result true -> Die Verarbeitung hat kein update durchgeführt
   */
  virtual void epilog( bool result );

  /** Die Funktion teilt mit, ob seit dem letzten Laden der Daten von der Datenbank
      sich Daten verändert haben.
   */
  bool getModified() { return m_db_modified; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  bool        m_db_modified;

};

#endif
