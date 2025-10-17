
#if !defined(XML_XPATH_ROOT_H)
#define XML_XPATH_ROOT_H

#include <assert.h>

#include "xfer/XferDataItem.h"
#include "xml/XMLElement.h"

class XferDataItem;

class XMLXPathRoot : public XMLElement{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLXPathRoot( XferDataItem *xfer, int cycle );
  ~XMLXPathRoot();
  //========================================================================
  // public functions of XMLElement
  //========================================================================
  virtual XMLElement *newElement( const std::string &element );
  virtual XferDataItem *getXferDataItem(){ return m_xfer; }
  virtual XferDataItem *getXferDataItem( const std::string &element );
 private:
  XferDataItem *m_xfer;
  int           m_cycle;
};

#endif
