
#if !defined(GUI_APPLHEADER_INCLUDED_H)
#define GUI_APPLHEADER_INCLUDED_H

#include "gui/GuiToggleListener.h"

class GuiApplHeader : public GuiToggleListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiApplHeader(){
  }
  virtual ~GuiApplHeader(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual GuiElement * getElement() = 0;

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual const std::string getTitle() = 0;
  virtual const std::string getSubTitle() = 0;

  virtual void serializeXML( std::ostream &os, bool recursive = false );
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

};

#endif
