
#include <stdio.h> // for sprintf
#include <iostream>
#include <iomanip>

#include "utils/utils.h"
#include "datapool/DataException.h"
#include "datapool/DataReference.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataIndices.h"
#include "datapool/DataInx.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataStructElement.h"
#include "datapool/DataVector.h"
#include "datapool/DataMatrix.h"
#include "datapool/DataAlterGetItem.h"
#include "datapool/DataAlterGetItemAttr.h"
#include "datapool/DataAlterGetItemUpdated.h"
#include "datapool/DataAlterGetElement.h"
#include "datapool/DataAlterGetElementUpdated.h"
#include "datapool/DataAlterGetDimensionInfo.h"
#include "datapool/DataAlterSetIntegerElement.h"
#include "datapool/DataAlterSetRealElement.h"
#include "datapool/DataAlterSetComplexElement.h"
#include "datapool/DataAlterSetStringElement.h"
#include "datapool/DataAlterClearDataItem.h"
#include "datapool/DataAlterClearElement.h"
#include "datapool/DataAlterLockElement.h"
#include "datapool/DataAlterAssignElement.h"
#include "datapool/DataAlterAssignDataItem.h"
#include "datapool/DataAlterAssignCorrDataItem.h"
#include "datapool/DataAlterEraseDataItem.h"
#include "datapool/DataAlterEraseElement.h"
#include "datapool/DataAlterSetTimestamp.h"
#include "datapool/DataAlterSetStylesheet.h"
#include "datapool/DataAlterGetStylesheet.h"
#include "datapool/DataAlterGetVector.h"
#include "datapool/DataAlterSetVector.h"
#include "datapool/DataAlterGetMatrix.h"
#include "datapool/DataAlterSetMatrix.h"
#include "datapool/DataAlterGetAttributes.h"
#include "datapool/DataAlterSetAttributes.h"
#include "datapool/DataAlterWriteJSON.h"
#include "datapool/DataAlterCompare.h"
#include "datapool/DataEvents.h"
#include "datapool/DataLogger.h"

static int offset = 1;

INIT_LOGGER();

/*=============================================================================*/
/* Initialisation							       */
/*=============================================================================*/

BUG_INIT_COUNT(DataReference);

/*=============================================================================*/
/* Constructor / Destructor						       */
/*=============================================================================*/

DataReference::DataReference( const char *addr )
  : m_cyclenum( -1 )
  , m_ref_valid( true )
  , m_dict( 0 )
  , m_indices( new DataIndices() ){
  BUG_INCR_COUNT;
  BUG( BugRef, "Constructor" );

  // Jede Referenz beginnt mit der untersten Struktur.
  m_dict = datapool().GetRootDictVar();
  assert( m_dict != 0 );

  if( !ParseSymbolicAdress( addr ) ){
    BUG_EXIT( "ParseSymbolicAdress failed" );
  }
  AssertReference();
}


DataReference::DataReference( const DataReference &ref, const char * addr )
  : m_cyclenum( ref.m_cyclenum )
  , m_ref_valid( ref.m_ref_valid )
  , m_dict( ref.m_dict )
  , m_indices( 0 ){
  BUG_INCR_COUNT;
  BUG( BugRef, "Copy-Constructor" );

  assert( hasDictionary() );
  copyIndices( ref );

  BUG_MSG( "Name: " << fullName() );
  if( addr != 0 ){
    BUG_MSG( "addr: " << std::string( addr ) );
  }

  if( isntValid() ){ // Referenz ist nicht gültig
    AssertReference();
    BUG_EXIT("Invalid Reference");
    return;
  }

  if( addr != 0 && strlen(addr) > 0 ){
    if( dictionary().getDataType() != DataDictionary::type_StructVariable ){
      // Nur Strukturvariablen können eine Adresse haben.
      // Die Referenz ist anschliessen ungültig.
      m_ref_valid = false;
      AssertReference();
      BUG_EXIT("Invalid Reference. SourceRef must be a Structure");
      return;
    }

    assert( dictionary().hasStructureNode() );
    assert( dictionary().structure().hasStructureNode() );

    setDictionary( dictionary().structure().getStructure() ); // Erstes Item der Struktur-Definition
    if( !ParseSymbolicAdress( addr ) ){
      BUG_EXIT( "ParseSymbolicAdress failed" );
    }
  }
  AssertReference();
}

