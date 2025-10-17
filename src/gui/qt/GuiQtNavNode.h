
#ifndef GUI_QT_NAV_NODE_H
#define GUI_QT_NAV_NODE_H

#include "gui/qt/GuiQtNavElement.h"

class GuiQtNavTextfield;

class GuiQtNavNode : public GuiQtNavElement{
  //-----------------------------------------------------------
  // Constructor / Destructor
  //-----------------------------------------------------------
 public:
  GuiQtNavNode( GuiNavElement *parent
	      , GuiNavigator *nav
	      , XferDataItem *item );
  virtual ~GuiQtNavNode();
 private:
  GuiQtNavNode( const GuiQtNavNode & );
  void operator=(const GuiQtNavNode & );

  //-----------------------------------------------------------
  // member functions of GuiNavElement
  //-----------------------------------------------------------
 public:
  virtual bool getChildrenCount( int &actualCount, int &newCount );
  virtual void update( GuiElement::UpdateReason ur );
  virtual void updateWidget();
  virtual void runJobFunction( JobElement::CallReason reason, XferDataItem *source = 0 );
  void createStructVariableChildren( XferDataItem * const item = 0 );
  virtual void createChildren( XferDataItem * const item = 0 );
  virtual bool isVisible() const;
  virtual bool isFolder();
  virtual void refresh();
protected:
  virtual GuiNavElement *addNode( XferDataItem *item );
  virtual GuiNavElement *addNode( const std::string &label );

  //-----------------------------------------------------------
  // member functions of GuiQtNavElement
  //-----------------------------------------------------------
public:
  virtual bool startRename( int col );
  virtual void okRename( int col );
  virtual void setValidator( int col );
  virtual void move( QTreeWidgetItem *i );
  virtual bool getColors( int column, QColor &background, QColor &foreground );

  //-----------------------------------------------------------
  // private member functions
  //-----------------------------------------------------------
private:
  bool isLastLevel() const;
  bool refreshLabels();
  std::vector<GuiQtNavTextfield *> &getTextfields(){ return m_textfields; }
  void createTextfields( GuiNavigator *nav, XferDataItem *item );
  void createTextfields4StructVariable( GuiNavigator *nav, XferDataItem *item );

  //-----------------------------------------------------------
  // private members
  //-----------------------------------------------------------
 private:
  std::vector<GuiQtNavTextfield *> m_textfields;
};

#endif
