
#if !defined(GUI_BUTTON_INCLUDED_H)
#define GUI_BUTTON_INCLUDED_H

#if HAVE_PROTOBUF
#include "protobuf/Message.pb.h"
#endif

/** ActionButton object used in GuiButtonbar
 */
class GuiButtonListener;
class GuiEventData;

class GuiButton
{
public:
  GuiButton( GuiButtonListener *l, GuiEventData *e );
  virtual ~GuiButton();
  virtual GuiElement * getElement() = 0;
  virtual void setLabel( const std::string &label ) { m_label = label; }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

 protected:
  void Activate();

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  std::string        m_label;
  bool               m_label_pixmap;

  GuiButtonListener *m_listener;
  GuiEventData      *m_event;
};

#endif
