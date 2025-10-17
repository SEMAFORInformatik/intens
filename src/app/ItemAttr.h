
#if !defined(APP_ITEMATTR_INCLUDED_H)
#define APP_ITEMATTR_INCLUDED_H

#include "datapool/DataPool.h"
#include "datapool/DataItemAttr.h"

class DataDictionary;
class DataItem;

class ItemAttr : public DataItemAttr
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  ItemAttr();
  ItemAttr( DataDictionary * pDict, DataItem * pItem );
  ItemAttr( const ItemAttr & attr );
  virtual ~ItemAttr();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual DataItemAttr *NewClass( DataDictionary *pDict = 0
                                , DataItem       *pItem = 0) const;
  virtual DataItemAttr *Clone();

  void setDataBaseTimestamp( TransactionNumber ts );
  TransactionNumber DataBaseTimestamp() const;

  /** Die Funktion schreibt die Attribute im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
   */
  virtual void writeDP( std::ostream &ostr ) const;

  /** Die Funktion zeigt an, ob das DataItem auch ohne gültige DataElement-Objekte
      relevant ist.
  */
  virtual bool hasValidAttributes() const;

  /** Die Funktion löscht relevante Attribute.
  */
  virtual void clearValidAttributes();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  TransactionNumber   m_database_timestamp;
};

#endif
