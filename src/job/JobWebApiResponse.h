
#if !defined(JOB_WEBAPIRESPONSE_INCLUDED_H)
#define JOB_WEBAPIRESPONSE_INCLUDED_H

#include "gui/GuiElement.h"
#include "job/JobAction.h"

class JobWebApiResponse
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobWebApiResponse();
  virtual ~JobWebApiResponse();

/*=============================================================================*/
/* public Function                                                             */
/*=============================================================================*/
public:
  void serializeForms(Json::Value& jsonElem);
  void serializeCycles(Json::Value& jsonElem);
  void serializeElements(Json::Value& jsonElem, bool updateAlways);
  void serializeFolderTab(Json::Value& jsonElem);
  void serializeFunctionStatus(Json::Value& jsonElem, JobAction::JobResult& rslt, JobAction& func);
  void serializeClipboard(Json::Value& jsonElem);
#if HAVE_PROTOBUF
  void serializeForms(in_proto::WebAPIResponse *reply);
  void serializeCycles(in_proto::WebAPIResponse *reply);
  void serializeElements(in_proto::WebAPIResponse *reply, bool updateAlways);
  void serializeFolderTab(in_proto::WebAPIResponse *reply);
  void serializeFunctionStatus(in_proto::WebAPIResponse *reply, JobAction::JobResult& rslt, JobAction& func);
  void serializeClipboard(in_proto::WebAPIResponse *reply);
#endif
  static void setNextOpenURL(const std::string& url) { s_openURL = url; }

  /***/
  static void addGuiElementReplaceData(GuiForm*, GuiElement *oldElement, GuiElement *newElement);
/*=============================================================================*/
/* private Class                                                               */
/*=============================================================================*/
private:
  struct ReplaceData {
    ReplaceData(GuiElement *oldElement, GuiElement *newElement) {
      add(oldElement, newElement);
    }
    void add(GuiElement *oldElement, GuiElement *newElement) {
      old_element.push_back(oldElement);
      new_element.push_back(newElement);
    }

    GuiElementList old_element;
    GuiElementList new_element;
  };
  static std::map<std::string, ReplaceData *> s_guiElementReplaceData;
/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  void serializeForm(std::ostream &os, GuiElement* guiForm, bool& bComma, bool proto);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiElement*    m_mainForm;
  GuiElementList m_formList;
  static std::string s_openURL;
};



#endif
