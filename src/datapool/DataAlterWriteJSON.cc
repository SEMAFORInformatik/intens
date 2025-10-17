
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterWriteJSON.h"
#include "datapool/DataReference.h"
#include "utils/utils.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterWriteJSON::DataAlterWriteJSON( std::ostream &ostr,
                                        int level,
                                        int indent,
                                        const SerializableMask flags,
                                        bool omit_first_indent )
  : m_ostr( ostr )
  , m_is_scalar( false )
  , m_is_struct( false )
  , m_level( level )
  , m_indentation( indent )
  , m_omit_first_indent( omit_first_indent )
  , m_serialize_flags( flags )
  , m_wildcards_prev_level( false )
  , m_single_element( true )
  , m_elements_written( 0 )
  , m_last_level_item( false )
  , m_max_elements( 0 ){
}

DataAlterWriteJSON::~DataAlterWriteJSON(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterWriteJSON::check( DataReference &ref ){
  m_is_scalar = ref.userAttr().IsScalar();
  m_is_struct = ref.getDataType() == DataDictionary::type_StructVariable;
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterWriteJSON::alterItem( DataItem &item
                                  , UpdateStatus &status
                                  , DataInx &inx
                                  , bool final )
{
  BUG_DEBUG("Begin of DataAlterWriteJSON::alterItem");

  status = NoUpdate;

  if( final ){
    BUG_DEBUG("End of DataAlterWriteJSON::alterItem: indexed range does not exist." );
    return false;
  }

  m_last_level_item = inx.isLastLevel();

  if( m_last_level_item ){
    if( m_wildcards_prev_level ){
      // Bei Wildcards nicht auf dem letzten Level werden die Werte immer als
      // Vektor in Klammern ausgegeben, also ist es kein <single element>.
      m_single_element = false;
    }
    else{
      if( item.isScalar() ){
        // Bei <scalar items> werden Wildcards auf dem letzten Level ignoriert.
        // Es handelt sich immer um ein <single element>.
        m_single_element = true;
      }
      else{
        if( inx.isIndexListEmpty() ){
          // Keine Indizes auf dem letzten Level.
          m_single_element = false;
        }
        else
        if( inx.hasWildcards() ){
          // Wildcards nur auf dem letzten Level.
          m_single_element = false;
        }
        else{
          // Wert ist exakt adressiert.
          m_single_element = true;
        }
      }
    }
  }
  else{
    // Wir befinden uns noch NICHT auf dem letzten Level.

    if( inx.isIndexListEmpty() ){
      // Wenn auf diesem Level keine Indizes vorhanden sind, wird
      // nur der Index 0 bearbeitet.
      inx.newIndex( 0 );
    }

    if( inx.hasWildcards() ){
      m_wildcards_prev_level = true;
    }
  }

  m_max_elements = 0;
  BUG_DEBUG("End of DataAlterWriteJSON::alterItem: continue");
  return true;
}

/* --------------------------------------------------------------------------- */
/* startWithWildcard --                                                        */
/* --------------------------------------------------------------------------- */

void DataAlterWriteJSON::startWithWildcard( int wildcard_num, int max_elements ){
  BUG_DEBUG( "DataAlterWriteJSON::startWithWildcard" );

  if( !m_sep_stack.empty() ){
    m_ostr << m_sep_stack.top();
  }

  m_sep_stack.push( Separator( 0, ',', ']' ) );
  // Die öffnende Klammer wird bereits jetzt ausgegeben. Der erste Aufruf des
  // Separators wird nichts ausgeben (siehe obiger Constructor).
  if( m_elements_written ){
    m_ostr << std::endl;
  }
  if( !m_omit_first_indent ){
    indent(m_level * m_indentation, m_ostr);
  }
  m_ostr << "[";
  m_omit_first_indent = false;
  m_level++;
  m_elements_written++;

  m_max_elements = max_elements;
}

/* --------------------------------------------------------------------------- */
/* endWithWildcard --                                                          */
/* --------------------------------------------------------------------------- */

void DataAlterWriteJSON::endWithWildcard( int wildcard_num ){
  BUG_DEBUG("DataAlterWriteJSON::endWithWildcard");

  if( m_sep_stack.empty() ){
    BUG_DEBUG("FATAL: no separator available ?!?");
  }

  m_ostr << std::endl;
  m_level--;
  indent(m_level * m_indentation, m_ostr) << m_sep_stack.top().close();
  m_sep_stack.pop();
}

/* --------------------------------------------------------------------------- */
/* startOneElement --                                                          */
/* --------------------------------------------------------------------------- */

void DataAlterWriteJSON::startOneElement(){
  BUG_DEBUG("Begin of DataAlterWriteJSON::startOneElement");

  if( !m_sep_stack.empty() ){
    m_ostr << m_sep_stack.top();
  }

  m_sep_stack.push( Separator( 0, ',', ']' ) );

  if( !m_last_level_item ){
    // Es werden keine Klammern benötigt.
    m_sep_stack.top().close();
    BUG_DEBUG("End of DataAlterWriteJSON::startOneElement: not last level" );
    return;
  }

  if( m_single_element ){
    // Es werden keine Klammern benötigt.
    m_sep_stack.top().close();
  }
  else{
    if( m_elements_written ){
      m_ostr << std::endl;
    }
    m_ostr << "[";
                // a list is an element, even if it is empty
                // -> otherwise JSONStreamParameter::write will add an empty element
    m_level++;
    m_elements_written++;
    m_omit_first_indent = false;
  }
  m_max_elements = 1;
  BUG_DEBUG("End of DataAlterWriteJSON::startOneElement");
}

/* --------------------------------------------------------------------------- */
/* endOneElement --                                                            */
/* --------------------------------------------------------------------------- */

void DataAlterWriteJSON::endOneElement(){
  BUG_DEBUG("DataAlterWriteJSON::endOneElement");

  if( m_sep_stack.empty() ){
    BUG_DEBUG( "FATAL: no separator available ?!?" );
  }

  if( !m_single_element && m_level > 0 ){
    m_ostr << std::endl;
  }
  m_level--;
  if( !m_omit_first_indent ){
    indent(m_level * m_indentation, m_ostr);
  }
  m_ostr << m_sep_stack.top().close();
  m_sep_stack.pop();
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterWriteJSON::UpdateStatus DataAlterWriteJSON::alterData( DataElement &el ){
  BUG_DEBUG("Begin of DataAlterWriteJSON::alterData");

  /*
  std::cout << "m_single_element.. " << m_single_element << std::endl
            << "m_elements_written " << m_elements_written << std::endl
            << "m_last_level_item. " << m_last_level_item << std::endl
            << "m_max_elements.... " << m_max_elements << std::endl;
  */

  if( el.isDataElement() ){
    BUG_DEBUG("ein DataElement");
    if( !el.isValid() ){
      BUG_DEBUG("Max Elements = " << m_max_elements);
      if( m_max_elements == 1 ){
        // Wenn genau ein Element erwartet wird und dieses ungültig ist,
        // wird es nicht ausgegeben. Die Liste bleibt leer!
        BUG_DEBUG("End of DataAlterWriteJSON::alterData: DataElement ungültig");
        return NoUpdate;
      }
    }
  }

  if( m_single_element ){
    BUG_DEBUG( "kein separator <single element>" );
  }
  else{
    if( m_sep_stack.empty() ){
      BUG_DEBUG( "kein separator (stack is empty)" );
    }
    else{
      BUG_DEBUG( "mit separator" );
      m_ostr << m_sep_stack.top() << std::endl;
    }
  }

  if( !m_omit_first_indent ){
    indent(m_level * m_indentation, m_ostr);
  }

  el.writeJSON( m_ostr, false, m_level, m_indentation, false, 1, m_serialize_flags );
  m_elements_written++;

  BUG_DEBUG("End of DataAlterWriteJSON::alterData");
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* nullElement --                                                              */
/* --------------------------------------------------------------------------- */

void DataAlterWriteJSON::nullElement( DataInx &inx, int num_dim ){
  if( inx.isLastLevel() ){
    m_ostr << m_sep_stack.top() << std::endl;
    indent(m_level * m_indentation, m_ostr);
    if( num_dim > 1 ){
      m_ostr << "[]";
    }
    else{
      m_ostr << "null";
    }
  }
}

/* --------------------------------------------------------------------------- */
/* epilog --                                                                   */
/* --------------------------------------------------------------------------- */

void DataAlterWriteJSON::epilog( bool result ){
  if( !hasElementsWritten() ){
    if( m_is_scalar ){
      if( m_is_struct ){
        m_ostr << "{}";
      }
      else{
        m_ostr << "null";
      }
    }
    else{
      // a list
      m_ostr << "[]";
    }
    m_elements_written++;
  }
}
