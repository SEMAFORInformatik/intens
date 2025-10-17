
#include <sstream>

#include <limits>
#include <qstring.h>

#include "utils/Debugger.h"
#include "datapool/DataRealValue.h"
#include "xml/XMLValue.h"
#include "XMLRealVector.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XMLRealVector::XMLRealVector( XMLElement *parent, XferDataItem *xfer )
  : XMLElement( parent )
  , XMLVector( xfer ){
  m_type = e_Vector;
}

XMLRealVector::~XMLRealVector(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void XMLRealVector::setValue( const std::string &value ){
  if( ignoreValues() ){
    return;
  }

#ifdef HAVE_QT
  bool b = false;
  double d = QString( value.c_str() ).toDouble(&b);
#else
  // no Qt -> no QApplication (which calls setlocale)
  // -> locale LC_NUMERIC is 'C' -> sscanf is ok
  double d = 0;
  bool b = sscanf(value.c_str(),"%lf",&d) == 1;
#endif
  if( b ){
    DataValue *data = new DataRealValue( d );
    insertValue( m_elements-1, data );
  }
}

/* --------------------------------------------------------------------------- */
/* getXferDataItem --                                                          */
/* --------------------------------------------------------------------------- */

XferDataItem *XMLRealVector::getXferDataItem(){
  return getDataItem();
}

/* --------------------------------------------------------------------------- */
/* end --                                                                      */
/* --------------------------------------------------------------------------- */

void XMLRealVector::end(){
  if( m_type == e_Matrix ){
    return;
  }
  setValues( ignoreValues() );
}

/* --------------------------------------------------------------------------- */
/* newElement --                                                               */
/* --------------------------------------------------------------------------- */

XMLElement *XMLRealVector::newElement( const std::string &element ){
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
    return new XMLRealVector( this, newDataItem() );
  }

  return new XMLElement( this );
}
