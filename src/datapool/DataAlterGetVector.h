
#if !defined(__DP_DATA_ALTER_GET_VECTOR__)
#define __DP_DATA_ALTER_GET_VECTOR__

#include "datapool/DataAlterFunctor.h"

class DataVector;

/** Mit einem DataAlterGetVector-Objekt wird ein Vektor mit DataValue-Objekten
    gefüllt. Die DataValue-Objekte werden von den Jeweiligen DataElement-Objekten
    erzeugt. Der zu füllende Vektor wird dem Konstruktor als Parameter mitgegeben.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetVector : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param v DataVector-Objekt für das Resultat
   */
  DataAlterGetVector( DataVector &v );

  /** Destruktor
   */
  virtual ~DataAlterGetVector();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion prüft, ob er die Arbeit ausführen kann. Es muss genau eine
      Wildcard auf der letzten Ebene vorhanden sein!
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

  /** Für jedes nicht vorhandene DataElement (Löcher) in einer DataDimension wird
      diese Funktion aufgerufen. Ein Update ist nicht möglich.
      @param inx Referenz auf die aktuellen Index-Verhältnisse
      @param num_dim Nummer der Dimension, in welcher das leere Element vorkommt.
  */
  virtual void nullElement( DataInx &inx, int num_dim );

  /** Die Funktion zeigt an, dass im Datapool kein Element als 'updated' markiert werden muss.
      @return false -> Keine 'update'-Markierungen nötig.
    */
  virtual bool updateElements() const { return false; }

  /** Die Funktion zeigt an, dass kein neues Element angelegt werden muss.
      @return false -> Kein Element anlegen
   */
  virtual bool createElements() const { return false; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  DataType       m_type;
  DataVector    &m_vector;
};

#endif
/** \file */
