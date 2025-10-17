
#if !defined(XML_VALUE_H)
#define XML_VALUE_H

#include "xml/XMLElement.h"

class XMLValue : public XMLElement{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLValue( XMLElement *parent )
    : XMLElement( parent )
    , m_isValue( false ){}

  //========================================================================
  // public functions of XMLElement
  //========================================================================
 public:
  virtual XferDataItem *getXferDataItem(){ return m_parent->getXferDataItem(); }
  virtual void characters( const std::string &value );
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
};

#endif
