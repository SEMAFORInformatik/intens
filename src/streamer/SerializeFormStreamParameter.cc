
#include <string>
#include <limits.h>
#ifdef __MINGW32__
#if !defined(SSIZE_MAX)
#define SSIZE_MAX INT_MAX
#endif
#endif

//#include <recode.h>

#include "utils/Debugger.h"
#include "datapool/DataReference.h"
#include "streamer/SerializeFormStreamParameter.h"
#include "app/HardCopyListener.h"
#include "app/ReportGen.h"
#include "app/AppData.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiForm.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

SerializeFormStreamParameter::SerializeFormStreamParameter( const std::string &formId, const std::string &xslFile )
  : StreamParameter()
  , m_xslFileName( xslFile )
  , m_useStylesheet( true )
  , m_mainFormPos( -1 ){
  if( xslFile == "<XML>" ){
    m_useStylesheet=false;
  }
  pushForm( formId );
}

SerializeFormStreamParameter::SerializeFormStreamParameter( GuiForm *form, const std::string &xslFile )
  : StreamParameter()
  , m_xslFileName( xslFile )
  , m_useStylesheet( true )
  , m_mainFormPos( -1 ){
  if( xslFile == "<XML>" ){
    m_useStylesheet=false;
  }
  m_formlist.push_back( form );
}

SerializeFormStreamParameter::SerializeFormStreamParameter( std::vector<std::string> &formIdList, const std::string &xslFile )
  : StreamParameter()
  , m_xslFileName( xslFile )
  , m_useStylesheet( true )
  , m_mainFormPos( -1 ){
  if( xslFile == "<XML>" ){
    m_useStylesheet=false;
  }
  std::vector<std::string>::iterator it;
  for( it = formIdList.begin(); it != formIdList.end(); ++it ){
    pushForm(*it);
  }
}
SerializeFormStreamParameter::~SerializeFormStreamParameter(){}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */
size_t SerializeFormStreamParameter::getDimensionSize(int ndim) const {
  return SSIZE_MAX;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */
bool SerializeFormStreamParameter::read( std::istream &is ){
  //
  std::cerr << "Not implemented yet: SerializeFormStreamParameter::read() Form:"  << std::endl;
  return false;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool SerializeFormStreamParameter::write( std::ostream &os ){
    serializeFormList( os );
    return true;
}

/* --------------------------------------------------------------------------- */
/* serializeFormList --                                                        */
/* --------------------------------------------------------------------------- */
bool SerializeFormStreamParameter::serializeFormList( std::ostream &_os ){
  std::ostringstream os;
  if( _os.tellp() < 1 ){
    os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
    os << "<?xml-stylesheet type=\"text/xsl\" href=\"https://pyrit.semafor.ch/intens.xsl\" ?>" << std::endl;
  }
  os << "<intens:Document xmlns:intens=\"http://www.semafor.ch/XML/2003/intens/1.0\""
     << " name=\"" << AppData::Instance().Title() << "\">" << std::endl;
  int i=0;
  GuiFormList::iterator fit;
  for( fit = m_formlist.begin(); fit != m_formlist.end(); ++fit, ++i ){
    if ( i == m_mainFormPos ){
      GuiForm *f = GuiElement::getMainForm();
      if( f != 0 )
	f->serializeXML(os);
    }
    else
      (*fit)->serializeXML(os);
  }
  os << "</intens:Document>" << std::endl;

//   RECODE_OUTER outer;
//   RECODE_REQUEST request;
//   outer = recode_new_outer (true);
//   request = recode_new_request (outer);
//   recode_scan_request (request, "ISO-8859-1..UTF-8");
//   _os << recode_string (request, os.str().c_str() );
//   recode_delete_request (request);
//   recode_delete_outer (outer);
_os << os.str().c_str();

  return true;
}


/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */
std::string SerializeFormStreamParameter::getName()const{
  return "SerializeFormStreamParameter::getName()";
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */
void SerializeFormStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 )
    dest -> putValue(*this, "SerializeFormStreamParameter::putValues()" );
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void SerializeFormStreamParameter::marshal( std::ostream &os ){
  os << "<SerializeFormStreamParameter";
  os << " xslFileName=\"" << m_xslFileName <<"\"";
  os << ">";
  os << "<formList>";
  GuiFormList::iterator it;
  for( it = m_formlist.begin(); it != m_formlist.end(); ++it ){
    os << "<form title=\"" << (*it)->getTitle() <<"\"/";
  }
  os << "</formList>";
  os << "</SerializeFormStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *SerializeFormStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "SerializeFormStreamParameter" ){
    m_xslFileName = attributeList["xslFileName"];
    return this;
  }
  if( element == "formList" ){
    return this;
  }
  if( element == "form" ){
    pushForm( attributeList["title"] );
    return this;
  }
  return 0;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void SerializeFormStreamParameter::setText( const std::string &text ){
}

/* --------------------------------------------------------------------------- */
/* pushForm --                                                                 */
/* --------------------------------------------------------------------------- */
bool  SerializeFormStreamParameter::pushForm( const std::string &form ){
  GuiForm *f = 0;
  if( ( form == "<MAINFORM>" || form.empty() ) && m_mainFormPos == -1 ){
    m_mainFormPos = m_formlist.size();
    f = GuiElement::getMainForm();
  }
  else
    f = GuiElement::getForm( form );

  if( f != 0 ){
    m_formlist.push_back( f );
    return true;
  }
  return false;
}
