
#include "utils/Separator.h"
#include "datapool/DataVector.h"
#include "datapool/DataValue.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataVector);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataVector::DataVector(){
  BUG_INCR_COUNT;
}

DataVector::~DataVector(){
  BUG_DECR_COUNT;
  clearVector();
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* appendValue --                                                              */
/* --------------------------------------------------------------------------- */

void DataVector::appendValue( DataValue::Ptr &val ){
  push_back( val );
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataVector::isValid( int pos ) const{
  if( pos < (int)size() ){
    if( at( pos ).is_valid() ){
      return at( pos )->isValid();
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataValue::Ptr DataVector::getValue( int pos ) const{
  if( pos >= (int)size() ){
    return 0;
  }
  return at( pos );
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void DataVector::setValue( int pos, DataValue::Ptr &val, bool do_resize ){
  assert( pos >= 0 );
  if( do_resize ){
    resizeVector( pos + 1 );
  }
  assert( pos < (int)size() );
  at( pos ) = val;
}

/* --------------------------------------------------------------------------- */
/* deleteValue --                                                              */
/* --------------------------------------------------------------------------- */

void DataVector::deleteValue( int pos ){
  if( pos < (int)size() ){
    at( pos ) = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* resizeVector --                                                             */
/* --------------------------------------------------------------------------- */

void DataVector::resizeVector( int sz ){
  if( (int)size() < sz ){
    resize( sz );
  }
}

/* --------------------------------------------------------------------------- */
/* crunchVector --                                                             */
/* --------------------------------------------------------------------------- */

int DataVector::crunchVector(){
  int sz = size();

  for( reverse_iterator it = rbegin(); it != rend(); ++it ){
    if( (*it).is_valid() ){ // Der Pointer ist nicht <null>
      if( (*it)->isValid() ){
        return sz;
      }
      *it = 0;
    }
    sz--;
  }
  // Es gab keine gültigen Werte
  return 0;
}

/* --------------------------------------------------------------------------- */
/* clearVector --                                                              */
/* --------------------------------------------------------------------------- */

void DataVector::clearVector(){
  clear();
}

/* --------------------------------------------------------------------------- */
/* packVector --                                                               */
/* --------------------------------------------------------------------------- */

bool DataVector::packVector(){
  iterator itsrc = begin();
  if( firstInvalidValue( itsrc ) ){
    iterator itdest = itsrc;

    while( itsrc != end() ){
      if( deleteInvalidValues( itsrc ) ){
        moveValidValues( itsrc, itdest );
      }
    }
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* firstInvalidValue --                                                        */
/* --------------------------------------------------------------------------- */

bool DataVector::firstInvalidValue( iterator &it ){
  for( ; it != end(); ++it ){
    if( (*it).isnt_valid() ){
      return true; // gefunden
    }
    if( !(*it)->isValid() ){
      return true; // gefunden
    }
  }
  return false; // keine ungültigen Einträge
}

/* --------------------------------------------------------------------------- */
/* deleteInvalidValues --                                                      */
/* --------------------------------------------------------------------------- */

bool DataVector::deleteInvalidValues( iterator &it ){
  for( ; it != end(); ++it ){
    if( (*it).is_valid() ){
      if( (*it)->isValid() ){
        return true; // Es gibt noch gültige Einträge
      }
      *it = 0;
    }
  }
  return false; // keine gültigen Einträge mehr
}

/* --------------------------------------------------------------------------- */
/* moveValidValues --                                                          */
/* --------------------------------------------------------------------------- */

void DataVector::moveValidValues( iterator &itsrc, iterator &itdest ){
  while( itsrc != end() ){
    if( (*itsrc).isnt_valid() ){
      return;
    }
    if( (*itsrc)->isInvalid() ){
      return;
    }
    assert( (*itdest).isnt_valid() );
    *itdest = *itsrc;
    *itsrc = 0;
    itsrc++;
    itdest++;
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataVector::print( std::ostream &ostr ) const{
  Separator sep( '[', ',', ']' );
  const_iterator i;
  for( i = begin(); i != end(); ++i ){
    std::cout << sep;
    if( (*i).is_valid() ){
      (*i)->print( ostr );
    }
    else{
      std::cout << "empty";
    }
    std::cout << std::endl;
  }
  ostr << sep.close();
}
