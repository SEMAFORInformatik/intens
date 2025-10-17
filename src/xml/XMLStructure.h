
#if !defined(XML_STRUCTURE_H)
#define XML_STRUCTURE_H

#include <assert.h>

#include "xml/XMLElement.h"

class XferDataItem;

class XMLStructure : public XMLElement{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLStructure( XMLElement *parent, XferDataItem *xfer, bool m_isRoot = false, bool allCycles = false );
  virtual ~XMLStructure();

  //========================================================================
  // public functions of XMLElement
  //========================================================================
 public:
  virtual XMLElement *newElement( const std::string &element );
  virtual XferDataItem *getXferDataItem(){ return m_xfer; }
  virtual XferDataItem *getXferDataItem( const std::string &element );
  virtual void clearRange();
  //========================================================================
  // private members
  //========================================================================
 private:
  XferDataItem *m_xfer;
  bool          m_isRoot;
  bool          m_allCycles;
};

#endif
