
#if !defined(__DP_DATA_ALTER_GET_DIMENSION_SIZE__)
#define __DP_DATA_ALTER_GET_DIMENSION_SIZE__

#include "datapool/DataAlterFunctor.h"
#include "datapool/DataContainer.h"

/** Mit einem DataAlterGetDimensionSize-Objekt werden die Dimensionen der Elemente
    eines DataItem-Objekts analysiert und die logische Grösse ermittelt.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterGetDimensionSize : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataAlterGetDimensionSize( DataContainer::SizeList &dimsize );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterGetDimensionSize fehlt
   */
  virtual ~DataAlterGetDimensionSize();

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

  /** Die Funktion hat die Möglichkeit, vor der Bearbeitung einer Dimension noch etwas
      zun tun bevor es losgeht.
      @param dim Referenz auf das DataDimension-Objekt
      @return true -> Die Dimension soll normal bearbeitet werden.
   */
  virtual bool startDimension( const DataDimension &dim, DataInx &inx );

  /** Die Funktion zeigt an, ob ohne Indexliste auf einer Ebene alle Elemente durch
      alle Dimension durchlaufen werden sollen.
      @return true -> alle Elemente durchlaufen
   */
  virtual bool setWildcardWithoutIndices() const { return true; }

  /** Diese Funktion wird nach einer Verarbeitung als letzte Aktion aufgerufen.
      Sie kann als Debug-Funktion implementiert werden.
      @param result true -> Die Verarbeitung hat kein update durchgeführt
   */
  virtual void epilog( bool result );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  DataContainer::SizeList   &m_dimsize;
  int                        m_num_dimensions;

};

#endif
/** \file */
