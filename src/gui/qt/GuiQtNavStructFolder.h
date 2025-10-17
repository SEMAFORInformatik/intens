
#ifndef GUI_QT_NAV_STRUCT_FOLDER_H
#define GUI_QT_NAV_STRUCT_FOLDER_H

#include "gui/qt/GuiQtNavElement.h"
class GuiQtNavStructFolder : public GuiQtNavElement{
  //------------------------------------------------------------
  // Constructor / Destructor
  //------------------------------------------------------------
public:
  GuiQtNavStructFolder ( GuiNavigator *nav
			 , GuiNavElement *parent
			 , const std::string &label );
  ~GuiQtNavStructFolder();

  //-----------------------------------------------------------
  // member functions of GuiNavElement
  //-----------------------------------------------------------
public:
  virtual void updateWidget();
  virtual bool isVisible() const;
  virtual void createChildren( XferDataItem * const item = 0 );
  virtual void refresh();
  virtual bool getChildrenCount( int &actualCount, int &newCount );
  virtual void move( QTreeWidgetItem *i );
protected:
  virtual GuiNavElement *addNode( XferDataItem *item );

  //-----------------------------------------------------------
  // member functions of GuiQtNavElement
  //-----------------------------------------------------------


};

#endif
