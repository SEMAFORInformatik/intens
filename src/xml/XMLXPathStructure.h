
#if !defined(XML_STRUCTURE_H)
#define XML_STRUCTURE_H

#include <assert.h>

#include "xml/XMLElement.h"

class XferDataItem;

class XMLXPathStructure : public XMLElement{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLXPathStructure( XMLElement *parent, XferDataItem *xfer );
  virtual ~XMLXPathStructure();

  //========================================================================
  // public functions of XMLElement
  //========================================================================
 public:
  virtual XferDataItem *getXferDataItem(){ return m_xferDataItem; }
  virtual XMLElement *newElement( const std::string &element );
  virtual void clearRange();
  //========================================================================
  // private members
  //========================================================================
 private:
  XferDataItem *m_xferDataItem;
};

#endif
