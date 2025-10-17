
#if !defined(XML_INTEGER_VECTOR_H)
#define XML_INTEGER_VECTOR_H

#include <assert.h>

#include "xml/XMLElement.h"
#include "xml/XMLVector.h"
#include "datapool/DataVector.h"

class XMLIntegerVector : public XMLElement
                       , public XMLVector
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XMLIntegerVector( XMLElement *parent, XferDataItem *xfer );
  virtual ~XMLIntegerVector();

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
