
#include <algorithm>

#include "job/JobIncludes.h"

#include "job/JobVarDataReference.h"
#include "job/JobEngine.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataString.h"
#include "job/JobStackDataStructure.h"
#include "datapool/DataReference.h"
#include "gui/GuiManager.h"
#include "app/DataPoolIntens.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobVarDataReference::JobVarDataReference(bool isJobVar, bool isJobParentVar)
  : JobDataReference()
  , m_data_root( 0 )
  , m_level_offset( 0 )
  , m_isJobVar(isJobVar)
  , m_isJobParentVar(isJobParentVar) {
}

JobVarDataReference::JobVarDataReference( JobDataReference &dataref )
  : JobDataReference( dataref )
  , m_data_root( 0 )
  , m_level_offset( 0 )
  , m_isJobVar(true)
  , m_isJobParentVar( false ) {
  m_data_root = m_data;
  m_data = 0;
}

JobVarDataReference::~JobVarDataReference(){
  if( m_data_root != 0 ){
    delete m_data_root;
  }
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* convertToVar --                                                             */
/* --------------------------------------------------------------------------- */

JobDataReference *JobVarDataReference::convertToVar( JobDataReference *ref ){
  JobVarDataReference *varref = new JobVarDataReference( *ref );
  delete ref;
  return varref;
}

/* --------------------------------------------------------------------------- */
/* convertToParentDataReference --                                             */
/* --------------------------------------------------------------------------- */

bool JobVarDataReference::convertToParentDataReference( DataReference *ref ){
  if( ref == 0 ){
    return false;
  }
  if( m_isJobParentVar ){
    return ref->ConvertToParentReference();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* updateDataReference --                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobVarDataReference::updateDataReference( JobEngine *eng ){
  BUG(BugJobCode,"JobVarDataReference::updateDataReference");
  IndexList::iterator i;
  IndexList::reverse_iterator ri;

  typedef std::vector<JobStackDataPtr> StackDataList;
  StackDataList datalist;
  StackDataList::reverse_iterator di;

  int index = 0, level = 0, value = 0;
  m_valid = true;
  IndexList  indices;

  if (m_isJobParentVar) { BUG_MSG( "is Parent" ); }
  if( m_data_root ){ BUG_MSG( "Root==" << m_data_root->fullName() ); }

  // als erstes wird der DataReference-Pointer initialisiert. In der nun folgenden
  // Schlaufe wird er anhand der Stack-Variablen neu generiert.
  // -----------------------------------------------------------------------------
  if( m_data != 0 ){
    delete m_data;
    m_data = 0;
  }

  // Eine THIS Variable landet auch in einem JobVarDataReference Objekt. Sie hat
  // jedoch unter Umständen keine Index-Liste. Dies ist bei VAR() Funktionen nicht
  // möglich. Wir handeln kurz und bündig.
  // -----------------------------------------------------------------------------
  if( m_index_list.empty() ){
    if( m_data_root != 0 ){
      m_data = DataPool::newDataReference( *m_data_root );
      m_valid = true;
    }
    else{
      m_valid = false;
    }
    return op_Ok;
  }

  // Nun werden alle für uns bestimmten Daten vom Stack abgeholt und in der tem-
  // porären Indexliste oder in der DataReference m_data abgelegt.
  // ---------------------------------------------------------------------------
  for( ri = m_index_list.rbegin(); ri != m_index_list.rend(); ++ri ){
    BUG_MSG("index_list #" << (*ri) );
    JobStackDataPtr dat( eng->pop() );
    if( dat.isnt_valid() ) return op_FatalError;

    if( (*ri) < 0 ){
      // Ein Variablenname !! Die DataReference m_data wird erst später erstellt.
      // Die Stack-Elemente werden zwischengelagert.
      datalist.push_back( dat );
    }
    else{
      if( m_valid ){
	// Falls das Objekt nicht mehr gültig ist, ist die Sache gelaufen.
	if( !dat->getIntegerValue( value ) ){
	  m_valid = false;
	}
	else{
	  indices.push_back( value );
	}
      }
    }
  }

  assert( !datalist.empty() );

  if( m_valid ){
    // Nun wird die DataReference generiert.
    // -------------------------------------
    std::string name;
    DataReference *data=0;
    for( di = datalist.rbegin(); di != datalist.rend() && m_valid; ++di ){
      // special fall: Struct
      if ((*di)->getDataType() == DataDictionary::type_StructVariable) {
	DataReference *ref = 0;
	if( (*di)->getStructureValue( ref ) ){
	  data = DataPoolIntens::getDataReference( ref->fullName(true) );
delete ref;
	}
	m_data_root = data;
	if( !convertToParentDataReference( m_data_root ) ){// nur fuer Job PARENT(...)
	  if (m_data_root) {
	    BUG_WARN( "DataReference convert to parent failed [" <<  m_data_root->fullName() << "]" );
	  }
	}
      }
      else if( (m_valid = (*di)->getStringValue( name )) ){
	// check endsWith "." => invalid reference
	if (name.size() && *(name.rbegin()) == '.') {
	  BUG_EXIT("no valid Reference generated");
	  m_valid = false;
	  return op_Ok;
	}

	if( m_data == 0 ){
	  if( m_data_root == 0 ){
	    data = DataPoolIntens::getDataReference( name );
	    if( convertToParentDataReference( data ) ){ // nur fuer Job PARENT(...)
	      if (data) {
		BUG_WARN( "DataReference convert to parent failed [" <<  data->fullName() << "]" );
	      }
	    }
	  }
	  else{
            BUG_MSG("data_root: " << m_data_root->fullName( true ) << " + " << name );
	    data = DataPoolIntens::getDataReference( m_data_root, name );
	    if( data == 0 ){
	      BUG_WARN( "DataPoolIntens::getDataReference() failed. "
                  << "data_root: " << m_data_root->fullName( true ) << " + " << name);
	    }
	  }
	}
	else{
    BUG_MSG("data: " << m_data->fullName( true ) << " + " << name );
	  data = DataPoolIntens::getDataReference( m_data, name );
	  if( data == 0 ){
	    BUG_WARN( "DataPoolIntens::getDataReference() failed. "
                << "data_root: "
                << (m_data_root ? m_data_root->fullName( true ) : "") << " + " << name);
	  }
	  delete m_data;
	}
	m_data = data;
	if( m_data == 0 ){
	  m_valid = false;
	}
      }
    }
  }

  // Die Stack-Elemente für das Generieren der DataReference werden gelöscht
  // -----------------------------------------------------------------------
  datalist.clear();

  if( !m_valid ){
    BUG_EXIT("no valid Reference generated");
    return op_Ok;
  }

  // Als letztes werden die Indizes gesetzt
  // --------------------------------------
  ri = indices.rbegin();

  // Korrektur fuer JobParentVar z.B: PARENT(PARENT(THIS))
  if (m_isJobParentVar && !m_data) {
    m_data =  m_data_root;
  }

  int _level_offset = m_level_offset;
  for( i = m_index_list.begin(); i != m_index_list.end(); ++i ){
    // Korrektur level_offset bei JobVar
    if( m_isJobVar && (*i) < 0 ) {
      _level_offset =m_data->GetNumberOfLevels()-1-std::count(i+1, m_index_list.end(), -1);
      IndexList::iterator n = i;
      while (n!= m_index_list.end() && (*n) == -1 ) ++n;
      if (n != m_index_list.end()) _level_offset -= (*n);
    }

    if( (*i) >= 0 ){
      if( level != _level_offset + (*i) ){
	level = _level_offset + (*i);
	index = 0;
      }
      BUG_MSG("SetDimensionIndexOfLevel( " << (*ri) << " , " << level << " , " << index << " )" );
      m_data->SetDimensionIndexOfLevel( (*ri), level, index );
      index++;
      ++ri;
    }
  }
#if defined(_DEBUG)
// verursacht Absturz
//   if( m_data->GetItemValid_PreInx() )
//     BUG_EXIT( m_data->fullName( true ) << " is valid");
//   else
//     BUG_EXIT( m_data->fullName( true ) << " is NOT valid");
#endif
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setDataRoot --                                                              */
/* --------------------------------------------------------------------------- */

void JobVarDataReference::setDataRoot( DataReference *ref ){
  BUG(BugJobCode,"JobVarDataReference::setDataRoot");
  if( m_data_root != 0 ){
    delete m_data_root;
    m_data_root = 0;
    m_level_offset = 0;
  }
  if( ref->RefValid() ){
    m_data_root = ref;
    m_level_offset = ref->GetNumberOfLevels() -1;
  }
  else{
    m_valid = false;
  }
}
