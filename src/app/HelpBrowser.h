
#if !defined(HELP_BROWSER_INCLUDED_H)
#define HELP_BROWSER_INCLUDED_H

#include "app/HelpFile.h"

class HelpWindow;

/** Help in Form eines HTML-Files fuer den QT-Browser.
 * Abgeleitet von der Klasse HelpFile.
 * @short Help in Form eines HTML-Files duer den QT-Browser.
 */
class HelpBrowser : public HelpFile {
  //---------------------------------------------------------------------------//
  // Constructor / Destructor                                                  //
  //---------------------------------------------------------------------------//
 public:
  HelpBrowser(HelpManager *helpManager);
  virtual ~HelpBrowser();

  //---------------------------------------------------------------------------//
  // public member functions                                                   //
  //---------------------------------------------------------------------------//
 public:
 /**
   * Hier werden die Help-Buttons in der Form installiert und mit dem dem HelpFile
   * verbunden
   * Wird direkt vom Parser aufgerufen.
   */
/*   virtual void installHelpCallback(); */
 /** Das Anzeigen von HelpBrowser wird gestartet.
   * @param keyName Leer
   */
  virtual void help(const std::string &helpKey = std::string(), const int chapterNo = 1);

  //---------------------------------------------------------------------------//
  // private member data                                                       //
  //---------------------------------------------------------------------------//
 private:
    HelpWindow  *m_helpwindow;
};

#endif
