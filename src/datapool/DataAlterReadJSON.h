
#if !defined(__DP_DATA_ALTER_READ_JSON__)
#define __DP_DATA_ALTER_READ_JSON__

#include "datapool/DataAlterSetElement.h"
#include <jsoncpp/json/json.h>

/** Mit einem DataAlterReadJSON-Objekt wird eine Json-Stream eingelesen und
    alle entsprechenden Werte zugewiesen.
    @author Copyright (C) 2020  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterReadJSON : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor mit dem Json Stream.
      @param stream
   */
  DataAlterReadJSON( std::string name, Json::Value &value );

  /** Destruktor
   */
  virtual ~DataAlterReadJSON();

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
              - NoUpdate      Keine Änderung
              - DataUpdated   Ein Attribut wurde verändert
              - ValueUpdated  Der Wert wurde verändert
   */
  virtual UpdateStatus alterData( DataElement &el );

  /** Die Funktion zeigt an, ob bei Bedarf ein neues Element angelegt werden muss.
      @return true -> Neues Element anlegen, wenn nötig
   */
  virtual bool createElements() const { return true; }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
private:
  void type( Json::Value &value );
  Json::ValueType getChildType( Json::Value &value );

  UpdateStatus read( Json::Value &value,
                     DataItem &item,
                     DataIndexList &inxlist,
		     DataDictionary &dict );
  UpdateStatus readObject( Json::Value &value,
                           DataItem &item,
                           DataElement &el,
			   DataDictionary &dict );
  UpdateStatus readArray( Json::Value &value,
                          DataItem &item,
                          DataIndexList &inxlist,
			  DataDictionary &dict );
  UpdateStatus readValue( Json::Value &value,
                          DataItem &item,
                          DataIndexList &inxlist,
			  DataDictionary &dict );
  virtual UpdateStatus setValue( DataElement &el ) {return NoUpdate;}
  UpdateStatus setValue( Json::Value &value,
                         DataElement &el,
                         DataDictionary &dict );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  std::string   m_name;
  Json::Value  &m_root;
};

#endif
/** \file */
