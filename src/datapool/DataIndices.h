
#if !defined(__DP_DATA_INDICES__)
#define __DP_DATA_INDICES__

#include <iostream>
#include <map>

#include "datapool/DataDebugger.h"
#include "datapool/DataIndexList.h"
#include "datapool/DataAttributes.h"

class DataPool;
class DataDictionary;

/** Das Objekt ist die komplette Liste aller Indizes über alle referenzierten
    Daten-Ebenen.
 */
class DataIndices
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataIndices();

  /** Copy-Konstruktor
      \param indices
      \todo Die Beschreibung der Funktion DataIndices fehlt
   */
  DataIndices( const DataIndices &indices );

  /** Konstruktor
      \param levels
      \param inx
      \todo Die Beschreibung der Funktion DataIndices fehlt
   */
  DataIndices( int levels, const int *inx );

  /** Destruktor
   */
  virtual ~DataIndices();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataIndices");

  /** Beschreibung der Funktion addIndexList
      \param item_idnr
      \return
      \todo Die Beschreibung der Funktion addIndexList fehlt
   */
  DataIndexList &addIndexList( int item_idnr );

  /** Beschreibung der Funktion getIndexList
      \param level
      \return
      \todo Die Beschreibung der Funktion getIndexList fehlt
   */
  DataIndexList *getIndexList( int level );

  /** Beschreibung der Funktion indexList
      \param level
      \return
      \todo Die Beschreibung der Funktion indexList fehlt
   */
  DataIndexList &indexList( int level=-1 );

  /** Beschreibung der Funktion indexList
      \param level
      \return
      \todo Die Beschreibung der Funktion indexList fehlt
   */
  const DataIndexList &indexList( int level=-1 ) const;

  /** Beschreibung der Funktion saveIndexList
      \param level
      \return
      \todo Die Beschreibung der Funktion saveIndexList fehlt
   */
  const DataIndexList *saveIndexList( int level=0 ) const;

  /** Beschreibung der Funktion restoreIndexList
      \param inxlist
      \param level
      \return
      \todo Die Beschreibung der Funktion restoreIndexList fehlt
   */
  void restoreIndexList( const DataIndexList &inxlist, int level=0 );

  /** Beschreibung der Funktion addIndex
      \param inx
      \return
      \todo Die Beschreibung der Funktion addIndex fehlt
   */
  DataIndex &addIndex( int inx );

  /** Beschreibung der Funktion addWildcard
      \return
      \todo Die Beschreibung der Funktion addWildcard fehlt
   */
  DataIndex &addWildcard();

  /** Beschreibung der Funktion clearIndices
      \return
      \todo Die Beschreibung der Funktion clearIndices fehlt
   */
  void clearIndices();

  /** Beschreibung der Funktion hasWildcards
      \return
      \todo Die Beschreibung der Funktion hasWildcards fehlt
   */
  bool hasWildcards() const;

  /** Beschreibung der Funktion hasWildcardsNotLastLevel
      \return
      \todo Die Beschreibung der Funktion hasWildcardsNotLastLevel fehlt
   */
  bool hasWildcardsNotLastLevel() const;

  /** Beschreibung der Funktion numberOfWildcards
      \return
      \todo Die Beschreibung der Funktion numberOfWildcards fehlt
   */
  int numberOfWildcards() const;

  /** Beschreibung der Funktion numberOfLevels
      \return
      \todo Die Beschreibung der Funktion numberOfLevels fehlt
   */
  int numberOfLevels() const;

  /** Beschreibung der Funktion getLastLevel
      \return
      \todo Die Beschreibung der Funktion getLastLevel fehlt
   */
  int getLastLevel() const;

  /** Beschreibung der Funktion isLastLevel
      \param level
      \return
      \todo Die Beschreibung der Funktion isLastLevel fehlt
   */
  bool isLastLevel( int level ) const;

  /** Beschreibung der Funktion deleteLastLevel
      \return
      \todo Die Beschreibung der Funktion deleteLastLevel fehlt
   */
  void deleteLastLevel();

  /** Beschreibung der Funktion remainingLevels
      \param level
      \return
      \todo Die Beschreibung der Funktion remainingLevels fehlt
   */
  int remainingLevels( int level ) const;

  /** Beschreibung der Funktion convert
      \param levels
      \param inx
      \return
      \todo Die Beschreibung der Funktion convert fehlt
   */
  static DataIndices *convert( int levels, const int *inx ); // old style

  /** Beschreibung der Funktion convert
      \param indices
      \param levels
      \param inx
      \return
      \todo Die Beschreibung der Funktion convert fehlt
   */
  static void convert( DataIndices &indices, int levels, const int *inx ); // old style

  /** Beschreibung der Funktion getDictionary
      \param datapool
      \return
      \todo Die Beschreibung der Funktion getDictionary fehlt
   */
  DataDictionary *getDictionary( DataPool &datapool ) const;

  /** Beschreibung der Funktion getUserDataAttributes
      \param datapool
      \return
      \todo Die Beschreibung der Funktion getUserDataAttributes fehlt
   */
  DataAttributes getUserDataAttributes( DataPool &datapool ) const;

  /** Beschreibung der Funktion getBaseItemIdnr
      \return
      \todo Die Beschreibung der Funktion getBaseItemIdnr fehlt
   */
  int getBaseItemIdnr() const;

  /** Beschreibung der Funktion print
      \param ostr
      \param dict
      \param withIndices
      \return
      \todo Die Beschreibung der Funktion print fehlt
   */
  void print( std::ostream &ostr, const DataDictionary *dict=0, bool withIndices=true ) const;

  /** Beschreibung der Funktion print
      \param dict
      \param withIndices
      \return
      \todo Die Beschreibung der Funktion print fehlt
   */
  const std::string print( const DataDictionary *dict=0, bool withIndices=true ) const;

/*=============================================================================*/
/* public operators                                                            */
/*=============================================================================*/
public:

  /** Beschreibung der Funktion operator==
      \param ind
      \return
      \todo Die Beschreibung der Funktion operator== fehlt
   */
  bool operator==( const DataIndices &ind ) const;

  /** Beschreibung der Funktion operator!=
      \param ind
      \return
      \todo Die Beschreibung der Funktion operator!= fehlt
   */
  bool operator!=( const DataIndices &ind ) const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private member variables                                                    */
/*=============================================================================*/
private:
  typedef std::map<int,DataIndexList> IndexLists;

  IndexLists  m_indexlists;

  BUG_DECLARE_COUNT;
};

#endif
/** \file */
