
#include <string>
#include <limits>
#include <cmath>
#include <limits.h>
#ifdef __MINGW32__
#if !defined(SSIZE_MAX)
#define SSIZE_MAX INT_MAX
#endif
#endif

#include "utils/Debugger.h"

#include "app/DataPoolIntens.h"
#include "datapool/DataReference.h"
#include "xfer/XferConverter.h"
#include "streamer/DataStreamParameter.h"
#include "streamer/IndexParameter.h"
#include "xml/XMLDocumentHandler.h"

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

size_t IndexParameter::getSize() const {
  DataStreamParameterVector::const_iterator e;
  size_t siz=SSIZE_MAX;
  size_t s;
  for( e=m_params.begin(); e!=m_params.end(); ++e ){
    XferDataItem *item = (*e)->DataItem();
    assert( item != 0 );
    XferDataItemIndex *index = m_name.empty()? item->getDataItemIndex( m_pos ): item->getDataItemIndex( m_name );
    assert( index != 0 );
    s=index->getDimensionSize( item->Data() );
    if( item->getParamType() == XferDataItem::param_Default &&
	item->getDataType() ==  DataDictionary::type_Complex )
      s *=2;
    if( s < siz )
      siz=s;
  }
  return siz;
}

/* --------------------------------------------------------------------------- */
/* getMaxIndex --                                                              */
/* --------------------------------------------------------------------------- */

size_t IndexParameter::getMaxIndex() const {
  DataStreamParameterVector::const_iterator e;
  size_t siz=0;
  size_t s;
  for( e=m_params.begin(); e!=m_params.end(); ++e ){
    XferDataItem *item = (*e)->DataItem();
    assert( item != 0 );
    XferDataItemIndex *index = m_name.empty()? item->getDataItemIndex( m_pos ): item->getDataItemIndex( m_name );
    assert( index != 0 );
    s=index->getDimensionSize( item->Data() );
    if( item->getParamType() == XferDataItem::param_Default &&
	item->getDataType() ==  DataDictionary::type_Complex )
      s *=2;
    if( s > siz )
      siz=s;
  }
  return siz;
}

/* --------------------------------------------------------------------------- */
/* inc --                                                                      */
/* --------------------------------------------------------------------------- */

int IndexParameter::inc(){
  ++m_val;
  setValue(m_val);
  return m_val;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool IndexParameter::read( std::istream &istr ){
  BUG( BugStreamer, "read( istream & )" );
  int i;
  if( m_conv.read( istr, i, ' ' ) ){
    if (std::isnan(i) || i==std::numeric_limits<int>::min())
      return false;
    BUG_MSG( i );
    if( i>=0 ){
      setValue(i);
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool IndexParameter::write( std::ostream &ostr ){
  BUG( BugStreamer, "write" );
  m_conv.write( m_val, ostr );
  if (m_conv.getWidth() == 0)
    ostr << ' ';
  return true;
}

/* --------------------------------------------------------------------------- */
/* --                                                                          */
/* --------------------------------------------------------------------------- */

void IndexParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 )
    dest->putValue( *this, static_cast<double>(m_val) );
}

/* --------------------------------------------------------------------------- */
/* getValues --                                                                */
/* --------------------------------------------------------------------------- */

void IndexParameter::getValues( StreamSource *src ){
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void IndexParameter::setValue( const std::string &v ){
  int i;
  std::istringstream is(v);
  if( m_conv.read( is, i, ' ' ) ){
    setValue(i);
  }
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void IndexParameter::setValue( int v ){
  BUG_PARA( BugStreamer, "setValue", "v=" << v );
  m_val=v;
  DataStreamParameterVector::iterator dsi;
  BUG_MSG( m_name << " = " << v );
  for(dsi=m_params.begin(); dsi!=m_params.end(); ++dsi ){
    if( m_name.empty() ){
      BUG_MSG( "call setIndex(pos=" << m_pos << ", v=" << v << " )" );
      (*dsi)->setIndex( m_pos, m_val );
    }
    else{
      BUG_MSG( "call setIndex(name=" << m_name << ", v=" << v << " )" );
      (*dsi)->setIndex( m_name, m_val );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool IndexParameter::getValue( std::string &v )const{
  std::ostringstream os;
  if( os << m_val ){
    v=os.str();
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool IndexParameter::isValid(bool validErrorMsg){
  BUG( BugStreamer, "isValid");

  DataStreamParameterVector::iterator I;
  for( I = m_params.begin(); I != m_params.end(); ++I ){
    if( ((*I) -> isValid(validErrorMsg)) ){
      BUG_EXIT("TRUE");
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* fixupItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void IndexParameter::fixupItemStreams( BasicStream &stream ){
  DataStreamParameterVector::iterator I;
  for( I = m_params.begin(); I != m_params.end(); ++I ){
    XferDataItem *item = (*I)->DataItem();
    DataPoolIntens::Instance().fixupItemStreams( stream, item->Data() );
  }
}
/* --------------------------------------------------------------------------- */
/* addIndexedParameter --                                                      */
/* --------------------------------------------------------------------------- */

void IndexParameter::addIndexedParameter( DataStreamParameter *d ){
  XferDataItem *x=d!=0?d->DataItem():0;
  if( x != 0 ){
    //    XferDataItemIndex *ix = m_name.empty()? x->getDataItemIndex( m_pos ): x->getDataItemIndex( m_name );
    m_params.push_back( d );
  }
}

/* --------------------------------------------------------------------------- */
/* removeIndexedParameter --                                                   */
/* --------------------------------------------------------------------------- */

void IndexParameter::removeIndexedParameter( DataStreamParameter *d ){
  DataStreamParameterVector::iterator iter;
  int f = 0;
  for( iter = m_params.begin(); iter != m_params.end(); ++iter ){
    int w = 0;
    while( (*iter) == d ){
      iter = m_params.erase( iter );
      if( iter == m_params.end() )
	break;
    }
    if( iter == m_params.end() )
      break;
  }
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void IndexParameter::marshal( std::ostream &os ){
  os << "<IndexParameter"
     << " name=\""  << m_name << "\"";
  if( m_pos != 0 )
    os << " pos=\"" << m_pos  << "\"";
  int iWidth=m_conv.getWidth();
  if( iWidth != 0 )
    os << " width=\"" << iWidth << "\"";
  os << ">";
  m_conv.marshal( os );
  os << "</IndexParameter>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *IndexParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  m_name = attributeList["name"];
  std::string sPos=attributeList["pos"];
  if( sPos.empty() )
    m_pos = 0;
  else {
    std::istringstream is( sPos );
    is >> m_pos;
  }
  std::string sWidth=attributeList["width"];
  if( sWidth.empty() )
    m_conv.setWidth( 0 );
  else {
    int iWidth;
    std::istringstream is( sWidth );
    is >> iWidth;
    m_conv.setWidth( iWidth );
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void IndexParameter::setText( const std::string &text ){
}
