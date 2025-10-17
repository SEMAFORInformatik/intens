
#include <string>
#include <typeinfo>

#include "utils/Debugger.h"
#include "streamer/BasicStream.h"
#include "streamer/IndexParameter.h"
#include "streamer/TableStreamGroup.h"
#include "streamer/DataStreamParameter.h"
#include "parser/IdManager.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

TableStreamGroup::TableStreamGroup( char delimiter, StreamParameterGroup *parent )
  : StreamParameterGroup(parent, delimiter)
  , m_index(0)
  ,m_hasExplicitIndex(true) {
}

TableStreamGroup::~TableStreamGroup(){}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool TableStreamGroup::read( std::istream &is ){
  BUG(BugStreamer, "TableStreamGroup::read(istream &)" );
  ParameterList::iterator I;

  if( m_index != 0 && !m_hasExplicitIndex && !m_pending ){
    m_index -> resetIndex();
  }
  bool finished( false );
  do{
    for( I=begin(); I!=end(); ++I ){
      if( !((*I) -> read( is )) ){
        finished = true;
        break;
      }
    }
    if( !finished && m_index != 0 && !m_hasExplicitIndex ){
      m_index -> inc();
    }
  } while( !finished );
  if( m_continuousUpdate ){
    if( is.peek() == EOF )
      m_pending = true;
    else
      m_pending = false;
  }
  return !( m_pending );
}

/* --------------------------------------------------------------------------- */
/* addIndex --                                                                 */
/* --------------------------------------------------------------------------- */

IndexParameter * TableStreamGroup::addIndex( const std::string &ident
					     , int length ){
  if( m_index == 0 )
    m_index = new IndexParameter( ident, length );
  addParameter(m_index);
  return m_index;
}

/* --------------------------------------------------------------------------- */
/* findIndex --                                                                */
/*    returns the StreamParameterGroup that has an index named 'indexname'     */
/* --------------------------------------------------------------------------- */

