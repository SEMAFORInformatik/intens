
#include "gui/GuiFactory.h"
#include "gui/GuiPopupMenu.h"
#include "gui/GuiMenuToggle.h"
#include "gui/GuiToggleListener.h"
#include "plot/ListPlot.h"
#include "plot/GdevSVG.h"
#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "utils/base64.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* saveFile --                                                                 */
/* --------------------------------------------------------------------------- */

bool ListPlot::saveFile( GuiElement *e ){
  BUG_DEBUG("ListPlot::saveFile" );
  /**/
  ///  m_saveButton = e;
  std::vector<HardCopyListener::FileFormat> formats;
  formats.push_back(HardCopyListener::PDF);
  formats.push_back(HardCopyListener::PNG);  // vielleicht?
  ///  ReportGen::getFormats( this, formats );
  GuiFactory::Instance()->showDialogFileSelection
    ( 0
      , compose(_("Save %1"), getTitle())
      , "PDF (*.pdf)", ""
      , &m_saveListener
      , DialogFileSelection::Save
      , &formats
      , DialogFileSelection::AnyFile
      , getElement()->getName());
  return true;
  /**/
  return false;
}

/* --------------------------------------------------------------------------- */
/* FileSelected --                                                             */
/*   called by DialogFileSelection                                             */
/* --------------------------------------------------------------------------- */

void ListPlot::SaveListener::FileSelected( const std::string &fn
					       , const HardCopyListener::FileFormat format
					       , const std::string dir ){
  BUG_DEBUG("ListPlot::SaveListener::FileSelected" );
  std::string filename = fn;
  m_listplot->write(fn);
}

/* --------------------------------------------------------------------------- */
/* sserializeJson --                                                           */
/* --------------------------------------------------------------------------- */

bool ListPlot::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  int w, h;
  //  if(onlyUpdated){
  //  return false;
  //}
  //
  getElement()->writeJsonProperties(jsonObj);
  getSize( w, h );
  jsonObj["type"] = "Text"; // "Pixmap";
  jsonObj["width"] = 2*w;
  jsonObj["height"] = 2*h;
  jsonObj["datatype"] = "cdata";

  std::ostringstream oss;
  Gdev *gdev = new GdevSVG(oss, getName(), 0, 0, 2*w, -2*h);
  repaint(*gdev);
  delete gdev;
  std::string sBase64;
  std::string s = oss.str();
  base64encode(reinterpret_cast<const unsigned char*>(s.c_str()),
               s.size(), sBase64, false);
  jsonObj["mimetype"] = "image/svg+xml";
  jsonObj["value"] = std::string("data:image/svg+xml;base64,") + sBase64;

  // popup base menu
  GuiPopupMenu* pm = getElement()->getPopupMenu();
  if (pm) {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    pm->serializeJson(jsonElem, onlyUpdated);
    jsonObj["popup_base_menu"] = jsonElem;
  }
  return true;
}


/* --------------------------------------------------------------------------- */
/* sserializeProtobuf --                                                       */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool ListPlot::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  int w, h;

  auto element = eles->add_texts();
  element->mutable_base()->set_allocated_base(getElement()->writeProtobufProperties());
  getSize( w, h );
  element->mutable_base()->mutable_base()->set_width(2*w);
  element->mutable_base()->mutable_base()->set_height(2*h);
  element->mutable_base()->mutable_value()->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_CDATA);

  std::ostringstream oss;
  Gdev *gdev = new GdevSVG(oss, getName(), 0, 0, 2*w, -2*h);
  repaint(*gdev);
  delete gdev;
  std::string sBase64;
  std::string s = oss.str();
  base64encode(reinterpret_cast<const unsigned char*>(s.c_str()),
               s.size(), sBase64, false);
  element->mutable_base()->mutable_value()->set_mimetype("image/svg+xml");
  element->mutable_base()->mutable_value()->set_string_value(std::string("data:image/svg+xml;base64,") + sBase64);

  // popup base menu
  GuiPopupMenu* pm = getElement()->getPopupMenu();
  if (pm) {
    pm->serializeProtobuf(eles, element->mutable_popup_base_menu(), onlyUpdated);
  }
  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */
std::string ListPlot::variantMethod(const std::string& method,
				    const Json::Value& jsonArgs,
				    JobEngine *eng) {
  Json::Value jsonElem = Json::Value(Json::objectValue);

  // MenuButtonAction
  if (lower(method) == "menubuttonaction") {
    if (jsonArgs.isMember("id") && jsonArgs["id"].isInt()) {
      GuiButtonListener* btnLsnr = GuiButtonListener::getButtonListenerByMenuButtonId(jsonArgs["id"].asInt());
      if (btnLsnr) {
        btnLsnr->ButtonPressed();
        jsonElem["status"] = "OK";
      } else {
	GuiElement *elem = GuiElement::findElementId(jsonArgs["id"].asString());
	GuiMenuToggle *menu = 0;
	if (elem)
	  menu = dynamic_cast<GuiMenuToggle*>(elem);
	if (menu && menu->getToggleListener()) {
	  menu->setToggleStatus(!menu->getToggleStatus());
	  menu->getToggleListener()->ToggleStatusChanged(menu->getToggleStatus());
	} else {
	  jsonElem["message"] = "Wrong Parameter, id=" + jsonArgs["id"].asString();
	  jsonElem["status"] = "Error";
	}
      }
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
