
#if !defined(URL_CONVERTER_H)
#define URL_CONVERTER_H

#include <string>
#include <map>

class UrlConverter {
public:
  /** Die Funktion konvertiert den text in einen URL-kodierten string.
   *  (siehe curl_easy_escape)
   */
  static void urlQuote(std::string &text);

  /** Die Funktion konvertiert die Zeichen 0x80..0xff im text in einen URL-kodierten string
   *  (percend-encoded). Dies sind alle nicht ASCII7 Bytes von UTF-8.
   */
  static void urlQuote_extendedAscii(std::string &text);
private:
  static void init_extendedAscii();
private:
  static std::string chars_extendedAscii;
  static std::map<char, std::string> replStr_extendedAscii;
};

#endif  // URL_CONVERTER_H
