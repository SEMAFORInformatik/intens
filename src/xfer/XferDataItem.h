
#if !defined(XFER_DATAITEM_INCLUDED_H)
#define XFER_DATAITEM_INCLUDED_H

#include "xfer/XferDataItemIndex.h"
#include "datapool/DataPoolDefinition.h"
#include "app/UserAttr.h"

#include "xml/Serializable.h"

class XferEditDataItemFunctor;
class ItemAttr;

/** Das XferDataItem beinhaltet ein DataReference-Objekt plus eine Liste aller
    Indizes für den Zugriff auf Datapool-Werte. Diese Zugriffe werden mit Hilfe
    dieser Objekt stark vereinfacht.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: XferDataItem.h,v 1.52 2005/11/08 12:06:55 ked Exp $
*/
class XferDataItem: public Serializable
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XferDataItem( DataReference *dref=0 )
    : m_dataref( dref )
    , m_paramtype( param_Default ){
  }

  /// Copy Constructor
  XferDataItem( const XferDataItem &item );
  XferDataItem( const XferDataItem &item, DataReference * const dref );
  virtual ~XferDataItem();

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum ParameterType
  { param_Default = 0
  , param_ComplexReal
  , param_ComplexImag
  , param_ComplexAbs
  , param_ComplexArg
  };

  typedef std::vector<XferDataItemIndex *> DataItemIndexList;

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

  DataDictionary *getDict();
  bool writeXML( std::ostream &os,
                 std::vector<std::string> &attrs,
                 int level,
                 bool debug );
  bool writeJSON( std::ostream &os,
                  int level,
                  int indent,
                  const SerializableMask flags );
  XferDataItemIndex *newDataItemIndex( int, bool isGuiIndex=false );
  XferDataItemIndex *newDataItemIndex();
  void eraseDataItemIndex( );
  void eraseDataItemIndex( int level );
  void eraseLastDataItemIndex( );
  void setDataReference( DataReference *ref );

  void setTypeComplexReal() { m_paramtype = param_ComplexReal; }
  void setTypeComplexImag() { m_paramtype = param_ComplexImag; }
  void setTypeComplexAbs() { m_paramtype = param_ComplexAbs; }
  void setTypeComplexArg() { m_paramtype = param_ComplexArg; }
  ParameterType getParamType() { return m_paramtype; }
  DataDictionary::DataType getDataType();
  bool isReadOnly() { return m_paramtype != param_Default; }
  bool checkDimensionIndizes();
  int getNumOfWildcards() const;
  int getIndexPosition( XferDataItemIndex * );
  XferDataItemIndex *getDataItemIndex( const std::string &name );
  XferDataItemIndex *getDataItemIndex( int pos );
  XferDataItemIndex *getDataItemIndexWildcard( int ) const;
  XferDataItemIndex *getDataItemIndexIndexed( int );
  void setDimensionIndizes();
  /** sets the collected indizes of specified level
      @param level specified level or all if -1
  */
  void setDimensionIndizes(int level);
  /// returns the current number of data item dimensions
  int getNumDimensions()const;
  /// returns the current size of one data item dimension
  int getDimensionSize(int dim)const;
  /// returns the maximum value of one index item
  int getMaxIndex( int pos )const;
  /// returns the last index
  XferDataItemIndex *getLastIndex() const;
  /// returns the validity of the data reference
  bool lastLevelHasIndices();
  bool isValid()const;
  //  bool hasElements()const;
  int getAllDataDimensionSize( DataContainer::SizeList &dimsize ) const;

  /// returns the data reference's node name
  const std::string getName()const{ return m_dataref->nodeName(); }
  const std::string getFullName( bool withIndexes ) const { return m_dataref != 0 ? m_dataref->fullName(withIndexes) : ""; }
  const std::string getFullName( const DataItemIndexList& ) const;
  const std::string getDbAttr()const{ return m_dataref->dbAttr(); }
  const DataItemIndexList& getDataItemIndexList(){ return m_indexlist; }
  void refreshDataItemIndexedList();

  /// returns the user attributes
  UserAttr *getUserAttr();

  /// returns the user attributes
  UserAttr &userAttr();

  /// returns the number of created indexes
  int getNumIndexes() const;
  /// returns the number of created indexes with non-empty name
  int getNumNamedIndexes() const;
  /// return index i (or 0 if it does not exist)
  XferDataItemIndex * getIndex( int i ) const;
  int getFirstIndexNum( int level ) const;

  DataReference *Data();
  size_t getSize() const;
  size_t getLastSize() const;
  int getSize(int indexnum) const;
  bool acceptIndex( const std::string &, int );
  bool setIndex( const std::string &, int );
  void setIndex( int, int );
  void setIndex( XferDataItemIndex *, int );

  /** Diese Funktion liefert den Wert des gewünschten Index.
   */
  int getIndex( int level, int index );

  /** Diese Funktion teilt mit, wieviel Levels die DataReference enthält. Mit
      dieser Funktion lässt sich prüfen, ob eine Levelnummer gültig ist. Die
      DataReference verzeiht falsche Levels nicht !!
  */
  int getNumberOfLevels();

  /** Diese Funktion prüft, ob alle abhängigen Variablen bereits gelöscht
      worden sind. Dies geschieht durch ein Analysieren der Input- und
      Output-Streams.
      @return True: => bereits gelöscht
  */
  bool StreamTargetsAreCleared(std::string& error_msg);
  void clearTargetStreams();

  //  void resetItemLock() {}
  //  void setItemLock() {}

  bool lockValue(){ assert(m_dataref!=0);return m_dataref->SetItemLocked_PreInx(true);}
  bool unlockValue() { assert(m_dataref!=0);return m_dataref->SetItemLocked_PreInx(false);}


  /** Die Funktion isUpdated() meldet, ob der momentan indizierte Wert seit
      dem Zeitpunkt der als Parameter übergebenen Transaktion verändert wurde.
      @param trans Für die Kontrolle benötigte Transaktionsnummer
      @return True: => Der Wert wurde geändert
  */
  bool isUpdated( TransactionNumber trans, bool gui = false );

  /** Die Funktion isDataItemUpdated() meldet, ob die momentan indizierte Variable
      seit dem Zeitpunkt der als Parameter übergebenen Transaktion verändert wurde.
      @param trans Für die Kontrolle benötigte Transaktionsnummer
      @return True: => Der Wert wurde geändert
  */
  bool isDataItemUpdated( TransactionNumber trans, bool gui = false );

  bool getValue( int & )const;
  bool getValue( double & )const;
  bool getValue( std::string & )const;
  bool getValue( dComplex & )const;

  bool getValues( DataVector& )const;

  bool getDataSetInputValue(std::string &input, const std::string &value);
  bool setValue( int );
  bool setValue( double );
  bool setValue( const std::string & );
  bool setValue( const dComplex & );

  bool setValues( const DataVector &v );

  bool setInvalid();

  void clear(){ m_dataref->SetItemInvalid_PreInx(); }

  /** clears (erases) data item beginning on last wildcard level
   * or on the lowest level that hase indexes
   */
  void clearRange();

  /** clears (erases) data item
   * on the highest level that has indexes
   * @param incl_itemattr true => Die Item-Attribute werden auch gelöscht
   */
  void clearRangeMaxLevel( bool incl_itemattr=false );

  /** clears (erases) data item beginning on level
      @param level
      @param incl_itemattr true => Die Item-Attribute werden auch gelöscht
  */
  void clearRange( int level, bool incl_itemattr=false );

  /** Die Funktion setzt bei allen verfügbaren, nicht transienten Items den
      DbTimestamp, damit die Werte 'not modified' sind. Diese Aktion ist nur
      nach dem Laden von Komponenten aus der Datenbank sinnvoll.
  */
  virtual void setDbItemsNotModified();

  void doEditData( XferEditDataItemFunctor &func
                 , const std::string &rowname, const std::string &colname, int inx );

  /** Diese Funktion setzt den Datenbank-Timestamp eines Items. Mit diesem
      Timestamp wird festgehalten, dass die Werte des Items von der Datenbank
      eingelesen wurden.
      @param level (last level if negative value)
  */
  void setDbItemTimestamp( TransactionNumber trans, int level=-1 );

  /**
     @return TransactionNumber
  */
  TransactionNumber getTransactionNumber(int level=-1) const;

  /** Mit dieser Funktion wird geprüft, ob Werte des Items seit dem Einlesen
      von der Datenbank verändert wurden. Ist kein Timestamp gesetzt worden
      (siehe setDbItemTimestamp() ), so wird als Resultat false zurückgegeben.
  */
  bool isDbItemUpdated(int level=-1) const;

  /** Mit dieser Funktion wird geprüft, ob ein Wert des Items seit dem Einlesen
      von der Datenbank verändert wurde. Ist kein Timestamp gesetzt worden
      (siehe setDbItemTimestamp() ), so wird als Resultat false zurückgegeben.
  */
  bool isDbValueUpdated() const;

  const std::string &getLabel();
  const std::string getUnit();

  /**  Ist das XferDataItem ein
       Vorfahr des uebergebenen XferDataItem */
  bool isAncestorOf( const XferDataItem *xfer );
  /**  Ist ein Vorfahr DbTransient */
  bool isAncestorDbTransient();

  bool alterData( DataAlterFunctor &f );

  /** Debug function
   */
  std::ostream & print( std::ostream &o ) const;

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  void setComplexAbsValue( double d );
  void setComplexArgValue( double d );
  void setComplexRealValue( double d );
  void setComplexImagValue( double d );
  ItemAttr &getItemAttrForUpdates( DataReference &ref );
  const ItemAttr *getItemAttr( DataReference &ref ) const;
  void setDataRefIndices() const;
  void setDataRefIndices( DataReference &ref ) const;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  DataReference        *m_dataref;
  DataItemIndexList     m_indexlist;
  ParameterType         m_paramtype;
};

/** Das XferEditDataItemFunctor

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: XferDataItem.h,v 1.52 2005/11/08 12:06:55 ked Exp $
*/
class XferEditDataItemFunctor
{
public:
  XferEditDataItemFunctor() {}
  virtual ~XferEditDataItemFunctor() {}
  virtual void operator() ( XferDataItemIndex &index, int inx
                          , const DataReference &ref ) = 0;
};

// Debug function
std::ostream &operator<<(std::ostream &, const XferDataItem & );

#endif
