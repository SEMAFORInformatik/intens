
#if !defined(HELP_FILE_INCLUDED_H)
#define HELP_FILE_INCLUDED_H

#include <string>
#include "gui/GuiMenuButtonListener.h"

class ProcessGroup;
class Process;
class HelpKeyword;
class HelpManager;

/**
 * Base class all Help classes
 */

class HelpFile : public GuiMenuButtonListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  HelpFile(HelpManager *helpManager);
  virtual ~HelpFile();

  //---------------------------------------------------------------------------//
  // public functions                                                          //
  //---------------------------------------------------------------------------//
 public:
  /**
   * Hier werden die Help-Buttons in der Form installiert und mit dem dem HelpFile
   * verbunden
   * Wird direkt vom Parser aufgerufen.
   */
  virtual void installHelpCallback();
  /**
   * Die Befehlzeile fuer den Netscape wird zusammengestellt.
   * @param cmd Leerer string& wird uebergeben und mit der Befehlszeile gefuellt.
   * @param keyName Direkte Sprung-'Adresse'.
   * @param remote True wenn netscape bereits geoeffnet.
   */
  virtual void helpBrowserCmd( std::string &, const std::string &keyName, bool remote ) const{}
  /** Das Anzeigen eines HelpFiles wird gestartet.
   * @param keyName Direkte Sprung-'Adresse' bei HelpURL und HelpHTML.
   * @param chapterNo Kapitelnummer bei HelpText.
   */
  virtual void help( const std::string &keyName = std::string(), const int chapterNo = 1 );
  /** Wird vom Parser und HelpText aufgerufen
   * @param keyName Direkte Sprung-'Adresse' bei HelpURL und HelpHTML.
   * @param chapterNo Kapitelnummer bei HelpText.
   */
  void installHelpKey( const std::string &keyName, int chapterNo = 1 );
  void setFileName(const std::string &name);
  /** Soll das HelpFile nur ueber einen HelpKey aufgerufen werden, kann der
   * Menueintrag unsichtbar gemacht werden
   */
  void hideHelpMenu();
  void setMenuTitle( const std::string &title );

  virtual void ButtonPressed();

  JobAction* getAction(){ return 0; }
  //---------------------------------------------------------------------------//
  // protected data                                                            //
  //---------------------------------------------------------------------------//
 protected:
  bool                m_MenuHidden;
  std::string         m_Title;
  std::string         m_FileName;
  HelpManager *const  m_Manager;
};

#endif
