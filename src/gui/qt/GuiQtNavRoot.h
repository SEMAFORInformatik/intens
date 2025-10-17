
#ifndef GUI_QT_NAV_ROOT_H
#define GUI_QT_NAV_ROOT_H

#include <string>
#include <vector>

#include "gui/qt/GuiQtNavElement.h"
#include "gui/GuiNavigator.h"

class XferDataItem;
class XferDataItemIndex;

class GuiQtNavRoot : public GuiQtNavElement, public GuiIndexListener {
  //------------------------------------------------------------
  // Constructor / Destructor
  //------------------------------------------------------------
 public:
  GuiQtNavRoot( GuiNavigator::Root *root, GuiNavigator *nav );
  GuiQtNavRoot( GuiNavigator *nav
	      , const std::string &name
	      , int *lastLevel
	      , int *firstStructFolder
	      , bool autoLevel
	      , int *openLevels
	      , bool hideEmptyFolder);
  virtual ~GuiQtNavRoot();

  void setIndex( const std::string &name, int inx );
  bool acceptIndex( const std::string &name, int inx );

 private:
  GuiQtNavRoot( const GuiQtNavRoot & );
  void operator=( const GuiQtNavRoot & );

  //-----------------------------------------------------------
  // member functions of GuiNavElement
  //-----------------------------------------------------------
 public:
  virtual void newParent();
  virtual void createChildren( XferDataItem * const item = 0 );
  virtual void refresh();
  virtual bool getChildrenCount( int &actualCount, int &newCount );
  virtual void move( QTreeWidgetItem *w ){ assert( false ); }
  virtual void moveChildren( QTreeWidgetItem *i = 0 );
  virtual bool isVisible() const;
  virtual void setRootIndex(int rootIdx);
  virtual int getRootIndex() const { return m_rootIndex; }
 protected:
  virtual GuiNavElement *addNode( XferDataItem *item );
  virtual bool getFirstStructFolder( int &firstStructFolder ) const;
  virtual bool getOpenLevels( int &openLevels ) const;
  virtual bool getLastLevel( int &lastLevel ) const;
  virtual bool getAutoLevel() const;
  virtual bool getHideEmptyFolder() const;
  virtual XferDataItemIndex *getIndex( XferDataItem *item );

  //-----------------------------------------------------------
  // private members
  //-----------------------------------------------------------
 private:
  int                   *m_lastLevel;
  int                   *m_firstStructFolder;
  int                   *m_openLevels;
  bool                   m_autoLevel;
  bool                   m_hideEmptyFolder;
  int                    m_rootIndex;
};

#endif
