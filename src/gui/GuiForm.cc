
#include "utils/HTMLConverter.h"
#include "utils/StringUtils.h"
#include "gui/GuiForm.h"
#include "gui/GuiDialog.h"
#include "gui/GuiIndex.h"

std::vector<GuiForm*> GuiForm::s_managedForms;

/* --------------------------------------------------------------------------- */
/* Type --                                                                     */
/* --------------------------------------------------------------------------- */

GuiElement::ElementType GuiForm::Type(){
  return m_main ? GuiElement::type_Main : GuiElement::type_Form;
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiForm::setTitle(const std::string &title){
  if (getDialog())
    getDialog()->setDialogTitle(title);
}

/* --------------------------------------------------------------------------- */
/* getTitle --                                                                 */
/* --------------------------------------------------------------------------- */

std::string GuiForm::getTitle(){
  if (isMain()) {
    std::ostringstream title;
    AppData &appdata = AppData::Instance();
    title << appdata.Title();
    if ( ! appdata.AppShortMainTitle() ) {
      title << " - " << appdata.ProgName() << "@" << appdata.HostName()
            << "(" << appdata.Version() << ") ";
    }
    return title.str();
  }
  return getDialog() ? getDialog()->getDialogTitle() : "";
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiForm::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:Form";
  std::string name( getElement()->getName() );
  HTMLConverter::convert2HTML( name );
  os << " name=\""<< name << "\"";
  std::string title( getTitle() );
  HTMLConverter::convert2HTML( title );
  os << " title=\""<< title << "\"";
  os << ">" << std::endl;
  serializeAttrs( os );
  os << "</intens:Form>"<<std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

void GuiForm::serializeJson(Json::Value& jsonObj, bool recursive) {
   Json::Value jsonValue = Json::Value(Json::objectValue);
   getElement()->writeJsonProperties(jsonObj);
   jsonValue["title"] = getTitle();
   jsonValue["main"] = getElement()->Type() == GuiElement::type_Main;
   jsonValue["modal"] = getDialog()->isApplicationModal();

   Json::Value jsonElem = Json::Value(Json::arrayValue);
   serializeAttrs(jsonElem);
   jsonValue["elements"] = jsonElem;

   jsonObj[getElement()->getName()] = jsonValue;
}

#if HAVE_PROTOBUF
void GuiForm::serializeProtobuf(in_proto::ElementList *eles, bool onlyUpdate) {
   auto element = eles->add_forms();
   element->set_allocated_base(getElement()->writeProtobufProperties());
   element->set_main(getElement()->Type() == GuiElement::type_Main);
   element->set_title(getTitle());
   element->set_modal(getDialog()->isApplicationModal());
   serializeAttrs(eles, element->mutable_elements(), onlyUpdate);
}
#endif

/* --------------------------------------------------------------------------- */
/* waitCursor --                                                               */
/* --------------------------------------------------------------------------- */

void GuiForm::waitCursor( bool wait, GuiDialog *installer ){
  if( wait ){
    getDialog()->installWaitCursor( installer );
  }
  else{
    getDialog()->removeWaitCursor( installer );
  }
}

/* --------------------------------------------------------------------------- */
/* setShown --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiForm::setShown(bool value){
  m_is_popped_up = value;
}

/* --------------------------------------------------------------------------- */
/* setMain --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiForm::setMain(){
  m_main = true;
  return true;
}

void GuiForm::registerIndex() {
  GuiElementList elist;
  GuiElementList::iterator it;
  GuiElement::findElementType(elist, GuiElement::type_Index);
  for( it = elist.begin(); it != elist.end(); ++it )
    (*it)->getGuiIndex()->registerIndexedElement( getElement() );
}

void GuiForm::unregisterIndex() {
  GuiElementList elist;
  GuiElementList::iterator it;
  GuiElement::findElementType(elist, GuiElement::type_Index);
  for( it = elist.begin(); it != elist.end(); ++it )
    (*it)->getGuiIndex()->unregisterIndexedElement( getElement() );
}

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */
std::string GuiForm::variantMethod(const std::string& method,
                                     const Json::Value& jsonArgs,JobEngine *eng) {
  Json::Value jsonElem = Json::Value(Json::objectValue);

  // MenuButtonAction
  if (lower(method) == "clicked") {
    if (jsonArgs.isMember("id") && jsonArgs["id"].isInt()) {
      GuiButtonListener* btnLsnr = GuiButtonListener::getButtonListenerByMenuButtonId(jsonArgs["id"].asInt());
      if (btnLsnr) {
        btnLsnr->ButtonPressed();
        jsonElem["status"] = "OK";
      } else {
        jsonElem["message"] = "Wrong Parameter, id=" + jsonArgs["id"].asString();
        jsonElem["status"] = "Error";
      }
    }
  }
  return getElement()->defaultVariantMethod(method, jsonArgs, eng);
}
