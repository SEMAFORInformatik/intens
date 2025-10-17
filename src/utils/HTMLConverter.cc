
#include <string>
#include <assert.h>

#ifdef HAVE_QT
#include <QTextDocument>
#endif

#include "HTMLConverter.h"
#include "app/AppData.h"

void HTMLConverter::convert2HTML( std::string &text ){
  std::string::size_type pos = text.find_first_of( "<>&\"'@\n" );
  if( pos != std::string::npos ){
    while( pos != std::string::npos ){
      if( text[pos] == '<' ){
	text.replace( pos, 1, "&lt;" );
	pos += 4;
      }
      else if( text[pos] == '>' ){
	text.replace( pos, 1, "&gt;" );
	pos += 4;
      }
      else if( text[pos] == '&' ){
	text.replace( pos, 1, "&amp;" );
	pos += 5;
      }
      else if( text[pos] == '"' ){
	text.replace( pos, 1, "&quot;" );
	pos += 6;
      }
      else if( text[pos] == '\'' ){
	text.replace( pos, 1, "&apos;" );
	pos += 6;
      }
      else if( text[pos] == '@' ){
	text.replace( pos, 1, "&gt;at&lt;" );
	pos += 6;
      }
      else if( text[pos] == '\n' ){
        if (AppData::Instance().HeadlessWebMode()) {
          text.replace( pos, 1, "<br/>" );
          pos += 5;
        } else {
          pos += 1;
        }
      }
//       else if( text[pos] == 'ä' ){
// 	text.replace( pos, 1, "&auml;" );
// 	pos += 6;
//       }
//       else if( text[pos] == 'ü' ){
// 	text.replace( pos, 1, "&uuml;" );
// 	pos += 6;
//       }
//       else if( text[pos] == 'ö' ){
// 	text.replace( pos, 1, "&ouml;" );
// 	pos += 6;
//       }
      else
	assert( false );
      pos = text.find_first_of( "<>&\"'\n", pos );
    }
  }
}

void HTMLConverter::convertFromHTML( std::string &text ){
#ifdef HAVE_QT
  QTextDocument doc;
  doc.setHtml(QString::fromStdString(text));
  text = doc.toPlainText().toStdString();
#endif
}
