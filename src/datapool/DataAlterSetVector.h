
#if !defined(__DP_DATA_ALTER_SET_VECTOR__)
#define __DP_DATA_ALTER_SET_VECTOR__

#include "datapool/DataAlterFunctor.h"

class DataVector;

/** Mit einem DataAlterSetVector-Objekt wird bei einem DataItem ein DataVector zugewiesen.
    Die Indexliste muss genau eine Wildcard haben.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterSetVector : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor mit dem zu setzenden DataVector-Parameter.
      @param v Vektor mit den neuen Werten für die referenzierten DataElemente.
   */
  DataAlterSetVector( const DataVector &v );

  /** Destruktor
   */
  virtual ~DataAlterSetVector();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion prüft, ob die gewünschte Bearbeitung durchgeführt werden kann.
      Es muss genau eine Wildcard auf der letzten Datenebene vorhanden sein.
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
  virtual bool updateElements() const { return true; }

  /** Die Funktion zeigt an, dass bei Bedarf ein neues Element angelegt werden muss.
      @return true -> Neues Element anlegen, wenn nötig
   */
  virtual bool createElements() const { return true; }

  /** Die Funktion teilt mit, ob nicht vorhandene Datenelemente bei Bedarf
      auch bei Iterationen über Wildcard-Indizes angelegt werden sollen.
      Achtung: Wenn der Funktor immer true zurück gibt, endet die Geschichte in
      einem Desaster !
      @param wildcard_num Nummer der betroffenen Wildcard
      @return true -> erstellen
  */
  virtual bool createWildcardElements( int wildcard_num );

  /** Die Funktion zeigt an, ob führende Indizes mit Wert 0 in einer Indexliste
      ignoriert werden sollen.
   */
  virtual bool ignoreLeadingZeroIndices() const { return false; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  const DataVector           &m_vector;
  DataVector::const_iterator  m_iter;
};

#endif
/** \file */
