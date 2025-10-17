
#ifndef XMLXPATHFACTORY_H
#define XMLXPATHFACTORY_H

#include <string>
#include <map>
#include <vector>

#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xmlmemory.h"
#include "libxml/xpath.h"
typedef xmlNode        XPathNode;
typedef const xmlChar* XPathString;

#include "xml/XMLXPathNodeManager.h"
#include "xml/XMLXPathDocumentHandler.h"

class XPathExpr;
class XMLXPathFactory;
class XMLXPathDocumentHandler;
class NodeInfo;
typedef std::map< std::string, XMLXPathFactory*> XPathFactories;
//typedef std::vector<std::string> SelectionList;

class XMLXPathFactory {
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLXPathFactory(const std::string& xmlFile);
  virtual ~XMLXPathFactory();

  //========================================================================
  // public functions
  //========================================================================
 public:
  static XMLXPathFactory &Instance(const std::string& xmlFile);
  static bool             ExistsInstance(const std::string& xmlFile);
  static std::string getLastXMLFile();

  bool   doQuery(const XPathExpr& xPathExpr, XferDataItem *xferdataitemDest);
  long   getNumResultNodes() { return m_resNodes.size(); }
  bool   doInsert(const XPathExpr& xPathExpr, XferDataItem *xferDest, long count);
  bool   doInsert(const XPathExpr& xPathExpr, XferDataItem *xferDest) {
    return doInsert(xPathExpr, xferDest, m_resNodes.size());
  };

  //========================================================================
  // private functions not yet implemented
  //========================================================================
 private:
  XMLXPathFactory(const XMLXPathFactory &);
  XMLXPathFactory &operator=(const XMLXPathFactory&);

  //========================================================================
  // protected functions
  //========================================================================
 protected:
  // minor facilties
  std::string  convertToXPathExpr(XferDataItem* xferDataItem);
  enum XML_FILE_TYPE {
    INTENS_XML_TYPE,
    UNKNONW_XML_TYPE
  };
  XML_FILE_TYPE            getXMLType();
  XMLXPathNodeManager&     getNodeManager()     { return  m_nodeMgr; }
  XMLXPathDocumentHandler& getDocumentHandler() { return  m_handler; }
  XPathNode*               getParentNode()      { return  m_parNode; }
  void                     setParentNode( XPathNode* pN)      { m_parNode = pN; }

  //========================================================================
  // private functions
  //========================================================================
 private:
  // main facilities
  virtual bool evaluate( std::vector<XPathNode*>& resXalaNodes,
			const std::string& contextPath,
			const std::vector<std::string>& xPathExpr=std::vector<std::string>()) = 0;
  virtual void           parseNodes(XPathNode *curNode, const NodeInfo::SelectionList& parSelList) = 0;
  virtual std::string    getRootName() const = 0;
  virtual std::string    convertToStdString(const XPathString& xstr) const = 0;

  //========================================================================
  // private variables
  //========================================================================
  // result node array and parent node
  std::vector<XPathNode*>    m_resNodes;
  long                  m_resCursor;
  XPathNode*            m_parNode;

  // instances from useful things
  XMLXPathNodeManager     m_nodeMgr;
  XMLXPathDocumentHandler m_handler;

  // array of XPathFactory instances
  static XPathFactories s_instances;

  friend class XMLXPathNodeManager;

};

class XMLGnomeXPathFactory  : public XMLXPathFactory {
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLGnomeXPathFactory(const std::string& xmlFile);
  virtual ~XMLGnomeXPathFactory();

  //========================================================================
  // private functions not yet implemented
  //========================================================================
 private:
  XMLGnomeXPathFactory(const XMLGnomeXPathFactory &);
  XMLGnomeXPathFactory &operator=(const XMLGnomeXPathFactory&);

  //========================================================================
  // private functions
  //========================================================================
 private:
  // main facilities
  bool        evaluate(std::vector<XPathNode*>& resXalaNodes,
		       const std::string& contextPath,
		       const std::vector<std::string>& xPathExpr=std::vector<std::string>());
  virtual void   parseNodes(XPathNode *curNode, const NodeInfo::SelectionList& parSelList);
  virtual std::string getRootName() const ;
  virtual std::string convertToStdString(const XPathString& xstr) const;

  //========================================================================
  // private variables
  //========================================================================
  xmlDocPtr          m_doc;
  xmlNodePtr         m_rootNode;
  xmlXPathContextPtr m_ctx;
};

class XPathExpr {

 public:
  XPathExpr(XferDataItem *xferDataItem);
  XPathExpr(const std::string& xPathExpr);

  const std::string&         getContextExpr()   const { return m_contextExpr;   }
  const std::vector<std::string>& getSelectedNodes() const { return m_selectedNodes; }
  const std::vector<std::string>& getXPathExprs()    const { return m_xpathExpr;     }

 private:
  bool addSelectedNode(const std::string& node);

  //  XPathExpr(const XPathExpr&);
  XPathExpr& operator=(const XPathExpr&);
  // XPathExpr* operator&();
  const XPathExpr* operator&() const;

  std::string          m_contextExpr;
  std::vector<std::string>  m_selectedNodes;
  std::vector<std::string>  m_xpathExpr;
};

class XMLXPathUtils {
 public:
  // DOM or XalanNode methods
 private:
  XMLXPathUtils();
};

#endif
