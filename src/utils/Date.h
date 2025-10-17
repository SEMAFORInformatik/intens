
#ifndef DATE_H
#define DATE_H
#include <string>

#include "app/UserAttr.h"

  std::string DateAndTime();
  std::string DateAndTime(const int offset);
  std::string GetTime();
  std::string GetDate();
  std::string convertFromISODate(const std::string& iso, const UserAttr::STRINGtype type, bool bShort=false);
  std::string convertToISODate(double julianDate, const UserAttr::STRINGtype type);
  double convertISOToJulianDate(const std::string& isoDate, bool* ok);

class Date {
public:
  static std::string currentIsoTime();
  static std::string currentIsoDate();
  static std::string currentIsoDateTime();
  static std::string getDateTimeElapsed(const std::string& isoBegin, const std::string& isoEnd, bool bShort=false);
  static std::string durationAsString(int msecs, bool bAppendix=true, const std::string& label=std::string());
};

#endif
