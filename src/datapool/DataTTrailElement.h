
#if !defined(__DP_DATA_TTRAIL_ELEMENT__)
#define __DP_DATA_TTRAIL_ELEMENT__

#include <iostream>

class DataItem;
class DataTTrailItem;
class DataPool;
class TransactionOwner;

/** DataTTrailElement
    @author Copyright (C) 2009  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataTTrailElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataTTrailElement() {}

  /** Destruktor
   */
  virtual ~DataTTrailElement() {}

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:

  /** Beschreibung der Aufzaehlung Typ
      \todo Die Beschreibung der Aufzaehlung Typ fehlt
   */
  enum Typ{
    POINT /** Beschreibung fehlt */  = 1
  , ITEM /** Beschreibung fehlt */
  , CYCLE /** Beschreibung fehlt */
  , LIST /** Beschreibung fehlt */
  , ACTION
  };
  /** \enum Typ
      \todo Die Beschreibung der Aufzaehlungswerte ist unvollstaendig
   */


/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Beschreibung der Funktion typ
      \return
      \todo Die Beschreibung der Funktion typ fehlt
   */
  virtual DataTTrailElement::Typ typ() const = 0;

  /** Beschreibung der Funktion isEqual
      \param el
      \return
      \todo Die Beschreibung der Funktion isEqual fehlt
   */
  virtual bool isEqual( DataTTrailElement &el ) = 0;

  /** Beschreibung der Funktion isEqual
      \param item
      \return
      \todo Die Beschreibung der Funktion isEqual fehlt
   */
  virtual bool isEqual( DataItem *item ) = 0;

  /** Beschreibung der Funktion isOwner
      \param owner
      \return
      \todo Die Beschreibung der Funktion isOwner fehlt
   */
  virtual bool isOwner( TransactionOwner *owner ) { return false; }

  /** Beschreibung der Funktion getOwner
      \return
      \todo Die Beschreibung der Funktion getOwner fehlt
   */
  virtual TransactionOwner *getOwner() const { return 0; }

  /** Beschreibung der Funktion restore
      \param dp
      \return
      \todo Die Beschreibung der Funktion restore fehlt
   */
  virtual bool restore( DataPool &dp ) = 0;

  /** Beschreibung der Funktion getDataItem
      \return
      \todo Die Beschreibung der Funktion getDataItem fehlt
   */
  virtual DataItem *getDataItem() { return 0; }

  /** Beschreibung der Funktion getItem
      \return
      \todo Die Beschreibung der Funktion getItem fehlt
   */
  virtual DataTTrailItem *getItem() { return 0; }

  /** Beschreibung der Funktion print
      \param ostr
      \return
      \todo Die Beschreibung der Funktion print fehlt
   */
  virtual void print( std::ostream &ostr ) const = 0;

  /** Beschreibung der Funktion name
      \return
      \todo Die Beschreibung der Funktion name fehlt
   */
  virtual const std::string name() const = 0;

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:

};

#endif
/** \file */
