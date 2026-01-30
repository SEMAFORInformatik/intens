
#include <typeinfo>
#include <QTime>

#include <stdarg.h>
#include "xfer/XferParameterString.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "streamer/Stream.h"
#include "streamer/StreamParameter.h"
#include "streamer/IndexParameter.h"
#include "streamer/SkipStreamParameter.h"
#include "streamer/TokenStreamParameter.h"
#include "streamer/StreamParameterGroup.h"
#include "streamer/TableStreamGroup.h"
#include "streamer/IntStreamParameter.h"
#include "streamer/RealStreamParameter.h"
#include "streamer/StringStreamParameter.h"
#include "streamer/StructStreamParameter.h"
#include "streamer/StringDateStreamParameter.h"
#include "streamer/CharDataStreamParameter.h"
#include "streamer/ComplexStreamParameter.h"
#include "streamer/VarStreamParameter.h"
#include "streamer/DatasetStreamParameter.h"
#include "streamer/NumericMatrixParameter.h"
#include "streamer/ComplexMatrixParameter.h"
#include "streamer/StringMatrixParameter.h"
#include "streamer/XMLPlotGroupStreamParameter.h"
#include "streamer/SerializeFormStreamParameter.h"
#include "streamer/GuiIndexStreamParameter.h"
#include "operator/InputChannelEvent.h"
#include "operator/OutputChannelEvent.h"

#include "app/UserAttr.h"
#include "app/AppData.h"
#include "utils/utils.h"
#include "utils/StringUtils.h"
#include "utils/HTMLConverter.h"
#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "app/log.h"
#include "parser/errorhandler.h"
#include "streamer/PlotGroupStreamParameter.h"
#include "streamer/XMLStreamParameter.h"
#include "streamer/JSONStreamParameter.h"

// Serialize and Rep
#include "parser/IdManager.h"
#include "gui/GuiIndex.h"
#include "app/Plugin.h"

class HardCopyListener;

INIT_LOGGER();

//#include "utils/rusage.h"
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

Stream::Stream( const std::string &name, bool check )
  : BasicStream( name )
  , m_current_group( &m_pargroup )
  , m_buffer( 0 )
  , m_delimiter( ' ' )
  , m_indentation( 0 )
  , m_show_hidden( false )
  , m_show_transient( true )
  , m_locale( false )
  , m_file( false )
  , m_filesize( 0 )
  , m_format( HardCopyListener::NONE ){
  if( check ){
    installData();
  }
}

Stream::~Stream(){
  if( m_buffer != 0 ){
    delete m_buffer;
  }
}

/* --------------------------------------------------------------------------- */
/* writeFileData --                                                            */
/* --------------------------------------------------------------------------- */

std::string Stream::writeFileData(std::istream& is) {
  std::string filename = newStreamFilename();
  std::ofstream outfile(filename.c_str(), std::ios::binary | std::ios::out);
  char c;
  ulong size=0;
  while (is.get(c))
    ++size, outfile << c;
  BUG_DEBUG("writeFileData, size: " << size);
  outfile.close();
  m_filesize = size;

  // write filename, AND NOT CONTENT to StreamParameter
  std::istringstream iss(filename);
  m_pargroup.read( iss );
  return filename;
}

/* --------------------------------------------------------------------------- */
/* writeFileData --                                                            */
/* --------------------------------------------------------------------------- */
std::string Stream::writeFileData(void* data, int size) {
  std::string filename = newStreamFilename();
  std::ofstream outfile(filename.c_str(), std::ios::binary | std::ios::out);
  outfile.write(static_cast<const char*>(data), size);
  BUG_DEBUG("writeFileData, filename: " << filename << " , size: " << size);
  outfile.close();
  m_filesize = size;

  // write filename, AND NOT CONTENT to StreamParameter
  std::istringstream iss(filename);
  m_pargroup.read( iss );
  return filename;
}

/* --------------------------------------------------------------------------- */
/* readFileData --                                                            */
/* --------------------------------------------------------------------------- */

void Stream::readFileData(std::ostream& os) {
  std::string filename = getStreamFilename();
  if (filename.empty())
    return;
  std::ifstream ifs(filename.c_str(), std::ios::binary | std::ios::in);
  char c;
  ulong i=0;
  while (ifs.get(c))
    i++, os << c;
  ifs.close();
  BUG_DEBUG("readFileData, size: " << i);
}

/* --------------------------------------------------------------------------- */
/* readFileData --                                                            */
/* --------------------------------------------------------------------------- */

void Stream::readFileData(char* cp) {
  std::string filename = getStreamFilename();
  if (filename.empty())
    return;
  BUG_DEBUG("readFileData filename: "<< filename);
  std::ifstream ifs(filename.c_str(), std::ios::binary | std::ios::in);
  ifs.read(static_cast<char*>(cp), getStreamFileSize());
  ifs.close();
}

