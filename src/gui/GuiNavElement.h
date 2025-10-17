
#ifndef GUI_NAV_ELEMENT_H
#define GUI_NAV_ELEMENT_H

#include "gui/GuiElement.h"
#include "gui/IconManager.h"
#include "job/JobElement.h"

class XferDataItem;
class GuiNavigator;
class XferDataItemIndex;
class JobFunction;
class GuiXNavElement;
class GuiQtNavElement;

/** Basis-Klasse fuer Navigator Elemente
 */
class GuiNavElement{
/*=============================================================================*/
/* public definitions of GuiXrtNavigatorNode                                   */
/*=============================================================================*/
 public:
  typedef std::vector<GuiNavElement *> NavElemVector;
  typedef std::pair<std::string, std::string> stringPair;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
 public:
  GuiNavElement( GuiNavigator *nav
		 , GuiNavElement *parent = 0
		 , XferDataItem *item = 0  );
  virtual ~GuiNavElement();
private:
  GuiNavElement( const GuiNavElement & );
  GuiNavElement &operator=( const GuiNavElement & );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
 public:
  /** liefert true, wenn das element als folder angezeigt werden soll
   */
  virtual bool isFolder(){ return true; }

  void destroy(){}
  /** erzeugt alle children zum eigenen
   *  oder zum angegebenen dataItem
   */
  virtual void createChildren( XferDataItem * const item = 0 ) = 0;
  void addChildren( int actualCount );
  void removeChildren( int count );
  /** startet die Funktion, die an dem dataItem hängt
   */
  virtual void runJobFunction( JobElement::CallReason reason, XferDataItem *source = 0, int xpos=-1, int ypos=-1 );
  virtual void runJobFunction( JobElement::CallReason reason, const std::vector<XferDataItem*>& sourceList, int xpos=-1, int ypos=-1 );
  virtual void runJobFunction( JobElement::CallReason reason, XferDataItem *source, XferDataItem *source2);
  /** Erzeugt das wenn nötig das widget und ruft refreshChildren auf
   */
  virtual void refresh() = 0;

  /** ermittelt die aktuelle Anzahl Children ( actualCount )
   *  und die DimensionSize des Xfers ( newCount )
   *  Ist kein Xfer vorhanden, wird false zurückgegeben
   */
  virtual bool getChildrenCount( int &actualCount, int &newCount ) = 0;
  virtual void update( GuiElement::UpdateReason ur );
  void updateNode(GuiElement::UpdateReason ur);


  const std::vector<stringPair> &getLabel() { return m_labels; }
  const std::string getNodeName() const;
  const std::string getClassName() const;
  bool getType(std::string& type) const;
  bool getDiagramNodeName(std::string& node_name) const;
  bool getDiagramMenuId(int& menuId) const;
  bool getDiagramPosition(int& xpos, int& ypos);
  bool isOpen();
  GuiNavigator *getNavigator(){ return m_navigator; }
  virtual void newParent();
  void transferProc( const std::string &data );
  JobFunction *getJobFunction() const;
  GuiPopupMenu* getPopupMenu(unsigned int idx=0) const;
  GuiPopupMenu* getPopupDropMenu() const;
  virtual GuiXNavElement *getXNavElement(){ return 0; }
  virtual GuiQtNavElement *getQtNavElement(){ return 0; }
  virtual void updateWidget() = 0;
  XferDataItem *getDataItem() const{ return m_dataItem; }
  void newParent( bool state ) { m_newParentFlag = state; }
  virtual bool isSelected(){ return false; }
  void deleteNodes();
  /** setzen des root index für die Reihenfolge im Navigator (nur NavRootElement)
   */
  virtual void setRootIndex(int rootIdx) {}
  /** abfrage des root index für die Reihenfolge im Navigator (nur NavRootElement)
   */
  virtual int getRootIndex() const { if (getParent()) { return getParent()->getRootIndex(); } return -1; }
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, in_proto::NavigatorNode* element, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
 protected:
  bool hasNewParent() const { return m_newParentFlag; }
  GuiNavElement *getParent() const { return m_parent; }
  GuiNavElement *getLastNode() const { return m_nodes.empty() ? 0 : m_nodes.back(); }
  /** liefert das dataItem
   */
  void setDataItem( XferDataItem *item ){ m_dataItem = item; }
  const NavElemVector& getNodes() const { return m_nodes; }
  GuiNavigator *getNavigator() const { return m_navigator; }
  /** Erzeugt einen neuen node
   */
  virtual GuiNavElement *addNode( XferDataItem *item )=0;
  void addNode( GuiNavElement *node );
  /** liefert true und den ersten level, auf dem die structFolders gezeigt
   *  werden sollen, wenn firstStructFolder beim root gesetzt ist.
   *  Sonst false.
   */
  virtual bool getFirstStructFolder( int &firstStructFolder ) const;
  /** liefert true und den letzten sichtbaren level,
   *  wenn lastLevel beim root gesetzt ist.
   *  Sonst false.
   */
  virtual bool getLastLevel( int &lastLevel ) const;
  /** liefert true und die Anzahl zu öffnende Levels, wenn openLevels beim root gesetzt ist.
   *  Sonst false.
   */
  virtual bool getOpenLevels( int &openLevels ) const;
  /** liefert true, wenn autoLevel beim root gesetzt ist
   */
  virtual bool getAutoLevel() const;
  /** liefert true, wenn hideEmptyFolder beim root gesetzt ist
   */
  virtual bool getHideEmptyFolder() const;
  /** liefert true, wenn das element angezeigt werden soll
   */
  virtual bool isVisible() const;
  /** liefert den letzten index
   */
  virtual XferDataItemIndex *getIndex( XferDataItem *item );
  /** fuegt beim navigator einen style dazu
   */
  virtual bool addNodeStyle(){ return false; }
  void refreshChildren();
  void getStructName( XferDataItem *item, std::string &name );
  void getToolTip( XferDataItem *item, std::string &tooltip );
  /** liefert den level des elementes
   */
  int getLevel() const;
  int getRootLevel() const;
  void clearLabels() { m_labels.clear(); }
  void addLabel( const std::string &label, const std::string &tag );
  void addLabel( const std::string &label );
  bool setLabel( const std::string &label, int index );
  std::string getIconPixmapFileName(IconManager::ICON_TYPE& icon_type);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  NavElemVector            m_nodes;
  std::vector<stringPair>  m_labels;
  GuiNavigator            *m_navigator;
  GuiNavElement           *m_parent;
  bool                     m_newParentFlag;
  XferDataItem            *m_dataItem;

  // used as pixmap cache
  struct PixmapData {
    PixmapData()
      : icon_type(IconManager::NONE) {}
    PixmapData(const std::string& filename,
               const IconManager::ICON_TYPE& type)
      : icon_filename(filename), icon_type(type) {}
    std::string            icon_filename;
    IconManager::ICON_TYPE icon_type;
  };
  typedef std::map<std::string, PixmapData> PixmapNameMap;
  static PixmapNameMap s_pixmap_name_map;
};

#endif
