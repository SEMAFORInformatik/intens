
#ifndef QTMULTIFONTSTRING_H
#define QTMULTIFONTSTRING_H

#include <iostream>
#include <string>
#include <map>

#include <qstringlist.h>

class QString;
class QFont;
class QSettings;

class QtMultiFontString
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  QtMultiFontString() {}
  virtual ~QtMultiFontString() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static QString getQString( const std::string &str );
  static QFont getQFont( const std::string &label, QFont &font, bool *available = 0 );
  static std::string getCss( const std::string &label, bool *available = 0, bool omitFontPointSize = false );
  static bool writeResourceFile( QSettings &settings );
  static void listFonts();
  static std::string printFont( QFont &font );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:


/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::map< std::string, QFont> StringQFontMap;
  typedef StringQFontMap::value_type StringQFontMapPair;
  static StringQFontMap    s_stringFontMap;

  typedef std::map<std::string, std::string> StringCssMap;
  typedef StringCssMap::value_type StringCssMapPair;
  static StringCssMap    s_stringCssMap;
};
#endif