DataReference::~DataReference(){
  if( m_indices != 0 ){
    delete m_indices;
    m_indices = 0;
  }
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* Member Functions							       */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* newDataReference --							       */
/* --------------------------------------------------------------------------- */

DataReference *DataReference::newDataReference( const char *addr ){
  BUG( BugRef, "newDataReference" );

  return new DataReference( addr );
}

/* --------------------------------------------------------------------------- */
/* newDataReference --							       */
/* --------------------------------------------------------------------------- */

DataReference *DataReference::newDataReference( const DataReference &ref, const char *addr ){
  BUG( BugRef, "newDataReference (Copy)" );

  return new DataReference( ref, addr );
}

/* --------------------------------------------------------------------------- */
/* setCycleNumber --							       */
/* --------------------------------------------------------------------------- */

void DataReference::setCycleNumber( int cyclenum ){
  assert( cyclenum >= 0 );
  m_cyclenum = cyclenum;
}

/* --------------------------------------------------------------------------- */
/* initCycleNumber --							       */
/* --------------------------------------------------------------------------- */

void DataReference::initCycleNumber(){
  m_cyclenum = -1;
}

/* --------------------------------------------------------------------------- */
/* isValid --								       */
/* --------------------------------------------------------------------------- */

bool DataReference::isValid() const{
  return m_ref_valid;
}

/* --------------------------------------------------------------------------- */
/* isntValid --								       */
/* --------------------------------------------------------------------------- */

bool DataReference::isntValid() const{
  return !m_ref_valid;
}

/* --------------------------------------------------------------------------- */
/* datapool --								       */
/* --------------------------------------------------------------------------- */

DataPool &DataReference::datapool() const{
  return DataPool::getDatapool();
}

/* --------------------------------------------------------------------------- */
/* hasDictionary --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::hasDictionary() const{
  return m_dict != 0;
}

/* --------------------------------------------------------------------------- */
/* setDictionary --							       */
/* --------------------------------------------------------------------------- */

void DataReference::setDictionary( DataDictionary *dict ){
  m_dict = dict;
}

/* --------------------------------------------------------------------------- */
/* dictionary --							       */
/* --------------------------------------------------------------------------- */

DataDictionary &DataReference::dictionary() const{
  assert( m_dict );
  return *m_dict;
}

/* --------------------------------------------------------------------------- */
/* GetDict --								       */
/* --------------------------------------------------------------------------- */

DataDictionary *DataReference::GetDict() const{
  assert( m_dict );
  return m_dict;
}

/* --------------------------------------------------------------------------- */
/* getUserAttr --							       */
/* --------------------------------------------------------------------------- */

DataUserAttr *DataReference::getUserAttr() const{
  return isValid() ? dictionary().GetAttr() : 0;
}

/* --------------------------------------------------------------------------- */
/* userAttr --								       */
/* --------------------------------------------------------------------------- */

DataUserAttr &DataReference::userAttr() const{
  assert( isValid() );
  return *dictionary().GetAttr();
}

/* --------------------------------------------------------------------------- */
/* getUserDataAttributes --						       */
/* --------------------------------------------------------------------------- */

DataAttributes DataReference::getUserDataAttributes() const{
  assert( isValid() );
  return indices().getUserDataAttributes( datapool() );
}

/* --------------------------------------------------------------------------- */
/* getDataType --							       */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataReference::getDataType() const{
  return isValid() ? dictionary().getDataType() : DataDictionary::type_None;
}

/* --------------------------------------------------------------------------- */
/* isSameDataType --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::isSameDataType( DataReference *ref ) const{
  if( isValid() && ref->isValid() ){
    return dictionary().isSameDataType( ref->dictionary() );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* dbAttr --                                								                   */
/* --------------------------------------------------------------------------- */

std::string DataReference::dbAttr() const{
  if(!hasDictionary()) {
    return "";
  }

  const UserAttr *attr = dynamic_cast<const UserAttr *>(dictionary().getDataUserAttr());
  if(attr == 0) {
    return "";
  }

  return attr->DbAttr();
}


/* --------------------------------------------------------------------------- */
/* nodeName --								       */
/* --------------------------------------------------------------------------- */

std::string DataReference::nodeName() const{
  if( hasDictionary() ){
    return dictionary().getName();
  }
  return std::string("");
}

/* --------------------------------------------------------------------------- */
/* getNodeName --							       */
/* --------------------------------------------------------------------------- */

void DataReference::getNodeName( std::string &Name ) const{
  if( hasDictionary() ){
    dictionary().getName( Name );
  }
  else{
    Name = "";
  }
}

/* --------------------------------------------------------------------------- */
/* staticName --							       */
/* --------------------------------------------------------------------------- */

std::string DataReference::staticName() const{
  if( hasDictionary() ){
    return dictionary().getStaticName();
  }
  return std::string("");
}

/* --------------------------------------------------------------------------- */
/* getStaticName --							       */
/* --------------------------------------------------------------------------- */

void DataReference::getStaticName( std::string &Name ) const{
  if( hasDictionary() ){
    dictionary().getStaticName( Name );
  }
  else{
    Name = "";
  }
}

/* --------------------------------------------------------------------------- */
/* fullName --								       */
/* --------------------------------------------------------------------------- */

std::string DataReference::fullName( bool WithIndizes ) const{
  std::string s;
  getFullName( s, WithIndizes );
  return s;
}

/* --------------------------------------------------------------------------- */
/* getFullName --							       */
/* --------------------------------------------------------------------------- */

void DataReference::getFullName( std::string &name, bool with_indizes ) const{
  AssertReference();

  name.clear();
  DataDictionary *dict = datapool().GetRootDictVar();
  name = indices().print( dict, with_indizes );
}

/* --------------------------------------------------------------------------- */
/* printFullName --							       */
/* --------------------------------------------------------------------------- */

void DataReference::printFullName( std::ostream &ostr, bool with_indizes ) const{
  AssertReference();

  if( isntValid() ){
    ostr << "<invalid ref>";
    return;
  }

  DataDictionary *dict = datapool().GetRootDictVar();
  indices().print( ostr, dict, with_indizes );
}

/*******************************************************************************/
/* Funktionen für die Bearbeitung einer Addresse mit Indizes		       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* ParseSymbolicAdress --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::ParseSymbolicAdress( const char * Addr ){
  BUG_PARA( BugRef, "ParseSymbolicAdress", std::string(Addr) );

  LOG_DEBUG( "Addr=[" << BugCharPtr(Addr) << "]");

  if ( Addr == 0 ){
    LOG_DEBUG("rslt= false null string");
    return m_ref_valid = false;
  }
  else if( strlen(Addr) == 0 ){
    LOG_DEBUG("rslt=false string is empty");
    return m_ref_valid = false;
  }
  else if( !(m_ref_valid = ParseAddr( Addr, m_dict ) ) ){
    m_dict = datapool().GetRootDictVar();
  }
  LOG_DEBUG("rslt=" << m_ref_valid);
  AssertReference();
  return m_ref_valid;
}

/* --------------------------------------------------------------------------- */
/* ParseAddr --								       */
/* --------------------------------------------------------------------------- */

bool DataReference::ParseAddr(const char * & Addr, DataDictionary * pDict){
  BUG( BugRef, "ParseAddr" );

  while (isspace(*Addr))  Addr++;

  const char * pName = Addr;
  int	       NameLen = 0;

  while ( DataDictionary::isValidNameChar(*Addr) ){
    NameLen++;
    Addr++;
  }

  BUG_MSG( "Recognized Name=[" << pName << "] Len=" << NameLen );

  // Name has been parsed. Check, if it really exists
  // ************************************************

  assert(pDict != 0);
  DataDictionary *pItemDict = pDict->SearchByName( pName,NameLen );
  if ( pItemDict == 0 ){
    BUG_EXIT( "Item " << std::string(pName, NameLen) << " not in dictionary of "
	      <<  pDict->getName() );
    return false;
  }

  BUG_MSG( "Item " << pItemDict->getName() << " found in dictionary of " << pDict->getName() );

  if( !DataDictionary::TypeNeedsAlloc( pItemDict->getDataType() ) ){
    BUG_EXIT( "rslt=false  Name=[" << std::string(pName, NameLen)
	      << "] Dataref not possible on Structdefinition");
    return false;
  }
  addItemToIndizes( pItemDict );
  BUG_MSG( "ok");

  // Parse Index-List
  // ****************

  while (isspace(*Addr))  Addr++;
  if ( !ParseIndexList(Addr) ){
    LOG_DEBUG("rslt = false (error in Indexliste)");
    return false;
  }
  BUG_MSG( "indexlist ok");

  // Check for subnodes
  // ******************

  while (isspace(*Addr))  Addr++;
  if ( *Addr == '.' ){
    // Dies muss ein Subnode sein. Wenn nicht, ist dies ein Adressierungsfehler.			    */
    // *************************************************************************

    if( pItemDict->getDataType() != DataDictionary::type_StructVariable ){
      // Falscher Dictionary-Typ. Kein Glueck gehabt.
      LOG_DEBUG("rslt = false error: no Subnode in Dictionary");
      return false;
    }

    Addr++;  // ueberspringe Typ
    assert( pItemDict->hasStructureNode() );
    assert( pItemDict->getStructure()->hasStructureNode() );

    bool rslt = ParseAddr( Addr, pItemDict->getStructure()->getStructure() );
    LOG_DEBUG("rslt=" << rslt << "  (after Recursion)");
    return rslt;
  }
  else{
    m_dict = pItemDict;
  }

  BUG_MSG("rslt=" << (bool)(*Addr == 0));

  AssertReference();
  return (*Addr == 0);
}

/* --------------------------------------------------------------------------- */
/* ParseIndexList --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::ParseIndexList( const char * & Addr ){
  LOG_DEBUG("DataReference::ParseIndexList Addr=[" <<
	    BugCharPtr(Addr) << "]");

  while( isspace(*Addr) ) Addr++;

  if( *Addr != '[' ){
    LOG_DEBUG("rslt=true (no IndexList found)");
    return true; // Dies is OK weil es auch etwas andres sein kann
  }

  Addr++; // Skip '['
  while( isspace(*Addr) ) Addr++;

  if( *Addr == ']' ){
    Addr++;  // Skip ']'
    LOG_DEBUG("rslt=true (empty IndexList found)");
    return true; // Dies is OK.
  }

  while( true ){
    int NewIndex = 0;
    while( isspace(*Addr) ) Addr++;

    if( !isdigit(*Addr) ){
      LOG_DEBUG("rslt=false (digit expected)");
      return false; // Hier sollte schon eine Zahl kommen, sonst ist was faul
    }
    while( isdigit(*Addr) ){
      NewIndex=NewIndex*10 + (*Addr -'0');
      Addr++;
    }

    // Hier haben wir eine Zahl erfolgreich geparsed.
    // Diese wird nun in die Dimensionsliste eingefuegt.
    addDimensionToIndizes(NewIndex);

    // Jetzt muessen wir schauen, ob es die letzte war.
    while( isspace(*Addr) ) Addr++;
    if( *Addr == ',' ){
      Addr++;
      continue;
    }

    // Ende der Fahnenstange. Keine Zahlen mehr. Wir brechen
    // die Schlaufe ab.
    break;
  }

  if( *Addr != ']' ){
    LOG_DEBUG("rslt=false (closing  ']' expected)");
    return false;
  }

  Addr++;

  LOG_DEBUG("rslt=true");
  return true;
}

/* --------------------------------------------------------------------------- */
/* addItemToIndizes --							       */
/* --------------------------------------------------------------------------- */

void DataReference::addItemToIndizes( DataDictionary *dict ){
  BUG( BugRef, "addItemToIndizes" );
  assert( isValid() );
  indices().addIndexList( dict->GetItemIndex() );
  AssertReference();
  BUG_EXIT( "Item '" << fullName(true) << "' added" );
}

/* --------------------------------------------------------------------------- */
/* addDimensionToIndizes --						       */
/* --------------------------------------------------------------------------- */

void DataReference::addDimensionToIndizes( int index ){
  BUG( BugRef, "addDimensionToIndizes" );
  assert( isValid() );
  indices().addIndex( index );
  AssertReference();
  BUG_EXIT( "Item '" << fullName(true) << "' added" );
}

/*******************************************************************************/
/* Funktionen für Bearbeitung der Indexliste				       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* copyIndices --							       */
/* --------------------------------------------------------------------------- */

void DataReference::copyIndices( const DataReference &ref ){
  if( m_indices != 0 ){
    delete m_indices;
    m_indices = 0;
  }
  m_indices = new DataIndices( ref.indices() );
}

/* --------------------------------------------------------------------------- */
/* indices --								       */
/* --------------------------------------------------------------------------- */

DataIndices &DataReference::indices() const{
  assert( m_indices != 0 );
  return *m_indices;
}

/* --------------------------------------------------------------------------- */
/* clearDimensionIndizes --						       */
/* --------------------------------------------------------------------------- */

void DataReference::ClearDimensionIndizes(){
  BUG( BugRef, "ClearDimensionIndizes" );

  assert( isValid() );
  indices().clearIndices();
  AssertReference();
}

/* --------------------------------------------------------------------------- */
/* setDimensionIndizes --						       */
/* --------------------------------------------------------------------------- */

void DataReference::SetDimensionIndizes( int num_inx, const int *inx ){
  BUG( BugRef, "setDimensionIndizes" );

  assert( isValid() );
  indices().indexList().setIndexList( num_inx, inx );
  AssertReference();
}

/* --------------------------------------------------------------------------- */
/* setDimensionIndizes --						       */
/* --------------------------------------------------------------------------- */

void DataReference::SetDimensionIndizes( int level, int num_inx, const int *inx ){
  BUG_PARA( BugRef, "setDimensionIndizes", "level=" << level );

  assert( isValid() );
  indices().indexList( level ).setIndexList( num_inx, inx );
  AssertReference();
}

/* --------------------------------------------------------------------------- */
/* setDimensionIndizes --						       */
/* --------------------------------------------------------------------------- */

void DataReference::SetDimensionIndizes( int level, const DataIndexList &indexlist ){
  BUG_PARA( BugRef, "setDimensionIndizes", "level=" << level );

  assert( isValid() );
  indices().indexList( level ).setIndexList( indexlist );
  AssertReference();
}

/* --------------------------------------------------------------------------- */
/* getDimensionIndizes --						       */
/* --------------------------------------------------------------------------- */

const DataIndexList &DataReference::getDimensionIndizes( int level ) const{
  BUG_PARA( BugRef, "getDimensionIndizes", "level=" << level );

  assert( isValid() );
  AssertReference();
  return indices().indexList( level );
}

/* --------------------------------------------------------------------------- */
/* getNumberOfLevels --							       */
/* --------------------------------------------------------------------------- */

int DataReference::GetNumberOfLevels() const{
  AssertReference();
  assert( isValid() );
  return indices().numberOfLevels();
}

/* --------------------------------------------------------------------------- */
/* getNumberOfDimensionIndizes --					       */
/* --------------------------------------------------------------------------- */

int DataReference::GetNumberOfDimensionIndizes( int level ) const{
  AssertReference();
  assert( isValid() );
  return indices().indexList( level ).getDimensions();
}

/* --------------------------------------------------------------------------- */
/* getDimensionIndexOfLevel --						       */
/* --------------------------------------------------------------------------- */

int DataReference::GetDimensionIndexOfLevel( int level, int index ) const{
  AssertReference();
  assert( isValid() );
  return indices().indexList( level ).index( index ).getIndex();
}

/* --------------------------------------------------------------------------- */
/* setDimensionIndexOfLevel --						       */
/* --------------------------------------------------------------------------- */

void DataReference::SetDimensionIndexOfLevel( int val, int level, int index ){
  BUG_PARA( BugRef, "setDimensionIndexOfLevel"
	  , "val=" << val << ", level=" << level << ", index=" << index );

  AssertReference();
  assert( isValid() );

  DataIndexList &inxlist = indices().indexList( level );
  if( inxlist.getDimensions() < index + 1 ){
    inxlist.resizeList( index + 1 );
  }
  if( val == WildcardIndex ){
    inxlist.index( index ).setWildcard();
  }
  else{
    inxlist.index( index ).setIndex( val );
  }
}

/* --------------------------------------------------------------------------- */
/* SetDimensionIndex --							       */
/* --------------------------------------------------------------------------- */

void DataReference::SetDimensionIndex( int val ){
  BUG_PARA( BugRef, "setDimensionIndex", "val=" << val );

  AssertReference();
  assert( isValid() );

  DataIndexList &inxlist = indices().indexList();
  if( inxlist.getDimensions() < 1 ){
    inxlist.resizeList( 1 );
  }
  if( val == WildcardIndex ){
    inxlist.index( 0 ).setWildcard();
  }
  else{
    inxlist.index( 0 ).setIndex( val );
  }
}

/* --------------------------------------------------------------------------- */
/* convertToParentReference --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::ConvertToParentReference(){
  assert( isValid() );
  AssertReference();

  if( indices().numberOfLevels() < 2 ){
    return false; // Konversion nicht möglich
  }

  indices().deleteLastLevel();
  m_dict = indices().getDictionary( datapool() );
  return true;
}

/* --------------------------------------------------------------------------- */
/* FirstWildcardIndex --						       */
/* --------------------------------------------------------------------------- */

int DataReference::FirstWildcardIndex( int NumIndizes, const int *Indizes ){
  for( int i = 0; i < NumIndizes; i++ ){
    if( Indizes[i] == WildcardIndex ){
      return i;
    }
  }
  return -1;
}

/* --------------------------------------------------------------------------- */
/* FirstWildcardIndex --						       */
/* --------------------------------------------------------------------------- */

int DataReference::FirstWildcardIndex( const int *Indizes ){
  return FirstWildcardIndex( Indizes[1], Indizes+2 );
}

/* --------------------------------------------------------------------------- */
/* LastWildcardIndex --							       */
/* --------------------------------------------------------------------------- */

int DataReference::LastWildcardIndex( int NumIndizes, const int *Indizes ){
  for( int i = NumIndizes-1; i > 0; i-- ){
    if( Indizes[i] == WildcardIndex ){
      return i;
    }
  }
  return -1;
}

/* --------------------------------------------------------------------------- */
/* LastWildcardIndex --							       */
/* --------------------------------------------------------------------------- */

int DataReference::LastWildcardIndex( const int *Indizes ){
  return LastWildcardIndex( Indizes[1], Indizes+2 );
}


/* --------------------------------------------------------------------------- */
/* NumberOfWildcardIndex --						       */
/* --------------------------------------------------------------------------- */

int DataReference::NumberOfWildcardIndex( int NumIndizes, const int *Indizes ){
  int j=0;
  for( int i = 0; i < NumIndizes; i++ ){
    if( Indizes[i] == WildcardIndex ){
      j++;
    }
  }
  return j;
}

/* --------------------------------------------------------------------------- */
/* NumberOfWildcardIndex --						       */
/* --------------------------------------------------------------------------- */

int DataReference::NumberOfWildcardIndex(const int *Indizes){
  return NumberOfWildcardIndex( Indizes[1], Indizes+2 );
}

/* --------------------------------------------------------------------------- */
/* WildcardInIndexList --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::WildcardInIndexList( int NumIndizes, const int *Indizes ){
  return FirstWildcardIndex( NumIndizes, Indizes ) >= 0;
}

/* --------------------------------------------------------------------------- */
/* WildcardInIndexList --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::WildcardInIndexList( const int *Indizes ){
  return FirstWildcardIndex( Indizes[1], Indizes+2 ) >= 0;
}

/* --------------------------------------------------------------------------- */
/* IndexListIsOnlyWildcards --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::IndexListIsOnlyWildcards( int NumIndizes, const int *Indizes ){
  for( int i = 0; i < NumIndizes; i++ ){
    if( *Indizes != WildcardIndex ){
      return false;
    }
    Indizes++;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* IndexListIsOnlyWildcards --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::IndexListIsOnlyWildcards( const int *Indizes ){
  return IndexListIsOnlyWildcards( Indizes[1], Indizes+2 );
}

/* --------------------------------------------------------------------------- */
/* IndexListsAreEqual --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::IndexListsAreEqual(const int *Inx1, const int *Inx2){
  bool rslt;
  LOG_DEBUG("IndexListsAreEqual");

  if( *Inx1 != *Inx2 ){
    LOG_DEBUG("rslt false (other Item adressed)");
    return false;
  }

  int Len1= *(Inx1+1);	  // Laenge des ersten Index
  int Len2= *(Inx2+1);	  // Laenge des zweiten Index

  Inx1 += 2;		  // Start der eigentlichen Indexliste
  Inx2 += 2;		  // Start der eigentlichen Indexliste

  /******************************************************************/
  /* Zuerst der einfachste Fall probieren. Damit kann etwas Zeit    */
  /* gewonnen werden.						    */
  /******************************************************************/
  if( Len1 == Len2 ){
    rslt = (memcmp(Inx1, Inx2, sizeof(int)*Len1) == 0);
    LOG_DEBUG("rslt=" << rslt <<
	      "(before Elimination of leading zeros (Squash))");
    return rslt;
  }

  /******************************************************************/
  /* Unterschiedlich Anzahl Indizes. Dies koennte jedoch trotzdem   */
  /* noch ein match werden, das A[0,1] gleich A[1] ist. Ich werde   */
  /* jetzt versuchen, bei beiden alle fuehrenden Nullen zu elimi-   */
  /* nieren und den obigen Test wiederholen.			    */
  /******************************************************************/
  while( Len1 > 0 && *Inx1 == 0 ){
    Len1--;
    Inx1++;
  }
  while( Len2 > 0 && *Inx2 == 0 ){
    Len2--;
    Inx2++;
  }
  if( Len1 == Len2 ){
    rslt = (memcmp(Inx1, Inx2, sizeof(int)*Len1) == 0);
    LOG_DEBUG("rslt=" << rslt <<
	      "(after Elimination of leading zeros (Squash))");
    return rslt;
  }

  LOG_DEBUG("rslt=false");
  return false;
}

