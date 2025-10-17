
#include <iostream>
#include <map>
#include <assert.h>

#include "LaTeXConverter.h"

// version mit QString
#if HAVE_Q
#include <QString>
#if QT_VERSION >= 0x060400
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

//static const char *specialChars = "\\^~|<>{}$_%#&[]¶§°∞±ΩΩφπ€─¢£\n";
static std::map<QString, QString> replStr;

void initReplStr() {
  if (replStr.size())
    return;
  replStr["\\"] = "\\textbackslash ";
  replStr["^"] = "\\textasciicircum ";
  replStr["~"] = "\\textasciitilde ";
  replStr["|"] = "\\textbar ";
  replStr["<"] = "\\textless ";
  replStr[">"] = "\\textgreater ";
  // {}$_%#& -> Standardersetzung
  replStr["["] = "{[}";
  replStr["]"] = "{]}";
  replStr["¶"] = "\\P{}";
  replStr["§"] = "\\S{}";
  replStr["°"] = "\\textordmasculine ";
  replStr["∞"] = "$\\infty$";
  replStr["±"] = "$\\pm$";
  replStr["Ω"] = "$\\Omega$";
  replStr["Ω"] = "$\\Omega$";
  replStr["φ"] = "$\\phi$";
  replStr["π"] = "$\\pi$";
  replStr["€"] = "\\euro{}";
  replStr["─"] = "-";
  replStr["¢"] = "c";
  replStr["£"] = "\\pounds ";
  replStr["\n"] = "\\\\";
}

void LaTeXConverter::convert2LaTeX( std::string &text ){
  /** description file is utf8 encoded
      all texinputs should also be utf8 encoded
      please use utf8x package
      \usepackage[utf8x]{inputenc}
  **/
  initReplStr();
  QString qstr( QString::fromStdString(text).trimmed() );
//static QRegExp qspecialChars("[\\\\|\\^|\\~|\\||\\<|\\>|\\{|\\}|\\$|\\_|\\%|\\#|\\&|\\[|\\]|\\¶|\\§|\\°|\\∞|\\±|\\Ω|\\Ω|\\φ|\\π|\\€|\\─|\\¢|\\£|\\\\\n]");
#if QT_VERSION >= 0x060600
  static QRegularExpression qspecialChars("[\\\\|\\^|\\~|\\||\\<|\\>|\\{|\\}|\\$|\\_|\\%|\\#|\\&|\\[|\\]|\\\\\n]");
#else
  static QRegExp qspecialChars("[\\\\|\\^|\\~|\\||\\<|\\>|\\{|\\}|\\$|\\_|\\%|\\#|\\&|\\[|\\]|\\\\\n]");
#endif
  int qpos = 0;
  while (qpos != -1) {
    qpos = qstr.indexOf(qspecialChars, qpos);
    if( qpos != -1){
      std::map<QString, QString>::const_iterator it = replStr.begin();
      for (; it != replStr.end(); ++it) {
	if (qstr.indexOf((*it).first, qpos) == qpos) {
	  int n=(*it).first.toStdString().size();
	  qstr.replace(qpos,n, (*it).second);
	  qpos += (*it).second.size(); // next character
	  break;
	}
      }
      // Standardersetzung
      if (it == replStr.end()) {
	qstr.insert(qpos, "\\");
	qpos += 2; // next character
      }
    }
  }
  text = qstr.toStdString();
}

#else // alte version (2010-06-25)
static std::string specialChars("\\^~|<>{}$_%#&\n");

void LaTeXConverter::convert2LaTeX( std::string &text ){
  std::string::size_type pos = text.find_first_of( specialChars.c_str() );
  if( pos != std::string::npos ){
    while( pos != std::string::npos ){
      if( text[pos]=='\\' ){
	text.replace( pos, 1, "\\textbackslash " );
	pos += 15;
      }
      else if( text[pos]=='^' ){
	text.replace( pos, 1, "\\textasciicircum " );
	pos += 17;
      }
      else if( text[pos]=='~' ){
	text.replace( pos, 1, "\\textasciitilde " );
	pos += 16;
      }
      else if( text[pos]=='|' ){
	text.replace( pos, 1, "\\textbar " );
	pos += 9;
      }
      else if( text[pos]=='<' ){
	text.replace( pos, 1, "\\textless " );
	pos += 10;
      }
      else if( text[pos]=='>' ){
	text.replace( pos, 1, "\\textgreater " );
	pos += 13;
      }
      else if( text[pos]=='\n' ){
	text.replace( pos, 1, "\\\\" );
	pos += 2;
      }
      else{
	text.insert( pos, 1, '\\' );
	pos += 2;
      }
      pos = text.find_first_of( specialChars.c_str(), pos );
    }
  }
}
#endif
