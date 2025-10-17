
#include "jsoncpp/json/writer.h"
#include "jsoncpp/json/reader.h"
#include <fstream>
#include <cmath>
#include <algorithm>

#include "JsonUtils.h"

namespace ch_semafor_intens {

  std::string JsonUtils::DRAG_OBJECT_VARNAME = "varname";
  std::string JsonUtils::DRAG_OBJECT_ISFOLDER = "isFolder";
  std::string JsonUtils::DRAG_OBJECT_SOURCE = "source";

  Json::Value JsonUtils::parseJson(const std::string& jsonString) {
    Json::Value root;   // will contain the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( jsonString, root );
    if ( !parsingSuccessful ) {
      throw JsonException( "JsonUtils::parseJson",
                           "Failed to parse jsonString\n" + reader.getFormattedErrorMessages()
                           );
    }
    return root;
  }

  Json::Value JsonUtils::parseJsonFile(const std::string& filename) {
    std::ifstream ifs ( filename.c_str() );
    if ( !ifs ) {
      throw JsonException( "JsonUtils::parseJsonFile",
                           "Failed to open file '" + filename + "'"
                           );
    }

    Json::Value root;   // will contain the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( ifs, root );
    ifs.close();
    if ( !parsingSuccessful ) {
      // report to the user the failure and their locations in the document.
      throw JsonException( "JsonUtils::parseJsonFile",
                           "Failed to parse " + filename + "\n" +
                           reader.getFormattedErrorMessages()
                           );
    }

    return root;
  }

  void JsonUtils::writeJsonFile(const std::string& filename,
                                const Json::Value& value) {

    std::ofstream ofs ( filename.c_str() );
    if ( !ofs ) {
      throw JsonException( "JsonUtils::writeJsonFile",
                           "Failed to open file " + filename
                           );
    }
    ofs << JsonUtils::value2string(value, true);
    ofs.close();
  }

  void JsonUtils::modifyJsonFile(const std::string& filename,
                                 const std::string& path,
                                 const Json::Value& value) {

    // read config file
    Json::Value content = JsonUtils::parseJsonFile(filename);

    // modify content
    Json::Path path_ ( path );
    path_.make(content) = value;

    // write config file
    JsonUtils::writeJsonFile(filename, content);
  }

  std::string JsonUtils::value2string(const Json::Value& value, bool styled){
    if ( styled ) {
      Json::StyledWriter writer = Json::StyledWriter();
      return writer.write(value);
    } else {
      Json::FastWriter writer = Json::FastWriter();
      return writer.write(value);
    }
  }

  bool JsonUtils::value2vectorInt(const Json::Value& value, std::vector<int>& res) {
    res.clear();
    if (value.isArray()) {
      for (Json::ArrayIndex i=0; i < value.size(); ++i) {
        res.push_back(value[i].asInt());
      }
      return true;
    }
    return false;
  }


  std::string JsonUtils::vector2string(const std::vector<double>& vector, bool styled){
    Json::Value array = Json::Value(Json::arrayValue);
    for(std::vector<double>::const_iterator it=vector.begin();
        it != vector.end(); ++it) {
      array.append((*it));
    }
    return value2string(array, styled);
  }

  std::string JsonUtils::response(const std::string& message) {
    Json::Value response = Json::Value(Json::objectValue);

    if ( message == "" ) {
      response["status"] = "ok";
    } else {
      response["status"] = "failure";
      response["message"] = message;
    }

    return JsonUtils::value2string(response);
  }

  Json::Value JsonUtils::getRealValueNotJson5(double rVal) {
    if (std::isnan(rVal)) return Json::nullValue;
    if (std::isinf(rVal)) return Json::nullValue;
    return rVal;
  }

  bool JsonUtils::resolve(const Json::Value& root,
                          const Json::Path& path,
                          std::string& value,
                          const bool useDefault,
                          const std::string defaultValue) {
    Json::Value v = path.resolve( root );
    if ( v.isNull() || !v.isString() ) {
      if ( useDefault ) {
        value = defaultValue;
        return true;
      }
      return false;
    }
    value = v.asString();
    return true;
  }

  bool JsonUtils::resolve(const Json::Value& root,
                          const Json::Path& path,
                          double& value,
                          const bool useDefault,
                          const double defaultValue) {
    Json::Value v = path.resolve( root );
    if ( v.isNull() || !v.isDouble() ) {
      if ( useDefault ) {
        value = defaultValue;
        return true;
      }
      return false;
    }
    value = v.asDouble();
    return true;
  }

  bool JsonUtils::resolve(const Json::Value& root,
                          const Json::Path& path,
                          int& value,
                          const bool useDefault,
                          const int defaultValue) {
    Json::Value v = path.resolve( root );
    if ( v.isNull() || !v.isInt() ) {
      if ( useDefault ) {
        value = defaultValue;
        return true;
      }
      return false;
    }
    value = v.asInt();
    return true;
  }

