
#if !defined(DATA_TTRAIL_ITEM_INCLUDED)
#define DATA_TTRAIL_ITEM_INCLUDED

#include "datapool/DataDebugger.h"
#include "datapool/DataTTrailElement.h"

class DataPool;
class DataItemContainer;
class DataItem;

/** DataTTrailItem
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataTTrailItem : public DataTTrailElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \param DataItemContainer
      \param DataItem
      \param int
      \todo Die Beschreibung der Funktion DataTTrailItem fehlt
   */
  DataTTrailItem( DataItemContainer *, DataItem *, int );

  /** Destruktor
   */
  virtual ~DataTTrailItem();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataTTrailItem");

  /** Beschreibung der Funktion typ
      \return
      \todo Die Beschreibung der Funktion typ fehlt
   */
  virtual DataTTrailElement::Typ typ() const { return ITEM; }

  /** Beschreibung der Funktion isEqual
      \param el
      \return
      \todo Die Beschreibung der Funktion isEqual fehlt
   */
  virtual bool isEqual( DataTTrailElement &el );

  /** Beschreibung der Funktion isEqual
      \param item
      \return
      \todo Die Beschreibung der Funktion isEqual fehlt
   */
  virtual bool isEqual( DataItem *item );

  /** Beschreibung der Funktion restore
      \param dp
      \return
      \todo Die Beschreibung der Funktion restore fehlt
   */
  virtual bool restore( DataPool &dp );

  /** Beschreibung der Funktion getDataItem
      \return
      \todo Die Beschreibung der Funktion getDataItem fehlt
   */
  virtual DataItem *getDataItem();

  /** Beschreibung der Funktion getItem
      \return
      \todo Die Beschreibung der Funktion getItem fehlt
   */
  virtual DataTTrailItem *getItem() { return this; }

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

  DataItemContainer  *m_container;
  DataItem           *m_item;
  int                 m_index;

};

#endif
/** \file */
