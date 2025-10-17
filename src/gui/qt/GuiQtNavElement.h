
#ifndef GUI_QT_NAV_ELEMENT_H
#define GUI_QT_NAV_ELEMENT_H

#include <map>

#include <QTreeWidget>

#include "gui/GuiElement.h"
#include "gui/GuiNavElement.h"
#include "gui/GuiMenuButtonListener.h"

class QColorGroup;
class QGraphicsItem;
class GuiQtDiagramPixmapItem;
class GuiQtNavigatorMenu;
class GuiQtPopupMenu;

class GuiQtNavElement : public GuiNavElement{
public:
  //-----------------------------------------------------------
  // Constructor / Destructor
  //-----------------------------------------------------------
public:
  GuiQtNavElement( GuiNavigator *nav
		 , GuiNavElement *parent = 0
		 , XferDataItem *item = 0  );
  virtual ~GuiQtNavElement();

  //-----------------------------------------------------------
  // member functions of GuiNavElement
  //-----------------------------------------------------------
public:
  virtual GuiQtNavElement *getQtNavElement(){ return this; }
  virtual void updateWidget();

  //-----------------------------------------------------------
  // public member functions
  //-----------------------------------------------------------
public:
  bool popup(const QPoint& pos, unsigned int idx=0, bool show=true) const;
  bool popupDrop(const QPoint& pos, std::string& data, bool show=true) const;
  bool addPopupMenu(std::map<std::string, GuiMenuButtonListener*>& addMenu);
  void setLabels();
  void setTreeItem( QTreeWidgetItem* item );
  void setDiagramItem( QGraphicsItem* item );
  QTreeWidgetItem *myTreeItem(){ return m_treeItem; }
  void setModelItemIndex(long index);
  long getModelItemIndex() { return m_modelItemIndex; }
  bool hasItem();
  int myTreeItemIndex(){ return m_treeItem->parent()->indexOfChild (m_treeItem); }
  QTreeWidgetItem *getTreeItem();
  QGraphicsItem *getDiagramItem();
  virtual void moveChildren( GuiNavElement *i = 0 );
  virtual bool startRename( int col ){ return false; }
  virtual void okRename( int col ){}
  virtual void setValidator( int col ){};
  virtual void move( QTreeWidgetItem *i )=0;
  virtual bool isSelected();
  virtual bool getColors( int column, QColor &background, QColor &foreground ){ return false; }
  void getPixmap(QPixmap& icon);

  GuiQtPopupMenu* getPopupMenu(unsigned int idx) const;
  GuiQtPopupMenu* getPopupDropMenu() const;

  //-----------------------------------------------------------
  // protected member functions
  //-----------------------------------------------------------
protected:
  QTreeWidgetItem *getParentTreeItem() const;
  QTreeWidgetItem *getLastTreeChild( QTreeWidgetItem *item ) const;

  //-----------------------------------------------------------
  // private members
  //-----------------------------------------------------------
private:
  QTreeWidgetItem        *m_treeItem;
  GuiQtDiagramPixmapItem *m_diagramItem;
  long                    m_modelItemIndex;
  GuiQtNavigatorMenu*     m_menu;

  // used as pixmap cache
  typedef std::map<std::string, QPixmap> PixmapMap;
  static PixmapMap        s_pixmap_map;
};

//*************************************************************
// class MyQListViewItem
//*************************************************************
class MyQListViewItem : public QTreeWidgetItem{
  //-----------------------------------------------------------
  // Constructor / Destructor
  //-----------------------------------------------------------
public:
  MyQListViewItem( GuiQtNavElement *parent, QTreeWidgetItem *item, QTreeWidgetItem *after );
  MyQListViewItem( GuiQtNavElement *parent, QTreeWidget *view, QTreeWidgetItem *after );
  virtual ~MyQListViewItem();

  //-----------------------------------------------------------
  // members of QListViewItem
  //-----------------------------------------------------------
public:
  virtual void startRename( int col );
  virtual void okRename( int col );

  //-----------------------------------------------------------
  // public members
  //-----------------------------------------------------------
public:
  GuiQtNavElement *parent(){ return m_parent; }
  void drop( const QString &data );
  bool isFolder(){ return m_parent->isFolder(); }
  void activated();
  void selected();

  //-----------------------------------------------------------
  // private members
  //-----------------------------------------------------------
private:
  GuiQtNavElement *m_parent;
};

#endif
