
#if !defined(XML_REAL_VECTOR_H)
#define XML_REAL_VECTOR_H

#include <assert.h>

#include "xml/XMLElement.h"
#include "xml/XMLVector.h"
#include "datapool/DataVector.h"

class XMLRealVector : public XMLElement
                    , public XMLVector
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XMLRealVector( XMLElement *parent, XferDataItem *xfer );
  virtual ~XMLRealVector();

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
