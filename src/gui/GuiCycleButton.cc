
#include "gui/GuiElement.h"
#include "gui/GuiCycleButton.h"
#include "app/DataPoolIntens.h"
#include "utils/gettext.h"

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiCycleButton::serializeXML(std::ostream &os, bool recursive){
   os << "<GuiCycleButton";
   os << ">" << std::endl;
   os <<"</GuiCycleButton>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiCycleButton::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  DataPoolIntens& dpi = DataPoolIntens::Instance();
  DataPool &datapool = dpi.getDataPool();
  int currentCycle = datapool.GetCurrentCycle();
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["type"] = compose("Cycle_%1",getElement()->StringType()); // override
  jsonObj["name"] = compose("cycle_%1",datapool.GetCurrentCycle()); // override

  jsonObj["label"] = getLabel(datapool.GetCurrentCycle());
  jsonObj["cycle"] = datapool.GetCurrentCycle();
  jsonObj["num_cycle"] = dpi.numCycles();
  // eigentlich immer true, Abfrage der DataReference in DataPoolIntens unnötig
  jsonObj["updated"] = true;

  // list cycle names
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  for (int c=0; c <= currentCycle; ++c) {
    dpi.goCycle( c, false ); // do not clear undo stack
    jsonAry.append(getLabel(datapool.GetCurrentCycle()));
  }
  jsonObj["list_cycle"] = jsonAry;

  dpi.goCycle( currentCycle, false ); // do not clear undo stack
  return true;
}

#if HAVE_PROTOBUF
bool GuiCycleButton::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  auto element = eles->add_cyclebuttons();
  DataPoolIntens& dpi = DataPoolIntens::Instance();
  DataPool &datapool = dpi.getDataPool();
  int currentCycle = datapool.GetCurrentCycle();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->mutable_base()->set_name(compose("cycle_%1",datapool.GetCurrentCycle())); // override
  element->set_label(getLabel(datapool.GetCurrentCycle()));

  element->set_cycle(datapool.GetCurrentCycle());
  element->set_num_cycle(dpi.numCycles());
  //
  // list cycle names
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  for (int c=0; c <= currentCycle; ++c) {
    dpi.goCycle( c, false ); // do not clear undo stack
    element->add_cycles(getLabel(datapool.GetCurrentCycle()));
  }

  dpi.goCycle( currentCycle, false ); // do not clear undo stack
  return true;
}
#endif
