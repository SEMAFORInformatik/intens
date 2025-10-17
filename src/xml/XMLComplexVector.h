
#if !defined(XML_COMPLEX_VECTOR_H)
#define XML_COMPLEX_VECTOR_H

#include <assert.h>

#include "xml/XMLElement.h"
#include "xml/XMLVector.h"
#include "datapool/DataVector.h"

class XMLComplexVector : public XMLElement
                       , public XMLVector
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XMLComplexVector( XMLElement *parent, XferDataItem *xfer );
  virtual ~XMLComplexVector();

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
