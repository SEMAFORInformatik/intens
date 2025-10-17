#include <iostream>
#include <sstream>

#define Arg XARG  // to prevent redeclaration complaints in X11/Intrinsic.h
#include "app/DataPoolIntens.h"
#include "xml/XMLXPathDocumentHandler.h"
#include "xml/XMLDocumentHandler.h"
// only needed to prevent undeclared compile errors with QT
#ifdef HAVE_QT
typedef short Position;
#endif
// FIX IT!

#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "xfer/XferDataItem.h"
#include "utils/Debugger.h"

#include <algorithm>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>

#include "XMLXPathFactory.h"

extern "C" {
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

};


/*********************************************************
     Constructor
     Initialize instance (and create a DOM tree)
**********************************************************/
XMLGnomeXPathFactory::XMLGnomeXPathFactory(const std::string& xmlFile)
  : XMLXPathFactory(xmlFile), m_doc(NULL), m_rootNode(NULL), m_ctx(NULL) {

  // do initializing
  xmlKeepBlanksDefault(1);
  if ( xmlFile.empty() ) return;
  m_doc = xmlParseFile(xmlFile.c_str());
  m_rootNode = xmlDocGetRootElement(m_doc);
  xmlInitParser();
  m_ctx = xmlXPathNewContext(m_doc);
}

/*********************************************************
     Destructor
     terminate instance (and xml things)
**********************************************************/
XMLGnomeXPathFactory::~XMLGnomeXPathFactory(){
  xmlXPathFreeContext(m_ctx);
  xmlFreeDoc(m_doc);
}


/*********************************************************
     parseNodes
     recursive function to read complete (incl. childs) node data
     and write it into the datapool
**********************************************************/
void XMLGnomeXPathFactory::parseNodes(XPathNode *curNode, const NodeInfo::SelectionList& parSelList) {

  if ( !curNode ) return;
  bool bTopNode = (getParentNode() == curNode);
  xmlNode*  chNode = curNode;

  // loop over all childs
  do {
    if (!chNode) break;

    //
    //  process text node
    if (chNode->type == XML_TEXT_NODE)  {
      xmlChar* ch = xmlXPathCastNodeToString(chNode);
      if ( xmlStrlen(ch) &&
           !(xmlStrlen(ch) == 1 && ch[0] == '\n')) {
        std::string value( convertToStdString(ch) );
        getDocumentHandler().characters( value );
      }
    }

    //
    //  process element node
    else  if (chNode->type == XML_ELEMENT_NODE) {

      // get element name
      const std::string stdName( convertToStdString(chNode->name) );
      const NodeInfo::SelectionList* selList = getNodeManager().getNodeInfo( parSelList, stdName);

      // if Node not selected (Parent SelList has entries, but new hasn't) ==> Continue
      if (parSelList.size() && !(selList)) {
        continue;
      }

      // get node properties
      XMLAttributeMap attrs;
      xmlAttrPtr n = chNode->properties;
      while(n) {
        std::string prop( convertToStdString( n->name ) );
        std::string val( convertToStdString( (xmlGetProp(chNode, n->name) )) );
        attrs[ prop ] = val;
        n = n->next;
      }

      // Child selected => do further processing
      getDocumentHandler().startElement( stdName, attrs );
      if ( selList ) {
        parseNodes( chNode->children, *(selList) );
      }
      else if (parSelList.empty()) {
        parseNodes( chNode->children, parSelList );
      }
      getDocumentHandler().endElement( stdName );

    }  // end process element node

    // goto next child
    if (chNode == curNode->last)
      break;

  }  while ((!bTopNode) && (chNode = chNode->next) != NULL);
}


