
#if !defined(DATAPOOLINTENS_INCLUDED_H)
#define DATAPOOLINTENS_INCLUDED_H

#include <string>
#include <map>

#include "datapool/DataPoolDefinition.h"
#include "streamer/TargetContainer.h"

#define INTERNAL_CYCLE_STRUCT "@CycleStruct"
#define INTERNAL_CYCLE_LIST "_CYCLELIST"
#define INTERNAL_CYCLE_NAME "_CYCLENAME"
#define INTERNAL_STREAM_STRUCT "@StreamStruct"
#define INTERNAL_LISTGRAPH_STRUCT "@ListGraphStruct"
#define INTERNAL_XRTGRAPH_STRUCT "@GuiXrtGraphStruct"
#define INTERNAL_XRT3DPLOT_STRUCT "@Gui3dPlotStruct"


// Data Item User Attributes
#define DATAeditable       (1L<<(DATAelAttrNext+0))
#define DATAoptional       (1L<<(DATAelAttrNext+1))
#define DATAlockable       (1L<<(DATAelAttrNext+2))
#define DATAuseColorset    (1L<<(DATAelAttrNext+3))
#define DATAtypeLabel      (1L<<(DATAelAttrNext+4))
#define DATAuserAttrNext   (DATAelAttrNext+5)

// Data Item Attributes
#define DATAIsEditable     (1L<<(DATAuserAttrNext+0))
#define DATAIsReadOnly     (1L<<(DATAuserAttrNext+1))
#define DATAcolor1         (1L<<(DATAuserAttrNext+2))
#define DATAcolor2         (1L<<(DATAuserAttrNext+3))
#define DATAcolor3         (1L<<(DATAuserAttrNext+4))
#define DATAcolor4         (1L<<(DATAuserAttrNext+5))
#define DATAcolor5         (1L<<(DATAuserAttrNext+6))
#define DATAcolor6         (1L<<(DATAuserAttrNext+7))
#define DATAcolor7         (1L<<(DATAuserAttrNext+8))
#define DATAcolor8         (1L<<(DATAuserAttrNext+9))
#define DATAcolorAll       (DATAcolor1 | DATAcolor2 | DATAcolor3 | DATAcolor4 | \
                            DATAcolor5 | DATAcolor6 | DATAcolor7 | DATAcolor8)
#define DATAcolorAlarm     (1L<<(DATAuserAttrNext+10))
#define DATAitemAttrNext   (DATAuserAttrNext+11)


class BasicStream;
class DataSet;
class ColorSet;

class DataPoolIntens
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  DataPoolIntens()
    : m_namespace_dict( 0 )
    , m_namespace_ref( 0 )
    , m_open_transactions( 0 )
    , m_cycle_ref( 0 ){
  }
public:
  virtual ~DataPoolIntens(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static DataPoolIntens &Instance();
  static void Destroy();
  static DataPool &getDataPool();
  void createDataPool();

  void setNamespace( const std::string &name );
  DataReference *getAndResetNamespace();
  bool noNamespace();
  void createNamespace( std::string &parent );

  DataReference *newDataReferenceInNamespace( const std::string &name );

  static DataDictionary *addToDictionary( const std::string &structName,
                                          const std::string &name,
                                          const DataDictionary::DataType type );
  static DataDictionary *addToDictionary( const std::string &structName,
                                          const std::string &name,
                                          const std::string &structDef );

  static DataDictionary *addDataVariable( const std::string &structName,
                                          const std::string &name,
                                          const DataDictionary::DataType type,
                                          bool isGlobal,
                                          bool isInternal );
  static DataDictionary *addStructureVariable( const std::string &structName,
                                               const std::string &name,
                                               const std::string &structDef,
                                               bool isGlobal,
                                               bool isInternal );
  static DataDictionary *addStructureDefinition( const std::string &name );
  static bool copyStructureDefinition( DataDictionary *, DataDictionary * );
  static DataReference *getDataReference( const std::string &name );
  static DataReference *getDataReference( const DataReference *,
                                          const std::string &name );

  static TransactionNumber NewTransaction();
  static TransactionNumber CurrentTransaction();
  static TransactionNumber LastSourceStreamTransaction();
  static void setLastSourceStreamTransaction(TransactionNumber transId);

  void BeginDataPoolTransaction( TransactionOwner *owner );
  void RollbackDataPoolTransaction( TransactionOwner *owner );
  void CommitDataPoolTransaction( TransactionOwner *owner );

  DataSet *newDataSet( const std::string &name,
                       bool isGlobal,
                       bool isInternal );
  DataSet *getDataSet( const std::string &name );
  void createDataSet( const std::string &name, DataSet *dataset );

  ColorSet *newColorSet( const std::string &name );
  ColorSet *getColorSet( const std::string &name );

  void setTargetStream( DataReference *ref, BasicStream *str );
  void fixupItemStreams( BasicStream &stream, DataReference *ref );
  void fixupItemStreams( BasicStream &stream, const std::string &name );
  bool checkTargetStreams( DataReference *ref, std::string& error_msg );
  void clearTargetStreams( DataReference *ref );
  void printTargetContainer();
  void lspWrite(std::ostream &ostr);

  bool clearCycle( int cyclenum );
  void newCycle( const std::string &title );
  bool firstCycle();
  bool lastCycle();
  bool nextCycle();
  int currentCycle();
  int numCycles();
  /**
     Wenn der Parameter clearUndoStack 'false' ist, wird der undo-stack nicht gelöscht.
     ACHTUNG: Allerdings sollte dann wirklich nur lesend auf den Cycle zugegriffen werden.
     ( Dies kann leider nicht validiert werden. )
   */
  bool goCycle( int num, bool clearUndoStack=true );
  bool removeCycle( int num );
  void setCycleName( int num, const std::string &name );
  bool getCycleName( int num, std::string &name );
  bool isCycleNameUpdated( int num );
  void removeCycleName( int num );

  static void printAttributes( std::ostream &ostr, DATAAttributeMask mask );
  void printItems( std::ostream &o );
  void printPersistentItems( std::ostream &o, bool restdb );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void initializeDataPool();
  void createDataSetItems();
  /** append all targetStreamLists of all parents
   */
  TargetStreamList* appendParentTargetStreamList(std::string& name, TargetStreamList* srcList);

  std::string getAddrInfo();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  void printPersistentItems( std::ostream &o,
			     std::vector< std::string > &xmlattrs,
			     DataReference *d );

  typedef std::map<std::string, DataSet *> DataSetList;
  typedef std::map<std::string, ColorSet *> ColorSetList;

  static DataPoolIntens     *s_instance;
  static DataPool           *s_datapool;
  static DataDictionary     *s_root_dict;
  static TransactionNumber   s_transaction;
  static TransactionNumber   s_transaction_last_source_stream;

  std::string                     m_namespace;
  DataDictionary            *m_namespace_dict;
  DataReference             *m_namespace_ref;

  DataReference             *m_cycle_ref;

  DataSetList                m_datasetlist;
  ColorSetList               m_colorsetlist;

  int                        m_open_transactions;
  TargetContainer            m_targets;
};

#endif
