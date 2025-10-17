
#if !defined(XML_STRUCT_VECTOR_H)
#define XML_STRUCT_VECTOR_H

#include <assert.h>

#include "xml/XMLElement.h"

class XMLStructVector : public XMLElement{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLStructVector( XMLElement *parent, XferDataItem *xfer, bool isRoot = false );
  virtual ~XMLStructVector();

  //========================================================================
  // public functions of XMLElement
  //========================================================================
 public:
  virtual void setValue( const std::string &value );
  virtual XferDataItem *getXferDataItem(){ return m_xferDataItem; }
  void setXferDataItem();
  virtual XMLElement *newElement( const std::string &element );

  //========================================================================
  // private members
  //========================================================================
 private:
  XferDataItem *m_xferDataItem;
  bool          m_isRoot;
};

#endif