StreamParameterGroup *TableStreamGroup::findIndex( const std::string &indexname ){
  if( m_index != 0 ){
    if( m_index -> getName() == indexname )
      return this;
  }
  StreamParameterGroup *p=getParent();
  if( p!=0 )
    return p->findIndex( indexname );
  return 0;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool TableStreamGroup::write( std::ostream &os ) {
  BUG(BugStreamer, "write(ostream &)" );

  bool finished=false;
  bool validRow;
  ParameterList::const_iterator I;
  size_t maxrows=1;

  if( m_index != 0 ){
    BUG_MSG( "call resetIndex" );
    m_index -> resetIndex();
    BUG_MSG( "returned from resetIndex" );
    if( m_format != HardCopyListener::LaTeX )
      maxrows = m_index ->getSize();
    else
      maxrows = m_index ->getMaxIndex();

    BUG_MSG( "index=" << m_index << ", maxrows=" << maxrows );
  }
  else{
    BUG_MSG( "no index" );
  }

  if( m_format == HardCopyListener::LaTeX ){
    BUG_MSG( "LaTeX" );
  }
  //  cerr << "MaxRows=" << maxrows << endl;
  for(int r=0; r<maxrows && !finished; ++r ){
    // check validity of row
    //
    validRow=true;
    if( m_hasExplicitIndex ){ // check validity only if row has explicit index
      for( I=begin(); I!=end(); ++I ){
	if( !((*I) -> isValid( )) ){
	  validRow=false;
	  break;
	}
      }
    }
    if( validRow || m_format == HardCopyListener::LaTeX ){
      for( I=begin(); I!=end(); ++I ){
	//	cerr << (*I) -> getName() << endl;
	if( !((*I) -> write( os )) ){
	  finished=true;
	  break;
	}
      }
    }
    if( m_index != 0 ){
      m_index -> inc();
    }
  }
  if( m_index != 0 ){
    BUG_MSG( "call resetIndex (writing done)" );
    m_index -> resetIndex();
    BUG_MSG( "returned from resetIndex (writing done)" );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool TableStreamGroup::isValid(bool validErrorMsg){
  BUG( BugStreamer, "isValid");

  if (size()) {
    // pass query to childs
    ParameterList::const_iterator I;
    for( I=begin(); I!=end(); ++I ){
      if( ((*I) -> isValid(validErrorMsg)) ){
	BUG_EXIT("TRUE");
	return true;
      }
    }
    // is it really invalid?
    // check if 'write'-method has no valid output
    std::ostringstream os;
    write(os);
    size_t pos;
    std::string s = os.str();
    while ((pos = s.find( m_delimiter)) != std::string::npos) {
      s=s.erase(pos, 1);
    }
    if (s.size() > 0) {
      BUG_EXIT("TRUE (empty output stream)");
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getInvalidItems                                                             */
/* --------------------------------------------------------------------------- */

bool TableStreamGroup::getInvalidItems( std::vector< std::string > & invaliditems ) {
  bool has_invalids=false;
  ParameterList::const_iterator I;
  size_t maxrows=1;

  BUG(BugStreamer, "TableStreamParameter::getInvalidItems()" );
  if( m_index != 0 ){
    m_index -> resetIndex();
  }
  for( I=begin(); I!=end(); ++I ){
    if( !((*I) -> getInvalidItems(invaliditems)) ){
      has_invalids=true;
    }
  }
  return has_invalids;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void TableStreamGroup::putValues( StreamDestination *dest, bool transposed ){
  StreamParameterGroup::putValues( dest, transposed );
}

/* --------------------------------------------------------------------------- */
/* getValues --                                                                */
/* --------------------------------------------------------------------------- */

void TableStreamGroup::getValues( StreamSource *src ){
  StreamParameterGroup::getValues( src );
}

/* --------------------------------------------------------------------------- */
/* addIndexedParameter --                                                      */
/* --------------------------------------------------------------------------- */

void TableStreamGroup::addIndexedParameter( const std::string &indexname
					    , DataStreamParameter *d ){
  if( m_index == 0 ){
    m_hasExplicitIndex = false;
    m_index = new IndexParameter( indexname, 0 );
  }
  m_index -> addIndexedParameter( d );
}

/* --------------------------------------------------------------------------- */
/* removeIndexedParameter --                                                   */
/* --------------------------------------------------------------------------- */

void TableStreamGroup::removeIndexedParameter( DataStreamParameter *d ){
  if( m_index == 0 )
    return;
  m_index -> removeIndexedParameter( d );
  if( m_index -> isEmpty() ){
    delete m_index;
    m_index = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* resetContinuousUpdate --                                                    */
/* --------------------------------------------------------------------------- */

void TableStreamGroup::resetContinuousUpdate(){
  m_pending = false;
  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I){
    (*I)->resetContinuousUpdate();
  }
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void TableStreamGroup::marshal( std::ostream &os ){
  os << "<TableStreamGroup>\n";
  ParameterList::const_iterator I;
  for( I=begin(); I!=end(); ++I ){
    (*I) ->  marshal( os );
  }
  os << " delimiter=\"" << m_delimiter << "\"";
  os << "</TableStreamGroup>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *TableStreamGroup::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "/TableStreamGroup" ){
    linkWithIndex();
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void TableStreamGroup::setText( const std::string &text ){
}

/* --------------------------------------------------------------------------- */
/* linkWithIndex --                                                            */
/* --------------------------------------------------------------------------- */

bool TableStreamGroup::linkWithIndex( StreamParameter *p ){
  BUG( BugStreamer, "linkWithIndex" );

  bool has_var_indexes=false;
  if( p==0 ){
    BUG_EXIT( "no StreamParameter" );
    return false;
  }
  if( p -> getNumIndexes() > 0 ){
    for( int i=0; i<p -> getNumIndexes(); ++i ){
      std::string index_name=p -> getIndexName(i);
      if ( index_name.size() > 0 ){
	has_var_indexes=true;
	if( IdManager::Instance().getType( index_name ) != IdManager::id_Index ){
	  StreamParameterGroup *pgroup=findIndex( index_name );
	  if( pgroup == 0 ){ // index not found, let's find the top most parent
	    StreamParameterGroup *pg=this;
	    do {
	      pgroup = pg;
	      pg = pg -> getParent();
	    } while( pg != 0 && !pg->hasIndex() && typeid( *pg ) == typeid( TableStreamGroup ) );
	  }
	  if( pgroup ){
	    // it must be a DataStreamParameter
	    assert( dynamic_cast<DataStreamParameter*>(p) != 0 );
	    pgroup -> addIndexedParameter( index_name, dynamic_cast<DataStreamParameter*>(p) );
	  }
	}
      }
    }
  }
  return has_var_indexes;
}

/* --------------------------------------------------------------------------- */
/* linkWithIndex --                                                            */
/* --------------------------------------------------------------------------- */

bool TableStreamGroup::linkWithIndex(){
  bool has_variable_index=false;
  ParameterList::iterator I;
  for( I=begin(); I!=end(); ++I ){
    if( linkWithIndex( *I ) )
      has_variable_index=true;
  }
  return has_variable_index;
}
