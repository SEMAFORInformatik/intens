
#if !defined(__DP_DATA_INDEX__)
#define __DP_DATA_INDEX__

#include <iostream>

#include "utils/Separator.h"
#include "datapool/DataDebugger.h"

/** Beschreibung der Klasse DataIndex
    \todo Die Beschreibung der Klasse DataIndex fehlt
 */
class DataIndex
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataIndex();

  /** Konstruktor
      \param index
      \todo Die Beschreibung der Funktion DataIndex fehlt
   */
  DataIndex( int index );

  /** Copy-Konstruktor
      \param index
      \todo Die Beschreibung der Funktion DataIndex fehlt
   */
  DataIndex( const DataIndex &index );

  /** Destruktor
   */
  virtual ~DataIndex();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataIndex");

  /** Beschreibung der Funktion setLowerbound
      \param int
      \return
      \todo Die Beschreibung der Funktion setLowerbound fehlt
   */
  void setLowerbound( int );

  /** Beschreibung der Funktion getLowerbound
      \return
      \todo Die Beschreibung der Funktion getLowerbound fehlt
   */
  int getLowerbound() const;

  /** Beschreibung der Funktion setUpperbound
      \param int
      \return
      \todo Die Beschreibung der Funktion setUpperbound fehlt
   */
  void setUpperbound( int );

  /** Beschreibung der Funktion getUpperbound
      \return
      \todo Die Beschreibung der Funktion getUpperbound fehlt
   */
  int getUpperbound() const;

  /** Beschreibung der Funktion setWildcard
      \return
      \todo Die Beschreibung der Funktion setWildcard fehlt
   */
  void setWildcard();

  /** Beschreibung der Funktion setIndex
      \param i
      \return
      \todo Die Beschreibung der Funktion setIndex fehlt
   */
  void setIndex( int i );

  /** Die Funktion erhöht den Index um 1.
   */
  void incrementIndex();

  /** Beschreibung der Funktion getIndex
      \return
      \todo Die Beschreibung der Funktion getIndex fehlt
   */
  int getIndex() const;

  /** Beschreibung der Funktion isWildcard
      \return
      \todo Die Beschreibung der Funktion isWildcard fehlt
   */
  bool isWildcard() const;

  /** Beschreibung der Funktion isZero
      \return
      \todo Die Beschreibung der Funktion isZero fehlt
   */
  bool isZero() const;

  /** Beschreibung der Funktion print
      \param ostr
      \param sep
      \return
      \todo Die Beschreibung der Funktion print fehlt
   */
  void print( std::ostream &ostr, Separator &sep ) const;

/*=============================================================================*/
/* public operators                                                            */
/*=============================================================================*/
public:

  /** Beschreibung der Funktion operator==
      \param inx
      \return
      \todo Die Beschreibung der Funktion operator== fehlt
   */
  bool operator==( const DataIndex &inx ) const;

  /** Beschreibung der Funktion operator!=
      \param inx
      \return
      \todo Die Beschreibung der Funktion operator!= fehlt
   */
  bool operator!=( const DataIndex &inx ) const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private member variables                                                    */
/*=============================================================================*/
private:
  int      m_index;
  int      m_lowerbound;
  int      m_upperbound;

  BUG_DECLARE_COUNT;
};

#endif
/** \file */
