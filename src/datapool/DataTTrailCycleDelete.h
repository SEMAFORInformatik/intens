
#if !defined(__DP_DATA_TTRAIL_DELETE_CYCLE__)
#define __DP_DATA_TTRAIL_DELETE_CYCLE__

#include "datapool/DataDebugger.h"
#include "datapool/DataTTrailElement.h"

class DataPool;
class DataItem;

/** DataTTrailCycleDelete
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataTTrailCycleDelete : public DataTTrailElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \param DataItem cycle
      \param int num
      \todo Die Beschreibung der Funktion DataTTrailCycleDelete fehlt
   */
  DataTTrailCycleDelete( DataItem *cycle, int num );

  /** Destruktor
   */
  virtual ~DataTTrailCycleDelete();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataTTrailCycleDelete");

  /** Beschreibung der Funktion typ
      \return
      \todo Die Beschreibung der Funktion typ fehlt
   */
  virtual DataTTrailElement::Typ typ() const { return CYCLE; }

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
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;

  DataItem           *m_cycle;
  int                 m_cyclenum;

};

#endif
/** \file */
