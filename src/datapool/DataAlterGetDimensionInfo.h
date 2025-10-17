
#if !defined(__DP_DATA_ALTER_GET_DIMENSION_INFO__)
#define __DP_DATA_ALTER_GET_DIMENSION_INFO__

#include "datapool/DataAlterFunctor.h"

/** Mit einem DataAlterGetDimensionInfo-Objekt werden die Dimensionen der Elemente
    eines DataItem-Objekts analysiert und die logische Grösse ermittelt.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetDimensionInfo : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param dimension Nummer der zu untersuchenden Dimension
      @param level Datenebene im Datenbaum (-1 -> aktuelle Ebene des referenzierten DataItems.
   */
  DataAlterGetDimensionInfo( int dimension, int level=-1 );

  /** Konstruktor
      @param sizelist Vektor für Grösse der vorhandenen Dimensionen
      @param level Datenebene im Datenbaum (-1 -> aktuelle Ebene des referenzierten DataItems.
   */
  DataAlterGetDimensionInfo( std::vector<int> *sizelist, int level=-1 );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterGetDimensionInfo fehlt
   */
  virtual ~DataAlterGetDimensionInfo();

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

  /** Die Funktion liefert die logische Grösse der untersuchten Dimension.
      @return logische Grösse
   */
  int sizeOfDimension() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  // Parameters
  int               m_level;
  int               m_dimension;
  // Results
  int               m_size_of_dim;
  std::vector<int> *m_sizelist;
};

#endif
/** \file */
