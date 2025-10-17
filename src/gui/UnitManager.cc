#if HAVE_QT
#include <QSettings>
#include "gui/qt/GuiQtManager.h"
#endif

#include "app/App.h"
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "gui/UnitManager.h"
#include "xfer/XferDataItem.h"

#include "utils/Debugger.h"
#include "utils/JsonUtils.h"
#include "utils/StringUtils.h"
#include "utils/FileUtilities.h"

INIT_LOGGER();

UnitManager::Unit defaults[] ={
  // name  factor  sh   base
  {"m",  1.0,   0.0, ""},
    {"cm", 1e2,  0.0, "m"},
    {"mm", 1e3,  0.0, "m"},
  {u8"m²",  1.0,  0.0, u8""},
  {"kg",  1.0,   0.0, ""},
  {"s",   1.0,   0.0, ""},
    {"ms", 1e3,  0.0, "s"},
  {"H", 1.0,   0.0, ""},
    {"mH", 1e3,   0.0, "H"},
    {u8"μH", 1e6,   0.0, u8"H"},
  {"T",  1.0,   0.0, ""},
  {"F",  1.0,   0.0, ""},
    {"mF",  1.0,   0.0, "F"},
    {u8"μF", 1.0,   0.0, u8"F"},
  {u8"Ω",  1.0,   0.0, u8""},
    {u8"mΩ", 1e3,   0.0, u8"Ω"},
  {"V",  1.0,   0.0, ""},
  {"A",  1.0,   0.0, ""},
  {"%",  100.0,   0.0, ""},
  {"Vs", 1.0,   0.0, ""},
  {"1/s", 1.0,  0.0, ""},
    {"1/min", 60,  0.0, "1/s"},
  {"Hz", 1.0,   0.0, ""},
  {u8"°C", 1.0,   0.0, u8""},
  {"K",  1.0,   0.0, ""},
  {"deg",  1.0,   0.0, ""},
  {u8"°",  1.0,   0.0, u8""},
  {"rad", 1.0,   0.0, ""},
  {"A/m", 1.0,   0.0, ""},
    {"kA/m", 1e-3,   0.0, "A/m"},
  {u8"A/m²", 1.0,   0.0, u8""},
  {"N",  1.0,   0.0, ""},
    {"kN", 1e-3,   0.0, "N"},
  {u8"N/m²", 1.0,  0.0, u8""},
  {u8"kN/m²", 1e-3,  0.0, u8"N/m²"},
  {"W",  1.0,   0.0, ""},
    {"kW",  1e-3,   0.0, "W"}
};


