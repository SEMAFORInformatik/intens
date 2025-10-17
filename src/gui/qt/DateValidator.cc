
#include "DateValidator.h"
#include <qstring.h>
#include <qdatetime.h>
#include <qwidget.h>

#if  QT_VERSION >= 0x030300
#include <qlocale.h>
#endif

#include "utils/gettext.h"
#include "utils/compose.h"

#include "gui/qt/GuiQtFactory.h"

#include <iostream>
#include <locale.h>

DateValidator::DateValidator(GuiElement* elem, QWidget* parent, const char* name)
  : QRegularExpressionValidator(parent), m_elem( elem )
{
#if  QT_VERSION >= 0x030300
  if (QLocale::system().language() == QLocale::English) {
       m_format = "MM\\dd\\yyyy";
       m_regExString = "[0-9]{1,2}\\\\[0-9]{1,2}\\\\[0-9]{4}";
  }
  if (QLocale::system().language() == QLocale::German) {
       m_format = "dd.MM.yyyy";
       m_regExString = "[0-9]{1,2}\\.[0-9]{1,2}\\.[0-9]{4}";
  }
#endif

#if 0
  int len = dstr.length();
  int pos = 0;
  while (pos < len) {
    if (dstr[pos] == '1' ) {  // month
      m_format += "MM";
      pos  +=2;
      m_regExString += "[0-1]{1}[0-9]{1}\\";
      if (pos >= len) break;
      m_format += dstr[pos];
      m_regExString += dstr[pos];
      ++pos;
    }

    if (dstr[pos] == '3' ) {  // day
      m_format += "dd";
      pos  +=2;
      m_regExString += "[0-3]{1}[0-9]{1}\\";
      if (pos >= len) break;
      m_format += dstr[pos];
      m_regExString += dstr[pos];
      ++pos;
    }

    if (dstr[pos] == '2' ) {  // year
      m_format += "yyyy";
      pos  +=4;
      m_regExString += "[0-9]{4}";
      if (pos >= len) break;
      m_format += dstr[pos];
      m_regExString += dstr[pos];
      ++pos;
    }
    if (dstr[pos] == '0' ) {  // year
      m_format += "yy";
      m_regExString += "[0-9]{2}";
      if (pos >= len) break;
      m_format += dstr[pos];
      m_regExString += dstr[pos];
      ++pos;
      pos  +=2;
    }
  }
#endif

  std::cout << " ==> Formart["<<m_format.toStdString()<<"]\n"<<std::flush;
  QDate cd = QDate::currentDate();
  std::cout << " ==> RegExpStr["<<m_regExString.toStdString()<<"]\n"<<std::flush;
  QRegularExpression rx( m_regExString );
  setRegularExpression( rx );
}


QValidator::State
DateValidator::validate(QString& text, int& n) const
{
  QValidator::State ret = QRegularExpressionValidator::validate(text, n);
  if (ret == QValidator::Acceptable) {
    QStringList list(text);

#if  QT_VERSION >= 0x030300
    if (QLocale::system().language() == QLocale::English) {
      QStringList list = text.split("\\");
      if (list.count() != 3)
	return QValidator::Invalid;
      QDate temp(list[2].toInt(), list[0].toInt(), list[1].toInt());
      if (temp.isNull()) {
	GuiQtFactory::Instance()->showDialogWarning(0, _("Convertion Error"), _("Invalid Date."), 0);
	return QValidator::Invalid;
      }
    }
    if (QLocale::system().language() == QLocale::German) {
      QStringList list = text.split(".");
      QDate temp(list[2].toInt(), list[1].toInt(), list[0].toInt());
      if (temp.isNull()) {
	GuiQtFactory::Instance()->showDialogWarning(0, _("Convertion Error"), _("Invalid Date."), 0);
	return QValidator::Invalid;
      }
    }
#endif

//     QDate temp;
//     // ----- everything is tested in date():
//     return date(text, temp);
  } else {
    if (text.length() <= 2)
    m_elem->printMessage( SemaforString::compose(_("Date Format is \'%1\'."), m_format.toStdString()),
			  GuiElement::msg_Information, 2 );
  }
  return ret;
}

QValidator::State
DateValidator::date(const QString& text, QDate& d) const
{

  QDate tmp(2004, 12, 77);
//   QDate tmp = KGlobal::locale()->readDate(text);
  if (!tmp.isNull())
    {
      d = tmp;
      return Acceptable;
    } else
      return Intermediate;
}

void
DateValidator::fixup( QString& ) const
{

}
