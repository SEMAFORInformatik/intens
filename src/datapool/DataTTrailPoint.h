
#if !defined(__DP_DATA_TTRAIL_POINT__)
#define __DP_DATA_TTRAIL_POINT__

#include <iostream>

#include "datapool/DataDebugger.h"
#include "datapool/DataPool.h"
#include "datapool/DataTTrailElement.h"

class TransactionOwner;

/** DataTTrailPoint
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataTTrailPoint : public DataTTrailElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \param level
      \param datapool
      \param owner
      \todo Die Beschreibung der Funktion DataTTrailPoint fehlt
   */
  DataTTrailPoint( int level, DataPool &datapool, TransactionOwner *owner );

  /** Destruktor
   */
  virtual ~DataTTrailPoint();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataTTrailPoint");

  /** Beschreibung der Funktion typ
      \return
      \todo Die Beschreibung der Funktion typ fehlt
   */
  virtual DataTTrailElement::Typ typ() const { return POINT; }

  /** Beschreibung der Funktion isEqual
      \param el
      \return
      \todo Die Beschreibung der Funktion isEqual fehlt
   */
  virtual bool isEqual( DataTTrailElement &el ) { return false; }

  /** Beschreibung der Funktion isEqual
      \param item
      \return
      \todo Die Beschreibung der Funktion isEqual fehlt
   */
  virtual bool isEqual( DataItem *item ) { return false; }

  /** Beschreibung der Funktion isOwner
      \param owner
      \return
      \todo Die Beschreibung der Funktion isOwner fehlt
   */
  virtual bool isOwner( TransactionOwner *owner );

  /** Beschreibung der Funktion getOwner
      \return
      \todo Die Beschreibung der Funktion getOwner fehlt
   */
  virtual TransactionOwner *getOwner() const;

  /** Beschreibung der Funktion restore
      \param dp
      \return
      \todo Die Beschreibung der Funktion restore fehlt
   */
  virtual bool restore( DataPool &dp );

  /** Beschreibung der Funktion print
      \param ostr
      \return
      \todo Die Beschreibung der Funktion print fehlt
   */
  virtual void print( std::ostream &ostr ) const;

/** Beschreibung der Funktion name
    \return
    \todo Die Beschreibung der Funktion name fehlt
 */
  virtual const std::string name() const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;

  int                m_level;
  TransactionNumber  m_transaction_num;
  TransactionOwner  *m_owner;
};

#endif // !defined(__DP_DATA_TTRAIL_POINT__)
/** \file */
