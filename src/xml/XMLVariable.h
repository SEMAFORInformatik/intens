
#if !defined(XML_VARIABLE_H)
#define XML_VARIABLE_H

#include <assert.h>

#include "xml/XMLElement.h"

class XMLVariable : public XMLElement{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLVariable( XMLElement *parent, XferDataItem *xfer );
  ~XMLVariable();

  //========================================================================
  // public functions of XMLElement
  //========================================================================
  virtual void setValue( const std::string & value );
  virtual void clearRange();
  virtual XferDataItem *getXferDataItem(){ return m_xferDataItem; }
  virtual XMLElement *newElement( const std::string &element );

  //========================================================================
  // private members
  //========================================================================
 private:
  XferDataItem *m_xferDataItem;
};

#endif
