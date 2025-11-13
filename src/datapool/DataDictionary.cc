
#include <iostream>
#include <algorithm>

#include "utils/utils.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataUserAttr.h"
#include "datapool/DataAttributes.h"
#include "datapool/DataLogger.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataDictionary);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataDictionary::DataDictionary( DataUserAttr *userattr )
  : m_Name( "Str_Root" )
  , m_pAttributes( 0 )
  , m_pNext( 0 )
  , m_pStruct( 0 )
  , m_pParentNode( 0 )
  , m_DatenType( type_StructDefinition )
  , m_ItemIndex( InvalidIndex )
  , m_ItemIsGlobal( false )
  , m_ItemIsInternal(false )
  , m_ItemIsProtected( false )
  , m_ItemWithoutTTrail( false )
  , m_folderFlag( true ){
  BUG_INCR_COUNT;

  if( userattr == 0 ){
    // Wenn kein Pointer auf ein DataUser-Attribut geliefert wird, verwendet
    // das Dictionary eine eigene leere Klasse.
    m_pAttributes = DefaultDataUserAttr().NewClass( this );
  }
  else{
    m_pAttributes = userattr->NewClass( this );
  }
  assert( m_pAttributes != 0 );

  assert( isValidNameString( m_Name ) );

  setNext( new DataDictionary(1) );
  m_pNext->m_pStruct = this;
  m_pNext->AllocAttr( m_pAttributes );
}


DataDictionary::DataDictionary( int special )
  : m_Name("")
  , m_pAttributes(0)
  , m_pNext(0)
  , m_pStruct(0)
  , m_pParentNode(0)
  , m_DatenType(type_StructDefinition)
  , m_ItemIndex(InvalidIndex)
  , m_ItemIsGlobal( false )
  , m_ItemIsInternal(false )
  , m_ItemIsProtected( false )
  , m_ItemWithoutTTrail( false )
  , m_folderFlag( true ){
  BUG_INCR_COUNT;
  LOG_DEBUG("Special="<<special);

  switch (special) {
    case 1:
      /************************************************************/
      /* Dieser Fall Konstuiert die Struktur-Variable fuer die    */
      /* Root-Struktur. Diese Variable wird in pNext angehaengt.  */
      /* Sie ist das DictionaryItem fuer das Rootitem.            */
      /* Dieser Constructor wird aus dem Constructor fuer das     */
      /* rootitem aufgerufen.                                     */
      /************************************************************/
      m_DatenType = type_StructVariable;
      m_Name="Root";
      m_ItemIndex = 0; // Erstes alloziertes Item in diesem Level.
      break;

    default:
      assert(false);
  }
  assert( isValidNameString(m_Name) );
  return;
}

/*******************************************************************************/
/* Constructor                                                                 */
/* Constructor fuer einen regulaeren Node                                      */
/*******************************************************************************/

DataDictionary::DataDictionary( const std::string &Name,
                                DataType Type,
                                bool ItemIsGlobal,
                                bool ItemIsInternal )
  : m_Name(Name)
  , m_pAttributes(0)
  , m_pNext(0)
  , m_pStruct(0)
  , m_pParentNode(0)
  , m_DatenType(Type)
  , m_ItemIndex(0)
  , m_ItemIsGlobal( ItemIsGlobal )
  , m_ItemIsInternal( ItemIsInternal )
  , m_ItemIsProtected( false )
  , m_ItemWithoutTTrail( false )
  , m_folderFlag( true ){
  BUG_INCR_COUNT;
  LOG_DEBUG("name=[" << Name << "] Type=" << Type << " Global=" << ItemIsGlobal);
  assert(IsValidType(m_DatenType));
  assert( isValidNameString(m_Name) );

  if( isInternalName( Name ) ){
    m_ItemIsInternal = true;
  }
  return;
}

/*******************************************************************************/
/* Constructor                                                                 */
/* Copy Constructor                                                            */
/*******************************************************************************/

