
#if !defined(XML_ELEMENT_H)
#define XML_ELEMENT_H

#include <string>

#include "datapool/DataDictionary.h"

class QTextCodec;
class XferDataItem;

class XMLElement{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLElement( XMLElement *parent );
  virtual ~XMLElement(){}

  //========================================================================
  // public functions
  //========================================================================
 public:
  void setIgnoreValues() { m_ignore = true; }
  bool ignoreValues() { return m_ignore; }
  virtual void setValue( const std::string &value ){}
  virtual void clearRange(){}
  virtual void characters( const std::string &value ){}
  virtual XferDataItem *getXferDataItem(){ return 0; }
  virtual void end(){}
  virtual XMLElement *newElement( const std::string &element );
  virtual XferDataItem *getXferDataItem( const std::string &element );
  XMLElement *getParent(){return m_parent;}
  bool getDataType( DataDictionary::DataType &type, const std::string &name );
  void clearDatapool( bool allCycles );

  //========================================================================
  // protected members
  //========================================================================
 protected:

  enum VECTOR_TYPE{
    e_Vector,
    e_Matrix
  };

  XMLElement         *m_parent;
  int                 m_elements;
  VECTOR_TYPE         m_type;
  bool                m_ignore;
};

#endif
