
#if !defined(GUI_SCROLLEDLIST_LISTENER_INCLUDED_H)
#define GUI_SCROLLEDLIST_LISTENER_INCLUDED_H

class GuiScrolledlistListener
{
/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  virtual bool start() = 0;
  virtual bool getNextItem( std::string & ) = 0;
  virtual void activated( int ) = 0;
  virtual void selected( int, bool ) = 0;
  virtual void setValue(int, const std::string &) = 0;
};

#endif
