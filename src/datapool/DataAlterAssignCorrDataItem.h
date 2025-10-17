
#if !defined(__DP_DATA_ALTER_ASSIGN_CORR_DATAITEM__)
#define __DP_DATA_ALTER_ASSIGN_CORR_DATAITEM__

#include "datapool/DataAlterFunctor.h"

class DataElement;
class DataDictionary;

/** Mit einem DataAlterAssignCorrDataItem-Objekt wird einem DataItem der Inhalt eines
    anderen DataItem zugewiesen. Es muss sich im ein Struktur-Item handeln, sonst
    geschieht nichts. In der Struktur werden nur DataItems mit übereinstimmendem
    DataDictionary zugewiesen. Die übrigen DataItems bleiben erhalten.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterAssignCorrDataItem : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param el Pointer auf das zuzuweisende DataElement
      @param dict zum DataElement zugehöriges DataDictionary
   */
  DataAlterAssignCorrDataItem( const DataElement *el, const DataDictionary &dict );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterAssignCorrDataItem fehlt
   */
  virtual ~DataAlterAssignCorrDataItem();

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
  virtual bool createElements() const;

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Die Funktion zeigt an, ob das Element gültig ist.
      @return true -> DataElement für den assign ist vorhanden.
   */
  bool isValid() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  const DataElement     *m_element;
  const DataDictionary  &m_dictionary;
};

#endif
/** \file */
