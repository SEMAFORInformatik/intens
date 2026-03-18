#if !defined(GUI_VOID_INCLUDED_H)
#define GUI_VOID_INCLUDED_H

#include <jsoncpp/json/value.h>

#if HAVE_PROTOBUF
#include "protobuf/Message.pb.h"
#endif

class GuiElement;

class GuiVoid
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiVoid()
    : m_width(10)
    , m_height(10)
    , m_displayPercentWidth(0)
    , m_displayPercentHeight(0)
  {}
  virtual ~GuiVoid(){}

protected:
  GuiVoid(const GuiVoid &v)
    : m_width(v.m_width)
    , m_height(v.m_height)
    , m_displayPercentWidth(v.m_displayPercentWidth)
    , m_displayPercentHeight(v.m_displayPercentHeight)
  {}

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  void serializeXML(std::ostream &os, bool recursive = false);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual GuiElement *getElement() = 0;

/*=============================================================================*/
/* public member functions of GuiVoid                                          */
/*=============================================================================*/
public:
  void setWidth( int w ) { m_width = w; }
  void setHeight( int h ) { m_height = h; }
  void setDisplayPercentWidth( int w );
  void setDisplayPercentHeight( int h );
  void getSize(int &x, int &y, bool hint=true) {x=m_width;y=m_height;}

/*=============================================================================*/
/* protected member functions of GuiVoid                                       */
/*=============================================================================*/
protected:
  int getWidth() { return m_width; }
  int getHeight() { return m_height; }
  int getDisplayPercentWidth() { return m_displayPercentWidth; }
  int getDisplayPercentHeight() { return m_displayPercentHeight; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int   m_width;
  int   m_height;
  int   m_displayPercentWidth;
  int   m_displayPercentHeight;
};

#endif
