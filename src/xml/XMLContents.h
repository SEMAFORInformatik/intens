
#if !defined(XML_CONTENTS_H)
#define XML_CONTENTS_H

#include "xml/XMLElement.h"

class XMLContents : public XMLElement{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLContents( XMLElement *parent, bool isRoot = false )
    : XMLElement( parent )
    , m_isValue( false )
    , m_isRoot( isRoot ){}

  //========================================================================
  // public functions of XMLElement
  //========================================================================
 public:
  virtual XferDataItem *getXferDataItem(){ return m_parent->getXferDataItem(); }
  //  virtual void characters( const std::string &value ){ m_value = value; m_isValue = true; }
  virtual void end();
  virtual XMLElement *newElement( const std::string &element );
 private:
  bool getValue( std::string &value ){ value = m_value; return m_isValue; }
  //========================================================================
  // private members
  //========================================================================
 private:
  std::string m_value;
  bool        m_isValue;
  bool        m_isRoot;
};

#endif
