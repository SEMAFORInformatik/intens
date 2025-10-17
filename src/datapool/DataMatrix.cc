
#include "utils/utils.h"
#include "DataMatrix.h"
#include "DataValue.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataMatrix);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataMatrix::DataMatrix(){
  BUG_INCR_COUNT;
}

DataMatrix::~DataMatrix(){
  BUG_DECR_COUNT;
  clearMatrix();
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* appendVector --                                                             */
/* --------------------------------------------------------------------------- */

DataVector *DataMatrix::appendVector(){
  DataVector *v = new DataVector;
  push_back( v );
  return v;
}

/* --------------------------------------------------------------------------- */
/* clearMatrix --                                                              */
/* --------------------------------------------------------------------------- */

void DataMatrix::clearMatrix(){
  for( iterator it = begin(); it != end(); it++ ){
    if( *it ){
      delete *it;
    }
  }
  clear();
}

/* --------------------------------------------------------------------------- */
/* packMatrixColumns --                                                        */
/* --------------------------------------------------------------------------- */

bool DataMatrix::packMatrixColumns(){
  bool rslt = false;
  for( iterator it = begin(); it != end(); it++ ){
    if( *it ){
      if( (*it)->packVector() ){
        rslt = true;
      }
    }
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* packMatrixRows --                                                           */
/* --------------------------------------------------------------------------- */

bool DataMatrix::packMatrixRows(){
  int col_size = 0;
  iterator it;
  for( it = begin(); it != end(); it++ ){
    if( *it ){
      col_size = maximum( col_size, (*it)->crunchVector() );
    }
  }

  if( col_size == 0 ){
    // Da gibt es keine gültigen Werte
    return false;
  }

  for( it = begin(); it != end(); it++ ){
    if( *it == 0 ){
      *it = new DataVector;
    }
    if( *it ){
      (*it)->resizeVector( col_size );
    }
    else{
      std::cout << "shit";
    }
  }

  bool rslt = false;
  for( int row = 0; row < col_size; row++ ){
    if( packOneRow( row ) ){
      rslt = true;
    }
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* packOneRow --                                                               */
/* --------------------------------------------------------------------------- */

bool DataMatrix::packOneRow( int row ){
  iterator itsrc = begin();
  if( firstInvalidValue( itsrc, row ) ){
    iterator itdest = itsrc;

    while( itsrc != end() ){
      if( deleteInvalidValues( itsrc, row ) ){
        moveValidValues( itsrc, itdest, row );
      }
    }
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* firstInvalidValue --                                                        */
/* --------------------------------------------------------------------------- */

bool DataMatrix::firstInvalidValue( iterator &it, int row ){
  for( ; it != end(); ++it ){
    if( !(*it)->isValid( row ) ){
      return true; // gefunden
    }
  }
  return false; // keine ungültigen Einträge
}

/* --------------------------------------------------------------------------- */
/* deleteInvalidValues --                                                      */
/* --------------------------------------------------------------------------- */

bool DataMatrix::deleteInvalidValues( iterator &it, int row ){
  for( ; it != end(); ++it ){
    if( (*it)->isValid( row ) ){
      return true; // Es gibt noch gültige Einträge
    }
    (*it)->deleteValue( row );
  }
  return false; // keine gültigen Einträge mehr
}

/* --------------------------------------------------------------------------- */
/* moveValidValues --                                                          */
/* --------------------------------------------------------------------------- */

void DataMatrix::moveValidValues( iterator &itsrc, iterator &itdest, int row ){
  while( itsrc != end() ){
    if( !(*itsrc)->isValid( row ) ){
      return;
    }
    DataValue::Ptr data( (*itsrc)->getValue( row ) );
    (*itdest)->setValue( row, data );
    (*itsrc)->deleteValue( row );
    itsrc++;
    itdest++;
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataMatrix::print( std::ostream &ostr ){
  for( iterator it = begin(); it != end(); it++ ){
    if( *it ){
      (*it)->print( ostr );
    }
    else{
      ostr << "<empty>" << std::endl;
    }
  }
}
