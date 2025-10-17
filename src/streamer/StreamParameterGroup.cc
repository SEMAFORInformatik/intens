
#include <string>
#include "utils/Debugger.h"
#include "app/AppData.h"
#include "streamer/StreamParameterGroup.h"
#include "streamer/JSONStreamParameter.h"
#include "streamer/TokenStreamParameter.h"

INIT_LOGGER();
#define __NEW_RAPID_JSON_PARSER__ 1

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

StreamParameterGroup::StreamParameterGroup(StreamParameterGroup *parent,
					   char delimiter)
  : StreamParameter(delimiter)
  , m_parent(parent)
  , m_pending( false )
  , m_end( false ){}

StreamParameterGroup::~StreamParameterGroup(){
  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    delete *I;
  }
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* addParameter --                                                             */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::addParameter( StreamParameter *p){
  m_parvec.push_back( p );
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::read( std::istream &is ){
  BUG_DEBUG( "Begin of StreamParameterGroup::read(istream &)" );
  if( m_end ){
    m_pending = false;
    BUG_DEBUG( "End of StreamParameterGroup::read(istream &): already ended" );
    return true;
  }

  // Ein Json-Stream funktioniert anders.
  if( m_parvec.empty() ){
    return true;
  }

#ifdef __NEW_RAPID_JSON_PARSER__
  if( m_parvec[0]->isJson() ){
    // Ein Json-Stream funktioniert anders.
    bool rslt = m_parvec[0]->readJson( is, m_parvec );
    BUG_DEBUG( "End #1 of StreamParameterGroup::read(istream &)" );
    return rslt;
  }
#endif

  ParameterList::iterator I;
  bool _do( true );

  for( I= m_pending ? m_currentParameter : m_parvec.begin(); I!=m_parvec.end() && _do; ){
    BUG_DEBUG( "---> call Parameter" );
    if( (*I) -> read( is ) || is.peek() != EOF )
      ++I;
    else
      _do = false;
    m_currentParameter = I;
  }

  if( m_continuousUpdate ){
    if( I != m_parvec.end() ){
      BUG_DEBUG( "* pending" );
      m_pending = true;
    }
    else{
      BUG_DEBUG( "* the end" );
      m_end = true;
    }
  }
  BUG_DEBUG( "End #2 of StreamParameterGroup::read(istream &)" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* resetContinuousUpdate --                                                    */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::resetContinuousUpdate(){
  BUG_DEBUG("resetContinuousUpdate");
  m_pending = false;
  m_end = false;
  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I){
    (*I)->resetContinuousUpdate();
  }
}

/* --------------------------------------------------------------------------- */
/* setContinuousUpdate --                                                      */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::setContinuousUpdate( bool flag ){
  m_continuousUpdate = flag;
  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I){
    (*I)->setContinuousUpdate( flag );
  }
  return true;
}
/* --------------------------------------------------------------------------- */
/* setAppendOption --                                                          */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::setAppendOption( bool flag ) {
  //  m_continuousUpdate = flag;
  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I){
    (*I)->setAppendOption( flag );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getValues --                                                                */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::getValues( StreamSource *src ){
  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    (*I) -> getValues( src );
  }
}

/* --------------------------------------------------------------------------- */
/* getInvalidItems                                                             */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::getInvalidItems( std::vector< std::string> &invaliditems ){
  std::vector< StreamParameter * >::iterator I;
  BUG(BugStreamer, "StreamParameterGroup::getInvalidItems()" );
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    (*I) -> getInvalidItems( invaliditems );
  }
  return !invaliditems.empty();
}