UnitManager *UnitManager::s_instance = 0;

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor                                                    */
/* --------------------------------------------------------------------------- */
UnitManager::UnitManager(){
  readUnitData();
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

UnitManager &UnitManager::Instance(){
  if( s_instance == 0 ){
    s_instance = new UnitManager;
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* update_unit --                                                              */
/* --------------------------------------------------------------------------- */

void UnitManager::update_unit(UserAttr* userattr, const std::string& new_unit,
                              const std::string& fullname){
#if HAVE_QT
  if (!fullname.empty()){
    QSettings *settings = GuiQtManager::Settings();
    settings->beginGroup(QString::fromStdString("UnitManager"));
    settings->setValue(QString::fromStdString(fullname),
                       QString::fromStdString(new_unit));
    settings->endGroup();
  }
#endif
  userattr->SetUnit(new_unit);
  GuiManager::Instance().update(GuiElement::reason_Unit);
}

/* --------------------------------------------------------------------------- */
/* parseIncludeFile --                                                         */
/* --------------------------------------------------------------------------- */

void UnitManager::parseIncludeFile(){
  if (!AppData::Instance().UnitManagerFeature())
    return;
  std::string tokenIntensNS(App::TOKEN_INTENS_NAMESPACE);
  std::string intensNS(App::INTENS_NAMESPACE);

  // load and parse UnitManager.inc
  std::vector<std::string> files;
  files.push_back(AppData::Instance().IntensHome() +"/etc/UnitManager.inc");
  //  files.push_back(AppData::Instance().IntensHome() +"/../etc/UnitManager.inc");
  std::string fn = FileUtilities::GetNewestFile(files);
  std::ostringstream os;
  std::ifstream file(fn.c_str());
  std::string line;
  while(std::getline(file, line)) {
    std::string::size_type n;
    while ((n = line.find(tokenIntensNS)) != std::string::npos) {
      std::ostringstream osp;
      line.replace(n, tokenIntensNS.size(), intensNS);
    }
    os << line << std::endl;
  }
  if (!App::Instance().parse(os.str())){
    BUG_ERROR("ERROR parsing mq reply include.");
  }
  else{
    BUG_INFO("MessageQueueReply successfully included");
  }
}

/* --------------------------------------------------------------------------- */
/* getComboBoxData --                                                          */
/* --------------------------------------------------------------------------- */

std::string UnitManager::getComboBoxData(const std::string &unit,
                                         UserAttr* userattr, DataReference* data_ref){
  if(unit.empty()){
    return "";
  }
  std::string _unit(unit);
#if HAVE_QT
  if (data_ref && AppData::Instance().PersistItemsFilename().empty()) {
    QSettings *settings = GuiQtManager::Settings();
    settings->beginGroup(QString::fromStdString("UnitManager"));
    std::string s;
    if (data_ref)
      s = settings->value(QString::fromStdString(data_ref->fullName()),
                          QString::fromStdString(unit)).toString().toStdString();
    settings->endGroup();
    if (s != unit) {
      _unit = s;
      userattr->SetUnit(s);
    }
  }
#endif
  UnitNameMap::const_iterator it = m_unit.find(_unit);
  if (it == m_unit.end()) {
    trim(_unit, "[");
    trim(_unit, "]");
    it = m_unit.find(_unit);
  }

  if (it != m_unit.end()) {
    BUG_DEBUG("found " << _unit << "(" << unit << ")");
    return comboBoxJsonData(_unit, userattr, data_ref);
  }
  BUG_DEBUG("not found " << unit);
  return "";
}

/* --------------------------------------------------------------------------- */
/* Unit::write_json --                                                         */
/* --------------------------------------------------------------------------- */

void UnitManager::Unit::write_json(std::ostream& os) const{
  if (!base.empty()){
    return;
  }
  os << " {";
  os << " \"name\" : \"" << name << "\",";
  os << " \"ui_name\" : \"" << ui_name << "\",";
  os << " \"factor\" : \"" << factor << "\",";
  os << " \"use_set\" : \"" << use_set << "\",";
  os << " \"base\" : \"" << base << "\"";
  if (base.empty() && derived.size()){
    os << ", \"derived\" : [\n";
    for(const auto &d: derived){
      Unit* u = UnitManager::Instance().getUnitData(d);
      if (u){
        os << "    {";
        os << " \"name\" : \"" << u->name << "\",";
        os << " \"ui_name\" : \"" << u->ui_name << "\",";
        os << " \"factor\" : \"" << u->factor << "\",";
        os << " \"use_set\" : \"" << u->use_set << "\",";
        os << " \"base\" : \"" << u->base << "\"";
        os << "}" << (d == derived.back() ? "" : ",") << std::endl;
      }
    }
    os << "    ]" << std::endl;
  }
  os << " }";
}

void UnitManager::writeUnitData(std::ostream& os){
  os << "[" << std::endl;
  bool first(true);
  for(const auto &u: m_unit){
    os << (first  || !(u.second->base.empty()) ? "" : ",\n");
    u.second->write_json(os);
    first = false;
  }
  ///  os << (m_unit.size() ? "," : "");
  os << std::endl << "]" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* readUnitData --                                                             */
/* --------------------------------------------------------------------------- */

void UnitManager::readUnitData(){
  std::string parent = "";
  for(auto &u: defaults){
    m_unit[u.name] = &u;
    if(!u.base.empty()){
      m_unit[parent]->add_derived(u.name);
    }
    else{
      parent = u.name;
    }
  }
  if (AppData::Instance().AppHome().empty()) return;
  try {
    m_filename = AppData::Instance().AppHome() + "/etc/application_unit.json";
    Json::Value content = ch_semafor_intens::JsonUtils::parseJsonFile(m_filename);
    if (content.isObject()) {
      addUnitData(content);
      return;
    }
    if (!content.isArray())
      return;
    for (Json::Value::iterator it = content.begin();
         it != content.end(); ++it) {
      addUnitData((*it));
    }
  } catch ( const ch_semafor_intens::JsonException& e ) {
    // ignored
  }
  //for(const auto &u: m_unit){
  //  u.second->print();
  //}
}

/* --------------------------------------------------------------------------- */
/* addUnitData --                                                              */
/* --------------------------------------------------------------------------- */

void UnitManager::addUnitData(Json::Value& data, std::string base){
  if (!data.isMember("name") ||
      !data.isMember("factor")) {
    return;
  }

  std::string name(data["name"].asString());
  std::vector<std::string> derived;
  if (data.isMember("derived")){
    Json::Value& derivedData = data["derived"];
    if (derivedData.isObject()) {
      derived.push_back(data["derived"]["name"].asString());
      addUnitData(data["derived"], name);
    }
    if (data["derived"].isArray()) {
      for (Json::Value::iterator it = derivedData.begin();
           it != derivedData.end(); ++it) {
        derived.push_back((*it)["name"].asString());
        addUnitData(*it, name);
      }
    }
  }
  std::string ui_name = data["name"].asString();
  if(data.isMember("ui_name")){
    ui_name = data["ui_name"].asString();
  }
  std::string db_name;
  if (data.isMember("db_name")){
    db_name = data["db_name"].asString();
  }
  bool use_set(true);
  if (data.isMember("use_set")){
    use_set = data["use_set"].asBool();
  }
  bool use_divide(false);
  if (data.isMember("use_divide")){
    use_divide = data["use_divide"].asBool();
  }
  double shift(0);
  if (data.isMember("shift")){
    shift = data["shift"].asDouble();
  }
  m_unit[name] = new Unit(name, ui_name, db_name,
                          data["factor"].asDouble(), use_divide, shift,
                          derived, base, use_set);
  //m_unit[name]->print();
}

/* --------------------------------------------------------------------------- */
/* getUnitData --                                                              */
/* --------------------------------------------------------------------------- */

UnitManager::Unit* UnitManager::getUnitData(const std::string& unit) const{
  std::string _unit(unit);
  UnitNameMap::const_iterator it = m_unit.find(_unit);
  if (it == m_unit.end()) {
    trim(_unit, "[");
    trim(_unit, "]");
    it = m_unit.find(_unit);
  }

  if (it != m_unit.end()) {
    return (*it).second;
  }

  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDbUnit -                                                                 */
/* --------------------------------------------------------------------------- */
const std::string UnitManager::getDbUnit(const std::string& unit) const{
  UnitManager::Unit* u = getUnitData(unit);
  if(AppData::Instance().UnitManagerFeature() &&
     u != 0 && !u->base.empty()){
      u = getUnitData(u->base);
    return u->db_unit();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* getBaseUnit --                                                              */
/* --------------------------------------------------------------------------- */
const std::string UnitManager::getBaseUnit(const std::string& unit) const{
  UnitManager::Unit* u = getUnitData(unit);
  if(u != 0){
    if(!u->base.empty()){
      u = getUnitData(u->base);
    }
    return u->name;
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* comboBoxJsonData --                                                         */
/* --------------------------------------------------------------------------- */

std::string UnitManager::comboBoxJsonData(const std::string &unit,
                                          const UserAttr* userattr,
                                          DataReference* data_ref){
  Unit *unitData = getUnitData(unit);
  Json::Value jsonObj = Json::Value(Json::objectValue);
  jsonObj["input"] = Json::Value(Json::arrayValue);
  jsonObj["output"] = Json::Value(Json::arrayValue);

  // derived unit
  if (unitData && !unitData->base.empty()) {  // derived list is filled
    Unit *unitDataBase = getUnitData(unitData->base);
    if (unitDataBase && unitDataBase->use_set) {
      jsonObj["input"].append(unitDataBase->ui_name);
      jsonObj["output"].append(unitDataBase->name);
    }
    //unitDataBase->print();
    for (std::vector<std::string>::iterator it=unitDataBase->derived.begin();
         it !=unitDataBase->derived.end(); ++it) {
      Unit *unitDataDerived = getUnitData(*it);
      if (unitDataDerived && unitDataDerived->use_set) {
        jsonObj["input"].append(unitDataDerived->ui_name);
        jsonObj["output"].append(unitDataDerived->name);
      }
    }
  } else {
    // base unit
    if (unitData && unitData->use_set) {
      jsonObj["input"].append(unitData->ui_name);
      jsonObj["output"].append(unitData->name);
    }

    // base unit derived list
    if (unitData && unitData->derived.size() > 0) {
      for (std::vector<std::string>::iterator it=unitData->derived.begin();
           it !=unitData->derived.end(); ++it) {
        Unit *unitDataDerived = getUnitData(*it);
        if (unitDataDerived && unitDataDerived->use_set) {
          jsonObj["input"].append(unitDataDerived->ui_name);
          jsonObj["output"].append(unitDataDerived->name);
        }
      }
    }
  }
  if(!AppData::Instance().UnitManagerAlwaysComboBox() &&
     jsonObj["input"].size() < 2){
    return "";
  }
  jsonObj["value"] = unitData->ui_name;
  std::ostringstream os;
  if (userattr){
    os << userattr;
    jsonObj["ptr_userattr"] = os.str();
  }
  if (data_ref){
    jsonObj["userattr_fullName"] = data_ref->fullName();
  }
  std::string s(ch_semafor_intens::JsonUtils::value2string(jsonObj));
  trim(s);
  return s;
}

/* --------------------------------------------------------------------------- */
/* extractValue --                                                             */
/* --------------------------------------------------------------------------- */
std::string UnitManager::extractValue(const std::string& json_string_or_text) {
  std::string value;
  extractValue(json_string_or_text, value);
  return value;
}

bool UnitManager::extractValue(const std::string& json_string_or_text, std::string& value) {
  value = json_string_or_text;
  if (!AppData::Instance().UnitManagerFeature())
    return false;
  std::string text;
  std::string bStr("{\"input\":");
  std::string eStr("\"}");
  std::size_t posB  = json_string_or_text.find(bStr);
  std::size_t posE  = json_string_or_text.find_last_of(eStr);
  if (posB == std::string::npos || posE == std::string::npos)
    return false;
  // parse json
  std::string s(json_string_or_text.substr(posB, posE+1));
  Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(s);
  if (!valueObject.isNull()) {
    if (valueObject["value"].isString())
      text = valueObject["value"].asString();
    else {
      std::ostringstream os;
      if (valueObject["value"].isInt() )
        os << valueObject["value"].asInt();
      else if (valueObject["value"].isDouble() )
        os << valueObject["value"].asDouble();
      else if (valueObject["value"].isNull())
        text = "";
      else {
        std::cerr <<"' ERROR: could not handle this jsonObject '" << json_string_or_text << "'\n";
        return false;
      }
      text = os.str();
    }
    value = json_string_or_text.substr(0, posB) + text + json_string_or_text.substr(posE+1);
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* extractJsonObject --                                                        */
/* --------------------------------------------------------------------------- */
Json::Value UnitManager::extractJsonObject(const std::string& json_string_or_text,
                                           std::string& extractPreString,
                                           std::string& extractPostString) {
  if (!AppData::Instance().UnitManagerFeature())
    return json_string_or_text;
  std::string text;
  std::string bStr("{\"input\":");
  std::string eStr("\"}");
  std::size_t posB  = json_string_or_text.find(bStr);
  std::size_t posE  = json_string_or_text.find_last_of(eStr);
  if (posB == std::string::npos || posE == std::string::npos)
    return Json::Value();
  // parse json
  extractPreString = json_string_or_text.substr(0, posB);
  extractPostString = json_string_or_text.substr(posE+1);
  trim(extractPreString);
  trim(extractPostString);
  std::string s(json_string_or_text.substr(posB, posE+1));
  return ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(s);
}

/* --------------------------------------------------------------------------- */
/* reset --                                                                    */
/* --------------------------------------------------------------------------- */
void UnitManager::reset() {
  m_unit.clear();
  readUnitData();
}
