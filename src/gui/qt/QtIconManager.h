
#if !defined(QTICONMANAGER_H)
#define QTICONMANAGER_H

#include <QFont>
#include <map>
#include <string>
#include "gui/IconManager.h"

class QPixmap;
class QWebView;
class QSvgWidget;

/** a Singleton object for managing XPM pixmap files
*/
class QtIconManager : public IconManager {
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  QtIconManager();
  ~QtIconManager();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Der IconManager ist eine singleton Class. Es darf nur ein Objekt
      instanziert werden. Diese Funktion liefert die Referenz auf den IconManager.
   */
  static QtIconManager &Instance();

  /// The member function getPixmap returns the specific Pixmap
  bool getPixmap( const std::string& iconname, QPixmap &pixmap, int width=-1, int height=-1 );
  bool getPixmap( const std::string& filename, ICON_TYPE icon_type, QPixmap &pixmap, int width=-1, int height=-1 );
#if QT_VERSION < 0x050900
  bool getWebViewWidget( const std::string& iconname, QWebView &webview, int width=-1, int height=-1 );
#else
  bool getWebViewWidget( const std::string& iconname, QSvgWidget &webview, int width=-1, int height=-1 );
#endif
  QPixmap* getLabelPixmap( const std::string& label, QPixmap& pixmap, QFont& font, bool& multiLine );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  static QtIconManager* s_instance;
};

#endif
