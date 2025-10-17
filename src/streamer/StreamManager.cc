
#include <limits.h>
#include "streamer/Stream.h"
#include "streamer/StreamManager.h"
#include "plot/PSPlot.h"
#include "utils/FileUtilities.h"

#include <sstream>

INIT_LOGGER();

/* initialize */
StreamManager *StreamManager::s_instance = 0;


/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

StreamManager &StreamManager::Instance(){
  if( s_instance == 0 ){
    s_instance = new StreamManager;
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* newStream --                                                                */
/* --------------------------------------------------------------------------- */

Stream *StreamManager::newStream( const std::string &name ){
  assert( !name.empty() );
  StreamMap::iterator iter = m_streams.find( name );
  if( iter != m_streams.end() ){
    return 0;
  }
  Stream *stream = new Stream( name, true );
  m_streams.insert( StreamMap::value_type( name, stream ) );
  return stream;
}

/* --------------------------------------------------------------------------- */
/* newStream --                                                                */
/* --------------------------------------------------------------------------- */

Stream *StreamManager::newStream( bool check ){
  std::ostringstream name( "@stream" );
  name << m_streams.size() << "@";
  StreamMap::iterator iter = m_streams.find( name.str() );
  assert( iter == m_streams.end() );

  Stream *stream = new Stream( name.str(), check );
  m_streams.insert( StreamMap::value_type( name.str(), stream ) );
  return stream;
}

/* --------------------------------------------------------------------------- */
/* attachStream --                                                             */
/* --------------------------------------------------------------------------- */

void StreamManager::attachStream( BasicStream *stream ){
  std::ostringstream name( "@stream" );
  name << m_streams.size() << "@";
  StreamMap::iterator iter = m_streams.find( name.str() );
  assert( iter == m_streams.end() );

  m_streams.insert( StreamMap::value_type( name.str(), stream ) );
}

/* --------------------------------------------------------------------------- */
/* getStream --                                                                */
/* --------------------------------------------------------------------------- */

Stream *StreamManager::getStream( const std::string &name ){
  StreamMap::iterator iter = m_streams.find( name );
  if( iter != m_streams.end() ){
    // Wir gehen mal davon aus, dass es sich tatsächlich um ein Stream handelt !
    // Sonst........
    return (Stream*)(*iter).second;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* newBasicStream --                                                           */
/* --------------------------------------------------------------------------- */

PSPlot::PSStream *StreamManager::newPSStream( const std::string &name ){
  assert( !name.empty() );
  StreamMap::iterator iter = m_streams.find( name );
  if( iter != m_streams.end() ){
    return 0;
  }
  PSPlot::PSStream *stream = new PSPlot::PSStream( name );
  m_streams.insert( StreamMap::value_type( name, stream ) );
  return stream;
}

/* --------------------------------------------------------------------------- */
/* newBasicStream --                                                           */
/* --------------------------------------------------------------------------- */

PSPlot::PSStream *StreamManager::newPSStream(){
  std::ostringstream name( "@stream" );
  name << m_streams.size() << "@";
  StreamMap::iterator iter = m_streams.find( name.str() );
  assert( iter == m_streams.end() );

  PSPlot::PSStream *stream = new PSPlot::PSStream( name.str() );
  m_streams.insert( StreamMap::value_type( name.str(), stream ) );
  return stream;
}

/* --------------------------------------------------------------------------- */
/* getBasicStream --                                                           */
/* --------------------------------------------------------------------------- */

PSPlot::PSStream *StreamManager::getPSStream( const std::string &name ){
  StreamMap::iterator iter = m_streams.find( name );
  if( iter != m_streams.end() ){
    // Wir gehen mal davon aus, dass es sich tatsächlich um ein PSStream handelt !
    // Sonst........
    return (PSPlot::PSStream*)(*iter).second;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* deleteStream --                                                             */
/* --------------------------------------------------------------------------- */

void StreamManager::deleteStream( const std::string &name ){
  StreamMap::iterator si = m_streams.find( name );
  if( si != m_streams.end() ){
    delete (*si).second;
    m_streams.erase( si );
  }
}

/* --------------------------------------------------------------------------- */
/* deleteStream --                                                             */
/* --------------------------------------------------------------------------- */

void StreamManager::deleteStream( Stream *stream ){
  StreamMap::iterator si;
  for( si = m_streams.begin(); si != m_streams.end(); ++si ){
    if( (*si).second == stream ){
      m_streams.erase( si );
      delete stream;
      return;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* fixupAllItemStreams --                                                      */
/* --------------------------------------------------------------------------- */

void StreamManager::fixupAllItemStreams(){
  StreamMap::iterator si;
  for( si = m_streams.begin(); si != m_streams.end(); ++si ){
    (*si).second->fixupItemStreams();
  }
}

/* --------------------------------------------------------------------------- */
/* readInitFile --                                                             */
/* --------------------------------------------------------------------------- */

void StreamManager::readInitFile( const std::string &filename ){
  std::ifstream ifstr( filename.c_str() );

  initialize( ifstr );
  return;
}

/* --------------------------------------------------------------------------- */
/* initialize --                                                               */
/* --------------------------------------------------------------------------- */

void StreamManager::initialize( std::ifstream &ifs ){
  std::string tok;
  std::string line;
  std::istringstream istr;
  char c;
  bool pre_indexed, is_matrix;
  DataReference *item;
  int   row, col, numitems;
  double  r;

//   cerr << "StreamManager::initialize"<< endl;
  is_matrix = pre_indexed = false;
  while( std::getline( ifs, line ) ){
    std::istringstream istr( line );
    istr >> tok;
//     cerr << "'" << tok << "'\n";
    if (tok == "#"){
      pre_indexed = true;
      if (!(istr >> tok ) )
	return;
      if (tok == "#"){
	is_matrix = true;
	if (!(istr >> tok ) )
	  return;
      }
      else
	is_matrix = false;
    }
    else
      pre_indexed = false;


    if ((item = DataPoolIntens::getDataReference( tok )) == 0){
      std::cerr << "InitFile: Variable " << tok << " not found.\n";
    }
    else{
      numitems=1;
      col = -1;
      row = 0;
//       cerr << "Entering set value loop\n";
      do {
	if (pre_indexed){
	  if (!(istr >> col))
	    break;
	  if (is_matrix){
	    if (!(istr>>row) )
	      break;
	  }
	}
	else
	  col++;

	if( !(istr >> tok) ){
	  break;
	}
// 	cerr << "setting value '" << tok << "' row="<<row << " col=" << col << endl;
	switch( item->getDataType() ){
	case DataDictionary::type_String:
	  if( tok.size()>0 && tok[0] =='"' ){
	    std::string tmp=tok;
	    tmp.erase(0,1);
	    while( tok[tok.size()-1] != '"' &&  istr >> tok )
	      tmp += " " + tok;
	    tmp.erase( tmp.size()-1, 1 );
	    tok=tmp;
	  }
// 	  cerr << "setting value '" << tok << "' row="<<row << " col=" << col << endl;
	  item->SetValue( tok, row, col );
	  break;
	case DataDictionary::type_Integer:
	  item->SetValue( atoi(tok.c_str()), row, col );
	  break;
	case DataDictionary::type_Real:
	  {
	    std::istringstream is( tok  );
	    is >> r;
	    if ( !(is.rdstate() & std::ios::failbit) )
// 	  if (sscanf(tok.c_str(), "%lf", &r) == 1)
	      item->SetValue(r, row, col );
	    break;
	  }
	default:
	  break;
	}
	// ResetModified( item );
      }while( 1 );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* printTargetStreamInfo --                                                    */
/* --------------------------------------------------------------------------- */
void StreamManager::printTargetStreamInfo(std::ostream& os) {
  StreamMap::iterator si;
  for( si = m_streams.begin(); si != m_streams.end(); ++si ){
    os << "  - " << (*si).first << std::endl;
    (*si).second->printItemStreams(os);
    (*si).second->printTargetStreams(os);
  }
}

void StreamManager::lspWrite( std::ostream &ostr ){
  for (auto pair : m_streams) {
    auto name = pair.first;
    auto stream = pair.second;

    ostr << "<ITEM name=\"" << name << "\"";
    ostr << " stream=\"1\"";
    ostr << " file=\"" << stream->DefinitionFilename() << "\"";
    ostr << " line=\"" << stream->Lineno() << "\"";
    ostr << ">" << std::endl;
    ostr << "</ITEM>" << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* deleteFiles --                                                                */
/* --------------------------------------------------------------------------- */

void StreamManager::deleteFiles(){
  BasicStream::deleteFiles();
}
