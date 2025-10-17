
#if !defined(ICONMANAGER_H)
#define ICONMANAGER_H

#include <map>
#include <string>
class QPixmap;
class QWebView;


/** The IconManager is a Singleton Object that manages XPM files.
*/
class IconManager {
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
protected:
  IconManager();
  ~IconManager();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Der IconManager ist eine singleton Class. Es darf nur ein Objekt
      instanziert werden. Diese Funktion liefert die Referenz auf den IconManager.
   */
  static IconManager &Instance();

  enum ICON_TYPE{BMP, GIF, MNG, JPG,
		 PBM, PGM, PNG, PNM,
		 PPM, XBM, XPM, TIF,
		 SVG, EPS,
		 NONE};
  struct ICON_DATA {
    ICON_DATA(const std::string& filt, const std::string&mime_type)
      : filter(filt), mimeType(mime_type){
    }
    std::string filter;
    std::string mimeType;
  };
  typedef std::multimap<ICON_TYPE, ICON_DATA> IconTypeMap;
  static IconTypeMap& getIconTypeMap() { return s_iconTypeMap; }

  bool getDataUrlFileContent(ICON_TYPE& icontype, const std::string &name, std::string &filecontent, bool locateFilename=true);
  bool getDataUrlContent(QPixmap& pixmap, std::string &filecontent);
  std::string locateFile(ICON_TYPE& icontype, const std::string &name);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  static IconManager* s_instance;
protected:
  static IconTypeMap    s_iconTypeMap;

};

#endif
