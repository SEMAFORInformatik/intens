
#if !defined(__DP_DATA_TTRAIL_ITEM_LIST__)
#define __DP_DATA_TTRAIL_ITEM_LIST__

#include <list>

#include "datapool/DataDebugger.h"
#include "datapool/DataTTrailElement.h"

class DataItem;
class DataPool;
class DataTTrailItem;

/** DataTTrailItemList
    @author Copyright (C) 2009 SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataTTrailItemList : public DataTTrailElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataTTrailItemList();

  /** Destruktor
   */
  virtual ~DataTTrailItemList();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataTTrailItemList");

  /** Beschreibung der Funktion typ
      \return
      \todo Die Beschreibung der Funktion typ fehlt
   */
  virtual DataTTrailElement::Typ typ() const { return LIST; }

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

  /** Beschreibung der Funktion insertItem
      \param item
      \return
      \todo Die Beschreibung der Funktion insertItem fehlt
   */
  void insertItem( DataTTrailElement *item );

  /** Beschreibung der Funktion isntEmpty
      \return
      \todo Die Beschreibung der Funktion isntEmpty fehlt
   */
  bool isntEmpty() const;

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;

  typedef std::list<DataTTrailElement *> ItemList;

  ItemList     m_item_list;
};

#endif
/** \file */
