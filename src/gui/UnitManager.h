#ifndef UNIT_MANAGER_INCLUDED_H
#define UNIT_MANAGER_INCLUDED_H

#include <map>
#include <vector>

class UserAttr;

class UnitManager{
 public:
  struct Unit {
    Unit(std::string name, std::string ui_name, std::string db_name,
         double factor, bool use_divide, double shift,
         std::vector<std::string> derived, std::string base, bool use_set)
      :name(name),
       ui_name(ui_name),
       db_name(db_name),
       factor(factor),
       use_divide(use_divide),
       shift(shift),
       derived(derived),
       base(base),
       use_set(use_set) {}

    Unit(std::string name, double factor, double shift, std::string base)
      :name(name),
       ui_name(name),
       db_name(name),
       factor(factor),
       use_divide(false),
       shift(shift),
       base(base),
       use_set(true) {}
    void add_derived(const std::string &d){
      derived.push_back(d);
    }
    Unit(std::u8string uname, double factor, double shift, std::u8string base)
      :name(uname.begin(), uname.end()),
       ui_name(this->name),
       db_name(this->name),
       factor(factor),
       use_divide(false),
       shift(shift),
       base(base.begin(), base.end()),
       use_set(true) {}
    void print() const {
      std::cout << "Unit name(" << name <<") ui_name(" << ui_name
                << ") db_name(" << db_name << ") factor(" << factor
                << ") use_divide(" << use_divide << ") shift(" << shift
                << ") Derived(";
      for(const auto &d: derived){
        std::cout << d << " ";
      }
      std::cout <<") base(" << base << ") use_set(" << use_set << ")\n";
    }
    const std::string db_unit() const{
      return db_name.empty() ? name : db_name;
    }
    void write_json(std::ostream& os) const;
    std::string name;
    std::string ui_name;
    std::string db_name;
    double factor;
    bool use_divide;
    double shift;
    std::vector<std::string> derived;
    std::string base;
    bool use_set;
  };
  typedef std::map<std::string, UnitManager::Unit*> UnitNameMap;

  /*******************************************************************************/
  /* Constructor / Destructor                                                    */
  /*******************************************************************************/
  UnitManager();

  static UnitManager &Instance();

  /*******************************************************************************/
  /* public member functions                                                     */
  /*******************************************************************************/
  /** updates unit in userattr */
  void update_unit(UserAttr* userattr, const std::string& new_unit, const std::string& fullname);
  /** returns db unit */
  const std::string getDbUnit(const std::string& unit) const;
  /** returns base unit */
  const std::string getBaseUnit(const std::string& unit) const;

  /** parse include file */
  void parseIncludeFile();

  /** initialise singleton instance */
  std::string getComboBoxData(const std::string& unit, UserAttr* userattr, DataReference* data_ref=0);

  /** get unit data */
  Unit* getUnitData(const std::string& unit) const;

  /** extract value */
  static UserAttr* extractValue(const std::string& json_string, std::string& value);

  /** extract value */
  static std::string extractValue(const std::string& json_string);

  /** extract json object */
  static Json::Value extractJsonObject(const std::string& json_string_or_text,
                                       std::string&  extractPreString, std::string& extractPos);
  /** reset, reread unit data */
  void reset();

  /** get json file name */
  const std::string& getFilename() { return m_filename; }

  /** write default unit data in json format */
  void writeUnitData(std::ostream& os);

  /*******************************************************************************/
  /* private Data                                                                */
  /*******************************************************************************/
  void readUnitData();
  void addUnitData(Json::Value& data, std::string base=std::string());
  std::string comboBoxJsonData(const std::string& unit, const UserAttr* userattr, DataReference* data_ref);
private:
  static UnitManager*     s_instance;
  UnitNameMap m_unit;
  std::string m_filename;
};

#endif
