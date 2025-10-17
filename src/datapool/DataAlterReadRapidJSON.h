
#if !defined(__DP_DATA_ALTER_READ_RAPID_JSON__)
#define __DP_DATA_ALTER_READ_RAPID_JSON__

#include "datapool/DataAlterSetElement.h"
#include <jsoncpp/json/json.h>
#include <stack>

/** Mit einem DataAlterReadRapidJSON-Objekt schreibt der MessageHandler von
    RapidJson die Werte in den Datapool.
    @author Copyright (C) 2020  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterReadRapidJSON : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor mit dem Json Stream.
      @param stream
   */
  DataAlterReadRapidJSON();

  /** Destruktor
   */
  virtual ~DataAlterReadRapidJSON();

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

  void setString( std::string &value );

  void setInteger( int value );

  void setUInteger( unsigned int value );

  void setDouble( double value );

  void setInvalid();

  bool setStartObject( int level );

  bool setKey( std::string &key, int level, bool tryDbAttrAsKey );

  bool setEndKey( int level );

  bool setEndObject( int level );

  void setStartArray( int level );

  void setEndArray();

  bool doOverride( int level ) const;

  void setStatus( UpdateStatus s );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
private:
  virtual UpdateStatus setValue( DataElement &el ) { return NoUpdate; }

  class ItemInfo
  {
  public:
    enum ItemType
    { type_Base,
      type_Key,
      type_Object,
      type_List
    };

  public:
    ItemInfo( int level, std::string name, ItemType type=type_Key );
    virtual ~ItemInfo();
  public:
    virtual ItemInfo *clone( int level, ItemType type );
    virtual int getRundung();
    virtual DataElement &dataElement();
    virtual void setString( std::string &value );
    virtual void setInteger( int value );
    virtual void setUInteger( unsigned int value );
    virtual void setDouble( double value );
    virtual void setInvalid();
    virtual void startNewIndex();
    virtual void endLastIndex();
    virtual bool isStructItem();
    bool isInfoType( ItemType type );
    int getLevel();
    std::string &getName();
    void setOverride();
    bool doOverride( int level );
    std::string itemType();
    virtual UpdateStatus markItemUpdated();
    virtual void setStatus( UpdateStatus s );
  protected:
    int             m_level;
    std::string     m_name;
    ItemType        m_type;
    bool            m_override;
    int             m_id;

    static int      s_lnr;
  };

  class ItemInfoExt : public ItemInfo
  {
  public:
    ItemInfoExt( DataItem &item,
                 DataIndexList *inxlist,
                 DataDictionary &dict,
                 int level,
                 ItemType type);
    virtual ~ItemInfoExt();
  public:
    virtual ItemInfo *clone( int level, ItemType type );
    virtual int getRundung();
    virtual DataElement &dataElement();
    virtual void setString( std::string &value );
    virtual void setInteger( int value );
    virtual void setUInteger( unsigned int value );
    virtual void setDouble( double value );
    virtual void setInvalid();
    virtual void startNewIndex();
    virtual void endLastIndex();
    virtual bool isStructItem();
    virtual UpdateStatus markItemUpdated();
    virtual void setStatus( UpdateStatus s );
  private:
    DataItem       &m_item;
    UpdateStatus    m_status;
    DataIndexList  *m_inxlist;
    DataDictionary &m_dict;
    DataDimension  *m_dim;
  };

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  typedef std::stack<ItemInfo *> ItemInfoStack;
  typedef std::stack<DataItem *> DataItemStack;
  ItemInfoStack  m_stack;
  DataItemStack  m_item_stack;
  UpdateStatus   m_status;
};

#endif
/** \file */
