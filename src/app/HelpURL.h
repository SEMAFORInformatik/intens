
#if !defined(HELP_URL_INCLUDED_H)
#define HELP_URL_INCLUDED_H

#include "app/HelpFile.h"

class HelpManager;

/** Help in Form eines HTML-Files.
 * Abgeleitet von der Klasse HelpFile.
 * @short Help in Form eines HTML-Files.
 */
class HelpURL : public HelpFile{

  //---------------------------------------------------------------------------//
  // Constructor / Destructor                                                  //
  //---------------------------------------------------------------------------//
 public:
  HelpURL(HelpManager *helpManager);
  virtual ~HelpURL(){}

  //---------------------------------------------------------------------------//
  // public member functions                                                   //
  //---------------------------------------------------------------------------//
 public:
/**
   * Die Befehlzeile fuer den Netscape wird zusammengestellt.
   * @param cmd Leerer std::string& wird uebergeben und mit der Befehlszeile gefuellt.
   * @param keyName Direkte Sprung-'Adresse'.
   * @param remote True wenn netscape bereits geoeffnet.
   */
  virtual void helpBrowserCmd( std::string &,  const std::string &keyName, bool remote ) const;
};

#endif
