
#ifndef GUI_EVENT_DATA_INCLUDED_H
#define GUI_EVENT_DATA_INCLUDED_H

class GuiElement;

class GuiEventData{
 public:
  GuiElement *m_element;
  bool        m_toggleStatus;
};

class GuiEventLoopListener
{
public:
  GuiEventLoopListener( bool grab = false )
    : m_go_away( false )
    , m_grab( grab ) {}
  virtual ~GuiEventLoopListener() {}
public:
  virtual bool stayHere(){
    if( m_go_away ){
      delete this;
      return false;
    }
    return true;
  }
  virtual void goAway() { m_go_away = true; }
  virtual bool grabButton() { return m_grab; }
private:
  bool m_go_away;
  bool m_grab;
};

#endif
