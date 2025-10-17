
#include "utils/Debugger.h"
#include <limits>
#include <math.h>

#include "app/DataSet.h"
#include "app/DataPoolIntens.h"
#include "xfer/XferDataParameter.h"
#include "xfer/XferParameterString.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "streamer/GuiIndexStreamParameter.h"
#include "xml/XMLDocumentHandler.h"
#include "utils/LaTeXConverter.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiIndexStreamParameter::GuiIndexStreamParameter( GuiIndex *index )
  : m_guiIndex( index ),  m_conv( 0, 0, false) {
}

GuiIndexStreamParameter::~GuiIndexStreamParameter(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

size_t GuiIndexStreamParameter::getDimensionSize(int ndim) const {
  return std::numeric_limits<int>::max();
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool GuiIndexStreamParameter::read( std::istream &is ){
  int i;
  bool rslt = true;
  if( m_conv.read( is, i, m_delimiter ) ){
    if (!std::isnan(i) && i!=std::numeric_limits<int>::min()) {
      if( m_guiIndex->isIndexAccepted(i) ){
	m_guiIndex->setIndex(i);
	m_guiIndex->setNewIndex();
      }
    }
  }

  return false;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool GuiIndexStreamParameter::write( std::ostream &os ){
  BUG_PARA(BugStreamer, "write", "name='" << m_guiIndex->getElement()->getName() << "'" );
  if( os << m_guiIndex->getIndex() ){
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void GuiIndexStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 )
    dest -> putValue(*this, m_guiIndex->getIndex() );
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiIndexStreamParameter::isValid(bool validErrorMsg){
  return m_guiIndex->isValid();
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiIndexStreamParameter::marshal( std::ostream &os ){
  os << "<GuiIndexStreamParameter>";
  os <<  m_guiIndex->getIndex();
  os << "</GuiIndexStreamParameter>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *GuiIndexStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiIndexStreamParameter::setText( const std::string &text ){
}
