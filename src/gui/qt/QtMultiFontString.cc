
#include <qapplication.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <QRegularExpression>
#include <qfontdatabase.h>
#include <QStringList>
#include <qsettings.h>

#include "utils/Debugger.h"
#include "utils/gettext.h"

#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQtManager.h"

// static string - QFont Map
QtMultiFontString::StringQFontMap QtMultiFontString::s_stringFontMap;
QtMultiFontString::StringCssMap QtMultiFontString::s_stringCssMap;

#ifdef Q_OS_UNIX
const char*  DEFAULT_FONT = "Adobe Helvetica";
#else
const char*  DEFAULT_FONT = "Arial";
#endif
const short  DEFAULT_FONT_SIZE = 10;

INIT_LOGGER();

//---------------------------------------------------------------------------
// getQString
//---------------------------------------------------------------------------
QString QtMultiFontString::getQString( const std::string &label ){
//  std::cout<<"QtMultiFontString::getQString label:["<<label<<"]\n";
  QString retLabel = QString::fromStdString( label );
  retLabel.replace( QRegularExpression("\\@[^@]*\\@"), "" );
  retLabel.replace( QRegularExpression("\\<at\\>"), "\\@" );
  retLabel.replace( QRegularExpression("\\\\n"), "\n" );
  retLabel.replace( QRegularExpression("\\\\\""), "\"" );
  return retLabel;
}

//---------------------------------------------------------------------------
// getQFont
//---------------------------------------------------------------------------
QFont QtMultiFontString::getQFont(const std::string &label,
                                  QFont &default_font,
                                  bool *available) {
  BUG_DEBUG("Begin getQFont(" << label << ")");
  if( available ){
    BUG_DEBUG("- with bool available");
  }
  QString regExp("@");
  int pos = -1;
  if( available ){ *available = false; }

  if( (pos = QString::fromStdString(label).indexOf(regExp) ) != -1 ){
    std::string fName( label.substr(pos+1,
                                    QString::fromStdString(label).indexOf(regExp, pos+1)-pos-1) );
    // falls vorhanden => return aus stringQFontMap
    StringQFontMap::const_iterator sit = s_stringFontMap.find(fName);
    if( sit != s_stringFontMap.end() ){
      if( available ){ *available = true; }
      BUG_DEBUG("Return: already available, family: " << (*sit).second.family().toStdString());
      return (*sit).second; // bereits vorhanden
    }

    QFont font = default_font;
    if( !GuiQtManager::Settings() ){
      BUG_DEBUG("Return: no settings");
      return default_font;
    }

    // Get Font Family
    std::string resname( "fonts/" + fName + ".Family" );
    QString family = GuiQtManager::Settings()->value( resname.c_str(), "" ).toString();
    BUG_DEBUG("Family from Settings = " << family.toStdString() );

    if( family.isEmpty() ){
      family = font.family();
      BUG_DEBUG("Family from Default = " << family.toStdString() );
    }
    else{
      if( available ){ *available = true; }
    }

    // find a font family that matches this family => choose shortest
    QStringList families = QFontDatabase::families();
    families = families.filter( family );
    if (families.size()) family = families[0];
    for ( QStringList::Iterator f = families.begin(); f != families.end(); ++f ) {
      if (family.length() > (*f).length() ){
        family = (*f);
      }
    }
    BUG_DEBUG("Used Family = " << family.toStdString() );

    // Get Font Weight
    resname = "fonts/" + fName + ".Weight";
    QString weight = GuiQtManager::Settings()->value( resname.c_str(), "Normal" ).toString();
    BUG_DEBUG("Weight from Settings = " << weight.toStdString() );

    // Get Font Size
    int defPointSize = font.pointSize();
    if( defPointSize < 0 ){
      defPointSize = DEFAULT_FONT_SIZE;
    }
    resname = "fonts/" + fName + ".PointSize";
    int pointSize = GuiQtManager::Settings()->value( resname.c_str(), 0 ).toInt();
    BUG_DEBUG("PointSize from Settings = " << pointSize );

    if( pointSize == 0 ){
      pointSize = defPointSize;
      BUG_DEBUG("PointSize from Defaults = " << pointSize );
    }
    else{
      if( available ){ *available = true; }
    }

    QFont newFont = QFont( family, pointSize );

    QString fi(QFontInfo(newFont).family());
    if ( !families.size() && (!fi.length() || fi != family) ) {
#if _DEBUG
      std::cerr << " WARNING Font Family unknown \'" << family.toStdString()
        << "\' ==USE=DEFAULT=FONT==> \'"
        << QApplication::font().family().toStdString()
        << "\' used by \'" << fName << "\'\n" << std::flush;
#endif
      default_font.setPointSize(pointSize);
      s_stringFontMap[fName] = default_font;
      BUG_DEBUG("Return: Font Family unknown. Use Default Font");
      return default_font;
    }

    QStringList styles = QFontDatabase::styles( fi );
    for ( QStringList::Iterator s = styles.begin(); s != styles.end(); ++s ) {
      std::string str(weight.toStdString());
      std::string search((*s).toStdString());
      std::string::size_type pos = str.find((*s).toStdString());
      if( pos == std::string::npos ){
        continue;
      }
      if ( search.find("Light") == 0) {
        newFont.setWeight( QFont::Light );
      } else
      if ( search.find("Normal") == 0) {
        newFont.setWeight( QFont::Normal );
      } else
      if ( search.find("Demi Bold") == 0) {
        newFont.setWeight( QFont::DemiBold );
      } else
      if ( search.find("Bold") == 0) {
        newFont.setWeight( QFont::Bold );
      } else
      if ( search.find("Black") == 0) {
        newFont.setWeight( QFont::Black );
      }
      if ( search.find("Italic") != std::string::npos) {
        newFont.setItalic(true);
      }
    }

    if( available ){
      if( *available ){
        s_stringFontMap[fName] = newFont;
      }
      else{
        BUG_DEBUG("Font NOT available");
      }
    }
    else {
      s_stringFontMap[fName] = newFont;
    }

    BUG_DEBUG("Return: New Font OK");
    return newFont;
  }
  s_stringFontMap["DefaultFont"] = default_font;
  BUG_DEBUG("Return: Use Default Font");
  return default_font;
}

