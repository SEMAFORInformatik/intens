
#if !defined(GUI_INDEX_LISTENER_INCLUDED_H)
#define GUI_INDEX_LISTENER_INCLUDED_H

#include <string>

class GuiIndexListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiIndexListener() {}
  virtual ~GuiIndexListener() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool acceptIndex( const std::string &, int ){ return true; }
  virtual void setIndex( const std::string &, int ){}
  virtual bool isIndexActivated() { return true; }

};

#endif
