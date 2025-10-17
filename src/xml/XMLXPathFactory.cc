
#include <iostream>
#include <sstream>

// only needed to prevent undeclared compile errors with QT
#ifdef HAVE_QT
typedef short Position;
#endif
// FIX IT!

#define Arg XARG  // to prevent redeclaration complaints in X11/Intrinsic.h
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "xml/XMLErrorHandler.h"
#include "xml/XMLXPathDocumentHandler.h"
#include "gui/GuiFactory.h"
#include "gui//GuiScrolledText.h"
#include "xfer/XferDataItem.h"
#include "utils/Debugger.h"
#include "utils/Date.h"

#include "gui/GuiIndex.h"

#include <algorithm>

#include "xml/XMLXPathFactory.h"
#include "xml/XMLXPathNodeManager.h"

// /*** static variable ***/
XPathFactories XMLXPathFactory::s_instances;

/*********************************************************
     Constructor
     Initialize instance (and create a DOM tree)
**********************************************************/
XMLXPathFactory::XMLXPathFactory(const std::string& xmlFile)
: m_nodeMgr(*this), m_parNode(NULL) {
}

/*********************************************************
     Destructor
     terminate instance (and xml things)
**********************************************************/
XMLXPathFactory::~XMLXPathFactory(){
}

/*********************************************************
//      Instance
//      create and handle for every xml file a (singleton) XMLXPathFactory instance
// **********************************************************/
XMLXPathFactory& XMLXPathFactory::Instance(const std::string& xmlFile) {

  XPathFactories::iterator it = XMLXPathFactory::s_instances.find( xmlFile );
  if (it == XMLXPathFactory::s_instances.end() )  {
    XMLXPathFactory* xPathFac = new XMLGnomeXPathFactory(xmlFile);
    s_instances[xmlFile] = xPathFac;
  }
  return (*(s_instances[xmlFile]));
}

/*********************************************************
//      ExistsInstance
//      query if instance of xml file exits
// **********************************************************/
bool XMLXPathFactory::ExistsInstance(const std::string& xmlFile) {

  XPathFactories::iterator it = XMLXPathFactory::s_instances.find( xmlFile );
  if (it == XMLXPathFactory::s_instances.end() )  {
    return false;
  }
  return true;
}

/**  presumably  ***/
std::string XMLXPathFactory::getLastXMLFile() {
  if (s_instances.empty())
    return std::string();
  XPathFactories::reverse_iterator it = s_instances.rbegin();
  return (*it).first;
}


/*********************************************************
     readData
     InputSource: XPathExpression
     read data from xml file and write into datapool
**********************************************************/
bool XMLXPathFactory::doQuery(const XPathExpr& xPathExpr, XferDataItem *xferdataitemDest) {
  BUG( BugPlugin, "XMLXPathFactory::readData(xPathExpr)");
//   cout << "  getNumIndexes(): " << xferdataitemDest->getNumIndexes() << endl;
//   cout << "  getNumNamedIndexes(): " << xferdataitemDest->getNumNamedIndexes() << endl;
//   cout << "  getNumOfWildcards(): " << xferdataitemDest->getNumOfWildcards() << endl;
//   for (int i=0; i < xferdataitemDest->getNumIndexes(); ++i) {
//     XferDataItemIndex* idx = xferdataitemDest->getDataItemIndex(i);
//     cout << "  isIndexed: " << idx->isIndexed() << endl;
//     cout << "  Lowerbound: " << idx->Lowerbound() << endl;
//     cout << "  Upperbound: " << idx->Upperbound() << endl;
//     cout << "  IndexNum: " << idx->IndexNum() << endl;
//     cout << "  InitialIndex: " << idx->InitialIndex() << endl;
//     cout << "  isWildcard: " << idx->isWildcard() << endl;
//     cout << "  IndexName: " << idx->IndexName() << endl;
//     //    cout << "  " << idx->() << endl;

//     if (idx->IndexName().size()) {
//       GuiElement *indx=GuiManager::Instance().getNamedElement( idx->IndexName() );
//       if (indx)
// 	cout  << "GuiIndexValue: " << dynamic_cast<GuiIndex *>( indx )->getIndex() << endl;
//     }
//   }

  if (!xferdataitemDest) {
    BUG_MSG("No destination set => return");
    return false;
  }

 // evaluate xpath expression
 try {
   //    vector<XPathNode*> m_resNodes;
   m_resNodes.erase(m_resNodes.begin(), m_resNodes.end());
   m_resCursor = 0;
   {
     std::stringstream logmsg;
     logmsg << " Start XPath query: [" << xPathExpr.getContextExpr() << "] into ["
	    << xferdataitemDest->getFullName(true) << "]" << std::endl;
     GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + logmsg.str() );
   }
   bool ret = evaluate(m_resNodes, xPathExpr.getContextExpr(), xPathExpr.getXPathExprs());

   if (!ret && m_resNodes.size() == 0) {
     // no node matched
     std::stringstream logmsg;
     logmsg << " No node matched the XPath query" << std::endl;
     GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + logmsg.str() );
     return false;
   } else {
     BUG_MSG("Result -- Got " <<  m_resNodes.size() << " nodes. ");

     return true;
    }
  }
  catch(...) {
    std::cerr << "Exception catched ... => abort XPath Query" << std::endl;
    std::stringstream logmsg;
    logmsg << "Exception catched ... => abort XPath Query" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    return false;
  }

  return true;
}

