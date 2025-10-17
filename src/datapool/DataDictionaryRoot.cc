
#include <iostream>

#include "datapool/DataDictionaryRoot.h"
#include "datapool/DataPool.h"
#include "datapool/DataLogger.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataDictionaryRoot::DataDictionaryRoot(DataPool *datapool, DataUserAttr *userattr )
  : DataDictionary( userattr )
  , m_DP( datapool ){
}

DataDictionaryRoot::~DataDictionaryRoot(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* datapool --                                                                 */
/* --------------------------------------------------------------------------- */

DataPool &DataDictionaryRoot::datapool() const{
  return *m_DP;
}

/* --------------------------------------------------------------------------- */
/* GetDataPool --                                                              */
/* --------------------------------------------------------------------------- */

DataPool *DataDictionaryRoot::GetDataPool() const{
  assert(m_DP != 0);
  return  m_DP;
}

/* --------------------------------------------------------------------------- */
/* AddToDictionary --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionaryRoot::AddToDictionary( const char *struct_name,
                                                     const std::string &name,
                                                     const DataType type,
                                                     const char *struct_def_name,
                                                     bool item_is_global,
                                                     bool item_is_internal )
{
  DataDictionary *struct_dict = 0;
  DataDictionary *struct_def_dict = 0;

  if( struct_name != 0 && *struct_name != 0 ){
    // Der Name einer Ziel-Struktur scheint vorhanden zu sein.
    while( isspace( *struct_name ) ){
      struct_name++;
    }
    if( *struct_name != 0 ){
      // Der Name einer Ziel-Struktur IST vorhanden!
      if( ( struct_dict = FindDictEntry( struct_name ) ) == 0 ){
        LOG_DEBUG( "FATAL: cannot find member [" << struct_name << "] in Dictionary.");
        assert( struct_dict != 0 );
        return 0;
      }
    }
  }

  if( struct_def_name != 0 && *struct_def_name != 0 ){
    // Der Name einer Strukturdefinition scheint vorhanden zu sein.
    while( isspace( *struct_def_name ) ){
      struct_def_name++;
    }
    if( *struct_def_name != 0 ){
      // Der Name einer Strukturdefinition IST vorhanden.
      if( ( struct_def_dict = FindDictEntry( struct_def_name ) ) == 0 ){
        LOG_DEBUG( "FATAL: cannot find Variable [" << struct_def_name << "] in Dictionary.");
        assert( struct_def_dict != 0 );
        return 0;
      }
    }
  }

  return AddToDictionary( struct_dict,
                          name, type,
                          struct_def_dict,
                          item_is_global,
                          item_is_internal );
}

/* --------------------------------------------------------------------------- */
/* AddToDictionary --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary * DataDictionaryRoot::AddToDictionary( DataDictionary *struct_dict,
                                                      const std::string &name,
                                                      const DataType type,
                                                      DataDictionary *struct_def_dict,
                                                      bool item_is_global,
                                                      bool item_is_internal )
{
  DataDictionary * pDict = 0;

  assert( IsValidType( type ) );
  assert( type == type_StructVariable ? struct_def_dict != 0 : struct_def_dict == 0 );

  if( item_is_global ){
    // Für globale Items gibt es einige Einschränkungen, welche hier geprüft werden:
    //  1. Es gibt keine globalen Strukturdefinitionen.
    //  2. Globale Struktur-Member können nur angelegt werden, solange der Datapool
    //     noch nicht alloziert worden ist.
    if( !TypeNeedsAlloc( type ) ){
      LOG_DEBUG( "Global cannot be set for StructDefinitions. rslt=0");
      assert(false);
      return 0;
    }
    if( struct_dict != 0 && DataPoolAllocated() ){
      LOG_DEBUG( "Global Struct member not allowed. rslt=0");
      assert(false);
      return 0;
    }
  }

  if( type == type_StructVariable ){
    // Struktur-Variablen brauchen eine Struktur-Definition, die ihr Aussehen bestimmt.
    // Dafür sorgt die Definition struct_def_dict, die als Pointer-Parameter geliefert wird.
    if( struct_def_dict == 0 || struct_def_dict->getDataType() != type_StructDefinition ){
      if( struct_def_dict == 0 ){
        LOG_ERROR("No StructDefinition available");
      }
      else{
        LOG_ERROR( "Internal Error with Struct Variable " << struct_def_dict->getName() );
      }
      assert(false);
      return 0;
    }
  }

  if( struct_dict == 0 ){
    pDict = this->GetLastEntry()->AddNextItem( name,
                                               type,
                                               struct_def_dict,
                                               item_is_global,
                                               item_is_internal );
  }
  else{
    if( struct_dict->getDataType() != type_StructDefinition ){
      LOG_DEBUG("Internal Error with Struct Definition " << struct_dict->getName() );
      assert(false);
      return 0;
    }

    if( !struct_dict->hasStructureNode() ){
      pDict = struct_dict->AddFirstMember( name,
                                           type,
                                           struct_def_dict,
                                           item_is_global,
                                           item_is_internal );
    }
    else{
      pDict = struct_dict->structure().GetLastEntry()->AddNextItem( name,
                                                                    type,
                                                                    struct_def_dict,
                                                                    item_is_global,
                                                                    item_is_internal );
    }
  }

  if( DataPoolAllocated() ){
    // Wenn der Datapool schon alloziert worden ist, muss eine dynamische
    // Allokation vorgenommen werden.
    if( !GetDataPool()->DynamicDatapoolAlloc(pDict) ){
      LOG_DEBUG("DynamicDatapoolAlloc failed rslt=0");
      assert( false );
      return 0;
    }
  }

  LOG_DEBUG( "All OK rslt=" << pDict);
  return pDict;
}

/* --------------------------------------------------------------------------- */
/* FindDictEntry --                                                            */
/* --------------------------------------------------------------------------- */

DataDictionary *DataDictionaryRoot::FindDictEntry(const char *addr_name ){
  DataDictionary * pDict = this;
  const char     * pName;           // Actueller Name
  int              NameLen;         // Lange des Namens

  LOG_DEBUG( "Name=" << addr_name );

  while( isspace(*addr_name) ){
    addr_name++; // Deblank
  }

  if( addr_name == 0 ){
    LOG_DEBUG("empty String. rslt=0");
    return 0;
  }

  while( *addr_name ){
    assert( pDict != 0 );
    assert( pDict->getDataType() == type_StructDefinition );

    pName   = addr_name;
    NameLen = 0;
    while( isValidNameChar(*addr_name) ){
      NameLen++;
      addr_name++;
    }

    if( NameLen == 0 ){
      /*************************************************************/
      /* Ungueltiger char. Das ist ein unverzeihlicher Fehler.     */
      /*************************************************************/
      LOG_DEBUG( "Invalid Charakter. rslt=0");
      return 0;
    }

    if( !pDict->hasStructureNode() ){
      /************************************************************/
      /* Kein Structure-Node vorhanden. Das ist ein Fehler.       */
      /************************************************************/
      LOG_DEBUG( "wrong (partial) Name. rslt=0");
      return 0;
    }

    if( (pDict = pDict->structure().SearchByName(pName,NameLen)) == 0 ){
      /************************************************************/
      /* Dies ist ein Fehler.                                     */
      /************************************************************/
      LOG_DEBUG( "wrong (partial) Name. rslt=0");
      return 0;
    }

    /****************************************************************/
    /* Jetzt bereiten wir uns auf den naechsten Durchgang vor.      */
    /* Dazu muss der Punk geparsed werden und schon gehts weiter.   */
    /****************************************************************/
    while( isspace(*addr_name) ){
      addr_name++; // Deblank
    }

    if( *addr_name != '.' ){
      /*************************************************************/
      /* Hier geths nimmer weiter. Entweder ungueltig oder Ende    */
      /*************************************************************/
      if (*addr_name){
        LOG_DEBUG("Dot in Name is missing. rslt=");
        return 0;
      }
      continue;
    }

    assert(*addr_name == '.');
    addr_name++;
    while( isspace(*addr_name) ){
      addr_name++; // Deblank
    }

    if (pDict->getDataType() == type_StructVariable){
      pDict = pDict->getStructure();
      assert(pDict != 0);
    }
    assert(pDict->getDataType() == type_StructDefinition);
  } // while(*pAddr)

  assert( pDict != 0 );
  assert( IsValidType( pDict->getDataType() ) );

  LOG_DEBUG("Found rslt=" << pDict);
  return pDict;
}
