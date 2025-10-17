
#if !defined(GUI_LABEL_INCLUDED_H)
#define GUI_LABEL_INCLUDED_H
#include "GuiElement.h"

class GuiLabel {
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void setTitle( const std::string & )=0;
  virtual void setIndexed()=0;
  virtual const std::string getLabel()=0;
  virtual std::string getSimpleLabel() = 0;
  virtual bool isLabelPixmap() = 0;
  virtual bool setLabel( const std::string & )=0;
  virtual bool setPixmap( const std::string &, bool withLabel=false )=0;

  virtual GuiElement *getElement() = 0;
  void serializeXML( std::ostream &os, bool recursive );
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated);
#endif
};

#endif