/* --------------------------------------------------------------------------- */
/* setDelimiter --                                                             */
/* --------------------------------------------------------------------------- */

void Stream::setDelimiter(char delim ){
  m_delimiter = delim;
  m_pargroup.setDelimiter(delim);
}

/* --------------------------------------------------------------------------- */
/* getHideFlags --                                                             */
/* --------------------------------------------------------------------------- */

SerializableMask Stream::getHideFlags() const{
  return m_pargroup.getHideFlags();
}

/* --------------------------------------------------------------------------- */
/* setHideFlags --                                                             */
/* --------------------------------------------------------------------------- */

void Stream::setHideFlags( const SerializableMask flags ){
  BUG_DEBUG("setHideFlags(" << flags << ")");
  m_pargroup.setHideFlags( flags );
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void Stream::marshal( std::ostream &os ){
  os << "<stream";
  os <<" name=\""<<Name()<<"\"";
  os <<">" << std::endl;
  m_pargroup.marshal( os );
  os << "</stream>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *Stream::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "stream" ){
    // name is set by constructor of BasicStream
  }
  else if( element == "SerializeFormStreamParameter" ){
    StreamParameter *p = new SerializeFormStreamParameter( "", "" );
    m_current_group -> addParameter( p );
    return p;
  }
  else if( element == "TokenStreamParameter" ){
    StreamParameter *p=addToken( "<EMPTY>", 0 );// "" -> SkipStreamParameter
    p -> unmarshal( element, attributeList );
    return p;
  }
  else if( element == "SkipStreamParameter" ){
    StreamParameter *p=addToken( "", 0 );	// "" -> SkipStreamParameter
    p -> unmarshal( element, attributeList );
    return this;
  }
  else if( element == "TableStreamGroup" ){
    beginGroup();
    return this;
  }
  else if( element == "/TableStreamGroup" ){
    getCurrentGroup() -> unmarshal( element, attributeList );
    endGroup();
    return this;
  }
  else if( element == "IndexParameter" ){
    StreamParameter *p=addIndexParameter( "", 0 );
    p -> unmarshal( element, attributeList );
    return p;
  }
  else if( element == "XMLStreamParameter" ){
    std::vector<std::string> attrs;
    attrs.clear();
    assert( false ); // Hier muss noch etwas gemacht werden ( allCycles )
    StreamParameter *p=addXMLParameter( 0, attrs, false, "", "", "", "" );
    return p;
  }
  else if( element == "DatasetStreamParameter" ){
    StreamParameter *p=addDatasetParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "VarStreamParameter" ){
    StreamParameter *p=addVarParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "IntStreamParameter" ){
    StreamParameter *p=addIntParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "RealStreamParameter" ){
    StreamParameter *p=addRealParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "StringStreamParameter" ){
    StreamParameter *p=addStringParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "StructStreamParameter" ){
    StreamParameter *p=addStructParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "ComplexStreamParameter" ){
    StreamParameter *p=addComplexParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "NumericMatrixDataParameter" ){
    StreamParameter *p=addNumericMatrixParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "StringMatrixDataParameter" ){
    StreamParameter *p=addStringMatrixParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "ComplexMatrixDataParameter" ){
    StreamParameter *p=addComplexMatrixParameter();
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "PlotGroupStreamParameter" ){
    StreamParameter *p=addPlotGroupParameter("");
    return p -> unmarshal( element, attributeList );
  }
  else if( element == "XMLPlotGroupStreamParameter" ){
    StreamParameter *p=addXMLPlotGroupStreamParameter( GuiManager::Instance().getPlugin( attributeList["name"] ));
    return this;
  }
  else if( element == "/stream" ){
    m_current_group -> setTargetStream( this );
    return this;
  }
  else if( element == "StringDateParameter" ){
    assert( false ); // Hier muss noch etwas gemacht werden
  }

  return this;	//this, damit jedes Element erkannt werden könnte
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void Stream::setText( const std::string &text ){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* beginGroup --                                                               */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::beginGroup(){
  StreamParameterGroup *g = new TableStreamGroup( m_delimiter, m_current_group );
  g->setFormat( m_format );
  assert(g != 0);

  m_current_group -> addParameter( g );
  m_current_group = g;

  return g;
}

/* --------------------------------------------------------------------------- */
/* getCurrentGroup --                                                          */
/* --------------------------------------------------------------------------- */

StreamParameterGroup *Stream::getCurrentGroup(){
  return m_current_group;
}

/* --------------------------------------------------------------------------- */
/* numberOfItems --                                                            */
/*   returns the number of valid parameters included in the stream             */
/* --------------------------------------------------------------------------- */

int Stream::numberOfItems( bool count_list ){
  return m_pargroup.size();
}

/* --------------------------------------------------------------------------- */
/* addParameter --  ( called from unmarshal )                                  */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addIntParameter(){
  XferDataItem *ditem = new XferDataItem();
  DataStreamParameter *p = new IntStreamParameter( ditem, 0, 0, m_delimiter );
  m_current_group -> addParameter( p );
  return p;
}
StreamParameter *Stream::addRealParameter(){
  XferDataItem *ditem = new XferDataItem();
  DataStreamParameter *p = new RealStreamParameter( ditem, 0, 0, m_delimiter, m_locale );
  m_current_group -> addParameter( p );
  return p;
}
StreamParameter *Stream::addStringParameter(){
  XferDataItem *ditem = new XferDataItem();
  DataStreamParameter *p = new StringStreamParameter( ditem, m_delimiter, 0, 0 );
  m_current_group -> addParameter( p );
  return p;
}
StreamParameter *Stream::addStructParameter(){
  XferDataItem *ditem = new XferDataItem();
  DataStreamParameter *p = new StructStreamParameter( ditem, m_delimiter, m_locale );
  m_current_group -> addParameter( p );
  return p;
}
StreamParameter *Stream::addComplexParameter(){
  XferDataItem *ditem = new XferDataItem();
  DataStreamParameter *p = new ComplexStreamParameter( ditem, 0, 0, m_locale );
  m_current_group -> addParameter( p );
  return p;
}
StreamParameter *Stream::addVarParameter(){
  XferDataItem *ditem = new XferDataItem();
  XferDataItem *ditem2 = new XferDataItem();
  DataStreamParameter *p = new VarStreamParameter( ditem, ditem2,
												   m_current_group, 0, m_delimiter, m_locale );
  m_current_group -> addParameter( p );
  return p;
}

/* --------------------------------------------------------------------------- */
/* addVarParameter --                                                          */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addVarParameter( XferDataItem *varDataItem, XferDataItem *dataItem ){
  if( varDataItem == 0 ){
    return 0;
  }

  int level = varDataItem ->getNumberOfLevels()-1;
  BUG_PARA( BugStreamer, "Stream::addVarParameter"
	    , dataItem->getName() << " level=" << level );
  assert( m_current_group != 0 );

  DataStreamParameter *p=0;

  if( varDataItem->Data()->getDataType() == DataDictionary::type_String ){
    p = new VarStreamParameter( varDataItem, dataItem, m_current_group, level, m_delimiter, m_locale);
    m_current_group -> addParameter( p );
  }
  if( p!=0 ){
    if( !m_current_group->linkWithIndex( p ) ) {
      // now this is a bit tricky, we want to check
      // if the parameter is scalar or not
      // It is scalar if:
      // a) there are const indexes or
      // b) it has a user attribute isScalar = true
      if( varDataItem ){
	UserAttr *attr = varDataItem->getUserAttr();
	if( varDataItem -> getNumIndexes() > 0 ){
	  p -> setScalar( true );
	}
	else {
	  if( attr != 0 ){
	    p -> setScalar( attr -> IsScalar() );
	  }
	}
	if( attr != 0 ){
	  p -> setCell( attr -> IsCell() );
	}
      }
    }
  }
  return p;
}

/* --------------------------------------------------------------------------- */
/* addParameter --                                                             */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addParameter( XferDataItem *ditem
				     , int length, int prec, Scale *scale, bool thousand_sep
				     , bool  mandatory
				     , bool isDatasetText)
{
  if( ditem == 0 )
    return 0;
  int level = ditem ->getNumberOfLevels()-1;
  BUG_PARA( BugStreamer, "Stream::addParameter"
	    , ditem->getName() << " level=" << level <<  " length=" << length << " prec=" << prec );
  assert( m_current_group != 0 );
//   std::cerr << "  Stream::addParameter " << ditem->getFullName( false ) << std::endl;
  DataStreamParameter *p=0;

  switch( ditem->Data()->getDataType() ){
  case DataDictionary::type_Integer:
    p = new IntStreamParameter( ditem, level, length, scale, m_delimiter, mandatory );
    break;

  case DataDictionary::type_Real:
    p = new RealStreamParameter( ditem, level, length, prec, scale, thousand_sep
								 , mandatory, m_delimiter, m_locale );
    break;

  case DataDictionary::type_String:
    p = new StringStreamParameter( ditem, level, length, m_delimiter, mandatory );
    break;
  case DataDictionary::type_CharData:
    p = new CharDataStreamParameter( ditem, level, mandatory );
    break;
  case DataDictionary::type_StructVariable:
    p = new StructStreamParameter( ditem, level, 0, m_delimiter, m_locale );
    {
      UserAttr *attr = static_cast<UserAttr*>(ditem->Data()->getUserAttr());
      if( attr != 0 ){
	static_cast<StructStreamParameter *>(p)->setClassname(  attr->Classname() );
      }
      else
	std::cerr << "UserAttr of " << p->getName() << " is NULL" << std::endl;
    }
    break;

  case DataDictionary::type_Complex:
    p = new ComplexStreamParameter( ditem, level, length, prec, scale, mandatory, m_locale );
    break;

  case DataDictionary::type_StructDefinition:
    std::cerr << "StructDefinition StreamParameter not yet implemented" << std::endl;
    return 0;

  case DataDictionary::type_Bad:
    return 0;
  default:
    break;
  }

  if( p!=0 ){
    p->setFormat( m_format );
    if( !m_current_group->linkWithIndex( p ) ) {
      // now this is a bit tricky, we want to check
      // if the parameter is scalar or not
      // It is scalar if:
      // a) there are const indexes or
      // b) it has a user attribute isScalar = true
      if( ditem ){
	UserAttr *attr = ditem->getUserAttr();
	if( ditem -> getNumIndexes() > 0 ){
	  p -> setScalar( true );
	}
	else {
	  if( attr != 0 ){
	    p -> setScalar( attr -> IsScalar() );
	  }
	}
	if( attr != 0 ){
	  p -> setCell( attr -> IsCell() );
	}
      }
    }
    m_current_group -> addParameter( p );
  }
  return p;
}

/* --------------------------------------------------------------------------- */
/* addXMLParameter --                                                          */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addXMLParameter( XferDataItem *ditem
                                          , const std::vector<std::string> &attrs
                                          , bool allCycles
                                          , const std::string &schema
                                          , const std::string &nspace
                                          , const std::string &version
                                          , const std::string &stylesheet ){
  assert( m_current_group != 0 );

  XMLStreamParameter *param = new XMLStreamParameter( ditem );
  if( !attrs.empty() )
    param -> setAttributes( attrs );
  if( allCycles )
    param->withAllCycles();
  param->setXMLSchema( schema );
  param->setXMLNamespace( nspace );
  param->setXMLVersion( version );
  param->setXMLStylesheet( stylesheet );
  m_current_group -> addParameter( param );
  return param;
}

/* --------------------------------------------------------------------------- */
/* addJSONParameter --                                                         */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addJSONParameter( XferDataItem *ditem,
                                           bool allCycles )
{
  BUG_DEBUG("addJSONParameter");
  assert( m_current_group != 0 );

  JSONStreamParameter *param = new JSONStreamParameter( ditem, m_indentation );

  if( allCycles ){
    param->withAllCycles();
  }

  SerializableMask mask = 0;
  if( !m_show_hidden ){
    BUG_DEBUG("set serialize hide hidden");
    mask = ( mask | HIDEhidden );
  }
  if( !m_show_transient ){
    BUG_DEBUG("set serialize hide transient");
    mask = ( mask | HIDEtransient );
  }
  param->setHideFlags( mask );

  m_current_group->addParameter( param );
  return param;
}

/* --------------------------------------------------------------------------- */
/* addDatasetParameter --                                                      */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addDatasetParameter(){
  DataStreamParameter *p = new DatasetStreamParameter();
  m_current_group -> addParameter( p );
  return p;
}

/* --------------------------------------------------------------------------- */
/* addDatasetParameter --                                                      */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addDatasetParameter( XferDataItem *ditem
				            , int length
					    , bool  mandatory ){
  if( ditem == 0 )
    return 0;
  int level=ditem->getNumberOfLevels()-1;

  BUG_PARA( BugStreamer, "Stream::addDatasetParameter"
	    , ditem->getName() << " level=" << level <<  " length=" << length );
  assert( m_current_group != 0 );
  std::string dsname = ditem->getUserAttr()->DataSetName();
  if( dsname.empty() ){
    return 0;
  }
  DataSet *dataset = DataPoolIntens::Instance().getDataSet( dsname );
  assert( dataset != 0 );

  XferDataParameter *param = 0;
  switch( ditem->Data()->getDataType() ){
  case DataDictionary::type_Integer:
    param = new XferParameterInteger( 0, 0, 0 );
    break;

  case DataDictionary::type_Real:{
    XferParameterReal *realParam = new XferParameterReal( 0, 0, -1 , new Scale(), true );
    param = realParam;
    break;
  }
  case DataDictionary::type_String:
    param = new XferParameterString( 0, 0, '\0' );
    break;

  case DataDictionary::type_StructVariable:
    BUG_EXIT("Bad type: StructVariable");
    return 0;

  case DataDictionary::type_Complex:
    BUG_EXIT("Bad type: Complex");
    return 0;

  case DataDictionary::type_StructDefinition:
    BUG_EXIT("Bad type: StructDefinition");
    return 0;

  case DataDictionary::type_Bad:
    BUG_EXIT("Bad type: Bad");
    return 0;

  default:
    BUG_EXIT("Bad type: unknown type");
    return 0;
  }

  assert( param != 0 );
  param->setDataItem( ditem );
  DataStreamParameter *p = new DatasetStreamParameter( param, level
						       , mandatory, dataset );

  if( p!=0 ){
    p->setFormat( m_format );
    if( !m_current_group -> linkWithIndex( p ) ) {
      // now this is a bit tricky, we want to check
      // if the parameter is scalar or not
      // It is scalar if:
      // a) there are indexes
      // b) it has a user attribute isScalar = true
      if( ditem ){
	UserAttr *attr = ditem->getUserAttr();
	if( ditem -> getNumIndexes() > 0 ){
	  p -> setScalar( true );
	}
	else {
	  if( attr != 0 ){
	    p -> setScalar( attr -> IsScalar() );
	  }
	}
	if( attr != 0 ){
	  p -> setCell( attr -> IsCell() );
	}
      }
    }
    m_current_group -> addParameter( p );
  }
  return p;
}

/* --------------------------------------------------------------------------- */
/* getInvalidItems                                                             */
/* --------------------------------------------------------------------------- */

bool Stream::getInvalidItems( std::vector< std::string > &invaliditems ){
  invaliditems.clear();
  return m_pargroup.getInvalidItems( invaliditems );
}

/* --------------------------------------------------------------------------- */
/* addIndexParameter --                                                        */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addIndexParameter( const std::string &name, int length ) {
  assert( m_current_group != 0 );

  // must be an index parameter
  return m_current_group ->addIndex( name, length );
}

/* --------------------------------------------------------------------------- */
/* endGroup --                                                                 */
/* --------------------------------------------------------------------------- */

void Stream::endGroup(){
  assert( m_current_group != 0 );
  m_current_group = m_current_group -> getParent();
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool Stream::read( std::istream &istr ){
  BUG( BugStreamer,"read( istream & )");
  m_pargroup.resolveVars();
  getErrorMessage();  // clears error messages
  // ignore clearRange if appendOption
  if (!hasAppendOption() ) {
    m_pargroup.clearRange();
    BUG_MSG("after clearRange()");
  }

  bool rslt = true;

  try{
    if (hasProcess()) {
      std::ostringstream os, ostr;
      os << istr.rdbuf();
      if (!runProcess(os, ostr)) {
        BUG_MSG( "runProcess failed!");
        return false;
      }
      std::istringstream is(ostr.str());
      if (hasAppendOption() ){
        m_pargroup.setAppendOption(true);
      }
      QTime startTime = QTime::currentTime();
      if (getFileFlag()) {
        rslt = !writeFileData(is).empty();
      } else
        rslt = m_pargroup.read( is );
      int msec  = startTime.msecsTo(QTime::currentTime());
      if (msec > 0) {
        BUG_DEBUG("Read Stream: " << Name() <<Date::durationAsString(msec));
      }
      if (hasAppendOption() ){
        m_pargroup.setAppendOption(false);
      }
    }
    else {
      if (hasAppendOption() ){
        m_pargroup.setAppendOption(true);
      }
      QTime startTime = QTime::currentTime();
      if (getFileFlag()) {
        rslt = !writeFileData(istr).empty();
      } else
        rslt = m_pargroup.read( istr );
      int msec  = startTime.msecsTo(QTime::currentTime());
      if (msec > 0) {
        BUG_DEBUG("Read Stream: " << Name() <<Date::durationAsString(msec));
      }
      if( !rslt ){
        BUG_MSG(" - read failed");
      }
      // has erros
      if (m_pargroup.hasErrorMessage()) {
        BUG_MSG(" - has errors");
        std::string msg(compose("Target Stream: %1\n%2", Name(), m_pargroup.getErrorMessageStream().str()));
        GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + " : <" + Name() + "> " + msg);
        GuiFactory::Instance() -> showDialogWarning( 0,
                                                     compose(_("<%1> Error"), Name()),
                                                     msg,
                                                     0 );
      }
      // has warnings
      if (m_pargroup.hasWarningMessage()) {
        BUG_MSG(" - has warnings");
        GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + " : <" + Name() + "> " + m_pargroup.getWarningMessageStream().str());
        std::ostringstream os;
        os << _("Stream has been successfully read, but warnings occured.")
           << std::endl << std::endl
           << m_pargroup.getWarningMessageStream().str();
        GuiFactory::Instance() -> showDialogInformation( 0,
                                                         compose(_("<%1> Information"), Name()),
                                                         os.str(),
                                                         0 );
      }
      if (hasAppendOption() ) {
        m_pargroup.setAppendOption(false);
      }
    }
  }
  catch( bad_call& bc ){
    if( m_buffer != 0 )
      delete m_buffer;
    m_buffer = new std::string;
    char c = istr.get();
    while( c != EOF ){
      *m_buffer += c;
      c = istr.get();
    }
    setContinuousUpdate( false );
  }
  catch( std::exception &e ){
    std::cout << e.what() << std::endl;
  }
  BUG_MSG("after read()");
  // Sobald von irgendwo her Daten eingelesen wurden, wird der Stream, resp.
  // die entsprechenden Daten als gültig erklärt. Diese Information wird be-
  // nötigt, um die Daten bei Bedarf wieder zu löschen (Targets).
  setDataItemsValid();
  BUG_EXIT("after setDataItemsValid()");
  return rslt;
}

#ifdef __MINGW32__
void Stream::read( const std::string &filename ) {
  BUG( BugStreamer,"Stream::read( const std::string & )");
  m_pargroup.resolveVars();
  m_pargroup.clearRange();
  BUG_MSG("after clearRange()");
  try{
    m_pargroup.read( filename );
  }
  catch( std::exception &e ){
    std::cout << e.what() << std::endl;
  }
  BUG_MSG("after read()");
  // Sobald von irgendwo her Daten eingelesen wurden, wird der Stream, resp.
  // die entsprechenden Daten als gültig erklärt. Diese Information wird be-
  // nötigt, um die Daten bei Bedarf wieder zu löschen (Targets).
  setDataItemsValid();
  BUG_EXIT("after setDataItemsValid()");
}
#endif

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool Stream::read( OutputChannelEvent &event ){
  if( m_buffer != 0 ){
    *m_buffer += event.getStreamBuffer();
    return true;
  }
  if ( event.getStreamBuffer().size() ) {
    std::istringstream istr( event.getStreamBuffer() );
    return Stream::read( istr );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool Stream::write( std::ostream &ostr ){
  BUG(BugStreamer, "write(ostream &)" );
  bool ret = true;
  m_pargroup.resolveVars();
  // Sobald von irgendwo her Daten geschrieben (InputStream) wurden, wird der Stream, resp.
  // die entsprechenden Daten als gültig erklärt. Diese Information wird be-
  // nötigt, um die Daten bei Bedarf wieder zu löschen (Targets).
  setDataItemsValid(false);  // input Stream
  if (hasProcess()) {
    std::ostringstream os;
    ret = m_pargroup.write( os );
    if (!runProcess(os, ostr)) {
      return false;
    }
    return ret;
  } else {
    if (getFileFlag()) {
      readFileData(ostr);
    } else
    ret = m_pargroup.write( ostr );
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool Stream::write( InputChannelEvent& event ){
  m_pargroup.resolveVars();
  // Sobald von irgendwo her Daten geschrieben (InputStream) wurden, wird der Stream, resp.
  // die entsprechenden Daten als gültig erklärt. Diese Information wird be-
  // nötigt, um die Daten bei Bedarf wieder zu löschen (Targets).
  setDataItemsValid(false);  // input Stream
#if defined HAVE_QT
  // with process
  if (hasProcess()) {
    std::ostringstream os;
    std::ostringstream osStr;
    bool ret = m_pargroup.write( os );
    if (!runProcess(os, osStr)) {
      return false;
    }
    return event.write( osStr );
  }

  std::ostringstream os;
  if (getFileFlag()) {
    readFileData(os);
    return event.write( os );
  } else
   if ( m_pargroup.write( os ) ){
    return event.write( os );
  }
  return false;
#else
  return m_pargroup.write( event.getOstream() );
#endif
}

/* --------------------------------------------------------------------------- */
/* resetContinuousUpdate --                                                    */
/* --------------------------------------------------------------------------- */

void Stream::resetContinuousUpdate(){
  if( m_buffer != 0 ){
    std::istringstream istr( *m_buffer );
    read( istr );
    delete m_buffer;
    m_buffer = 0;
  }
  m_pargroup.resetContinuousUpdate();
}

/* --------------------------------------------------------------------------- */
/* setContinuousUpdate --                                                      */
/* --------------------------------------------------------------------------- */

bool Stream::setContinuousUpdate( bool flag ){
  return m_pargroup.setContinuousUpdate( flag );
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void Stream::putValues( StreamDestination *dest, bool transposed ){
  m_pargroup.putValues( dest, transposed );
  // Sobald von irgendwo her Daten geschrieben (InputStream) wurden, wird der Stream, resp.
  // die entsprechenden Daten als gültig erklärt. Diese Information wird be-
  // nötigt, um die Daten bei Bedarf wieder zu löschen (Targets).
  setDataItemsValid(false);
}

/* --------------------------------------------------------------------------- */
/* getValues --                                                                */
/* --------------------------------------------------------------------------- */

void Stream::getValues( StreamSource *src ){
  m_pargroup.clearRange();
  m_pargroup.getValues( src );
  // Sobald von irgendwo her Daten eingelesen wurden, wird der Stream, resp.
  // die entsprechenden Daten als gültig erklärt. Diese Information wird be-
  // nötigt, um die Daten bei Bedarf wieder zu löschen (Targets).
  setDataItemsValid();
}

/* --------------------------------------------------------------------------- */
/* getParameters --                                                            */
/*   creates an identifier list of the included parameters                     */
/* --------------------------------------------------------------------------- */

int Stream::getParameters( std::ostream &ostr )const{
  return m_pargroup.getParameters( ostr );
}

/* --------------------------------------------------------------------------- */
/* addLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool Stream::addLabel( DataReference *data_ref  ){
  std::string  label = static_cast<UserAttr*>(data_ref->getUserAttr())->Label( data_ref );
  StreamParameter *p = addToken( label, label.size() );
  if( p == 0 ){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* addUnits --                                                                 */
/* --------------------------------------------------------------------------- */

bool Stream::addUnits( DataReference *data_ref ){
  std::string  units = static_cast<UserAttr*>(data_ref->getUserAttr())->getOriginUnit();
  StreamParameter *p = addToken(units, units.size() );
  if( p == 0 ){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* addMatrixParameter --                                                       */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addNumericMatrixParameter(){
  XferDataItem *ditem = new XferDataItem();
//   StreamParameter *p = new NumericMatrixDataParameter( ditem, 0,0,-1,0,false,false );
  StreamParameter *p = new NumericMatrixDataParameter( ditem, 0,0, m_delimiter, m_locale );
  m_current_group -> addParameter( p );
  return p;
}
StreamParameter *Stream::addStringMatrixParameter(){
  XferDataItem *ditem = new XferDataItem();
  StreamParameter *p = new StringMatrixDataParameter( ditem, 0 );
  m_current_group -> addParameter( p );
  return p;
}
StreamParameter *Stream::addComplexMatrixParameter(){
  XferDataItem *ditem = new XferDataItem();
  StreamParameter *p = new ComplexMatrixDataParameter( ditem, 0, 0, ' ', m_locale );
//   StreamParameter *p = new ComplexMatrixDataParameter( ditem, 0,0,-1,0,false );
  m_current_group -> addParameter( p );
  return p;
}

StreamParameter *Stream::addMatrixParameter( XferDataItem *ditem
					     , int length, int prec, Scale *scale
					     ,  bool thousand_sep, char delimiter, bool mandatory ) {
  assert( m_current_group != 0 );
  assert( ditem!=0 );
  int level=ditem -> getNumberOfLevels() -1;
  StreamParameter *p;

  ditem->eraseDataItemIndex( level ); // get rid of any indexes on this level !

  switch( ditem->Data()->getDataType() ){
  case DataDictionary::type_Integer:
  case DataDictionary::type_Real:
    p = new NumericMatrixDataParameter( ditem, level,length,prec,scale,thousand_sep,mandatory, m_delimiter, m_locale );
    break;

  case DataDictionary::type_String:
    p = new StringMatrixDataParameter( ditem, level, length, delimiter, mandatory );
    break;
  case DataDictionary::type_StructVariable:
    p = new StructStreamParameter( ditem, level,0, m_delimiter, m_locale );
    {
      UserAttr *attr = ditem->getUserAttr();
      if( attr != 0 ){
	static_cast<StructStreamParameter *>(p)->setClassname(  attr->Classname() );
      }
      else
	std::cerr << "UserAttr of " << p->getName() << " is NULL" << std::endl;
    }
    break;
  case DataDictionary::type_Complex:
    p = new ComplexMatrixDataParameter( ditem, level, length, prec,scale,mandatory, ' ', m_locale );
    break;
  case DataDictionary::type_StructDefinition:
  case DataDictionary::type_Bad:

  default:
    break;
  }

  m_current_group -> addParameter( p );
  return p;
}

/* --------------------------------------------------------------------------- */
/* AddToken --                                                                 */
/*   adds a token (a string constant or a regular expression) to the Stream    */
/*   commands.                                                                 */
/*   Returns a 0 pointer if it couldn't be allocated.                          */
/*   A string with "length" spaces is allocated if the token is 0.             */
/* --------------------------------------------------------------------------- */

StreamParameter * Stream::addToken( const std::string &token, int length, bool setFormat ){
  StreamParameter *p;
  if( token.empty() )
    p = new SkipStreamParameter( length );
  else {
    p = new TokenStreamParameter( token, length );
    if (setFormat)
      p->setFormat(m_format );
  }
  m_current_group -> addParameter( p );
  return p;
}

/* --------------------------------------------------------------------------- */
/* addSerializedFormParameter --                                               */
/*   adds a form to serialize to the Stream                                    */
/* --------------------------------------------------------------------------- */

StreamParameter * Stream::addSerializedFormParameter( std::vector<std::string> &formIdList
						    , const std::string &xslFile ){
  StreamParameter *p;
  p = new SerializeFormStreamParameter( formIdList, xslFile );
  m_current_group -> addParameter( p );
  return p;
}

/* --------------------------------------------------------------------------- */
/* addPlotGroupParameter --                                                    */
/*   adds a plot or xml group to the currently open group                      */
/*   Returns a 0 pointer if it couldn't be allocated.                          */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addPlotGroupParameter( const std::string &plotname ){
  StreamParameter *p = new PlotGroupStreamParameter( plotname );
  m_current_group -> addParameter( p );
  return p;
}

StreamParameter *Stream::addXMLPlotGroupStreamParameter( HardCopyListener *hcl ){
  StreamParameter *p = new XMLPlotGroupStreamParameter( hcl );
  m_current_group -> addParameter( p );
  return p;
}

/* --------------------------------------------------------------------------- */
/* addStringDateParameter --                                                   */
/* --------------------------------------------------------------------------- */

StreamParameter *Stream::addStringDateParameter( XferDataItem *ditem
						 , UserAttr::STRINGtype type
						 , int length
						 , bool  mandatory ) {
  if( ditem == 0 )
    return 0;

  int level=ditem->getNumberOfLevels()-1;

  BUG_PARA( BugStreamer, "Stream::addStringDateParameter "
	    , ditem->getName() << " level=" << level <<  " length=" << length );
  assert( m_current_group != 0 );

  XferDataParameter *param = 0;
  switch( ditem->Data()->getDataType() ){
  case DataDictionary::type_String:
    param = new XferParameterString( 0, 0, '\0' );
    break;

  default:
    BUG_EXIT("Bad type: unknown type");
    return 0;
  }

  assert( param != 0 );
  param->setDataItem( ditem );
  DataStreamParameter *p = new StringDateStreamParameter( param, level, length
							  , mandatory, type, m_delimiter );

  if( p!=0 ){
    if( !m_current_group -> linkWithIndex( p ) ) {
      // now this is a bit tricky, we want to check
      // if the parameter is scalar or not
      // It is scalar if:
      // a) there are indexes
      // b) it has a user attribute isScalar = true
      if( ditem ){
	UserAttr *attr = ditem->getUserAttr();
	if( ditem -> getNumIndexes() > 0 ){
	  p -> setScalar( true );
	}
	else {
	  if( attr != 0 ){
	    p -> setScalar( attr -> IsScalar() );
	  }
	}
	if( attr != 0 ){
	  p -> setCell( attr -> IsCell() );
	}
      }
    }
    m_current_group -> addParameter( p );
  }
  return p;
}

/* --------------------------------------------------------------------------- */
/*addGuiIndexParameter --                                                      */
/* --------------------------------------------------------------------------- */

StreamParameter* Stream::addGuiIndexParameter( GuiIndex *elem ) {
  StreamParameter *p = new GuiIndexStreamParameter(elem);
  m_current_group -> addParameter( p );
  return p;
}
/* --------------------------------------------------------------------------- */
/* reset --                                                                    */
/* --------------------------------------------------------------------------- */

void Stream::reset(){
  m_current_group = &m_pargroup;
}

/* --------------------------------------------------------------------------- */
/* clearRange --                                                               */
/* --------------------------------------------------------------------------- */

void Stream::clearRange(){
  m_pargroup.clearRange();
}

/* --------------------------------------------------------------------------- */
/* setDbItemsNotModified --                                                    */
/* --------------------------------------------------------------------------- */

void Stream::setDbItemsNotModified(){
  m_pargroup.setDbItemsNotModified();
}

/* --------------------------------------------------------------------------- */
/* fixupItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void Stream::fixupItemStreams(){
  BUG( BugStreamer, "fixupItemStreams" );
  m_pargroup.fixupItemStreams( *this );
}

/* --------------------------------------------------------------------------- */
/* hasValidData --                                                             */
/* --------------------------------------------------------------------------- */

bool Stream::hasValidData() {
  BUG( BugTargets, "hasValidData" );
  return getParameterGroup()->isValid(true);  // need errorMessages
}

/* --------------------------------------------------------------------------- */
/* getErrorMessage --                                                          */
/* --------------------------------------------------------------------------- */

std::string Stream::getErrorMessage(bool target) {
  std::string msg(target ? std::string() : BasicStream::getErrorMessage());

  if (m_pargroup.hasErrorMessage()) {
    std::string errmsg(m_pargroup.getErrorMessageStream().str());
    ///    if (msg.size()) msg += "\n";
    std::string name(Name());
    BUG_DEBUG("Stream: " << name << ", errorMessage: " << errmsg);
    if (__debugLogger__) {
      replaceAll(name, "@","<at>");
      msg += std::string("StreamName ") + name + ":\n";
    }
    msg += "" + errmsg + ""  + "\n";
    m_pargroup.getErrorMessageStream().str("");   // clear
  }
  return msg;
}
