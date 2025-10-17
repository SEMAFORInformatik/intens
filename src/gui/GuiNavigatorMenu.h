
#ifndef GUI_NAVIGATOR_MENU_INCLUDED_H
#define GUI_NAVIGATOR_MENU_INCLUDED_H

#include "gui/GuiPopupMenu.h"

class GuiNavigator;
class XferDataItem;

class GuiNavigatorMenu
{
/*=============================================================================*/
/* constructor / destructor                                                    */
/*=============================================================================*/
protected:
  GuiNavigatorMenu();
  GuiNavigatorMenu( const GuiNavigatorMenu& menu );
  virtual ~GuiNavigatorMenu();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool cloneable() { return true; }
  virtual GuiNavigatorMenu* clone() = 0;
  virtual void create( GuiNavigator *parent, XferDataItem *xfer ) = 0;
  virtual XferDataItem *getXfer();
  virtual XferDataItem *getXferSource();
  virtual const std::vector<XferDataItem*>& getXferSourceList(){ return m_xferSrcList; }
  virtual void popup(int x, int y, XferDataItem *xferSrc=0) = 0;
  virtual void popup(int x, int y, const std::vector<XferDataItem*> xferSrcList) = 0;
  virtual int getXPosition(){ return m_xpos; }
  virtual int getYPosition(){ return m_ypos; }

protected:
  void setXfer( XferDataItem *xfer );
  void setXferSource( XferDataItem *xfer );
  void setXYPosition( int x, int y );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  XferDataItem    *m_xfer;
  XferDataItem    *m_xferSrc;
  int              m_xpos;
  int              m_ypos;
protected:
  std::vector<XferDataItem*> m_xferSrcList;
};

#endif
