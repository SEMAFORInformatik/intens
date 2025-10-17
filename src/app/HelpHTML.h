
#if !defined(HELP_HTML_INCLUDED_H)
#define HELP_HTML_INCLUDED_H

#include "app/HelpFile.h"

class HelpManager;

/** Help in Form eines HTML-Files auf dem WWW.
 * Abgeleitet von der Klasse HelpFile.
 * @short Help in Form eines HTML-Files (WWW).
 */
class HelpHTML : public HelpFile{

  //---------------------------------------------------------------------------//
  // Constructor / Destructor                                                  //
  //---------------------------------------------------------------------------//
 public:
  HelpHTML(HelpManager *helpManager);
  virtual ~HelpHTML(){}

  //---------------------------------------------------------------------------//
  // public functions                                                          //
  //---------------------------------------------------------------------------//
 public:
  /**
   * Die Befehlzeile fuer den Netscape wird zusammengestellt.
   * @param cmd Leerer string& wird uebergeben und mit der Befehlszeile gefuellt.
   * @param keyName Direkte Sprung-'Adresse'.
   * @param remote True wenn netscape bereits geoeffnet.
   */
  virtual void helpBrowserCmd( std::string &, const std::string &keyName, bool remote ) const;
 /** Das Anzeigen von HelpBrowser wird gestartet.
   * @param keyName Leer
   */
  virtual void help(const std::string &helpKey = std::string(), const int chapterNo = 1);
};

#endif
