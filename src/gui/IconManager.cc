
#include<QDir>
#include<QStringList>
#include<QPixmap>
#include<QBuffer>

#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"
#include "utils/base64.h"

#include "gui/IconManager.h"
#include "app/AppData.h"
#include <iostream>
#include <stdlib.h>

INIT_LOGGER();

IconManager::IconTypeMap IconManager::s_iconTypeMap;

IconManager* IconManager::s_instance = 0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
IconManager::IconManager() {
  s_iconTypeMap.insert( IconTypeMap::value_type(XPM, ICON_DATA("Xpm (*.xpm)", "image/x-xpixmap") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(BMP, ICON_DATA("Bitmap (*.bmp)", "image/x-bmp") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(GIF, ICON_DATA("GIF (*.gif)", "image/gif") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(MNG, ICON_DATA("Mng (*.mng)", "video/x-mng") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(JPG, ICON_DATA("Jpg (*.jpg)", "image/jpeg") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(JPG, ICON_DATA("Jpeg (*.jpeg)", "image/jpeg") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(PBM, ICON_DATA("Pbm (*.pbm)", "image/x-portable-bitmap") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(PGM, ICON_DATA("Pgm (*.pgm)", "image/x-portable-graymap") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(PNG, ICON_DATA("Png (*.png)", "image/png") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(PNM, ICON_DATA("Pnm (*.pnm)", "image/x-portable-anymap") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(PPM, ICON_DATA("Ppm (*.ppm)", "image/x-portable-pixmap") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(TIF, ICON_DATA("Tif (*.tif)", "image/tiff") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(SVG, ICON_DATA("Svg (*.svg)", "image/svg+xml") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(EPS, ICON_DATA("Eps (*.eps)", "application/postscript") ) );
  s_iconTypeMap.insert( IconTypeMap::value_type(XBM, ICON_DATA("Xbm (*.xbm)", "image/x-xbitmap") ) );
}

IconManager::~IconManager() {
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

IconManager &IconManager::Instance() {
  if (s_instance == 0) {
    s_instance = new IconManager;
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* getDataUrlFileContent --                                                    */
/* --------------------------------------------------------------------------- */
bool IconManager::getDataUrlFileContent(ICON_TYPE& icon_type, const std::string &name,
                                        std::string &filecontent, bool locateFilename) {
  filecontent.clear();
  std::string file = name;
  if (locateFilename) {
    file = IconManager::Instance().locateFile(icon_type, name);
  }
  if (!file.empty()) {
    std::ifstream ifs(file.c_str());
    std::string fcontent = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    std::string sBase64;
    base64encode(reinterpret_cast<const unsigned char*>(fcontent.c_str()),
                 fcontent.size(), sBase64, false);
    if (icon_type != IconManager::NONE) {
      IconTypeMap::iterator it = s_iconTypeMap.find(icon_type); //]->second.mimeType;
      if (it != s_iconTypeMap.end()) {
        std::ostringstream os;
        os << "data:" << it->second.mimeType << ";base64," << sBase64;
        filecontent = os.str();
      }
    }
    ifs.close();
    return !filecontent.empty();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getDataUrlContent --                                                        */
/* --------------------------------------------------------------------------- */
bool IconManager::getDataUrlContent(QPixmap& pixmap, std::string &datauri) {
 QByteArray bytes;
 QBuffer buffer(&bytes);
 ICON_TYPE icon_type = ICON_TYPE::PNG;
 buffer.open(QIODevice::WriteOnly);
 pixmap.save(&buffer, "PNG"); // writes pixmap i
 std::string fcontent = buffer.data().toStdString();
  if (!fcontent.empty()) {
    std::string sBase64;
    base64encode(reinterpret_cast<const unsigned char*>(fcontent.c_str()),
                 fcontent.size(), sBase64, false);
    if (icon_type != IconManager::NONE) {
      IconTypeMap::iterator it = s_iconTypeMap.find(icon_type); //]->second.mimeType;
      if (it != s_iconTypeMap.end()) {
        std::ostringstream os;
        os << "data:" << it->second.mimeType << ";base64," << sBase64;
        datauri = os.str();
      }
    }
    return !datauri.empty();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* locateFile --                                                               */
/* --------------------------------------------------------------------------- */
std::string IconManager::locateFile(ICON_TYPE& icon_type, const std::string &name ) {
  BUG_PARA(BugGuiIcon,"IconManager::locateFile",name);
  icon_type = NONE;
  if( name.empty() )
    return name;
  std::string _name(name);
  if( name[name.size()-1] == '.')
    _name.erase(name.size()-1);

  QDir d;
  QString dir, bitmapSubDir;
  QStringList dirs;
  const char *envVarValue;

  bitmapSubDir = QDir::separator();
  bitmapSubDir+= "bitmaps";

  // --- BITMAP_PATH -----------------------------------------
  if( (envVarValue=getenv("BITMAP_PATH")) != NULL ){
    BUG_DEBUG("BITMAP_PATH: "<< envVarValue);
    QChar pathSeparator = QDir::separator() == '/' ? ':' : ';';
    QStringList list = QString(envVarValue).split(pathSeparator);
    for (int i=0; i < list.size(); ++i) {
      dir = list.at(i);
      dirs += dir;
    }
  }
  // --- APP HOME -----------------------------------------
  if( (envVarValue=getenv("APPHOME")) != NULL ){
    dir = envVarValue;
    dir+= bitmapSubDir;
    dirs += dir;
  }
  // --- INTENS_HOME ----------------------------------
  if( AppData::Instance().IntensHome().size() ){
    dir = QString::fromStdString(AppData::Instance().IntensHome());
    dir+= bitmapSubDir;
    dirs += dir;
  }
  // --- ./bitmaps ------------------------------------
  dir = QDir().absolutePath();
  dir+= bitmapSubDir;
  dirs += dir;
  // --- current diur ---------------------------------
  dirs += QDir().absolutePath();


  IconTypeMap::const_iterator tit = s_iconTypeMap.begin();
  for( ; tit != s_iconTypeMap.end(); ++tit){
    for ( QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
      std::string fname( _name );
      if ( d.cd(*it) ) {
        // get file extension
        std::string::size_type pos = tit->second.filter.find_last_of(".");
        if (pos == std::string::npos) continue;
        std::string::size_type posE = tit->second.filter.find_last_of(")");
        if (posE == std::string::npos) posE = tit->second.filter.size();
        std::string ext( tit->second.filter.substr(pos, posE-pos) );

        pos = fname.find( ext, (fname.size()-ext.size()) );
        if ( pos == std::string::npos)
          fname += ext;

        QString fileExt = QString::fromStdString(fname);
        QString file = d.filePath( fileExt );
        if( d.exists(file) ) {
          icon_type = tit->first;
          BUG_DEBUG("pixmap file found in directory: '"<<d.absolutePath().toStdString()<<"'");
          return file.toStdString();
        }
      }
    }
  }

  BUG_DEBUG("no icon found for '"<<name<<"'");
  return "";
}