/*********************************************************
     doInsert
     makes insert from values into datapool
**********************************************************/
bool XMLXPathFactory::doInsert(const XPathExpr& xPathExpr, XferDataItem *xferDest, long count) {

  // look if an index was set
  int numIndexed = 0;
  XferDataItemIndex *idx;
  int numIdx = xferDest->getNumIndexes();
  while(numIdx-- > 0) {
    idx =  xferDest->getIndex(numIdx);
    if (idx->isIndexed()) {
      if (idx->IndexName().size()) {
	GuiElement *indx=GuiElement::findElement( idx->IndexName() );
	if (indx) {
// 	  int val = dynamic_cast<GuiIndex *>( indx )->getIndex();
	  int val = ( indx )->getGuiIndex()->getIndex();
	  //	  cout << " ==>   GuiIndexValue SET TO: " << val << std::endl;
	  idx->setIndex( xferDest->Data(), val );
	  continue;
	}
      }
      numIndexed++;
    }
  }
  assert( numIndexed <= 1 );

  // loop over all result XalanNodes
  int xn= m_resCursor;
  std::vector<XPathNode*>::iterator it = m_resNodes.begin() + xn;
  for(;it != m_resNodes.end(); ++it, ++xn) {

    // set index
    if (numIndexed == 1)
      idx->setIndex( xferDest->Data(), xn );

    // read process
    const std::string stdName( (char*) (*it)->name );
    const NodeInfo::SelectionList* selList = m_nodeMgr.getNodeInfo( xPathExpr.getSelectedNodes(), stdName );

    // initialize handler
    m_handler.setRoot(xferDest );
    m_handler.startDocument();
    if (xferDest->getName() != stdName &&
	(stdName == "vector" || stdName == "contents") ) {
      XMLAttributeMap attrs;
      m_handler.startElement(xferDest->getName(), attrs);
      //      DocumentHandler::gnomeStartElement(&m_handler, xferDest->getName().c_str(), NULL);
    }

    setParentNode( (*it) );
    if ( xPathExpr.getSelectedNodes().size())
      parseNodes( *it, *(selList) );
    else
      parseNodes( *it, xPathExpr.getSelectedNodes() );

    // clear  handler
    if (xferDest->getName() != stdName &&
 	(stdName == "vector" || stdName == "contents") )
      m_handler.endElement(xferDest->getName());
    m_handler.endDocument();

    // break?
    if (xn == (m_resCursor+count) ) {
      m_resCursor = xn;
      break;
    }
  }

  // end of inserting => do clear things
  if (xn == m_resNodes.size()) {
    //      delete handler;
    m_nodeMgr.clear();
    std::stringstream logmsg;
    logmsg << " " << m_resNodes.size() << " row(s) inserted" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + logmsg.str() );
    m_resNodes.erase(m_resNodes.begin(), m_resNodes.end());
    return true;
  }

  return false;
}

/*********************************************************
     convertToXPathExpr
     translate Varname of XferDataItem to a absolute XPath statement
**********************************************************/
std::string XMLXPathFactory::convertToXPathExpr(XferDataItem* xferDataItem) {
  BUG( BugPlugin, "XMLXPathFactory::convertToXPathExpr");
  std::string elemVarname( xferDataItem->getFullName(true) );
  std::string contextStr(elemVarname);

  // convert datapool varname to unabbreviated absolute location XPath
  // 1) replace char '.' to '/'
  // 2) increase (+1) indexes to normal denominatation
  std::replace(contextStr.begin(), contextStr.end(), '.', '/');
  std::string::size_type pos = 0, pos2 = 0;
  std::ostringstream os;
  int level = 0;
  while ( (pos = contextStr.find("[", pos2)) != std::string::npos) {
    if (contextStr[pos2] == '/') {
      os << '/';
      pos2++;
    }

    // make intens type file happy
    if ( getRootName() == "DATAPOOL") {
      if (pos2 != 0) {
	os << "descendant::";
      }
    }

    os << contextStr.substr(pos2,pos+0-pos2); // << "/descendant::contents[";
    pos2 = contextStr.find("]", pos) + 1;
    if (pos2 == std::string::npos) {
      BUG_MSG("Varname is invalid: [" << elemVarname << "]");
      return "";
    }
    std::istringstream is(contextStr.substr(pos+1, pos2-pos-1));
    int num;
    is >>  num;
    num++;

    // make intens type file happy
    if ( getRootName() == "DATAPOOL") {
      if (num > 1) {  //INTENS XML TYPE
	os << "/vector/contents[";
      } else {
	os << "/descendant::contents[";
      }
    } else
      os << "[";

    if ( xferDataItem->getDataItemIndex( level )->isWildcard()) {
      os << "*" << "]";
    } else
      os << num << "]";
    level++;
  }
  os << contextStr.substr(pos2);

  return std::string("/" + getRootName() + "/" + os.str());
}