/*********************************************************
     evaluate
     InputSource: resXalaNodes result vector
                  contextPath context xpath expression
		  xPathExpr (optional) further xpath expression
		            for context path (=> where clause)
     process xpath expression and returns vector of selected nodes
**********************************************************/
bool XMLGnomeXPathFactory::evaluate(std::vector<XPathNode*>& resXalaNodes,
                                    const std::string& contextPath,
                                    const std::vector<std::string>& xPathExpr) {
  BUG( BugPlugin, "XMLGnomeXPathFactory::evaluate");
  bool bSetContextIdx = false;
  xmlXPathParserContextPtr ctxt;

  // clear resXalaNodes
  resXalaNodes.erase(resXalaNodes.begin(), resXalaNodes.end());

  try {
    std::string tmp(contextPath);
    if (tmp.find_last_of(']') != (tmp.size()-1) &&
        tmp.find("descendant") == std::string::npos     ) {
      tmp += "[1]";
      bSetContextIdx = true;
    }

    xmlChar *contextExpr = xmlCharStrdup( tmp.c_str() );
    BUG_MSG("ContextExpression: " << contextExpr);

    xmlXPathEval(contextExpr, m_ctx);
  }
  catch (...) {
    std::stringstream logmsg;
    logmsg << "Warning -- Excecption catched => invalid context XPath Expression\""
           << contextPath << "\"." << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    throw;
  }

  if (ctxt->valueNr == 0 ||
      !ctxt->valueTab[0]->nodesetval ||
      (ctxt->valueTab[0]->nodesetval && ctxt->valueTab[0]->nodesetval->nodeNr == 0)) {
    // no context selected => wrong context query
    BUG_MSG("Warning -- No nodes matched the location path \"" << contextPath << "\"");
    std::cerr << "Warning -- No nodes matched the location path \"" << contextPath << "\"." << std::endl;
    std::stringstream logmsg;
    logmsg << "Warning -- No node(s) matched the location path \"" << contextPath << "\"." << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    xmlXPathFreeParserContext(ctxt);
    return false;
  } else {

    // XPathExpr not set => no (further) xpath expression => copy result array
    int n;
    if (xPathExpr.empty() ) {
      if(ctxt->valueNr > 0 &&
         ctxt->valueTab[0]->nodesetval->nodeNr > 0) {
        if (bSetContextIdx) {
          xmlNodePtr node;
          for (node = ctxt->valueTab[0]->nodesetval->nodeTab[0];node;
               node = xmlXPathNextFollowingSibling(ctxt, node) ) {
            if (node->type == XML_ELEMENT_NODE)
              resXalaNodes.push_back( node );
          }
        } else {
          for (long n=0; n < ctxt->valueTab[0]->nodesetval->nodeNr; ++n) {
            xmlNodePtr node = ctxt->valueTab[0]->nodesetval->nodeTab[n];
            if (!node) break;
            resXalaNodes.push_back( node );
          }
        }
      }
      BUG_MSG("No xpath expression set => copy ALL context results");
    } else {

      // compile xpath (where) expression
      std::vector<xmlXPathCompExprPtr> xPathCompiledExpr;
      std::vector<std::string>::const_iterator xit = xPathExpr.begin();
      for (; xit != xPathExpr.end(); ++xit) {
        xPathCompiledExpr.push_back(xmlXPathCompile(  xmlCharStrdup((*xit).c_str())  ) );
      }


      // XPathExpr set => process xpath expression
      long n = 0;
      for (m_ctx->node = ctxt->valueTab[0]->nodesetval->nodeTab[0];m_ctx->node; ) {

        // validate all xpath (where) expressions
        std::vector<xmlXPathCompExprPtr>::iterator eit = xPathCompiledExpr.begin();
        for (; eit != xPathCompiledExpr.end(); ++eit) {

          try {
            xmlXPathObjectPtr pathExpr = xmlXPathCompiledEval ((*eit), m_ctx);

            if ( !(pathExpr->boolval) ) {
              xmlXPathFreeObject(pathExpr);
              break;
            }
            xmlXPathFreeObject(pathExpr);
          }
          catch(...) {
            break;   // also not fulfilled if exception occurs
          }
        }
        // add to result array if all xpath (where) expressions are going well
        if (eit == xPathCompiledExpr.end())
          resXalaNodes.push_back(m_ctx->node);

        // goto next node
        if (bSetContextIdx) {
          do {
            m_ctx->node = xmlXPathNextFollowingSibling(ctxt, m_ctx->node);
          } while (m_ctx->node && m_ctx->node->type != XML_ELEMENT_NODE);
        } else {
          if (++n >= ctxt->valueTab[0]->nodesetval->nodeNr)
            break;
          m_ctx->node = ctxt->valueTab[0]->nodesetval->nodeTab[n];
        }
      }

      // free compiled xpath (where) expression
      std::vector<xmlXPathCompExprPtr>::const_iterator eit2 = xPathCompiledExpr.begin();
      for (; eit2 != xPathCompiledExpr.end(); ++eit2) {
        xmlXPathFreeCompExpr( (*eit2) );
      }

      BUG_MSG("Number where expressions[" << xPathExpr.size() << "] => xpath expression results [" << resXalaNodes.size() << "]");
    }
  }
  // free context object
  xmlXPathFreeParserContext(ctxt);

  return true;
}
/*********************************************************
     getRootName
     return name of root tag
**********************************************************/
std::string XMLGnomeXPathFactory::getRootName() const {
  if (!m_rootNode)
    return "";
  std::string root( convertToStdString(m_rootNode->name) );
  return root;
};

/*********************************************************
     convertToStdString
     return name of parent tag
**********************************************************/
std::string XMLGnomeXPathFactory::convertToStdString(const XPathString& src) const {

  std::ostringstream os;

  // convert to iso8859-1
  int lenUTF = strlen((const char*) src);
  xmlChar *stringISO = new xmlChar[lenUTF+1];
  int lenISO = sizeof(xmlChar) * (lenUTF+1);
  memset(stringISO, 0, lenISO);
  if (UTF8Toisolat1(stringISO, &lenISO, src, &lenUTF) == 0) {
    os << stringISO;
  } else {
    os << src;
  }
  delete[] stringISO;

  return std::string( os.str() );
}
