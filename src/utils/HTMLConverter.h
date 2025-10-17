
#if !defined(HTML_CONVERTER_H)
#define HTML_CONVERTER_H

#include <string>

class HTMLConverter{
public:
  static void convert2HTML( std::string &text );
  static void convertFromHTML( std::string &text );
};

#endif