/* --------------------------------------------------------------------------- */
/* isValid                                                                     */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::isValid(bool validErrorMsg) {
  BUG(BugStreamer, "StreamParameterGroup::isValid()" );

  std::vector< StreamParameter * >::iterator I;
  bool ret(false);
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    if ((*I) -> isValid(validErrorMsg)) {
      if (validErrorMsg &&
          (dynamic_cast<TokenStreamParameter*>(*I) == 0)  // ignore token, as they are not cleared
          ) {
        // write label to error message stream
        // this is done to show what dependencies are cleared
        // in dependency clear confirm dialog
        std::string msg = (*I)->getLabel();
        BUG_INFO("target stream '" << msg << "' is valid");
        if (AppData::Instance().TargetStreamInfo())
          (*I) ->getErrorMessageStream() << "- " << msg << "\n";;
      }
      ret = true;
    }
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/* addIndex --                                                                 */
/* --------------------------------------------------------------------------- */

IndexParameter * StreamParameterGroup::addIndex(const std::string &ident
						, int length){
  return 0;
}

/* --------------------------------------------------------------------------- */
/* hasIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::hasIndex() const {
  return false;
}

/* --------------------------------------------------------------------------- */
/* findIndex --                                                                 */
/* --------------------------------------------------------------------------- */

StreamParameterGroup *StreamParameterGroup::findIndex( const std::string &indexname ){
  BUG_PARA( BugStreamer, "findIndex", "name=" << indexname );
  StreamParameterGroup *p=m_parent;
  if( p!=0 ){
    return p -> findIndex( indexname );
  }
  BUG_EXIT( "index not found" );
  return 0;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::write( std::ostream &os ){
  BUG(BugStreamer, "write(ostream &)" );
  bool result(true);
  ParameterList::const_iterator I;

  if ( m_format == HardCopyListener::JSON && m_parvec.size() > 1 ) { // multipleJsonElements
    // start json dictionnary
    os << "{" << std::endl;

    for( I=m_parvec.begin(); I!=m_parvec.end() && result; ++I ){
      if ( I!=m_parvec.begin() ) {
        os << ",";
      }
      std::string name = (*I)->getName();
      if(getHideFlags() & HIDEtransient) {  // stream hides TRANSIENT items, use DBATTR if given
        const JSONStreamParameter *par = dynamic_cast<JSONStreamParameter * const>(*I);
        if(par != 0 && !par->getDbAttr().empty()) {  // DBATTR is given
          name = par->getDbAttr();
        }
      }
      os << '"' << name << "\":";
			(*I)->setLevel(2);
			result = (*I) -> write( os );
    }

    // end json dictionnary
    os << "}" << std::endl;

  } else {
    for( I=m_parvec.begin(); I!=m_parvec.end() && result; ++I ){
      result = (*I) -> write( os );
    }
  }
  return result;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

const std::string StreamParameterGroup::getLabel() const {
  std::string ret;
  ParameterList::const_iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    ret += (ret.size() ? "," : "") +  (*I) -> getLabel();
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::putValues( StreamDestination *dest, bool transposed ){
  ParameterList::const_iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    (*I) -> putValues( dest, transposed );
  }
}

/* --------------------------------------------------------------------------- */
/* getParameters --                                                            */
/* --------------------------------------------------------------------------- */

int StreamParameterGroup::getParameters( std::ostream &os ) const{
  int i;
  int siz= m_parvec.size();
//   cerr << "StreamParameterGroup::getParameter()" << endl;
  for( i=0; i<siz; ++i ){
//     cerr << m_parvec[i]->getName() << endl;
    os << m_parvec[i]->getName();
    if( i<siz-1 ){
      os << ",";
    }
  }
  return siz;
}

/* --------------------------------------------------------------------------- */
/* clearRange --                                                               */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::clearRange( bool incl_itemattr ){
  if( m_pending ){
    return;
  }
  for( ParameterList::iterator it=begin(); it!=end(); ++it ){
    (*it)->clearRange( incl_itemattr );
  }
}

/* --------------------------------------------------------------------------- */
/* setDbItemsNotModified --                                                    */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::setDbItemsNotModified(){
  if( m_pending ){
    return;
  }
  for( ParameterList::iterator it=begin(); it!=end(); ++it ){
    (*it)->setDbItemsNotModified();
  }
}

/* --------------------------------------------------------------------------- */
/* resolveVars --                                                              */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::resolveVars(){
  if( m_pending )
    return;
  ParameterList::iterator I;
  for( I=begin(); I!=end(); ++I ){
    (*I) -> resolveVars( );
  }
}

/* --------------------------------------------------------------------------- */
/* fixupItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::fixupItemStreams( BasicStream &stream ){
  ParameterList::iterator I;
  for( I = begin(); I != end(); ++I ){
    (*I)->fixupItemStreams( stream );
  }
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::marshal( std::ostream &os ){
  ParameterList::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    (*I) -> marshal( os );
  }
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *StreamParameterGroup::unmarshal( const std::string &element
					       , const XMLAttributeMap &attributeList ){
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::setText( const std::string &text ){
}

/* --------------------------------------------------------------------------- */
/* setTargetStream --                                                          */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::setTargetStream( BasicStream *s ){
  ParameterList::iterator I;
  for( I=begin(); I!=end(); ++I ){
    (*I) -> setTargetStream( s );
  }
}

/* --------------------------------------------------------------------------- */
/* setDelimiter --                                                             */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::setDelimiter( char delim ){
  ParameterList::iterator I;
  for( I=begin(); I!=end(); ++I ){
    (*I) -> setDelimiter( delim );
  }
}

/* --------------------------------------------------------------------------- */
/* getHideFlags --                                                             */
/* --------------------------------------------------------------------------- */

SerializableMask StreamParameterGroup::getHideFlags() const{
  ParameterList::iterator I;
  if ( begin() != end() ) {
    return (*begin())->getHideFlags();
  }
  return HIDEnothing;
}

/* --------------------------------------------------------------------------- */
/* setHideFlags --                                                             */
/* --------------------------------------------------------------------------- */

void StreamParameterGroup::setHideFlags( const SerializableMask flags ){
  ParameterList::iterator I;
  for( I=begin(); I!=end(); ++I ){
    (*I) -> setHideFlags( flags );
  }
}

/* --------------------------------------------------------------------------- */
/* hasErrorMessage --                                                          */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::hasErrorMessage() {
  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    if((*I) ->hasErrorMessage())
      return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getErrorMessageStream --                                                    */
/* --------------------------------------------------------------------------- */

std::ostringstream& StreamParameterGroup::getErrorMessageStream() {
  clearErrorMessageStream();

  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    if((*I)->hasErrorMessage()) {
      m_errorStream << (*I)->getErrorMessageStream().str();
      (*I)->getErrorMessageStream().str("");  // clear
    }
  }

  return m_errorStream;
}

/* --------------------------------------------------------------------------- */
/* hasWarningMessage --                                                          */
/* --------------------------------------------------------------------------- */

bool StreamParameterGroup::hasWarningMessage() {
  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    if((*I) ->hasWarningMessage())
      return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getWaningMessageStream --                                                   */
/* --------------------------------------------------------------------------- */

std::ostringstream& StreamParameterGroup::getWarningMessageStream() {
  clearWarningMessageStream();
  m_warningStream.str("");

  std::vector< StreamParameter * >::iterator I;
  for( I=m_parvec.begin(); I!=m_parvec.end(); ++I ){
    if((*I)->hasWarningMessage())
      m_warningStream << (*I)->getWarningMessageStream().str();
  }

  return m_warningStream;
}