DataDictionary::DataDictionary(const DataDictionary &ref )
  : m_Name( ref.m_Name )
  , m_pAttributes( 0 )
  , m_pNext( 0 )
  , m_pStruct( ref.m_pStruct )
  , m_pParentNode( 0 )
  , m_DatenType( ref.m_DatenType )
  , m_ItemIndex( 0 )
  , m_ItemIsGlobal( ref.m_ItemIsGlobal )
  , m_ItemIsInternal( ref.m_ItemIsInternal )
  , m_ItemIsProtected( ref.m_ItemIsProtected )
  , m_ItemWithoutTTrail( ref.m_ItemWithoutTTrail )
  , m_folderFlag( ref.m_folderFlag ){
  BUG_INCR_COUNT;
  LOG_DEBUG( "name=[" << m_Name << "] Type=" << m_DatenType << " Global=" << m_ItemIsGlobal );
  assert(IsValidType(m_DatenType));
  assert( isValidNameString(m_Name) );

  m_pAttributes = ref.m_pAttributes->copyClass();
  return;
}


/*******************************************************************************/
/* Destructor                                                                  */
/*******************************************************************************/

DataDictionary::~DataDictionary(){
  LOG_DEBUG("(destructor)");
  if (m_pStruct && m_DatenType == type_StructDefinition){
    delete m_pStruct;
  }
  if (m_pNext){
    delete m_pNext;
  }
  if (m_pAttributes){
    delete m_pAttributes;
  }
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* datapool --                                                                 */
/* --------------------------------------------------------------------------- */

DataPool &DataDictionary::datapool() const{
  if( hasParentNode() ){
    return parent().datapool();
  }
  // Wir sind auf der Root-DataDictionary-Variable und brauchen nun die
  // Root-Definition.
  return structure().datapool();
}

/* --------------------------------------------------------------------------- */
/* hasParentNode --                                                            */
/* --------------------------------------------------------------------------- */

bool DataDictionary::hasParentNode() const{
  return m_pParentNode != 0;
}

/* --------------------------------------------------------------------------- */
/* setParent --                                                                */
/* --------------------------------------------------------------------------- */

void DataDictionary::setParent( DataDictionary *dict ){
  m_pParentNode = dict;
}

/* --------------------------------------------------------------------------- */
/* GetParent --                                                                */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionary::GetParent() const{
  return m_pParentNode;
}

/* --------------------------------------------------------------------------- */
/* parent --                                                                   */
/* --------------------------------------------------------------------------- */

DataDictionary &DataDictionary::parent() const{
  assert( m_pParentNode != 0 );
  return *m_pParentNode;
}

/* --------------------------------------------------------------------------- */
/* hasNextNode --                                                              */
/* --------------------------------------------------------------------------- */

bool DataDictionary::hasNextNode() const{
  return m_pNext != 0;
}

/* --------------------------------------------------------------------------- */
/* setNext --                                                                  */
/* --------------------------------------------------------------------------- */

void DataDictionary::setNext( DataDictionary *dict ){
  m_pNext = dict;
}

/* --------------------------------------------------------------------------- */
/* getNext --                                                                  */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionary::getNext() const{
  return m_pNext;
}

/* --------------------------------------------------------------------------- */
/* next --                                                                     */
/* --------------------------------------------------------------------------- */

DataDictionary &DataDictionary::next() const{
  assert( m_pNext != 0 );
  return *m_pNext;
}

/* --------------------------------------------------------------------------- */
/* hasStructureNode --                                                         */
/* --------------------------------------------------------------------------- */

bool DataDictionary::hasStructureNode() const{
  return m_pStruct != 0;
}

/* --------------------------------------------------------------------------- */
/* getStructure --                                                             */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionary::getStructure() const{
  return m_pStruct;
}

/* --------------------------------------------------------------------------- */
/* structure --                                                                */
/* --------------------------------------------------------------------------- */

DataDictionary &DataDictionary::structure() const{
  assert( m_pStruct );
  return *m_pStruct;
}

/* --------------------------------------------------------------------------- */
/* AddNextItem --                                                              */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionary::AddNextItem( const std::string &Name,
                                             const DataType Type,
                                             DataDictionary * StructRef,
                                             bool ItemIsGlobal,
                                             bool ItemIsInternal )
{
  DataDictionary * pDict = 0;

  LOG_DEBUG( "Name=" << Name << " Type=" <<Type << " MyName=[" << getName() << "]" );

  if( !hasParentNode() ){
    // Dies ist das erste Item im Dictionary. Wir befinden uns im Root-Item.
    assert( isRootNode() );
    pDict = AddFirstMember( Name, Type, StructRef, ItemIsGlobal, ItemIsInternal );
    LOG_DEBUG( "call of AddFirstMember: Name=[" << Name << "] rslt=" << pDict );
    return pDict;
  }

  assert( !hasNextNode() );
  assert( m_pAttributes != 0 );
  assert( Type == type_StructVariable ? StructRef != 0 :  StructRef == 0 );
  assert( StructRef != 0 ? StructRef->getDataType() == type_StructDefinition : true );
  assert( ItemIsGlobal ? TypeNeedsAlloc(Type) : true );

  if( !isNameUnique( Name ) ){
    LOG_DEBUG( "Name " << Name << " is not unique rstl 0");
    return 0;
  }

  // An dieser Stelle muss geprüft werden, ob ein gelöschter Eintrag in der
  // Kette ist. Diese kann man wiederverwenden (recyclen)
  if( (pDict = SearchDeletedNode()) != 0 ){
    // Dieser gelöschte Eintrag kann wieder verwendet werden.
    pDict->ReuseNode(Name, Type, StructRef, ItemIsGlobal, ItemIsInternal );
    assert(pDict->GetParent() == GetParent());
  }
  else{
    // Ein neuer Eintrag wird erstellt.
    if( (pDict = new DataDictionary(Name, Type, ItemIsGlobal, ItemIsInternal )) == 0 ){
      LOG_DEBUG("no mem rstl 0");
      assert(false);
      return 0;   // Kein Memory. Nicht schoen
    }
    pDict->setParent( m_pParentNode );
    pDict->setNext( m_pNext );
    pDict->m_pStruct    = StructRef;
    setNext( pDict );
    pDict->m_ItemIndex  = TypeNeedsAlloc(Type) ? GetHighestIndex()+1 : InvalidIndex;
    pDict->AllocAttr(m_pAttributes);
  }

  assert( ItemIsGlobal ? pDict->isParentNodeRoot() : true );

  LOG_DEBUG( "Name=" << Name << " rslt=" << pDict);
  return pDict;
}

/* --------------------------------------------------------------------------- */
/* AddNextItem --                                                              */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::AddNextItem( const DataDictionary &dict ) {
  DataDictionary *pDict = new DataDictionary( dict );

  pDict->setParent( m_pParentNode );
  pDict->setNext( m_pNext );
  setNext( pDict );
  pDict->m_ItemIndex  = TypeNeedsAlloc( pDict->m_DatenType ) ?
                        GetHighestIndex()+1 : InvalidIndex;
  return pDict;
}

/* --------------------------------------------------------------------------- */
/* AddFirstMember --                                                           */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionary::AddFirstMember( const std::string &Name,
                                                const DataType Type,
                                                DataDictionary *StructRef,
                                                bool ItemIsGlobal,
                                                bool ItemIsInternal )
{
  DataDictionary * pDict = 0;

  LOG_DEBUG("Name=[" << Name << " Type=" << Type << " MyName=[" << getName() << "]");

  assert(m_DatenType == type_StructDefinition);

  if ((pDict = new DataDictionary( Name, Type, ItemIsGlobal, ItemIsInternal )) == 0) {
    LOG_DEBUG( "no mem rstl 0");
    assert(false);
    return 0;   // Kein Memory. Nicht schoen
  }

  assert( m_pStruct == 0 );
  assert( m_pAttributes != 0 );
  assert( Type == type_StructVariable ? StructRef != 0 :  StructRef == 0 );
  assert( StructRef != 0 ? StructRef->getDataType() == type_StructDefinition : true );
  assert( ItemIsGlobal ? TypeNeedsAlloc(Type) : true );

  pDict->setParent( this );
  pDict->setNext( 0 );
  pDict->m_pStruct    = StructRef;
  m_pStruct           = pDict;
  pDict->m_ItemIndex  = TypeNeedsAlloc(Type) ? 0 : InvalidIndex;
  pDict->AllocAttr(m_pAttributes);

  assert( ItemIsGlobal ? pDict->isParentNodeRoot() : true );

  LOG_DEBUG("Name=" << Name << " rslt=" << pDict);
  return pDict;
}

/* --------------------------------------------------------------------------- */
/* AddFirstMember --                                                           */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::AddFirstMember( const DataDictionary &dict ) {
  assert( m_DatenType == type_StructDefinition );

  DataDictionary *pDict = new DataDictionary( dict );

  pDict->setParent( this );
  pDict->setNext( 0 );
  m_pStruct           = pDict;
  pDict->m_ItemIndex  = TypeNeedsAlloc( pDict->m_DatenType ) ? 0 : InvalidIndex;
  return pDict;
}

/* --------------------------------------------------------------------------- */
/* isNameUnique --                                                             */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isNameUnique( const char *name, int len ) const{
  assert( name != 0 );
  if( len == 0 ){
    len=strlen( name );
  }
  return SearchByName( name, len ) == 0;
}

/* --------------------------------------------------------------------------- */
/* isNameUnique --                                                             */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isNameUnique( const std::string &name ) const{
  return SearchByName( name ) == 0;
}

/* --------------------------------------------------------------------------- */
/* GetAttr --                                                                  */
/* --------------------------------------------------------------------------- */

DataUserAttr *DataDictionary::GetAttr() const{
  assert( m_pAttributes != 0 );
  return m_pAttributes;
}

/* --------------------------------------------------------------------------- */
/* getDataUserAttr --                                                          */
/* --------------------------------------------------------------------------- */

const DataUserAttr *DataDictionary::getDataUserAttr() const {
  return m_pAttributes;
}

/* --------------------------------------------------------------------------- */
/* AllocAttr --                                                                */
/* --------------------------------------------------------------------------- */

void DataDictionary::AllocAttr( DataUserAttr *pParentAttribute ){
  LOG_DEBUG("AllocAttr");

  assert(m_pAttributes    == 0);
  assert(pParentAttribute != 0);

  m_pAttributes = pParentAttribute->NewClass(this);
  assert(m_pAttributes != 0);
}

/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */

const std::string &DataDictionary::getName() const{
  return m_Name;
}

/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */

void DataDictionary::getName( std::string &name ) const{
  name = m_Name;
}

/* --------------------------------------------------------------------------- */
/* isParentNodeRoot --                                                         */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isParentNodeRoot() const{
  if( hasParentNode() ){
    return GetParent()->isRootNode();
  }
  assert( isRootNode() );
  return false; // Root
}

/* --------------------------------------------------------------------------- */
/* getDataType --                                                              */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataDictionary::getDataType() const{
  return m_DatenType;
}

/* --------------------------------------------------------------------------- */
/* changeDataType --                                                           */
/* --------------------------------------------------------------------------- */

void DataDictionary::changeDataType( DataType typ ){
  assert(IsBasicType(m_DatenType));
  assert(IsBasicType(typ));
  m_DatenType = typ;
}

/* --------------------------------------------------------------------------- */
/* getDefaultDimensions --                                                     */
/* --------------------------------------------------------------------------- */

int DataDictionary::getDefaultDimensions(){
  return maximum( 0, (int)m_DefaultDimensions.size() - 1 );
}

/* --------------------------------------------------------------------------- */
/* GetDefaultDimension --                                                      */
/* --------------------------------------------------------------------------- */

int DataDictionary::GetDefaultDimension(int nDim ){
  assert(nDim > 0);
  if ( nDim >= (int)m_DefaultDimensions.size() ){
    return 1;
  }
  return m_DefaultDimensions[ nDim ];
}

/* --------------------------------------------------------------------------- */
/* unsetFolder --                                                              */
/* --------------------------------------------------------------------------- */

void DataDictionary::unsetFolder(){
  m_folderFlag = false;
}

/* --------------------------------------------------------------------------- */
/* getFolderFlag --                                                            */
/* --------------------------------------------------------------------------- */

bool DataDictionary::getFolderFlag() const{
  return m_folderFlag;
}

/* --------------------------------------------------------------------------- */
/* SetDefaultDimension --                                                      */
/* --------------------------------------------------------------------------- */

void DataDictionary::SetDefaultDimension( int nDim,int val ){
  assert(nDim > 0);
  if (nDim >= (int)m_DefaultDimensions.size()){
    m_DefaultDimensions.resize(nDim+1,0);
  }
  m_DefaultDimensions[nDim] = val;
}

/* --------------------------------------------------------------------------- */
/* ClearDefaultDimension --                                                    */
/* --------------------------------------------------------------------------- */

void DataDictionary::ClearDefaultDimension(){
  m_DefaultDimensions.clear();
}

/* --------------------------------------------------------------------------- */
/* GetNumberOfNextNodes --                                                     */
/* --------------------------------------------------------------------------- */

int DataDictionary::GetNumberOfNextNodes() const{
  int cnt = 1;
  const DataDictionary *pI = this;

  while( (pI=pI->getNext()) ){
    cnt++;
  }
  return cnt;
}

/* --------------------------------------------------------------------------- */
/* GetHighestIndex --                                                          */
/* --------------------------------------------------------------------------- */

int DataDictionary::GetHighestIndex() const{
  LOG_DEBUG("GetHighestIndex");

  int inx                      = InvalidIndex;
  const DataDictionary * pDict = GetFirstUserEntry();
  assert( pDict != 0 );

  do {
    if( pDict->GetItemIndex() > inx ){
      inx = pDict->GetItemIndex();
    }
    pDict = pDict->getNext();
  }
  while( pDict != 0 );

  LOG_DEBUG( "Highest Index=" << inx);
  return inx;
}

/* --------------------------------------------------------------------------- */
/* GetLastEntry --                                                             */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::GetLastEntry() const{
  LOG_DEBUG("GetLastEntry");

  DataDictionary * pDict = GetFirstUserEntry();
  assert(pDict != 0);

  if( pDict == this                                         &&
      pDict->GetParent() == 0                               &&
      pDict->getNext() != 0                                 &&
      pDict->getNext()->getDataType()== type_StructVariable &&
      pDict->getDataType()== type_StructDefinition          ){
    /********************************************************/
    /* Wir deuten auf die Root-Struktur-Definition. Dabei   */
    /* macht es wenig Sinn, das nextitem zu suchen. Wir     */
    /* werden desshlab den thispointer zurueck geben.       */
    /********************************************************/
    return const_cast<DataDictionary *>(this);
  }

  DataDictionary *pD;
  while( (pD = pDict->getNext()) != 0 ){
    pDict = pD;
  }
  return pDict;
}

/* --------------------------------------------------------------------------- */
/* SearchByName --                                                             */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::SearchByName(const char * pName, int len) const {
  if (len <= 0 && pName != 0)
    len = strlen(pName);

  LOG_DEBUG("Name=[" << std::string(pName,len) << "] len=" << len);

  assert(pName != 0);
  assert(len > 0);

  DataDictionary * pDict = GetFirstUserEntry();
  assert(pDict != 0);

  do {
    if( len == pDict->m_Name.length()                     &&
        strncmp( pDict->m_Name.c_str(), pName, len ) == 0 ){
      return  pDict;
    }
    pDict = pDict->getNext();
  }
  while( pDict != 0 );

  LOG_DEBUG("Not Found");
  return 0;
}

/* --------------------------------------------------------------------------- */
/* SearchByName --                                                             */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::SearchByName(const std::string &Name) const{
  return SearchByName( Name.c_str(), Name.length() );
}

/* --------------------------------------------------------------------------- */
/* SearchByIndex --                                                            */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::SearchByIndex(int index) const {
  LOG_DEBUG("Index=" << index);

  DataDictionary * pDict = GetFirstUserEntry();
  assert(pDict != 0);

  do {
    if( pDict->GetItemIndex() == index ){
      return pDict;
    }
    pDict = pDict->getNext();
  }
  while( pDict != 0 );

  LOG_DEBUG("Not Found");
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getStaticName --                                                            */
/* --------------------------------------------------------------------------- */

const std::string DataDictionary::getStaticName() const{
  LOG_DEBUG("getStaticName");

  if( GetParent()==0 || GetParent()->GetParent()==0 ){
    return getName();
  }
  return GetParent()->getStaticName() + std::string(".") + getName();
}

/* --------------------------------------------------------------------------- */
/* getStaticName --                                                            */
/* --------------------------------------------------------------------------- */

void DataDictionary::getStaticName( std::string &name ) const{
  LOG_DEBUG("getStaticName");

  if( GetParent()==0 || GetParent()->GetParent()==0 ){
    name = m_Name;
  }
  else{
    GetParent()->getStaticName( name );
    name +="."+m_Name;
  }
}

/* --------------------------------------------------------------------------- */
/* GetFirstUserEntry --                                                        */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionary::GetFirstUserEntry() const{
  LOG_DEBUG("GetFirstUserEntry");

  if( hasParentNode() ){
    return parent().getStructure();
  }

  if( getDataType() == type_StructDefinition ){
    /***************************************************/
    /* Wir deuten auf das Item im DataPool.            */
    /* Achtung: Im Falle, dass der Dict Leer ist, wird */
    /*          der thispointer geliefert.             */
    /***************************************************/
    assert( getNext() != 0 );
    assert( getNext()->getDataType() == type_StructVariable );
    return hasStructureNode() ? getStructure() : const_cast<DataDictionary *>(this);
  }
  else{
    /***************************************************/
    /* Wir deuten auf das dem Datapool-Item angehaengte*/
    /* Root-Data-Item.                                 */
    /* Hier pointen wir auf das Rootitem und machen    */
    /* damit einen recursiven Aufruf.                  */
    /***************************************************/
    assert( getNext() == 0 );
    assert( structure().getDataType() == type_StructDefinition);
    return structure().GetFirstUserEntry();
  }
}

/* --------------------------------------------------------------------------- */
/* GetRootDictionary --                                                        */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::GetRootDictionary(){
  LOG_DEBUG("GetRootDictionary");

  DataDictionary * pDict = this;

  while (pDict->hasParentNode() ){
    pDict = pDict->GetParent();
  }
  assert( pDict != 0 );

  if( pDict->getDataType() == type_StructVariable){
    /*********************************************/
    /* Wir sind auf der Root Dictionary Variable */
    /* Wir muessen aber auf die root dictionary- */
    /* definition.                               */
    /*********************************************/
    pDict = pDict->getStructure();
    assert( pDict != 0 );
  }
  assert( pDict->getDataType() == type_StructDefinition );
  return pDict;
}

/* --------------------------------------------------------------------------- */
/* GetDataPool --                                                              */
/* --------------------------------------------------------------------------- */

DataPool * DataDictionary::GetDataPool(){
  LOG_DEBUG("GetDataPool");

  DataDictionary * pDict     = GetRootDictionary();
  DataDictionaryRoot * pRoot = static_cast<DataDictionaryRoot *>(pDict);
  return pRoot->GetDataPool();
}

/* --------------------------------------------------------------------------- */
/* DataPoolAllocated --                                                        */
/* --------------------------------------------------------------------------- */

bool DataDictionary::DataPoolAllocated(){
  LOG_DEBUG("DataPoolAllocated");
  return GetDataPool()->DataPoolAllocated();
}

/* --------------------------------------------------------------------------- */
/* isValidNameChar --                                                          */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isValidNameChar( char c ){
  return (isalnum(c) || c == '_' || c == '@' || c == '#');
}

/* --------------------------------------------------------------------------- */
/* isSameDataType --                                                           */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isSameDataType( DataDictionary &dict ) const{
  LOG_DEBUG(  "this=" << m_Name << " other=" << dict.m_Name );

  if( getDataType() != dict.getDataType() ){
    LOG_DEBUG( "rslt = false. DataType not equal");
    return false;
  }
  if( getDataType() != type_StructVariable ){
    LOG_DEBUG("rslt = true. DataType is same but no struct");
    return true;
  }
  if( getStructure() == dict.getStructure() ){
    LOG_DEBUG("rslt = true. Same structure-definition");
    return true;
  }
  LOG_DEBUG("rslt = false. Different Structur-Definitions");
  return false;
}

/* --------------------------------------------------------------------------- */
/* isSameDataTypeAndName --                                                    */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isSameDataTypeAndName( DataDictionary &dict ) const{
  if( m_Name != dict.m_Name ){
    return false;
  }
  return isSameDataType( dict );
}

/* --------------------------------------------------------------------------- */
/* SetDataTypeInvalid --                                                       */
/* --------------------------------------------------------------------------- */

void  DataDictionary::SetDataTypeInvalid(){
  LOG_DEBUG("Name=" << m_Name);

  if( m_DatenType == type_StructDefinition && m_pStruct != 0 ){
    delete m_pStruct;
    m_pStruct = 0;
  }
  m_Name = "$<Invalid>$" + m_Name;
  m_pStruct = 0;
  m_DatenType = type_Bad;
}


/* --------------------------------------------------------------------------- */
/* MarkNodeDeleted --                                                          */
/* --------------------------------------------------------------------------- */

void  DataDictionary::MarkNodeDeleted(){
  LOG_DEBUG("Name=" << m_Name);

  assert( m_DatenType != type_StructDefinition);

  m_Name            = "$$" + m_Name;
  m_pStruct         = 0;
  setItemGlobal( false );
  setItemInternal( false );
  setItemProtected( false );
  m_DatenType       = type_None;
  ClearDefaultDimension();
}

/* --------------------------------------------------------------------------- */
/* IsNodeDeleted --                                                            */
/* --------------------------------------------------------------------------- */

bool DataDictionary::IsNodeDeleted() const{
  LOG_DEBUG("Name=" << m_Name);

  bool rslt = getDataType()         == type_None   &&
              !ItemIsGlobal()                      &&
              getName().size()      >= 2           &&
              getName().substr(0,2) == std::string("$$")  ;

  LOG_DEBUG("rslt=" << rslt);
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* SearchDeletedNode --                                                        */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::SearchDeletedNode() const{
   LOG_DEBUG("SearchDeletedNode");

   DataDictionary * pDict = GetFirstUserEntry();

   assert(pDict != 0);

  do {
    if( pDict->IsNodeDeleted() ){
      LOG_DEBUG("Fond rslt=" << pDict);
      return  pDict;
    }
    pDict = pDict->getNext();
  }
  while( pDict != 0 );

  LOG_DEBUG("NOTFOUND rslt=0");
  return 0;
}

/* --------------------------------------------------------------------------- */
/* ReuseNode --                                                                */
/* --------------------------------------------------------------------------- */

void DataDictionary::ReuseNode( const std::string &Name,
                                DataType Type,
                                DataDictionary *StructRef,
                                bool ItemIsGlobal,
                                bool ItemIsInternal )
{
  LOG_DEBUG( "OldName=" << m_Name << " NewName=" << Name <<
             " Typ=" << Type << " ItemIsGlobal=" << ItemIsGlobal );

  assert(IsValidType(Type));
  assert(m_DatenType == type_None);
  assert(Name.size() > 0);
  assert(Type == type_StructVariable   ? StructRef != 0
                                       : StructRef == 0);
  assert(Type == type_StructDefinition ? !ItemIsGlobal
                                       : true          );
  assert(m_pAttributes != 0);

  if( isInternalName( Name ) ){
    ItemIsInternal = true;
  }

  m_Name         = Name;
  m_DatenType    = Type;
  m_pStruct      = StructRef;
  setItemGlobal( ItemIsGlobal );
  setItemInternal( ItemIsInternal );

  // Wir werden den Index auch wiederverwenden. Dies ist jedoch ein zweischneidiges Schwert:
  // Datenreferenzen, die auf ein geloeschtes Objekt deuten werden damit ploetzlich wieder
  // gueltig. Auf der anderen Seite kann dies eine recht substanzielle Memoryeinsparung bringen
  // besonders bei Arrays von Strukturen.

  if( TypeNeedsAlloc(Type) ){
    if( m_ItemIndex == InvalidIndex ){
      m_ItemIndex = GetHighestIndex()+1;
    }
  }
  else{
    m_ItemIndex = InvalidIndex;  // Wir brauchen das nicht.
  }

  // Um sicher zu gehen, muessen wir auch noch neue Userdata produzieren und die alten
  // dabei auch noch los werden. Etwas umstaendlich, aber ich muss dies so machen, da die
  // Userdaten vom Dictionary-Type abhaengen.

  DataUserAttr *pAttr = m_pAttributes;
  m_pAttributes = 0;
  AllocAttr(pAttr);
  delete pAttr;

  return;
}

/* --------------------------------------------------------------------------- */
/* isValidNameString --                                                        */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isValidNameString(const char * pt, int len)
{
  if (len == 0 && pt!=0)
      len=strlen(pt);
   while(len-- > 0)
     {if (!isValidNameChar(*pt))
          return false;
      ++pt;
     }
   return true;
}

/* --------------------------------------------------------------------------- */
/* isValidNameString --                                                        */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isValidNameString( const std::string &name ){
  return isValidNameString( name.c_str(), name.length() );
}

/* --------------------------------------------------------------------------- */
/* PrintDataDictionary --                                                      */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionary::PrintDataDictionary( int Level ){
  LOG_DEBUG("Level=" << Level);

  int j=0;
  DataDictionary *pD;
  std::string Attr;

  if (ItemIsGlobal())
    if (IsNodeDeleted())    Attr = "[Global,Deleted]"  ;
    else                    Attr = "[Global]"          ;
  else
    if (IsNodeDeleted())    Attr = "[Deleted]"         ;
    else                    Attr = ""                  ;

  while(j++ < Level)
    putchar(' ');

  switch( getDataType() ){
  case type_None:
    printf("<none>          %s%s;\n",getName().c_str(),Attr.c_str());
    break;
  case type_Integer:
    printf("int             %s%s;\n",getName().c_str(),Attr.c_str());
    break;
  case type_Real:
    printf("real            %s%s;\n",getName().c_str(),Attr.c_str());
    break;
  case type_String:
    printf("std::string     %s%s;\n",getName().c_str(),Attr.c_str());
    break;
  case type_Complex:
    printf("complex         %s%s;\n",getName().c_str(),Attr.c_str());
    break;
  case type_StructVariable:
    printf("%-15s %s%s;\n",structure().getName().c_str(), getName().c_str(),Attr.c_str());
    break;
  case type_StructDefinition:
    printf("struct          %s   %s{\n",getName().c_str(),Attr.c_str());
    pD = getStructure();
    while (pD)
      pD = pD->PrintDataDictionary(Level+2);
    j=0;
    while(j++ < Level)
      putchar(' ');
    puts("  };\n");
    break;
  case type_Bad:
    printf("<bad>          %s%s;\n",getName().c_str(),Attr.c_str());
    break;
  default:
    printf("<unknown>      %s%s;\n",getName().c_str(),Attr.c_str());
    break;
  }
  return getNext();
}

/* --------------------------------------------------------------------------- */
/* copyDataDictionary --                                                       */
/* --------------------------------------------------------------------------- */

void DataDictionary::copyDataDictionary( const DataDictionary &dict ) {
  LOG_DEBUG(  "copyDataDictionary" );

  assert( dict.m_DatenType == type_StructDefinition );

  DataDictionary *src  = dict.getStructure();
  DataDictionary *dest = 0;

  if( m_DatenType == type_StructDefinition ){
    dest = AddFirstMember( *src );
  }
  else{
    dest = AddNextItem( *src );
  }

  while( (src=src->getNext()) != 0 ){
    dest = dest->AddNextItem( *src );
  }
}

/* --------------------------------------------------------------------------- */
/* hasSortKey --                                                               */
/* --------------------------------------------------------------------------- */

bool DataDictionary::hasSortKey() const{
  if( m_DatenType != type_StructDefinition ){
    return false;
  }
  DataDictionary *str = getStructure();
  while( str != 0 ){
    if( str->isSortKey() ){
      return true;
    }
    str = str->getNext();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isSortKey --                                                                */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isSortKey() const{
  if( m_pAttributes != 0 ){
    return m_pAttributes->isSortKey();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* writeDD --                                                                  */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionary::writeDD( std::ostream &ostr, int i ){
  indent( i, ostr ) << "<ITEM name=\"" << getName() << "\"";

  ostr << " idnr=\"" << GetItemIndex() << "\"";
  ostr << " type=\"" << getDataType() << "\"";
  ostr << " line=\"" << LSPLineno() << "\"";
  auto filename = LSPFilename();
  std::replace(filename.begin(), filename.end(), '"', '\'');
  ostr << " file=\"" << filename << "\"";
  if( ItemIsGlobal() ){
    ostr << " global=\"1\"";
  }
  if( ItemIsInternal() ){
    ostr << " internal=\"1\"";
  }
  if( IsNodeDeleted() ){
    ostr << " del=\"1\"";
  }
  if( isParentNodeRoot() ){
    ostr << " zero=\"1\"";
  }
  ostr << " this=\"" << this << "\"";
  ostr << " parent=\"" << this->m_pParentNode << "\"";
  ostr << " next=\"" << this->m_pNext << "\"";

  const DataUserAttr *userattr = getDataUserAttr();
  DataAttributes attr( userattr != 0 ? userattr->getAttributes() : 0 );
  attr.writeDPattributeBits( ostr );

  if( getDataType() == type_StructDefinition ){
    ostr << ">" << std::endl;
    DataDictionary *dict = getStructure();
    while( dict ){
      dict = dict->writeDD( ostr, i+2 );
    }
    indent( i, ostr ) << "</ITEM>" << std::endl;
  }
  else{
    if( getDataType() == type_StructVariable ){
      if( hasStructureNode() ){
        ostr << " struct=\"" << structure().getName() << "\"";
      }
    }
    ostr << "/>" << std::endl;
  }

  return getNext();
}

/* --------------------------------------------------------------------------- */
/* writeDD --                                                                  */
/* --------------------------------------------------------------------------- */

void DataDictionary::writeDD( std::ostream &ostr ){
  DataDictionary *dict = GetFirstUserEntry();
  assert( dict != 0 );

  do {
    dict = dict->writeDD( ostr, 0 );
  }
  while ( dict != 0 );
}

/* --------------------------------------------------------------------------- */
/* isInternalName --                                                           */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isInternalName() const{
  return m_Name.find_first_of( '@' ) != std::string::npos;
}

/* --------------------------------------------------------------------------- */
/* isInternalName --                                                           */
/* --------------------------------------------------------------------------- */

bool DataDictionary::isInternalName( const std::string &name ){
  return name.find_first_of( '@' ) != std::string::npos;
}
