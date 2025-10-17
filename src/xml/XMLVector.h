
#if !defined(__XML_VECTOR_H__)
#define __XML_VECTOR_H__

#include <map>
#include "datapool/DataVector.h"

class XferDataItem;
class XferDataItemIndex;

class XMLVector
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XMLVector( XferDataItem *xfer );
  virtual ~XMLVector();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  void insertValue( int inx, DataValue *data );
  void setValues( bool ignoreValues );
  void setLastIndexWildcard();
  void setLastIndexLowerbound( int inx );
  XferDataItem *getDataItem();
  XferDataItem *newDataItem();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::map<int,DataValue *> ValueList;
  XferDataItem *m_xferDataItem;
  DataVector    m_values;
  ValueList     m_value_list;
  int           m_max_index;
};

#endif