/* --------------------------------------------------------------------------- */
/* hasWildcardsNotLastLevel --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::hasWildcardsNotLastLevel(){
  return indices().hasWildcardsNotLastLevel();
}

/* --------------------------------------------------------------------------- */
/* hasWildcards --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::hasWildcards() const{
  return indices().hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* numberOfWildcards --							       */
/* --------------------------------------------------------------------------- */

int DataReference::numberOfWildcards() const{
  return indices().numberOfWildcards();
}

/* --------------------------------------------------------------------------- */
/* printIndexList --							       */
/* --------------------------------------------------------------------------- */

void DataReference::printIndexList( std::ostream &ostr, int NumIndizes, const int * Indizes ){
  ostr << "[";
  for( int i = 0; i < NumIndizes; i++ ){
    if( i>0 ) ostr << ",";
    if( Indizes[i] == WildcardIndex )
      ostr << "*";
    else
      ostr << Indizes[i];
  }
  ostr << "]";
}

/* --------------------------------------------------------------------------- */
/* getIndexList --							       */
/* --------------------------------------------------------------------------- */

std::string DataReference::getIndexList( int numIndizes, const int * Indizes ){
  std::ostringstream inxlist;
  printIndexList( inxlist, numIndizes, Indizes );
  return inxlist.str();
}

