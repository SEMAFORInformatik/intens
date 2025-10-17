
#if !defined(HELPMANAGER_INCLUDED_H)
#define HELPMANAGER_INCLUDED_H

#include <map>
#include <string>

#include "operator/BatchProcess.h"
#include "operator/DaemonProcess.h"
#include "gui/TimerTask.h"

class HelpText;
class HelpURL;
class HelpHTML;
class HelpFile;
class HelpBrowser;
class HelpCall;
class Timer;

/**
 * Die Klasse HelpManager fuehrt eine Liste aller HelpFiles und HelpKeys.
 * Sie kontrolliert den Start des Netscape-Browsers, wenn dieser benoetigt wird.
 * @short Der HelpManager
 * @author Daniel Kessler
 * @version 0.1 beta
 */

class HelpManager : private TimerTask
{
  //---------------------------------------------------------------------------//
  // Constructor / Destructor                                                  //
  //---------------------------------------------------------------------------//

private:
  HelpManager();
  virtual ~HelpManager();

  friend class HelpCall;

  //---------------------------------------------------------------------------//
  // private member classes                                                    //
  //---------------------------------------------------------------------------//
public:
  /** Die Klasse HelpKey beinhaltet alle Informationen, um in einem
   * HelpFile direkt an einen bestimmten Punkt zu gelangen.
   * @short Die Klasse HelpKey
   * @author Daniel Kessler
   * @version 0.1 beta
   */
  class HelpKey {
  public:
    HelpKey(std::string name, HelpFile *helpFile, int chapterNo)
      : m_Name( name )
      , m_HelpFile( helpFile )
      , m_ChapterNo( chapterNo ){
    }
    /**
     * Ueber die Methode show() wird das HelpFile angezeigt.
     */
    void show();
  private:
    const std::string       m_Name;
    HelpFile  * const  m_HelpFile;
    const int	       m_ChapterNo;
};

  //---------------------------------------------------------------------------//
  // public member functions                                                   //
  //---------------------------------------------------------------------------//
public:

  /**
   * Nur ueber die Methode Instance() kann der HelpManager erzeugt werden.
   * Sie stellt sicher, dass nur ein einziger HelpManager existiert.
   * ( Konstruktor ist private ).
   * @return Pointer auf den HelpManager
   */
  static HelpManager &Instance();
  /**
   * Wird von HelpFile aufgerufen.
   * @param keyName Wird bei HelpHTML und HelpURL benoetigt.
   * @param chapterNo entspricht der Kapitelnummer bei HelpText
   */
  void installHelpKey( HelpFile *const helpFile, const std::string &keyName, int chapterNo );
  /** Wird vom Parser aufgerufen um den Key (direkter Sprung) in einer Form zu installieren
   */
  HelpKey *getHelpKey( const std::string &name ) const;
  /** Wird direkt vom Parser aufgerufen.
   */
  HelpFile *installHelpText( const std::string &name );
  /** Wird direkt vom Parser aufgerufen.
   */
  HelpFile *installHelpHTML( const std::string &name );
  /** Wird direkt vom Parser aufgerufen.
   */
  HelpFile *installHelpURL( const std::string &name );
  /** Wird von den HelpFiles nach dem Callback aufgerufen, wenn der HTML-Browser benoetigt wird.
   * @param keyName Sprung-'Adresse'
   */
  void helpBrowser( HelpFile *const helpfile, const std::string &keyName );

  void createAllHelpText();
  //---------------------------------------------------------------------------//
  // private member functions                                                  //
  //---------------------------------------------------------------------------//
private:
  HelpText *addHelpText( const std::string &name );
  HelpFile *addHelpHTML( const std::string &name );
  HelpFile *addHelpURL( const std::string &name );
  bool manageStartBrowser( HelpCall *hc );
  std::string getFileName( const std::string &name ) const;

/*=============================================================================*/
/* private tick method from TimerTask                                          */
/*=============================================================================*/
private:
  void tick();

  //---------------------------------------------------------------------------//
  // private member data                                                       //
  //---------------------------------------------------------------------------//
private:
  typedef std::map<std::string, HelpFile *>    HelpFileList;
  typedef std::map<std::string, HelpKey>       HelpKeyList;

  mutable HelpFileList         m_HelpFiles;
  mutable HelpKeyList          m_HelpKeyList;
  BatchProcess                 m_remoteBrowserProcess;
  DaemonProcess                 m_browserProcess;
  static HelpManager          *s_Instance;

  HelpCall                    *m_helpCall;
  Timer                       *m_timer;
};

#endif