/*********************************************************
     getXMLType
     get type of xml file
**********************************************************/
XMLXPathFactory::XML_FILE_TYPE  XMLXPathFactory::getXMLType() {
  if (getRootName() == "DATAPOOL")
    return INTENS_XML_TYPE;
  else
    return UNKNONW_XML_TYPE;
}










/*********************************************************
     Constructor
     Initialize instance
**********************************************************/
XPathExpr::XPathExpr(XferDataItem *xferDataItem) {

  // read all xpath Expression
  if (xferDataItem) {
    int level = xferDataItem->getNumberOfLevels();
    XferDataItemIndex* xferIndex;
    xferIndex = xferDataItem->getDataItemIndex(level-1);
    if (!xferIndex) {
      xferIndex = xferDataItem->newDataItemIndex(level-1);
    }
    xferIndex->setIndexName("ID", "");
    xferDataItem->setDimensionIndizes();
    int i =0;
    xferDataItem->setIndex("ID", i);
    std::string str;
    while(xferDataItem->getValue(str) || i<2)  {
      if (!str.empty()) {
	switch (i) {
	  case 0:
	    m_contextExpr = str;
	    //	    cout << "  ContextExpr: [" << str << "]\n";
	    break;
          case 1:
	    {
	      size_t pos = 0, pos2;
	      //	    remove(str.begin(), str.end(), ' ');
	      while ( (pos2=str.find_first_of(',', pos)) != std::string::npos) {
		addSelectedNode( str.substr(pos, pos2-pos) );
		pos = pos2 + 1;
	      }
	      addSelectedNode( str.substr(pos, str.size()-pos) );
	      break;
	    }
          default:
	    m_xpathExpr.push_back(str);
	    //	    cout << i << " .XPathExpr: [" << str << "]\n";
	}
	str="";
      }
      xferDataItem->setIndex("ID",++i);
    }
  }

/**** TEST
if (!m_xpathExpr.empty()) {
  bool bOkey = true;

  // get parts of first XPathExpression
  vector<std::string> XPathParts;
  vector<std::string> reverseXPathParts;
  size_t pos = 0, pos2;
  std::string str=m_xpathExpr[0];
  while ( (pos2=str.find_first_of('/', pos)) != std::string::npos) {
    XPathParts.push_back( str.substr(pos, pos2-pos) );
    pos = pos2 + 1;
  }
  XPathParts.push_back( str.substr(pos, str.size()-pos) );

  // process XPathExpression parts
  vector<std::string>::iterator it = XPathParts.begin();
  for(; it != XPathParts.end(); ++it) {
    // preprocessing
    if ((*it).find("::") == std::string::npos)
      (*it).insert(0, "child::");
    size_t pos = (*it).find("::");
    std::string axis;
    if (pos != std::string::npos) {
      axis = (*it).substr(0, pos);
    } else {
      bOkey = false;
      axis="nada";
    }

    // look for axis
    if (axis == "child") {
      reverseXPathParts.push_back("parent::"+ (*it).substr(pos+2));
    }
    else if (axis == "parent") {
      reverseXPathParts.push_back("child::"+ (*it).substr(pos+2));
    } else {
      bOkey = false;
    }
  }

}
****/
}

/*********************************************************
     Constructor
     Initialize instance
**********************************************************/
XPathExpr::XPathExpr(const std::string& xPathExpr)
  : m_contextExpr(xPathExpr) {
}

/*********************************************************
     addSelectedNode
     parse comma separated string and build a vector array
**********************************************************/
bool XPathExpr::addSelectedNode(const std::string& node) {

  std::string::size_type posBeg  = node.find_first_not_of(' ');
  if (posBeg == std::string::npos) posBeg = 0;
  std::string::size_type posEnd  = node.find_last_not_of(' ');
  if (posEnd == std::string::npos) posEnd = node.size()-1;
  std::string str = node.substr(posBeg, posEnd-posBeg+1);
  //cout << "  STR: [" << str << "]" <<std::endl;
  if (str.find(' ') != std::string::npos) // not allowed
    return false;
  m_selectedNodes.push_back( str );
  return true;
}
