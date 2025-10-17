#ifndef JSONUTILS_H_
#define JSONUTILS_H_

#include <jsoncpp/json/value.h>
#include <string>

#include "JsonException.h"

namespace ch_semafor_intens {

  class JsonUtils
  {
  public:
    static Json::Value parseJson(const std::string& jsonString);
    static Json::Value parseJsonFile(const std::string& filename);
    static void writeJsonFile(const std::string& filename,
                              const Json::Value& value);

    static void modifyJsonFile(const std::string& filename,
                               const std::string& path,
                               const Json::Value& value);

    /** @return parse a combobox json object */
    static Json::Value parseJsonObjectComboBox(const std::string& text);
    /** @return left value modified (use it) */
    static bool assignJsonObjectComboBox(std::string &leftValue, const std::string &rightValue, const bool rightValid=true);

    /** @return creates a drag json object */
    static Json::Value createDragJsonObject(const std::string& source,
					    const std::vector<std::string>& varnameList,
					    const std::vector<bool>& isFolderList);
    /** @return creates a drag json object */
    static Json::Value createDragJsonObject(const std::string& source,
					    const std::string& varname,
					    bool isFolder);
    static std::string value2string(const Json::Value& value, bool styled=false);
    static bool value2vectorInt(const Json::Value& value, std::vector<int>& res);

    static std::string vector2string(const std::vector<double>& vector, bool styled=false);
    static std::string response(const std::string& message = "");
    static Json::Value getRealValueNotJson5(double rVal);

    static bool resolve(const Json::Value& root,
                        const Json::Path& path,
                        std::string& value,
                        const bool useDefault=false,
                        const std::string defaultValue="");
    static bool resolve(const Json::Value& root,
                        const Json::Path& path,
                        double& value,
                        const bool useDefault=false,
                        const double defaultValue=0.0);
    static bool resolve(const Json::Value& root,
                        const Json::Path& path,
                        int& value,
                        const bool useDefault=false,
                        const int defaultValue=0);
    static bool resolve(const Json::Value& root,
                        const Json::Path& path,
                        unsigned int& value,
                        const bool useDefault=false,
                        const unsigned int defaultValue=0);

    static std::string DRAG_OBJECT_VARNAME;
    static std::string DRAG_OBJECT_SOURCE;
    static std::string DRAG_OBJECT_ISFOLDER;
  };

} // end namespace ch_semafor_intens
#endif /* JSONUTILS_H_ */
