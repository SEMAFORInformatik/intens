
#include "utils/Debugger.h"
#include "streamer/CharDataStreamParameter.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
CharDataStreamParameter::CharDataStreamParameter()
  : DataStreamParameter( 0, 0, 0, false, 'd' ) {
}

CharDataStreamParameter::CharDataStreamParameter( XferDataItem *ditem, int level
					      , bool mand)
  : DataStreamParameter( ditem ,level ,0 ,mand, 'd' )
{
}

CharDataStreamParameter::~CharDataStreamParameter(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool CharDataStreamParameter::read( std::istream &is ){
  BUG_PARA(BugStreamer, "CharDataStreamParameter::read" ,getName() );
  std::ostringstream os;
#if 1
  while (is.good())
    {
      char c = is.get();       // get character from stream
      if (is.good())
	os << c;
    }
  BUG_INFO("read(" << getName() << "): " << os.str().size());
#endif
  // !! append option set ?
  if (hasAppendOption()) {
    std::string oldValue;
    if ( m_dataitem->getValue(oldValue) ) {
      return m_dataitem->setValue( oldValue + os.str() );
    }
  }
  // general case
  return os.str().size() ? m_dataitem->setValue( os.str() ) : false;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool CharDataStreamParameter::write( std::ostream &os ){
   BUG( BugStreamer, "CharDataStreamParameter::write");
   createDataItemIndexes();

   std::string s;
   m_dataitem->getValue( s );
   os << s;
   BUG_DEBUG("write(" << getName() << "): " << s.size());
   return true;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void CharDataStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  assert( false );
}