/*******************************************************************************/
/* Timestamp-Handling							       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::isDataItemUpdated( UpdateStatus status ){
  return isDataItemUpdated( status, DataPool::getTransactionNumber() );
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::isDataItemUpdated( UpdateStatus status, TransactionNumber t ){
  AssertReference();
  assert( isValid() );

  DataAlterGetItemUpdated get_updated( t );
  alterData( get_updated );
  return get_updated.isUpdated( status );
}

/* --------------------------------------------------------------------------- */
/* getDataItemUpdated --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataReference::getDataItemUpdated( UpdateStatus status ){
  AssertReference();
  assert( isValid() );

  DataAlterGetItemUpdated get_updated( DataPool::getTransactionNumber() );
  alterData( get_updated );
  return get_updated.getUpdated( status );
}

/* --------------------------------------------------------------------------- */
/* getDataItemValueUpdated --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataReference::getDataItemValueUpdated(){
  return getDataItemUpdated( ValueUpdated );
}

/* --------------------------------------------------------------------------- */
/* getDataItemLastRollback --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataReference::getDataItemLastRollback(){
  AssertReference();
  assert( isValid() );

  return getDataItem( m_indices->getBaseItemIdnr(), false ).getLastRollbackTimestamp();
}

/* --------------------------------------------------------------------------- */
/* isDataContainerUpdated --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::isDataContainerUpdated( UpdateStatus status, int num_inx, int *inx ){
  if( num_inx > 0 ){
    indices().indexList().setIndexList( num_inx, inx );
  }
  return isDataContainerUpdated_PreInx( status );
}

bool DataReference::isDataContainerUpdated( UpdateStatus status, int i ){
  int Inx[] = {i};
  return isDataContainerUpdated( status, 1, Inx );
}

/* --------------------------------------------------------------------------- */
/* isDataContainerUpdated --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::isDataContainerUpdated( UpdateStatus status, TransactionNumber t, int num_inx, int *inx ){
  if( num_inx > 0 ){
    indices().indexList().setIndexList( num_inx, inx );
  }
  return isDataContainerUpdated_PreInx( status, t );
}

bool DataReference::isDataContainerUpdated( UpdateStatus status, TransactionNumber t, int i ){
  int Inx[] = {i};
  return isDataContainerUpdated( status, t, 1, Inx );
}

/* --------------------------------------------------------------------------- */
/* isDataContainerUpdated_PreInx --					       */
/* --------------------------------------------------------------------------- */

bool DataReference::isDataContainerUpdated_PreInx( UpdateStatus status ){
  return isDataContainerUpdated_PreInx( status, DataPool::getTransactionNumber() );
}

bool DataReference::isDataContainerUpdated_PreInx( UpdateStatus status, TransactionNumber t ){
  DataAlterGetElementUpdated get_updated( t );
  alterData( get_updated );
  return get_updated.isUpdated( status );
}

/* --------------------------------------------------------------------------- */
/* getDataContainerUpdated --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataReference::getDataContainerUpdated( UpdateStatus status ){
  AssertReference();
  assert( isValid() );

  DataAlterGetElementUpdated get_updated( DataPool::getTransactionNumber() );
  alterData( get_updated );
  return get_updated.getUpdated( status );
}

/* --------------------------------------------------------------------------- */
/* getDataTimestamp --							       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataReference::getDataTimestamp( UpdateStatus status ){
  BUG_PARA( BugRef, "getDataTimestamp", fullName( true ) );

  DataAlterGetElementUpdated get_updated( DataPool::getTransactionNumber() );
  alterData( get_updated );
  return get_updated.getUpdated( status );
}

/* --------------------------------------------------------------------------- */
/* setDataTimestamp --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::setDataTimestamp( bool set, UpdateStatus status ){
  TransactionNumber t = set ? datapool().GetCurrentTransaction() : 0;
  DataAlterSetTimestamp timestamp( t, status, t > 0 );
  return alterData( timestamp );
}


/* --------------------------------------------------------------------------- */
/* setStylesheet --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::setStylesheet(const std::string& stylesheet) {
  DataAlterSetStylesheet sObj(stylesheet);
  return alterData(sObj);
}

/* --------------------------------------------------------------------------- */
/* getStylesheet --							       */
/* --------------------------------------------------------------------------- */

const std::string DataReference::getStylesheet(TransactionNumber t, bool& changed) {

  DataAlterGetStylesheet sObj(t);
  alterData(sObj);

  return sObj.get(changed);
}

