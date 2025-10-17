
#include "utils/Debugger.h"

#include "app/DataSet.h"
#include "app/DataPoolIntens.h"
#include "xfer/XferDataParameter.h"
#include "xfer/XferParameterString.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "streamer/DatasetStreamParameter.h"
#include "xml/XMLDocumentHandler.h"
#include "utils/LaTeXConverter.h"
#include "utils/UrlConverter.h"

class DataIndexList;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
DatasetStreamParameter::DatasetStreamParameter()
  : DataStreamParameter( 0, 0, 0, false, ' ' ){
}

DatasetStreamParameter::DatasetStreamParameter( XferDataParameter *param, int level
					      , bool mand, DataSet *dataset )
  : DataStreamParameter( param->DataItem() ,level ,0 ,mand, ' ' )
  , m_param( param )
  , m_dataset( dataset ) {
}

DatasetStreamParameter::~DatasetStreamParameter(){
  delete m_param;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool DatasetStreamParameter::read( std::istream &is ){
  BUG_PARA(BugStreamer, "DatasetStreamParameter::read" ,getName() );
  std::string s;
  if( !m_converter.read( is, s, getDelimiter() ) ){
    return false;
  }
  int inx = m_dataset->getInputPosition( s );
  if( inx < 0 ){
    m_param->clear();
    BUG_EXIT("getInputPosition failed");
    return false;
  }
  if( !m_dataset->getOutputValue( s, inx ) ){
    m_param->clear();
    BUG_EXIT("getOutputValue failed");
    return false;
  }
  return m_param->setFormattedValue( s );
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool DatasetStreamParameter::write( std::ostream &os ){
  BUG( BugStreamer, "DatasetStreamParameter::write");
  createDataItemIndexes();

  std::string s;
  m_param->getFormattedValue( s );
  if( s.empty() ){
    removeDataItemIndexes();
    BUG_MSG("not valid or empty");
    // is it ok to silently ignore this?
  }

  if( !m_dataset->getInputValue( s, data() ) ){
    removeDataItemIndexes();
    BUG_MSG("getIndexedInputValue( " << s << " ) failed" );
  }

  // amg: habe das mit dem setWidth eingefügt (um Blanks im DataSet in den Stream zu uebernehmen)
  //      hoffentlich ist das ueberall erwuenscht.
  int saveWidth = m_converter.getWidth();
  if( m_format == HardCopyListener::LaTeX )
    LaTeXConverter::convert2LaTeX( s );
  if( m_format == HardCopyListener::URL )
    UrlConverter::urlQuote( s );
  m_converter.setWidth(s.size());
  bool rslt = m_converter.write( s, os );
  m_converter.setWidth(saveWidth);

  removeDataItemIndexes();
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void DatasetStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  BUG( BugStreamer, "DatasetStreamParameter::putValues" );
  if( dest != 0 ){
    BUG_MSG( getName() );
    std::string s;
    createDataItemIndexes();
    setDataItemIndexes();
    if( m_dataitem->getValue(s) ){
      dest -> putValue(*this, s );
    }
    else
      dest -> putValue( *this, "" );
    removeDataItemIndexes();
  }
}

void DatasetStreamParameter::setValue( double v ){
  std::ostringstream os;
  RealConverter( 0, -1, 0, false, true ).write( v, os );
  m_dataitem->setValue(os.str());
}

bool DatasetStreamParameter::getValue( double &v )const{
  std::string s;
  if(m_dataitem->getValue(s)){
    std::istringstream is(s);
    return RealConverter( 0, -1, 0, false, true ).read( is, v, getDelimiter() );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void DatasetStreamParameter::marshal( std::ostream &os ){
  os << "<DatasetStreamParameter";
  if( isMandatory() )
    os << " mand=\"true\"";
  if( isScalar() )
    os << " scalar=\"true\"";
  if( isCell() )
    os << " cell=\"true\"";
  os << " delimiter=\"" << getDelimiter() << "\"";
  os << ">\n";
  m_dataitem->marshal( os );
  m_converter.marshal( os );
  os << "</DatasetStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *DatasetStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "DatasetStreamParameter" ){
    std::string s;
    s = attributeList["mand"];
    if( !s.empty() )
      setMandatory();
    s = attributeList["scalar"];
    if( !s.empty() )
      setScalar( true );
    s = attributeList["cell"];
    if( !s.empty() )
      setCell( true );
    s = attributeList["delimiter"];
    if( !s.empty() ){
      const char delim = s[0];
      setDelimiter( delim );
    }
    else {
      setDelimiter(' ');
    }
    return this;
  }
  else if( element == "StringConverter" ){
    m_converter.unmarshal( element, attributeList );
    return this;
  }
  else if( element == "XferDataItem" ){
    m_dataitem = new XferDataItem();
    m_dataitem -> unmarshal( element, attributeList );

    setLevel( m_dataitem -> getNumberOfLevels() -1 );
    m_dataset = DataPoolIntens::Instance().getDataSet( m_dataitem->getUserAttr()->DataSetName() );

    m_param = 0;
    switch( m_dataitem->Data()->getDataType() ){
    case DataDictionary::type_Integer:
      m_param = new XferParameterInteger( m_dataitem->Data(), 0, 0 );
      break;
    case DataDictionary::type_Real:
      m_param = new XferParameterReal( m_dataitem->Data(), 0, 0, 0, true );
      break;
    case DataDictionary::type_String:
      m_param = new XferParameterString( m_dataitem->Data(), 0, '\0' );
      break;
    default:
      break;
    }
    return m_dataitem;
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void DatasetStreamParameter::setText( const std::string &text ){
}
