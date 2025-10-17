
#if !defined(XML_XPATHSELECTIONLIST_H)
#define XML_XPATHSELECTIONLIST_H

class XMLXPathFactory;

#include <string>
#include <vector>
#include <map>

class NodeInfo{
 public:
  typedef std::vector<std::string> SelectionList;

  NodeInfo(const std::string& name, const SelectionList* slist=NULL) : stdName(name), selList(slist) {}
  ~NodeInfo() { if (selList) delete selList; }
  const SelectionList* SelList() const { return selList; }
  const std::string&   StdName() const { return stdName; }

 private:
  const SelectionList *selList;
  std::string          stdName;
};

class XMLXPathNodeManager {
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLXPathNodeManager( const XMLXPathFactory& xpathFac )
    : m_xpathFactory(xpathFac)  {}
  ~XMLXPathNodeManager();

  //========================================================================
  // public functions of XMLXPathNodeManager
  //========================================================================
 public:
  const NodeInfo::SelectionList* getNodeInfo(const NodeInfo::SelectionList& parentSL, const std::string& elemName);
  void  clear();

  //========================================================================
  // private members
  //========================================================================
 private:
  typedef std::map<std::string, NodeInfo::SelectionList*>  ElemNameSelecListMap;
  typedef std::map<const NodeInfo::SelectionList*, ElemNameSelecListMap*> RootSelecListMap;

  RootSelecListMap     m_rootSelecListMap;

  const XMLXPathFactory& m_xpathFactory;
};

#endif
