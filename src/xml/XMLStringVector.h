
#if !defined(XML_STRING_VECTOR_H)
#define XML_STRING_VECTOR_H

#include <assert.h>

#include "xml/XMLElement.h"
#include "xml/XMLVector.h"
#include "datapool/DataVector.h"

class XMLStringVector : public XMLElement
                      , public XMLVector
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XMLStringVector( XMLElement *parent, XferDataItem *xfer );
  virtual ~XMLStringVector();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void setValue( const std::string &value );
  virtual XferDataItem *getXferDataItem();
  virtual XMLElement *newElement( const std::string &element );
  virtual void end();
};

#endif
