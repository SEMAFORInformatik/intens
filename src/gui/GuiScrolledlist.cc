
#include "gui/GuiElement.h"
#include "gui/GuiManager.h"
#include "gui/GuiScrolledlist.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"
#include "utils/Debugger.h"

INIT_LOGGER();


/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiScrolledlist::setTitle( const std::string &title ){
  m_title = title;
}

/* --------------------------------------------------------------------------- */
/* setTableSize --                                                             */
/* --------------------------------------------------------------------------- */
void GuiScrolledlist::setTableSize( int size ) {
  m_tableSize = size;
}


/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiScrolledlist::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  bool updated = getElement() ? getElement()->isShown() : false;
  if (onlyUpdated && !updated) {
    return false;
  }
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["title"] = getTitle();
  jsonObj["expand"] = true ? getElement()->StringExpandType() : "none"; // override
  jsonObj["rowCount"] = tableSize();
  jsonObj["updated"] = updated;
  std::vector<int> selectedIdxs;
  jsonObj["action"] = "GUIELEMENT_METHOD";

  int index = 0;
  std::string listText;
  getListender()->start();
  jsonObj["element"] = Json::Value(Json::arrayValue);
  jsonObj["element"].resize(1);
  jsonObj["element"][(Json::Value::ArrayIndex)0] = Json::Value(Json::objectValue);
  jsonObj["element"][(Json::Value::ArrayIndex)0]["values"] = Json::Value(Json::arrayValue);
  while( getListender()->getNextItem( listText ) ){
    jsonObj["element"][(Json::Value::ArrayIndex)0]["values"].append(listText);
  }
  jsonObj["element"][(Json::Value::ArrayIndex)0]["label"] = getHeaderLabel().empty() ? _("Cases") : getHeaderLabel();
  jsonObj["element"][(Json::Value::ArrayIndex)0]["width"] = getColumnLength();
  jsonObj["element"][(Json::Value::ArrayIndex)0]["alignment"] = "left";

  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiScrolledlist::serializeProtobuf(in_proto::ElementList *eles, bool onlyUpdated) {
  bool updated = getElement() ? getElement()->isShown() : false;
  if (onlyUpdated && !updated) {
    return false;
  }
  auto element = eles->add_lists();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_title(getTitle());
  element->set_row_count(tableSize());
  element->set_action("GUIELEMENT_METHOD");

  int index = 0;
  std::string listText;
  getListender()->start();
  auto entry = element->add_entries();
  while( getListender()->getNextItem( listText ) ){
    entry->add_values(listText);
  }
  entry->set_label(getHeaderLabel().empty() ? _("Cases") : getHeaderLabel());
  entry->set_width(getColumnLength());
  entry->set_alignment(in_proto::GuiElement::Alignment::GuiElement_Alignment_Left);

  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */
std::string GuiScrolledlist::variantMethod(const std::string& method,
                                     const Json::Value& jsonArgs,
                                           JobEngine *eng) {
  Json::Value jsonElem = Json::Value(Json::objectValue);

  // clickedKSetSelectList
  if (lower(method) == "clicked") {
    Json::Value::Members members = jsonArgs.getMemberNames();
    BUG_DEBUG("SelectList Args: " << ch_semafor_intens::JsonUtils::value2string(jsonArgs)
              << ", isArray: " << jsonArgs["id"].isArray());
    if (jsonArgs.isMember("id") && jsonArgs["id"].isArray()) {
      std::vector<int> intVector;
      for (int i=0; i< jsonArgs["id"].size(); ++i) {
        int idx = jsonArgs["id"].get(i, 0).asInt();
        intVector.push_back(idx);
      }

      if(intVector.size()){
        getListender()->activated(intVector[0]);
      }
      jsonElem["status"] = "OK";
    } else {
      jsonElem["message"] = "Wrong Parameter";
      jsonElem["status"] = "Error";
    }
  }

  if (!jsonElem.isNull()) {
    BUG_DEBUG("variantMethod Method[" << method << "], Args["
              << ch_semafor_intens::JsonUtils::value2string(jsonArgs) << "]  Return: "
              << ch_semafor_intens::JsonUtils::value2string(jsonElem));
    return ch_semafor_intens::JsonUtils::value2string(jsonElem);
  }
  return getElement()->defaultVariantMethod(method, jsonArgs, eng);
}
