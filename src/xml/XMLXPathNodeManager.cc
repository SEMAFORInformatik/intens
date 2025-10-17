
#include "xml/XMLXPathFactory.h"
#include "xml/XMLXPathNodeManager.h"
#include <iostream>

//========================================================================
// destructor
//========================================================================
XMLXPathNodeManager::~XMLXPathNodeManager() {
  // erase RootSelecListMap
  RootSelecListMap::iterator rit = m_rootSelecListMap.begin();
  for (;rit != m_rootSelecListMap.end(); ++rit) {
    ElemNameSelecListMap::iterator eit2 = (*rit).second->begin();
    for (;eit2 != (*rit).second->end(); ++eit2) {
      if ((*eit2).second)
	delete (*eit2).second;
    }
    (*rit).second->erase( (*rit).second->begin(), (*rit).second->end() );
    delete (*rit).second;
  }
  m_rootSelecListMap.erase( m_rootSelecListMap.begin(), m_rootSelecListMap.end() );
}

//========================================================================
// getNodeInfo
//========================================================================
const NodeInfo::SelectionList*
XMLXPathNodeManager::getNodeInfo(const NodeInfo::SelectionList& parentSL, const std::string& elemName) {

  // IF EMPTY PARENT SELECTION LIST => get only element name in std::string format
  // => return stdName (SL is NULL)
  if ( parentSL.empty() ) {
    return NULL;
  }

  // IF SELECTION LIST WAS ALREADY CALCULATED => return found entry
  RootSelecListMap::iterator rit = m_rootSelecListMap.find( &parentSL );
  if (rit != m_rootSelecListMap.end()) {
    ElemNameSelecListMap::iterator it2 = (*rit).second->find( elemName );
    if ( it2 != (*rit).second->end() ) {
      return (*it2).second;
    }
  }

  // IF SELECTION LIST NOT CALCULATED => calcuate and return new entry
  bool bValid = false;
  const std::string&  eName( elemName );
  NodeInfo::SelectionList *nSL = new NodeInfo::SelectionList();
  if (eName == m_xpathFactory.getRootName() ||
      eName == "v"                          ||
      eName == "value"                      ||  // obsolete
      eName == "vector"                     ||
      eName == "contents"                     ) {
    nSL->insert(nSL->end(), parentSL.begin(), parentSL.end());
    bValid = true;
  }
  else {
    NodeInfo::SelectionList::const_iterator pit = parentSL.begin();
    for (;pit != parentSL.end(); ++pit) {

      if ((*pit).find("*/") == 0) {
	nSL->push_back((*pit).substr(2));
      } else {
	std::string::size_type pos = (*pit).find( eName );
	if (pos == 0) {
	  std::string::size_type pos2 = (*pit).find(".");
	  if (eName.size() < (*pit).size() && pos2 != std::string::npos) {
	    nSL->push_back((*pit).substr(pos2+1));
	  } else if (eName.size() == (*pit).size()) {
//	    nSL->push_back("ALL_CHILDS");
	    bValid = true;
	    break;
	  }
	} else {
	  std::string desc("descendant::");
	  if ( (pos = (*pit).find( desc )) == 0) {
	    std::string::size_type pos2 = (*pit).find(".", pos+1);
	    if (pos2 == std::string::npos)
	      pos2 = (*pit).size();
	    std::string rest( (*pit).substr( desc.size(), pos2 - desc.size()) );
	    if (rest == eName && pos2 != (*pit).size()) {
	      nSL->push_back( (*pit).substr(pos2+1, (*pit).size()-pos2) );
	    } else
	      nSL->push_back( (*pit) );
	  }
	}
      }
    }
  }

  // insert new node (child) selection list
  if (!bValid && nSL->size())
    bValid = true;
  NodeInfo *chInfo = new NodeInfo( eName, (bValid ? nSL : NULL) );
  if (rit != m_rootSelecListMap.end()) {
    (*((*rit).second))[ elemName ] = (bValid ? nSL : NULL);
  } else {
    ElemNameSelecListMap *eSL = new ElemNameSelecListMap();
    //    (*eSL)[ &elemName ] = chInfo;
    (*eSL)[ elemName ] = (bValid ? nSL : NULL);
    m_rootSelecListMap[&parentSL] = eSL;
    //    cout << "1 Insert NEW ENTRY IN Elem List[" << eName << "] of new Root [" <<  &parentSL << "] " << nSL->size() << endl;
  }
  if (!bValid)
    delete nSL;
  return (bValid ? nSL : NULL);
}


//========================================================================
// clear
//========================================================================
void XMLXPathNodeManager::clear() {

  // erase RootSelecListMap
  RootSelecListMap::iterator rit = m_rootSelecListMap.begin();
  for (;rit != m_rootSelecListMap.end(); ++rit) {
    delete (*rit).second;
  }
  m_rootSelecListMap.erase( m_rootSelecListMap.begin(), m_rootSelecListMap.end() );
}