/*******************************************************************************/
/* Loesch-Funktionen							       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* EraseElements --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::EraseElements( int num_inx, int *inx ){
  indices().indexList().setIndexList( num_inx, inx );
  return EraseElements_PreInx();
}

bool DataReference::EraseElements( int i ){
  int Inx[] = {i};
  return EraseElements( 1, Inx );
}

bool DataReference::EraseElements( int i1, int i2 ){
  int Inx[] = {i1,i2};
  return EraseElements( 2, Inx );
}

/* --------------------------------------------------------------------------- */
/* EraseElements_PreInx --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::EraseElements_PreInx(){
  AssertReference();
  assert( isValid() );

  DataAlterEraseElement erase;
  return alterData( erase );
}

/* --------------------------------------------------------------------------- */
/* clearAllElements --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::clearAllElements(){
  BUG_DEBUG( "clearAllElements of " << fullName( true ) );

  // Auf dem letzten Level werden alle Datenelemente ungültig gemacht.
  DataAlterClearDataItem clear;
  return alterData( clear );
}

/* --------------------------------------------------------------------------- */
/* eraseAllElements --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::eraseAllElements( bool incl_itemattr ){
  BUG_DEBUG( "eraseAllElements of " << fullName( true ) );

  // Auf dem letzten Level werden alle Datenelemente gelöscht.
  DataAlterEraseDataItem erase( incl_itemattr );
  return alterData( erase );
}

/* --------------------------------------------------------------------------- */
/* packDataItem --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::packDataItem( bool packRow ){
  if( numberOfWildcards() == 1 ){
    DataVector data;
    DataAlterGetVector get( data );
    alterData( get );

    if( data.packVector() ){
      // Es gab Verschiebungen im Vektor
      DataAlterSetVector set( data );
      return alterData( set );
    }
    return false;
  }

  if( numberOfWildcards() == 2 ){
    DataMatrix data;
    DataAlterGetMatrix get( data );
    alterData( get );

    if( packRow ? data.packMatrixRows() : data.packMatrixColumns() ){
      DataAlterSetMatrix set( data );
      return alterData( set );
    }
    return false;
  }

  return false;
}

/*******************************************************************************/
/* Dimensionen								       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* getNumberOfDataDimensions --						       */
/* --------------------------------------------------------------------------- */