  bool JsonUtils::resolve(const Json::Value& root,
                          const Json::Path& path,
                          unsigned int& value,
                          const bool useDefault,
                          const unsigned int defaultValue) {
    Json::Value v = path.resolve( root );
    if ( v.isNull() || !v.isUInt() ) {
      if ( useDefault ) {
        value = defaultValue;
        return true;
      }
      return false;
    }
    value = v.asUInt();
    return true;
  }

  static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](unsigned char c) { return !std::isspace(c);}));
  }

  static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            [](unsigned char c) { return !std::isspace(c);}).base(), s.end());
  }

  static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
  }

  Json::Value JsonUtils::parseJsonObjectComboBox(const std::string& text) {
    try {
      std::string qs = text;
      trim(qs);
      if ( *qs.begin() == '{' && *qs.rbegin() == '}') {
        Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJson(qs);
        if (valueObject.isObject() &&
            valueObject.isMember("value") &&
            (valueObject.isMember("input") &&  valueObject["input"].isArray() ||
             valueObject.isMember("inputPixmap") &&  valueObject["inputPixmap"].isArray() ) &&
            valueObject.isMember("output") &&  valueObject["output"].isArray() ) {
          return valueObject;
        }
      }
    } catch ( const ch_semafor_intens::JsonException& e ) {
      // do nothing
    }
    return Json::Value();
  }

  bool JsonUtils::assignJsonObjectComboBox(std::string &leftValue, const std::string &rightValue, const bool rightValid) {
    // right side is json object combo box
    if ( rightValid ) {
      Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(rightValue);
      if ( !valueObject.isNull() ) { // right side is json object combo box
        leftValue = rightValue;
        return true;
      }
    }

    // right side is string or invalid
    Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(leftValue);
    // left side is string
    if ( valueObject.isNull() ) {
      if ( rightValid ) {
        leftValue = rightValue;
        return true;
      }
      return false;
    }


    // left side is json object combo box

    // replace value, if found in 'output'
    for(unsigned int i=0; i<valueObject["output"].size(); ++i) {
      if ( rightValid && valueObject["output"][i] == rightValue ) {
        valueObject["value"] = rightValue;
        leftValue = ch_semafor_intens::JsonUtils::value2string(valueObject);
        return true;
      }
      if ( !rightValid && valueObject["output"][i].isNull() ) {
        valueObject["value"] = Json::Value(); // nullValue
        leftValue = ch_semafor_intens::JsonUtils::value2string(valueObject);
        return true;
      }
    }

    // right value not found
    if ( rightValid ) {
      leftValue = rightValue;
      return true;
    }
    return false;
  }

  Json::Value JsonUtils::createDragJsonObject (const std::string& source,
					       const std::vector<std::string>& varnameList,
					       const std::vector<bool>& isFolderList) {
    try {
      // create json object
      Json::Value dragJson = Json::Value(Json::objectValue);
      dragJson[ DRAG_OBJECT_SOURCE ] = source;
      Json::Value itemAry = Json::Value(Json::arrayValue);
      Json::Value item2Ary = Json::Value(Json::arrayValue);

      // varname list
      std::vector<std::string>::const_iterator it = varnameList.begin();
      for(; it != varnameList.end(); ++it) {
	itemAry.append( (*it) );
      }
      dragJson[ DRAG_OBJECT_VARNAME ] = itemAry;

      // varname list
      std::vector<bool>::const_iterator it2 = isFolderList.begin();
      for(; it2 != isFolderList.end(); ++it2) {
	item2Ary.append( (*it2) );
      }
      dragJson[ DRAG_OBJECT_ISFOLDER ] = item2Ary;

      return dragJson;

    } catch ( const ch_semafor_intens::JsonException& e ) {
      // do nothing
    }
    return Json::Value();
  }

  Json::Value JsonUtils::createDragJsonObject (const std::string& source,
					       const std::string& varname,
					       bool isFolder) {
    try {
      // create json object
      Json::Value dragJson = Json::Value(Json::objectValue);
      dragJson[ DRAG_OBJECT_SOURCE ] = source;
      // varname
      Json::Value itemAry = Json::Value(Json::arrayValue);
      itemAry.append( varname );
      dragJson[ DRAG_OBJECT_VARNAME ] = itemAry;
      // isFolder
      Json::Value item2Ary = Json::Value(Json::arrayValue);
      itemAry.append( isFolder );
      dragJson[ DRAG_OBJECT_ISFOLDER ] = item2Ary;
      return dragJson;

    } catch ( const ch_semafor_intens::JsonException& e ) {
      // do nothing
    }
    return Json::Value();
  }

} // end namespace ch_semafor_intens