//---------------------------------------------------------------------------
// getCss
//---------------------------------------------------------------------------
std::string QtMultiFontString::getCss(const std::string &fontName,
                                      bool *available, bool omitFontPointSize){
  std::string css;
  if( available ){ *available = false; }

  if( fontName.empty() ){
    return css;
  }

  // schon vorhanden => return aus stringQFontMap
  std::ostringstream os_fontName;
  os_fontName << fontName << "@" << omitFontPointSize;
  StringCssMap::const_iterator sit = s_stringCssMap.find(os_fontName.str());
  if( sit != s_stringCssMap.end() ){
      return (*sit).second;
  }
  if (!GuiQtManager::Settings()){
    return css;
  }

  QFont font;
  // Get Font Family
  std::string resname( "fonts/" + fontName + ".Family" );
  QString family = GuiQtManager::Settings()->value( resname.c_str(), "" ).toString();
  if( family.isEmpty() ){
    family = font.family();
  }
  else{
    if( available ){ *available = true; }
  }

  // find a font family that matches this family => choose shortest
  QStringList families = QFontDatabase::families();
  families = families.filter( family );
  if (families.size()) family = families[0];
  for ( QStringList::Iterator f = families.begin(); f != families.end(); ++f ) {
    if (family.length() > (*f).length() ){
      family = (*f);
    }
  }

  // Get Font Weight
  resname = "fonts/" + fontName + ".Weight";
  QString weight = GuiQtManager::Settings()->value( resname.c_str(), "Normal" ).toString();

  // Get Font Size
  int defPointSize = font.pointSize();
  if( defPointSize < 0 ){
    defPointSize = DEFAULT_FONT_SIZE;
  }
  resname = "fonts/" + fontName + ".PointSize";
  int pointSize = GuiQtManager::Settings()->value( resname.c_str(), 0 ).toInt();
  if( pointSize == 0 ){
    pointSize = defPointSize;
  }
  else{
    if( available ){ *available = true; }
  }

  if (omitFontPointSize) {
    css = compose("font-family: %1;", family.toStdString());
  } else {
    css = compose("font-family: %1; font-size: %2pt;", family.toStdString(), pointSize);
  }
  std::string sweight( weight.toStdString() );
  if ( sweight.find("Normal") == 0) {
    css += " font-weight: normal;";
  } else
  if ( sweight.find("Bold") == 0) {
    css += " font-weight: bold;";
  }
  if (sweight.find("Italic") != std::string::npos) {
    css += " font-style: italic;";
  }
  css += " ";
  s_stringCssMap[os_fontName.str()] = css;
  return css;
}

