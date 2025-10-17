
#if !defined(__DP_DATA_ALTER_COMPARE__)
#define __DP_DATA_ALTER_COMPARE__

#include "datapool/DataAlterFunctor.h"

class DataItem;

/** Mit einem DataAlterCompare-Objekt werden Listen von DataStructElement-Objekten
    miteinander verglichen.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterCompare : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor mit dem zu setzenden DataVector-Parameter.
      @param v Vektor mit den neuen Werten für die referenzierten DataElemente.
   */
  DataAlterCompare( DataItem &rslt_item, const DataElement *el_left = 0, int next_fall = 0 );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterCompare fehlt
   */
  virtual ~DataAlterCompare();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion prüft, ob die gewünschte Bearbeitung durchgeführt werden kann.
      Es dürfen nur auf der letzten Datenebene Wildcards vorhanden sein.
      @param ref Referenz auf das DataReference-Objekt
      @return true -> Die Verarbeitung ist nicht möglich.
   */
  virtual bool check( DataReference &ref );

  /** Für jedes DataItem auf dem Weg zu den gewünschten DataElementen wird diese Funktion
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
  virtual bool updateElements() const { return false; }

  /** Die Funktion zeigt an, dass bei Bedarf ein neues Element angelegt werden muss.
      @return true -> Neues Element anlegen, wenn nötig
   */
  virtual bool createElements() const { return false; }

  /** Beschreibung der Funktion numberOfCompares
      \return
      \todo Die Beschreibung der Funktion numberOfCompares fehlt
   */
  int numberOfCompares() const { return m_fall; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  DataItem            &m_rslt_item;
  bool                 m_count_elements;
  int                  m_valid_elements;
  int                  m_fall;
};

#endif
/** \file */
