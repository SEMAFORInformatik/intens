
#if !defined(__DP_DATA_ALTER_SET_ATTRIBUTES__)
#define __DP_DATA_ALTER_SET_ATTRIBUTES__

#include "datapool/DataAlterFunctor.h"

/** Mit einem DataAlterSetAttributes-Objekt wird bei DataElementen die Attribute
    verändert.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterSetAttributes : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor mit dem zu setzenden Attribut-Masken.
      @param set_mask Maske mit den zu setzenden Attribut-Bits
      @param reset_mask Maske mit den zurück zu setzenden Attribut-Bits
      @param item_only Nur das DataItem soll mutiert werden
   */
  DataAlterSetAttributes( DATAAttributeMask set_mask, DATAAttributeMask reset_mask, bool item_only );

  /** Destruktor
   */
  virtual ~DataAlterSetAttributes();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion prüft, ob die gewünschte Bearbeitung durchgeführt werden kann.
      Wildcards sind erlaubt. Nur ohne Wildcards wird bei Bedarf ein neues
      DataElement erstellt.
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
  virtual bool createElements() const { return m_create; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  bool               m_item_only;
  DATAAttributeMask  m_set_mask;
  DATAAttributeMask  m_reset_mask;
  bool               m_create;
};

#endif
/** \file */