int DataReference::GetNumberOfDataDimensions(){
  const DataItem *item = getItem();
  if( item != 0 ){
    return item->getNumberOfDimensions();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* GetDataDimensionSize --						       */
/* --------------------------------------------------------------------------- */

int DataReference::GetDataDimensionSize( int dim_num ){
  const DataItem *item = getItem();
  if( item != 0 ){
    return item->getDataDimensionSizeOf( dim_num );
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getAllDataDimensionSize_PreInx --					       */
/* --------------------------------------------------------------------------- */

int DataReference::getAllDataDimensionSize_PreInx( DataContainer::SizeList &dimsize ){
  dimsize.clear();
  const DataItem *item = getItem();
  DataIndices indices( *m_indices );
  DataInx index( indices );
  index.lastLevel();

  if( item != 0 ){
    return item->getAllDataDimensionSize_PreInx( dimsize, index );
  }

  int num_wildcards  = index.remainingWildcards();
  dimsize.push_back( 0 );
  for( int i=1; i<num_wildcards;i++ ){
    dimsize.push_back( 0 );
  }
  return (int)dimsize.size();
}

/* --------------------------------------------------------------------------- */
/* GetAllDataDimensionSize --						       */
/* --------------------------------------------------------------------------- */

int DataReference::GetAllDataDimensionSize( DataContainer::SizeList &dimsize, bool incl_itemattr ){
  dimsize.clear();
  const DataItem *item = getItem();
  if( item != 0 ){
    return item->getAllDataDimensionSize( dimsize, incl_itemattr );
  }
  dimsize.push_back( 0 );
  return 1;
}

/* --------------------------------------------------------------------------- */
/* GetDataDimensionSizeOfLevel_PreInx --				       */
/* --------------------------------------------------------------------------- */

int DataReference::GetDataDimensionSizeOfLevel_PreInx( int level, int dimension ){
  LOG_DEBUG("Level=" << level << " dimension=" << dimension );

  AssertReference();
  assert( isValid() );

  DataAlterGetDimensionInfo dim_info( dimension, level );
  alterData( dim_info );
  int rslt = dim_info.sizeOfDimension();

  LOG_DEBUG( "rslt=" << rslt );
  return rslt;
}

/*******************************************************************************/
/* Differenz-Feature							       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* getCompareResultItem --						       */
/* --------------------------------------------------------------------------- */

DataItem *DataReference::getCompareResultItem(){
  DataAlterGetItem get_item( true ); // create, if not there
  alterData( get_item );
  // In diesem Fall wird trotzdem ein Update stattfinden. Der Name der Methode
  // dient lediglich als Warnung, damit man daran denkt, was man tut !!
  DataItem *rslt_item = get_item.getItemNoUpdate();
  assert( rslt_item != 0 );

  rslt_item->eraseContainer();
  rslt_item->markDataItemValueUpdated();
  return rslt_item;
}

/* --------------------------------------------------------------------------- */
/* compareElements --							       */
/* --------------------------------------------------------------------------- */

void DataReference::compareElements( DataReference &ref_left, DataReference &ref_right ){
  BUG_PARA( BugRef, "compareElements", fullName( true ) );

  DataItem *rslt_item = getCompareResultItem();
  DataType rslt_data_type = rslt_item->getDataType();

  if( !isSameDataType( &ref_left ) || !isSameDataType( &ref_right ) ){
    DataEvents().writeLogMessage( "Ein Compare ist nicht möglich. Die Elemente haben nicht dieselbe Struktur." );
    return;
  }

  // linke Seite
  // -----------
  const DataItem *item_left = 0;
  const DataElement *el_left = ref_left.getElement( item_left ); // ohne Wildcards
  if( el_left == 0 ){
    // Kein Vergleich möglich
    DataEvents().writeLogMessage( "Ein Compare ist nicht möglich. Das linke Element ist nicht vorhanden." );
    return;
  }

  DataAlterCompare compare( *rslt_item, el_left );
  ref_right.alterData( compare );
}

/* --------------------------------------------------------------------------- */
/* compareElements --							       */
/* --------------------------------------------------------------------------- */

void DataReference::compareElements( DataReference &ref, bool cycles ){
  DataItem *rslt_item = getCompareResultItem();

  if( !isSameDataType( &ref ) ){
    DataEvents().writeLogMessage( "Ein Compare ist nicht möglich. Die Elemente haben nicht dieselben Element-Typ." );
    return;
  }

  if( cycles ){
    if( ref.hasWildcards() ){
      DataEvents().writeLogMessage( "Ein Compare über alle Cycles ist mit Wildcards nicht möglich." );
      return;
    }
    int anzahl = 0;
    for( int cycle = 0; cycle < datapool().NumCycles(); cycle++ ){
      DataAlterCompare compare( *rslt_item, 0, anzahl );
      ref.alterData( compare, cycle );
      anzahl = compare.numberOfCompares();
    }
  }
  else{
    if( !ref.hasWildcards() ){
      DataEvents().writeLogMessage( "Ein Compare ohne Wildcards ist nicht möglich." );
      return;
    }
    DataAlterCompare compare( *rslt_item );
    ref.alterData( compare );
  }
}

/*******************************************************************************/
/* Datenzugriff mit Alter-Funktoren					       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* alterData --								       */
/* --------------------------------------------------------------------------- */

bool DataReference::alterData( DataAlterFunctor &func, int cycle ){
  BUG_PARA( BugRef, "alterData", fullName( true ) );

  AssertReference();
  assert( isValid() );

  if( cycle < 0 ){
    cycle = m_cyclenum;
  }

  if( datapool().NumCycles() <= cycle ){
    func.epilog( false );
    return false;
  }

  try{
    if( func.check( *this ) ){
     func.epilog( false );
     return false;
    }

    DataItem &item = getDataItem( m_indices->getBaseItemIdnr(), func.writeTTrail(), cycle );

    DataInx index( *m_indices );
    bool rslt = item.alterData( func, index ) != NoUpdate;
    func.epilog( rslt );
    return rslt;
  }
  catch( const std::exception &ex ){
    std::cerr << ex.what() << std::endl;
    std::cerr << "Indices without names: " << indices().print() << std::endl;

    std::ostringstream msg;
    msg << ex.what() << std::endl;
    msg << "Indices without names: " << indices().print() << std::endl;
    msg << "DataReference: " << fullName( true ) << std::endl;
    DataEvents().writeLogMessage( msg.str() );
    std::cerr << msg.str();
    assert( false );
  }

  func.epilog( false );
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterData --								       */
/* --------------------------------------------------------------------------- */

bool DataReference::alterData( DataAlterFunctor &func, int num_inx, int *inx ){
  AssertReference();
  assert( isValid() );

  // Die Indexliste auf dem letzten Level muss am Ende wieder
  // im Original-Zustand vorhanden sein.
  const DataIndexList *savedlist = indices().saveIndexList( -1 );
  indices().indexList().setIndexList( num_inx, inx );

  bool rslt = alterData( func );

  indices().restoreIndexList( *savedlist, -1 );
  delete savedlist;

  return rslt;
}

/* --------------------------------------------------------------------------- */
/* getItem --								       */
/* --------------------------------------------------------------------------- */

const DataItem *DataReference::getItem(){
  DataAlterGetItem get_item;
  alterData( get_item );
  return get_item.getItem();
}

/* --------------------------------------------------------------------------- */
/* getItemNoUpdate --							       */
/* --------------------------------------------------------------------------- */

DataItem *DataReference::getItemNoUpdate(){
  DataAlterGetItem get_item;
  alterData( get_item );
  return get_item.getItemNoUpdate();
}

/* --------------------------------------------------------------------------- */
/* getElement --							       */
/* --------------------------------------------------------------------------- */

const DataElement *DataReference::getElement(){
  DataAlterGetElement get_element;
  alterData( get_element );
  return get_element.getElement();
}

/* --------------------------------------------------------------------------- */
/* getElement --							       */
/* --------------------------------------------------------------------------- */

const DataElement *DataReference::getElement( const DataItem *&item ){
  DataAlterGetElement get_element;
  alterData( get_element );
  item = get_element.getItem();
  return get_element.getElement();
}

/* --------------------------------------------------------------------------- */
/* getElement --							       */
/* --------------------------------------------------------------------------- */

const DataElement *DataReference::getElement( int num_inx, int *inx ){
  // Die Indexliste auf dem letzten Level muss am Ende wieder
  // im Original-Zustand vorhanden sein.
  const DataIndexList *savedlist = indices().saveIndexList();
  indices().indexList().setIndexList( num_inx, inx );

  DataAlterGetElement get_element;
  alterData( get_element );

  indices().restoreIndexList( *savedlist );
  delete savedlist;

  return get_element.getElement();
}

/* --------------------------------------------------------------------------- */
/* getDataItem --							       */
/* --------------------------------------------------------------------------- */

DataItem &DataReference::getDataItem( int item_index, bool for_updates, int cycle ){
  BUG_PARA( BugRef, "getDataItem"
		  , "item_index=" << item_index << ", for_updates=" << BugBool(for_updates) );

  if( cycle < 0 ){
    cycle = m_cyclenum;
  }
  DataStructElement *base_struct = datapool().GetRootElement( cycle );
  assert( base_struct != 0 );

  if( for_updates ){
    // DataItems, welche mutiert werden, werden während einer Transaction im TTrail
    // gesichert, damit bei einem Abort der Datapool restauriert werden kann.
    datapool().writeTransactionTrail( base_struct->getItemContainer(), item_index );
  }

  return base_struct->item( item_index );
}

/* --------------------------------------------------------------------------- */
/* clearDataItem --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::clearDataItem( bool incl_itemattr ){
  BUG_DEBUG( "clearDataItem " << fullName( true ) );

  DataAlterClearDataItem clear( incl_itemattr );
  return alterData( clear );
}

/* --------------------------------------------------------------------------- */
/* getDataItemAttr --							       */
/* --------------------------------------------------------------------------- */

const DataItemAttr *DataReference::getDataItemAttr(){
  AssertReference();
  assert( isValid() );

  DataAlterGetItemAttr get_attr( false );
  alterData( get_attr );
  return get_attr.getConstItemAttr();
}

/* --------------------------------------------------------------------------- */
/* getDataItemAttrForUpdates --						       */
/* --------------------------------------------------------------------------- */

DataItemAttr &DataReference::getDataItemAttrForUpdates(){
  AssertReference();
  assert( isValid() );

  DataAlterGetItemAttr get_attr( true );
  alterData( get_attr );
  return *get_attr.getItemAttr();
}

/* --------------------------------------------------------------------------- */
/* setDataItemAttr --							       */
/* --------------------------------------------------------------------------- */

void DataReference::setDataItemAttr( DATAAttributeMask set_mask, DATAAttributeMask reset_mask ){
  AssertReference();
  assert( isValid() );

  DataAlterSetAttributes setAttr( set_mask, reset_mask, true );
  alterData( setAttr );
}

/* --------------------------------------------------------------------------- */
/* setAttributes --							       */
/* --------------------------------------------------------------------------- */

void DataReference::setAttributes( DATAAttributeMask set_mask, DATAAttributeMask reset_mask ){
  AssertReference();
  assert( isValid() );

  DataAlterSetAttributes setAttr( set_mask, reset_mask, false );
  alterData( setAttr );
}

/* --------------------------------------------------------------------------- */
/* getAttributes --							       */
/* --------------------------------------------------------------------------- */

DATAAttributeMask DataReference::getAttributes( TransactionNumber t ){
  BUG_PARA( BugRef, "getAttributes" , "Name=" << fullName( true ) );
  AssertReference();
  assert( isValid() );

  DATAAttributeMask mask = dictionary().GetAttr()->getAttributes();

  DataAlterGetAttributes getAttr( t );
  alterData( getAttr );
  mask = mask | getAttr.get();
  BUG_EXIT( "return mask=" << mask );
  return mask;
}

/* --------------------------------------------------------------------------- */
/* isSerializable --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::isSerializable( const SerializableMask flags ){
  const DataItem *item = getItem();
  if( item != 0 ){
    return item->hasSerializableValidElements( flags );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* AssertReference --							       */
/* --------------------------------------------------------------------------- */

void DataReference::AssertReference() const{
  assert( isValid() ? hasDictionary() : true );
}

/*******************************************************************************/
/* Getter und Setter von DataElement-Objekten				       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* getDataVector --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::getDataVector( DataVector &vector, int num_inx, int *inx ){
  // Die Indexliste auf dem letzten Level muss am Ende wieder
  // im Original-Zustand vorhanden sein.
  const DataIndexList *savedlist = indices().saveIndexList();
  indices().indexList().setIndexList( num_inx, inx );

  bool rslt = getDataVector( vector );

  indices().restoreIndexList( *savedlist );
  delete savedlist;
  return rslt;
}

bool DataReference::getDataVector( DataVector &vector ){
  DataAlterGetVector getVector( vector );
  return alterData( getVector );
}

/* --------------------------------------------------------------------------- */
/* setDataVector --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::setDataVector( const DataVector &vector, int num_inx, int *inx ){
  // Die Indexliste auf dem letzten Level muss am Ende wieder
  // im Original-Zustand vorhanden sein.
  const DataIndexList *savedlist = indices().saveIndexList();
  indices().indexList().setIndexList( num_inx, inx );

  bool rslt = setDataVector( vector );

  indices().restoreIndexList( *savedlist );
  delete savedlist;
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* setDataVector --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::setDataVector( const DataVector &vector ){
  DataAlterSetVector setVector( vector );
  return alterData( setVector );
}

/* --------------------------------------------------------------------------- */
/* getDataMatrix --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::getDataMatrix( DataMatrix &matrix, int num_inx, int *inx ){
  // Die Indexliste auf dem letzten Level muss am Ende wieder
  // im Original-Zustand vorhanden sein.
  const DataIndexList *savedlist = indices().saveIndexList();
  indices().indexList().setIndexList( num_inx, inx );

  bool rslt = getDataMatrix( matrix );

  indices().restoreIndexList( *savedlist );
  delete savedlist;
  return rslt;
}

bool DataReference::getDataMatrix( DataMatrix &matrix ){
  DataAlterGetMatrix get_matrix( matrix );
  return alterData( get_matrix );
}

/* --------------------------------------------------------------------------- */
/* setDataMatrix --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::setDataMatrix( const DataMatrix &matrix, int num_inx, int *inx ){
  // Die Indexliste auf dem letzten Level muss am Ende wieder
  // im Original-Zustand vorhanden sein.
  const DataIndexList *savedlist = indices().saveIndexList();
  indices().indexList().setIndexList( num_inx, inx );

  bool rslt = setDataMatrix( matrix );

  indices().restoreIndexList( *savedlist );
  delete savedlist;
  return rslt;
}

bool DataReference::setDataMatrix( const DataMatrix &matrix ){
  DataAlterSetMatrix set_matrix( matrix );
  return alterData( set_matrix );
}

/* --------------------------------------------------------------------------- */
/* GetValue (integer) --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::GetValue( int &val ,int num_inx, int *inx ){
  const DataElement *el = getElement( num_inx, inx );
  if( el != 0 ){
    return el->getValue( val );
  }
  return false;
}

bool DataReference::GetValue( int &val, int i1, int i2 ){
  int Inx[] = {i1,i2};
  return GetValue( val, 2, Inx );
}

bool DataReference::GetValue( int &val, int i1 ){
  int Inx[] = {i1};
  return GetValue( val, 1, Inx );
}

bool DataReference::GetValue_PreInx( int &val ){
  const DataElement *el = getElement();
  if( el != 0 ){
    return el->getValue( val );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* SetValue (integer) --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::SetValue( int val, int num_inx, int *inx ){
  DataAlterSetIntegerElement set_int( val );
  return alterData( set_int, num_inx, inx );
}

bool DataReference::SetValue( int val, int i1, int i2 ){
  int Inx[] = {i1,i2};
  return SetValue( val, 2, Inx );
}

bool DataReference::SetValue( int val, int i ){
  int Inx[] = {i};
  return SetValue( val, 1, Inx );
}

bool DataReference::SetValue_PreInx( int val ){
  DataAlterSetIntegerElement set_int( val );
  return alterData( set_int );
}

/* --------------------------------------------------------------------------- */
/* GetValue (real) --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::GetValue( double &val, int num_inx, int *inx ){
  const DataElement *el = getElement( num_inx, inx );
  if( el != 0 ){
    return el->getValue( val );
  }
  return false;
}

bool DataReference::GetValue( double &val, int i1, int i2 ){
  int Inx[] = {i1,i2};
  return GetValue( val, 2, Inx );
}

bool DataReference::GetValue( double &val, int i ){
  int Inx[] = {i};
  return GetValue( val, 1, Inx );
}

bool DataReference::GetValue_PreInx( double &val ){
  const DataElement *el = getElement();
  if( el != 0 ){
    return el->getValue( val );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* SetValue (real) --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::SetValue( double val, int num_inx, int *inx ){
  DataAlterSetRealElement set_real( val );
  return alterData( set_real, num_inx, inx );
}

bool DataReference::SetValue( double val, int i1, int i2 ){
  int Inx[] = {i1,i2};
  return SetValue( val, 2, Inx );
}

bool DataReference::SetValue( double val, int i ){
  int Inx[] = {i};
  return SetValue( val, 1, Inx );
}

bool DataReference::SetValue_PreInx( double val ){
  DataAlterSetRealElement set_real( val );
  return alterData( set_real );
}

/* --------------------------------------------------------------------------- */
/* GetValue (string) --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::GetValue( std::string &val, int num_inx, int *inx ){
  const DataElement *el = getElement( num_inx, inx );
  if( el != 0 ){
    return el->getValue( val );
  }
  return false;
}

bool DataReference::GetValue( std::string &val, int i1, int i2 ){
  int Inx[] = {i1,i2};
  return GetValue( val, 2, Inx );
}

bool DataReference::GetValue( std::string &val, int i ){
  int Inx[] = {i};
  return GetValue( val, 1, Inx );
}

bool DataReference::GetValue_PreInx( std::string &val ){
  const DataElement *el = getElement();
  if( el != 0 ){
    return el->getValue( val );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* SetValue (string) --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::SetValue( const std::string &val, int num_inx, int *inx ){
  DataAlterSetStringElement set_string( val );
  return alterData( set_string, num_inx, inx );
}

bool DataReference::SetValue( const std::string &val, int i1, int i2 ){
  int Inx[] = {i1,i2};
  return SetValue( val, 2, Inx );
}

bool DataReference::SetValue( const std::string &val, int i ){
  int Inx[] = {i};
  return SetValue( val, 1, Inx );
}

bool DataReference::SetValue_PreInx( const std::string &val ){
  DataAlterSetStringElement set_string( val );
  return alterData( set_string );
}

/* --------------------------------------------------------------------------- */
/* GetValue (complex) --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::GetValue( dComplex &val, int num_inx, int *inx ){
  const DataElement *el = getElement( num_inx, inx );
  if( el != 0 ){
    return el->getValue( val );
  }
  return false;
}

bool DataReference::GetValue( dComplex &val, int i ){
  int Inx[] = {i};
  return GetValue( val, 1, Inx );
}

bool DataReference::GetValue_PreInx( dComplex &val ){
  const DataElement *el = getElement();
  if( el != 0 ){
    return el->getValue( val );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* SetValue (complex) --						       */
/* --------------------------------------------------------------------------- */

bool DataReference::SetValue( const dComplex &val, int num_inx, int *inx ){
  DataAlterSetComplexElement set_complex( val );
  return alterData( set_complex, num_inx, inx );
}

bool DataReference::SetValue( const dComplex &val, int i ){
  int Inx[] = {i};
  return SetValue( val, 1, Inx );
}

bool DataReference::SetValue_PreInx( const dComplex &val ){
  DataAlterSetComplexElement set_complex( val );
  return alterData( set_complex );
}

/* --------------------------------------------------------------------------- */
/* GetItemValid --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::GetItemValid( int num_inx, int *inx ){
  const DataElement *el = getElement( num_inx, inx );
  if( el != 0 ){
    return el->isValid();
  }
  return false;
}

bool DataReference::GetItemValid( int i ){
  int Inx[] = {i};
  return GetItemValid( 1, Inx );
}

bool DataReference::GetItemValid_PreInx(){
  const DataElement *el = getElement();
  if( el != 0 ){
    return el->isValid();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* SetItemInvalid --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::SetItemInvalid( int num_inx, int *inx ){
  DataAlterClearElement clear;
  return alterData( clear, num_inx, inx );
}

bool DataReference::SetItemInvalid( int i ){
  int Inx[] = {i};
  return SetItemInvalid( 1, Inx );
}

bool DataReference::SetItemInvalid_PreInx(){
  DataAlterClearElement clear;
  return alterData( clear );
}

/* --------------------------------------------------------------------------- */
/* GetItemLocked --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::GetItemLocked( int num_inx, int *inx ){
  const DataElement *el = getElement( num_inx, inx );
  if( el != 0 ){
    return el->isLocked();
  }
  return false;
}

bool DataReference::GetItemLocked( int i ){
  int Inx[] = {i};
  return GetItemLocked( 1, Inx );
}

bool DataReference::GetItemLocked_PreInx(){
  const DataElement *el = getElement();
  if( el != 0 ){
    return el->isLocked();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* SetItemLocked --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::SetItemLocked( bool val, int num_inx, int *inx ){
  DataAlterLockElement lock( val );
  return alterData( lock, num_inx, inx );
}

bool DataReference::SetItemLocked( bool val, int i ){
  int Inx[] = {i};
  return SetItemLocked( val, 1, Inx );
}

bool DataReference::SetItemLocked_PreInx( bool val ){
  DataAlterLockElement lock( val );
  return alterData( lock );
}

/*******************************************************************************/
/* Assign-Funktionen							       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* assignDataItem --							       */
/* --------------------------------------------------------------------------- */

void DataReference::AssignDataItem( DataReference &ref ){
  LOG_DEBUG("AssignDataItem");

  AssertReference();
  assert( isValid() );

  assert( DataDictionary::TypeNeedsAlloc( dictionary().getDataType() ) );
  assert( DataDictionary::TypeNeedsAlloc( ref.dictionary().getDataType() ) );

  if( !dictionary().isSameDataType( ref.dictionary() ) ){
    // Darf nicht vorkommen !!
    assert(false);
    return;
  }

  const DataItem *item = ref.getItem();
  assert( item != 0 );

  DataAlterAssignDataItem assign( item );
  alterData( assign );
}

/* --------------------------------------------------------------------------- */
/* assignCorrespondingDataItem --					       */
/* --------------------------------------------------------------------------- */

void DataReference::AssignCorrespondingDataItem( DataReference &ref ){
  AssertReference();
  assert( isValid() );

  assert( DataDictionary::TypeNeedsAlloc( dictionary().getDataType() ) );
  assert( DataDictionary::TypeNeedsAlloc( ref.dictionary().getDataType() ) );

  if( dictionary().getDataType() != DataDictionary::type_StructVariable ){
    // Nur Struktur-Items können bearbeitet werden.
    return;
  }
  if( ref.dictionary().getDataType() != DataDictionary::type_StructVariable ){
    // Nur Struktur-Items können bearbeitet werden.
    return;
  }

  // -------------------------------------------------------------------------
  // Nun gehts los. Das Source-Element muss nicht vorhanden sein.
  // -------------------------------------------------------------------------
  const DataElement *src_element = ref.getElement();

  // -------------------------------------------------------------------------
  // Das Destination-Element muss nur, falls nicht vorhanden, kreiert werden,
  // wenn das Source-Element vorhanden und gültig ist.
  // -------------------------------------------------------------------------
  if( src_element != 0 ){
    if( !src_element->isValid() ){
      src_element = 0;
    }
  }

  DataAlterAssignCorrDataItem assign( src_element, ref.dictionary() );
  alterData( assign );
}

/* --------------------------------------------------------------------------- */
/* assignDataElement --							       */
/* --------------------------------------------------------------------------- */

void DataReference::AssignDataElement( DataReference &ref ){
  // -------------------------------------------------------------------------
  // Das Source-Element muss nicht vorhanden sein. Der Wert ist in diesem
  // Fall einfach ungültig.
  // -------------------------------------------------------------------------
  const DataElement *src_element = ref.getElement();

  // -------------------------------------------------------------------------
  // Das Destination-Element muss nur, falls nicht vorhanden, kreiert werden,
  // wenn das Source-Element vorhanden und gültig ist.
  // -------------------------------------------------------------------------
  bool src_is_valid = false;
  if( src_element != 0 ){
    src_is_valid = src_element->isValid();
  }

  if( src_is_valid ){
    DataAlterAssignElement assign( src_element );
    alterData( assign );
  }
  else{
    DataAlterAssignElement assign;
    alterData( assign );
  }
}

/*******************************************************************************/
/* Write-Funktionen							       */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* writeXML --								       */
/* --------------------------------------------------------------------------- */

bool DataReference::writeXML( std::ostream &os
			    , std::vector<std::string> &attrs
			    , int level
			    , bool debug )
{
  LOG_DEBUG( "write" );
  DataItem *item = getItemNoUpdate();
  if( item != 0 ){
    item->writeXML( os, attrs, level, debug );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* writeJSON --								       */
/* --------------------------------------------------------------------------- */

bool DataReference::writeJSON( std::ostream &os,
                               int level,
                               int indent,
                               const SerializableMask flags,
                               bool with_endl )
{
  DataAlterWriteJSON write( os,
                            level,
                            indent,
                            flags,
                            !with_endl /* omit first indent */ );
  alterData( write );
  if( with_endl ){
    os << std::endl;
  }
  return write.hasElementsWritten();
}

/* --------------------------------------------------------------------------- */
/* writeDP --								       */
/* --------------------------------------------------------------------------- */

void DataReference::writeDP( std::ostream &ostr, int i ){
  const DataItem *item = getItem();
  if( item != 0 ){
    item->writeDP( ostr, i );
  }
  else{
    indent( i, ostr ) << "<Item not available>" << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* PrintItem --								       */
/* --------------------------------------------------------------------------- */

void DataReference::PrintItem( bool withDimensions, std::ostream &sout ){
  AssertReference();

  if( isntValid() ){
    sout << "The DataReference is not valid and cannot be printed" << std::endl;
    return;
  }

  std::string addr( indices().print( datapool().GetRootDictVar(), withDimensions ) );

  // sout << "====> Printout of " << addr	<< " <====" << std::endl;

  std::vector<std::string> attrs;
  writeXML( sout, attrs, 0, false );
  // sout << "====> End of Printout <====" << std::endl;
  return;
}

/*=============================================================================*/
/* Operatoren								       */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* operator==  --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::operator==( const DataReference &ref ) const{
  AssertReference();

  if( isValid() != ref.isValid() ){
    return false;
  }
  if( isntValid() ){
    return true;
  }
  return indices() == ref.indices() && fullName() == ref.fullName();
}

/* --------------------------------------------------------------------------- */
/* operator!=  --							       */
/* --------------------------------------------------------------------------- */

bool DataReference::operator!=( const DataReference &ref ) const{
  return !operator==( ref );
}

/* --------------------------------------------------------------------------- */
/* operator=  --							       */
/* --------------------------------------------------------------------------- */

DataReference& DataReference::operator=( const DataReference &ref ){
  AssertReference();

  m_dict	 = ref.m_dict;
  m_ref_valid	 = ref.m_ref_valid;
  m_cyclenum	 = ref.m_cyclenum;

  copyIndices( ref );
  AssertReference();
  return *this;
}

/*=============================================================================*/
/* Constructor / Destructor of StructIterator				       */
/*=============================================================================*/

DataReference::StructIterator::StructIterator( const DataReference *pRef
					     , const DataDictionary *pDict )
  : m_pRef( pRef )
  , m_pDict( pDict ){
}

DataReference::StructIterator::StructIterator( const StructIterator &s )
  : m_pRef( s.m_pRef )
  , m_pDict( s.m_pDict ){
}

DataReference::StructIterator::~StructIterator(){
}

/*=============================================================================*/
/* Operators of StructIterator						       */
/*=============================================================================*/

DataReference::StructIterator &DataReference::StructIterator::operator=( const StructIterator &s ){
  m_pRef = s.m_pRef;
  m_pDict = s.m_pDict;
  return *this;
}

bool DataReference::StructIterator::operator==(const StructIterator & Other) const{
  return m_pRef == Other.m_pRef && m_pDict == Other.m_pDict;
}

bool DataReference::StructIterator::operator!=(const StructIterator & Other) const{
  return m_pRef != Other.m_pRef || m_pDict != Other.m_pDict;
}

DataReference::StructIterator &DataReference::StructIterator::operator++(int){
  if (m_pDict != 0){
    m_pDict=m_pDict->getNext();
  }
  return *this;
}

DataReference::StructIterator &DataReference::StructIterator::operator++(){
  return operator++(1);
}

/*=============================================================================*/
/* Member Functions of StructIterator					       */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* NewDataReference --							       */
/* --------------------------------------------------------------------------- */

DataReference *DataReference::StructIterator::NewDataReference() const{
  assert(m_pRef	 != 0);
  assert(m_pDict != 0);
  assert(m_pRef->isValid());
  DataReference *pRef = DataPool::newDataReference( *m_pRef, m_pDict->getName().c_str() );
  assert(pRef->isValid());
  return pRef;
}

/* --------------------------------------------------------------------------- */
/* GetDict --								       */
/* --------------------------------------------------------------------------- */

const DataDictionary *DataReference::StructIterator::GetDict() const{
  assert(m_pRef	 != 0);
  assert(m_pDict != 0);
  return m_pDict;
}

/* --------------------------------------------------------------------------- */
/* GetAttr --								       */
/* --------------------------------------------------------------------------- */

DataUserAttr *DataReference::StructIterator::GetAttr() const{
  assert(m_pRef	 != 0);
  assert(m_pDict != 0);
  return GetDict()->GetAttr();
}

/* --------------------------------------------------------------------------- */
/* GetDataType --							       */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataReference::StructIterator::GetDataType() const{
  assert(m_pRef	 != 0);
  assert(m_pDict != 0);
  return GetDict()->getDataType();
}

/* --------------------------------------------------------------------------- */
/* begin --								       */
/* --------------------------------------------------------------------------- */

DataReference::StructIterator DataReference::begin() const{
  AssertReference();
  assert( isValid() );
  assert( hasDictionary() );
  assert(GetDict()->getDataType() == DataDictionary::type_StructVariable);
  if( !GetDict()->hasStructureNode() ){
    return end();
  }
  return StructIterator( this, GetDict()->getStructure()->getStructure() );
}

/* --------------------------------------------------------------------------- */
/* end --								       */
/* --------------------------------------------------------------------------- */

DataReference::StructIterator DataReference::end() const{
  AssertReference();
  assert( isValid() );
  assert( hasDictionary() );
  assert( GetDict()->getDataType() == DataDictionary::type_StructVariable );
  return StructIterator( this, 0 );
}
