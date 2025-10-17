
#if !defined(__DP_DATAITEM_ATTRIBUTES__)
#define __DP_DATAITEM_ATTRIBUTES__

#include <stdio.h>
#include <assert.h>
#include <ostream>

#include "datapool/DataDebugger.h"
#include "datapool/DataPool.h"

class DataDictionary;
class DataItem;

/** Die DataItemAttr-Objekte sind eine Erweiterung zum Dataitem-Objekt. Der
    Benutzer des Datapools kann sich eine eigene Klasse ableiten und dem Dataitem
    weitere Eigenschaften und Attribute geben.
 */
class DataItemAttr
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataItemAttr();

  /** Copy-Konstruktor
      @param attr Zu kopierendes Objekt
   */
  DataItemAttr( const DataItemAttr & attr );

  /** Konstruktor
      @param dict Pointer auf das DataDictionary-Objekt des Dataitems
      @param item Pointer auf das zugehörige DataItem-Objekt
   */
  DataItemAttr( DataDictionary *dict, DataItem *item );

  /** Destruktor
   */
  virtual ~DataItemAttr();

protected:
  /** Zuweisungsoperator
      \param ref
      \return
      \note Der Zuweisungsoperator ist nicht implementiert
   */
  DataItemAttr& operator=(const DataItemAttr &ref);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion erstellt ein neues Objekt der DataItemAttr-Klasse.
      @param dict Pointer auf das DataDictionary-Objekt des Dataitems
      @param item Pointer auf das zugehörige DataItem-Objekt
   */
  virtual DataItemAttr *NewClass( DataDictionary *dict = 0, DataItem *item = 0 ) const = 0;

  /** Diese Funktion erstellt ein Duplikat des Objekts und gibt einen
      Pointer darauf zurück.
   */
  virtual DataItemAttr *Clone() = 0;

  /** Die Funktion schreibt die Attribute im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
   */
  virtual void writeDP( std::ostream &ostr ) const{}

  /** Die Funktion zeigt an, ob das DataItem auch ohne gültige DataElement-Objekte
      relevant ist.
  */
  virtual bool hasValidAttributes() const{ return false; }

  /** Die Funktion löscht relevante Attribute.
  */
  virtual void clearValidAttributes(){}

};



/** Die DefaultDataItemAttr-Klasse kommt zum Einsatz, wenn der Benutzer des DataPools
    keine eigene Implementation der Klasse DataItemAttr erstellt hat.
 */
class DefaultDataItemAttr : public DataItemAttr
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DefaultDataItemAttr();

  /** Copy-Konstruktor
      @param attr Zu kopierendes Objekt
   */
  DefaultDataItemAttr( const DefaultDataItemAttr &attr );

  /** Konstruktor
      @param dict Pointer auf das DataDictionary-Objekt des Dataitems
      @param item Pointer auf das zugehörige DataItem-Objekt
   */
  DefaultDataItemAttr( DataDictionary *dict, DataItem *item );

  /** Destruktor
   */
  virtual ~DefaultDataItemAttr();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion erstellt ein neues Objekt der DataItemAttr-Klasse.
      @param dict Pointer auf das DataDictionary-Objekt des Dataitems
      @param item Pointer auf das zugehörige DataItem-Objekt
   */
  virtual DataItemAttr *NewClass( DataDictionary *dict = 0, DataItem *item = 0 ) const;

  /** Diese Funktion erstellt ein Duplikat des Objekts und gibt einen
      Pointer darauf zurück.
   */
  virtual DataItemAttr *Clone();
};

#endif
/** \file */