//---------------------------------------------------------------------------
// writeResourceFile
//---------------------------------------------------------------------------
bool QtMultiFontString::writeResourceFile( QSettings &settings ){
  settings.setValue( "fonts/boldfont.Weight", "Bold" );

  // default
  settings.setValue( "fonts/default.Family", DEFAULT_FONT );
  settings.setValue( "fonts/default.Weight", "Normal" );
  settings.setValue( "fonts/default.PointSize", 12 );

  // application title
  settings.setValue( "fonts/appTitle.Family", DEFAULT_FONT );
  settings.setValue( "fonts/appTitle.Weight", "Bold" );
  settings.setValue( "fonts/appTitle.PointSize", 18 );

  // application subtitle
  settings.setValue( "fonts/copyright.Family", DEFAULT_FONT );
  settings.setValue( "fonts/copyright.Weight", "Bold" );
  settings.setValue( "fonts/copyright.PointSize", 12 );

  // copyright
  settings.setValue( "fonts/subTitle.Family", DEFAULT_FONT );
  settings.setValue( "fonts/subTitle.Weight", "Normal" );
  settings.setValue( "fonts/subTitle.PointSize", 10 );

  // combobox
  settings.setValue( "fonts/combobox.Family", "Courier" );
  settings.setValue( "fonts/combobox.Weight", "Normal" );
  settings.setValue( "fonts/combobox.PointSize", 10 );

  // button
  settings.setValue( "fonts/button.Family", DEFAULT_FONT );
  settings.setValue( "fonts/button.Weight", "Bold" );
  settings.setValue( "fonts/button.PointSize", 12 );

  // index
  settings.setValue( "fonts/index.Family", DEFAULT_FONT );
  settings.setValue( "fonts/index.Weight", "Bold" );
  settings.setValue( "fonts/index.PointSize", 12 );

  // label
  settings.setValue( "fonts/label.Family", DEFAULT_FONT );
  settings.setValue( "fonts/label.Weight", "Normal" );
  settings.setValue( "fonts/label.PointSize", 10 );

  // messageline
  settings.setValue( "fonts/messageLine.Family", DEFAULT_FONT );
  settings.setValue( "fonts/messageLine.Weight", "Normal" );
  settings.setValue( "fonts/messageLine.PointSize", 10 );

  // list title
  settings.setValue( "fonts/listTitle.Family", "Courier" );
  settings.setValue( "fonts/listTitle.Weight", "Bold" );
  settings.setValue( "fonts/listTitle.PointSize", 8 );

  // list
  settings.setValue( "fonts/list.Family", "Courier" );
  settings.setValue( "fonts/list.Weight", "Normal" );
  settings.setValue( "fonts/list.PointSize", 10 );

  // menu
  settings.setValue( "fonts/menu.Family", DEFAULT_FONT );
  settings.setValue( "fonts/menu.Weight", "Bold" );
  settings.setValue( "fonts/menu.PointSize", 10 );

  // pulldown
  settings.setValue( "fonts/pulldown.Family", DEFAULT_FONT );
  settings.setValue( "fonts/pulldown.Weight", "Normal" );
  settings.setValue( "fonts/pulldown.PointSize", 8 );

  // popup
  settings.setValue( "fonts/popup.Family", DEFAULT_FONT );
  settings.setValue( "fonts/popup.Weight", "Normal" );
  settings.setValue( "fonts/popup.PointSize", 8 );

  // messagebox
  settings.setValue( "fonts/messagebox.Family", DEFAULT_FONT );
  settings.setValue( "fonts/messagebox.Weight", "Normal" );
  settings.setValue( "fonts/messagebox.PointSize", 8 );

  // print
  settings.setValue( "fonts/print.Family", DEFAULT_FONT );
  settings.setValue( "fonts/print.Weight", "Normal" );
  settings.setValue( "fonts/print.PointSize", 8 );

  // symbol
  settings.setValue( "fonts/symbol.Family", "Symbol" );
  settings.setValue( "fonts/symbol.Weight", "Normal" );
  settings.setValue( "fonts/symbol.PointSize", 12 );

  // tabs
  settings.setValue( "fonts/folderTab.Family", DEFAULT_FONT );
  settings.setValue( "fonts/folderTab.Weight", "Bold" );
  settings.setValue( "fonts/folderTab.PointSize", 12 );

  // groupboxTitle
  settings.setValue( "fonts/groupboxTitle.Family", DEFAULT_FONT );
  settings.setValue( "fonts/groupboxTitle.Weight", "Bold" );
  settings.setValue( "fonts/groupboxTitle.PointSize", 12 );

  // text
  settings.setValue( "fonts/text.Family", "Courier" );
  settings.setValue( "fonts/text.Weight", "Normal" );
  settings.setValue( "fonts/text.PointSize", 12 );

  // title
  settings.setValue( "fonts/title.Family", DEFAULT_FONT );
  settings.setValue( "fonts/title.Weight", "Normal" );
  settings.setValue( "fonts/title.PointSize", 12 );

  // banner
  settings.setValue( "fonts/banner.Family", DEFAULT_FONT );
  settings.setValue( "fonts/banner.Weight", "Normal" );
  settings.setValue( "fonts/banner.PointSize", 14 );

  // image
  settings.setValue( "fonts/imageTitle.Family", DEFAULT_FONT );
  settings.setValue( "fonts/imageTitle.Weight", "Normal" );
  settings.setValue( "fonts/imageTitle.PointSize", 14 );

  // plot
  settings.setValue( "fonts/plotTitle.Family", DEFAULT_FONT );
  settings.setValue( "fonts/plotTitle.Weight", "Normal" );
  settings.setValue( "fonts/plotTitle.PointSize", 14 );

  settings.setValue( "fonts/plotAxis.Family", DEFAULT_FONT );
  settings.setValue( "fonts/plotAxis.Weight", "Normal" );
  settings.setValue( "fonts/plotAxis.PointSize", 14 );

  settings.setValue( "fonts/plotAxisTitle.Family", DEFAULT_FONT );
  settings.setValue( "fonts/plotAxisTitle.Weight", "Normal" );
  settings.setValue( "fonts/plotAxisTitle.PointSize", 14 );

  settings.setValue( "fonts/plotLegend.Family", DEFAULT_FONT );
  settings.setValue( "fonts/plotLegend.Weight", "Normal" );
  settings.setValue( "fonts/plotLegend.PointSize", 14 );

  settings.setValue( "fonts/plotMarker.Family", DEFAULT_FONT );
  settings.setValue( "fonts/plotMarker.Weight", "Normal" );
  settings.setValue( "fonts/plotMarker.PointSize", 14 );

  // thermo
  settings.setValue( "fonts/thermoLabel.Family", DEFAULT_FONT );
  settings.setValue( "fonts/thermoLabel.Weight", "Normal" );
  settings.setValue( "fonts/thermoLabel.PointSize", 14 );

  settings.setValue( "fonts/thermoUnits.Family", DEFAULT_FONT );
  settings.setValue( "fonts/thermoUnits.Weight", "Normal" );
  settings.setValue( "fonts/thermoUnits.PointSize", 14 );

  settings.setValue( "fonts/thermoScale.Family", DEFAULT_FONT );
  settings.setValue( "fonts/thermoScale.Weight", "Normal" );
  settings.setValue( "fonts/thermoScale.PointSize", 14 );

  return true;
}


//---------------------------------------------------------------------------
// listFonts
//---------------------------------------------------------------------------
void QtMultiFontString::listFonts(){

//   std::cout<< "QtMultiFontString::listFonts" <<std::flush;
  std::cout << std::endl << std::flush;
  std::cout<<_("Following fonts are available on your system:" ) <<std::flush;
  std::cout << std::endl << std::flush;

  QStringList families = QFontDatabase::families();
  for ( QStringList::Iterator f = families.begin(); f != families.end(); ++f ) {
    QString family = *f;
//     std::cout<<" -family ["<<family<<"]\n";
  }
  std::cout << std::flush;
}

/* --------------------------------------------------------------------------- */
/* printFont --                                                                */
/* --------------------------------------------------------------------------- */

std::string QtMultiFontString::printFont( QFont &font ){
  std::ostringstream ostr;

  ostr <<
    "Family=" << font.family().toStdString() <<
    " Size=" << font.pointSize() <<
    " Weight=" << font.weight()
    ;
  return ostr.str();
}
