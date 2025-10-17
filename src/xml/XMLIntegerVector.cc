
#include <sstream>

#include <limits>
#ifdef HAVE_QT
#include <qstring.h>
#endif

#include "datapool/DataIntegerValue.h"
#include "xml/XMLValue.h"
#include "XMLIntegerVector.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XMLIntegerVector::XMLIntegerVector( XMLElement *parent, XferDataItem *xfer )
  : XMLElement( parent )
  , XMLVector( xfer ){
  m_type = e_Vector;
}

XMLIntegerVector::~XMLIntegerVector(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void XMLIntegerVector::setValue( const std::string & value ){
  if( ignoreValues() ){
    return;
  }

  int d = 0;
  bool b = sscanf(value.c_str(),"%d",&d) == 1;
  if( b ){
    DataValue *data = new DataIntegerValue( d );
    insertValue( m_elements-1, data );
  }
}

/* --------------------------------------------------------------------------- */
/* getXferDataItem --                                                          */
/* --------------------------------------------------------------------------- */

XferDataItem *XMLIntegerVector::getXferDataItem(){
  return getDataItem();
}

/* --------------------------------------------------------------------------- */
/* end --                                                                      */
/* --------------------------------------------------------------------------- */

void XMLIntegerVector::end(){
  if( m_type == e_Matrix ){
    return;
  }
  setValues( ignoreValues() );
}

/* --------------------------------------------------------------------------- */
/* newElement --                                                               */
/* --------------------------------------------------------------------------- */

XMLElement *XMLIntegerVector::newElement( const std::string &element ){
  if( element == "value" || element=="v" ){
    if( m_elements == 0 ){
      setLastIndexWildcard();
    }
    ++m_elements;
    return new XMLValue( this );
  }

  if( element == "vector" ){
    m_type = e_Matrix;
    setLastIndexLowerbound( m_elements );

    ++m_elements;
    return new XMLIntegerVector( this, newDataItem() );
  }

  return new XMLElement( this );
}
