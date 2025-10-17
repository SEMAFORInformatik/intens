
#if !defined(__DP_DATA_ALTER_LOCK_ELEMENT__)
#define __DP_DATA_ALTER_LOCK_ELEMENT__

#include "datapool/DataAlterFunctor.h"

/** Mit einem DataAlterLockElement-Objekt wird bei DataElementen das Lock-Bit mutiert.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterLockElement : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param lock true -> Das Lock-Bit wird gesetzt. false -> Das Lock-Bit wird zurück gesetzt.
   */
  DataAlterLockElement( bool lock );

  /** Destruktor
   */
  virtual ~DataAlterLockElement();

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

  /** Die Funktion zeigt an, ob bei Bedarf ein neues Element angelegt werden muss.
      @return true -> Neues Element anlegen, wenn nötig
   */
  virtual bool createElements() const { return m_lock; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  bool   m_lock;

};

#endif
/** \file */
