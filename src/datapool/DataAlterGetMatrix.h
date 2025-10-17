
#if !defined(__DP_DATA_ALTER_GET_MATRIX__)
#define __DP_DATA_ALTER_GET_MATRIX__

#include "datapool/DataAlterFunctor.h"

class DataMatrix;

/** Mit einem DataAlterGetMatrix-Objekt wird eine Matrix mit DataValue-Objekten
    gefüllt. Die DataValue-Objekte werden von den Jeweiligen DataElement-Objekten
    erzeugt. Die zu füllende Matrix wird dem Konstruktor als Parameter mitgegeben.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetMatrix : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param m DataMatrix-Objekt für das Resultat
   */
  DataAlterGetMatrix( DataMatrix &m );

  /** Destruktor
   */
  virtual ~DataAlterGetMatrix();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion prüft, ob er die Arbeit ausführen kann. Es müssen genau zwei
      Wildcards auf der letzten Ebene vorhanden sein!
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

  /** Diese Funktion wird aufgerufen mit dem gesuchten DataElement als Parameter.
      @param el Referenz auf das DataElement
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataElement &el );

  /** Die Funktion zeigt an, dass im Datapool kein Element als 'updated' markiert werden muss.
      @return false -> Keine 'update'-Markierungen nötig.
    */
  virtual bool updateElements() const { return false; }

  /** Die Funktion zeigt an, dass kein neues Element angelegt werden muss.
      @return false -> Kein Element anlegen
   */
  virtual bool createElements() const { return false; }

  /** Dem Funktor wird angezeigt, wenn auf einer Wildcard mit dem nächsten
      Index-Wert gestartet wird.
      @param wildcard_num Nummer der Wildcard
      @param max_elements Anzahl der vorhandenen Elemente auf dieser Wildcard-Ebene
  */
  virtual void startWithWildcard( int wildcard_num, int max_elements );

  /** Dem Funktor wird angezeigt, wenn auf einer Wildcard das letzte Element
      erreicht wurde und die Liste zu Ende ist.
      @param wildcard_num Nummer der Wildcard
  */
  virtual void endWithWildcard( int wildcard_num );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  DataMatrix    &m_matrix;
  DataVector    *m_vector;
};

#endif
/** \file */
