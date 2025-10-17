#include <string>
#include <time.h>
#include <limits>
#include <iomanip>

#ifdef HAVE_QT
#include <iostream>
#include <QDate>
#include <QLocale>
#include <QDateEdit>
#include <QString>
#include <QTimeZone>

#if QT_VERSION < 0x050000
#define ISO_DATE_FORMAT Qt::ISODate
#else
#define ISO_DATE_FORMAT Qt::ISODateWithMs
#endif

#endif

#include "utils/Debugger.h"
#include "utils/utils.h"

INIT_LOGGER();

/*---------------------------------------------------------------------
  GetTime --
    returns the current time in "hh:mm:ss"

  ---------------------------------------------------------------------
*/
std::string GetTime()
{
  static char timestr[20];
  time_t now;

  (void) time( &now );
  strftime( timestr, sizeof timestr, "%H:%M:%S", localtime( &now ) );
  return timestr;
}

/*---------------------------------------------------------------------
  GetDate --
    returns the current date in "yyyy-mm-dd"

  ---------------------------------------------------------------------
*/
std::string GetDate()
{
  static char datestr[20];
  time_t now;

  (void) time( &now );
  strftime( datestr, sizeof datestr,"%Y-%m-%d", localtime( &now ) );
  return datestr;
}
/*---------------------------------------------------------------------
  DateAndTime --
    returns the current date and time in "yyyy-mm-dd hh:mm:ss"

  ---------------------------------------------------------------------
*/
std::string DateAndTime()
{
  return GetDate() + " " + GetTime();
}

/*---------------------------------------------------------------------
  DateAndTime(offset) --
    returns the current date and time + offset in "yyyy-mm-dd hh:mm:ss"

  ---------------------------------------------------------------------
*/
std::string DateAndTime(const int offset)
{
  static char datestr[20];
  time_t now;

  (void) time( &now );
  struct tm stm = *localtime( &now );

  if(offset != 0) {
    stm.tm_sec += offset;
    mktime(&stm);
  }
  strftime( datestr, sizeof datestr,"%Y-%m-%d %H:%M:%S", &stm );

  return datestr;
}

/*---------------------------------------------------------------------
  DateAndTime --
    returns the current date and time in "yyyy-mm-dd hh:mm:ss"

  ---------------------------------------------------------------------
*/
std::string convertFromISODate( const std::string& iso,
                                const UserAttr::STRINGtype type,
                                bool bShort) {
  BUG_DEBUG("convertFromISODate(" << iso << ")");

#ifdef HAVE_QT
  switch(type) {
  case UserAttr::string_kind_date:
    {
      BUG_DEBUG("-- date --");
      // not a date, it is only a date difference
      if (iso.size() && (iso[0] ==  '-' || iso[0] == '+' || iso[0] == ' ')) {
	bool pos = iso[0] ==  '-' ? false : true;
	std::string dIso = iso.substr(1, iso.size()-1);
	QString qStr = QString::fromStdString(dIso);
	QStringList qList = qStr.split("-");
	if (qList.size() != 3)
	  return "";
	int y= QString(qList.at(0)).toInt();
	int m= QString(qList.at(1)).toInt();
	int d= QString(qList.at(2)).toInt();
	std::string ret;
	if (y > 0)
	  ret = compose("%1 %2 years %3 months %4 days", ((pos)? ' ':'-'),y,m,d);
	else if (m > 0)
	  ret = compose("%1 %2 months %3 days", ((pos)? ' ':'-'),m,d);
	else
	  ret = compose("%1 %2 days", ((pos)? ' ':'-'),d);
	BUG_DEBUG("--> y["<<y<<"] m["<<m<<"] d["<<d<<"] ret["<<ret<<"]");
	return ret;
      }

      QDate date = QDate::fromString( QString::fromStdString(iso),
                                      Qt::ISODate);
      if( !date.isValid() ){
        BUG_DEBUG("==> date is not valid");
      }
      if (!iso.size()){
	date = QDate::currentDate();
      }
      QString localDateFormat = QLocale().dateFormat(QLocale::ShortFormat);
      BUG_DEBUG("--> DateFormat = " << localDateFormat.toStdString());

      std::string rslt = date.toString(localDateFormat).toStdString();
      BUG_DEBUG("Result: Date [" << rslt << "]");
      return rslt;
    }
  case UserAttr::string_kind_datetime:
    {
      BUG_DEBUG("-- datetime --");
      QDateTime datetime = QDateTime::fromString( QString::fromStdString(iso),
                                                  ISO_DATE_FORMAT);
      if( !datetime.isValid() ){
        BUG_DEBUG("==> datetime is not valid");
      }
      if (!iso.size()){
	datetime = QDateTime::currentDateTime();
      }
      QString localDateFormat = QLocale().dateFormat(QLocale::ShortFormat);
      QString localTimeFormat = QLocale().timeFormat(QLocale::ShortFormat);
      BUG_DEBUG("--> DateFormat = " << localDateFormat.toStdString() );
      BUG_DEBUG("--> TimeFormat = " << localTimeFormat.toStdString() );

      if (bShort) {
	localDateFormat = localDateFormat.left( localDateFormat.lastIndexOf(':') );
	localTimeFormat = localTimeFormat.left( localTimeFormat.lastIndexOf(':') );
        BUG_DEBUG("--> short DateFormat = " << localDateFormat.toStdString() );
        BUG_DEBUG("--> short TimeFormat = " << localTimeFormat.toStdString() );
      }
      QString localDateTimeFormat = localDateFormat + " " + localTimeFormat;
      BUG_DEBUG("--> DateTimeFormat = " << localDateTimeFormat.toStdString() );

#if QT_VERSION < 0x050000
      // qt4 does not convert timezones (correctly)
      // the following code helps qt4 to convert utc to localtime
      if(QString::fromStdString(iso).endsWith("+0000")) {
        BUG_DEBUG("utc to localtime");
        datetime.setTimeSpec(QTimeZone(Qt::UTC));
      }
#endif
      std::string rslt = datetime.toLocalTime().toString(localDateTimeFormat).toStdString();
      BUG_DEBUG("Result: DateTime [" << rslt << "]");
      return rslt;
    }
  case UserAttr::string_kind_time:
    {
      // not a date, it is only a date difference
      std::string dIso = iso;
      std::string pre;
      if (iso.size() && (iso[0] ==  '-' || iso[0] == '+' || iso[0] == ' ')) {
	if (iso[0] ==  '-') pre = "-";
	dIso = iso.substr(1, iso.size()-1);
      }
      QTime time  = QTime::fromString(QString::fromStdString(dIso), ISO_DATE_FORMAT); //"hh:mm:ss.zzz" );
      if (!iso.size())
	time = QTime::currentTime();
      QString localTimeFormat = "hh:mm:ss.zzz";//QLocale().timeFormat(QLocale::LongFormat);
      if (bShort)
	localTimeFormat = localTimeFormat.left( localTimeFormat.lastIndexOf('.') );
      return pre + time.toString(localTimeFormat).toStdString();
    }
  default:
    {
      assert( false );
    }
  }
  return std::string(iso);
#else
  assert(false);
#endif
}

// not used
std::string convertToISODate(const std::string& iso, const UserAttr::STRINGtype type) {
#ifdef HAVE_QT
  switch(type) {
  case UserAttr::string_kind_date:
    {
      QString localDateFormat = QLocale().dateFormat(QLocale::ShortFormat);
      QDate date  = QDate::fromString(QString::fromStdString(iso), localDateFormat );
      return                    date.toString(ISO_DATE_FORMAT).toStdString();
    }
  case UserAttr::string_kind_time:
  case UserAttr::string_kind_datetime:
  default:
    {
      assert( false );
    }
  }
  return std::string(iso);
#else
  assert(false);
#endif
}

/*---------------------------------------------------------------------
  convertISOToJulianDate --
    returns an julian date

  ---------------------------------------------------------------------*/
double convertISOToJulianDate(const std::string& isoDate, bool* ok) {
  #ifdef HAVE_QT
  QDateTime datetime  = QDateTime::fromString(QString::fromStdString(isoDate), ISO_DATE_FORMAT);

  if (datetime.isValid()) {
    *ok = true;
    return datetime.date().toJulianDay() + ((double) QTime(0, 0, 0, 0).msecsTo(datetime.time()))/(86400.0 * 1000);
  } else {
    QTime time  = QTime::fromString(QString::fromStdString(isoDate), ISO_DATE_FORMAT);
    if (time.isValid()) {
      *ok = true;
      return ((double) QTime(0, 0, 0, 0).msecsTo(time))/(86400.0 * 1000);
    }
  }
  *ok = false;
  #else
  // TODO
  #endif
  return std::numeric_limits<double>::quiet_NaN();
}

std::string convertToISODate(double julianDate, const UserAttr::STRINGtype type) {
#ifdef HAVE_QT
  QDate d = QDate::fromJulianDay(julianDate);
  double value = d.toJulianDay();
  double dMSecs = (julianDate - round(value))* 86400.0 * 1000;
  QTime t = QTime(0, 0, 0, 0).addMSecs(qRound(dMSecs));
#if QT_VERSION >= 0x060500
  QDateTime dt(d, t, QTimeZone(QTimeZone::UTC));
#else
  QDateTime dt(d, t, Qt::UTC);
#endif
  switch(type) {
  case UserAttr::string_kind_date:
    return d.toString(ISO_DATE_FORMAT).toStdString();
  case UserAttr::string_kind_time:
    return t.toString(ISO_DATE_FORMAT).toStdString();
  case UserAttr::string_kind_datetime:
    return dt.toString(ISO_DATE_FORMAT).toStdString();
  default:
    // ignored 'string_kind_none', 'string_kind_value', 'string_kind_password'
    ;
  }
#else
  // TODO
#endif
  return ""; // should not happen
}

std::string Date::currentIsoTime() {
  #ifdef HAVE_QT
  return QTime::currentTime().toString(ISO_DATE_FORMAT).toStdString();
  #else
  return ""; // TODO
  #endif
}

std::string Date::currentIsoDate() {
  #ifdef HAVE_QT
  return QDate::currentDate().toString(ISO_DATE_FORMAT).toStdString();
  #else
  return ""; // TODO
  #endif
}

std::string Date::currentIsoDateTime() {
  #ifdef HAVE_QT
  return QDateTime::currentDateTime().toString(ISO_DATE_FORMAT).toStdString();
  #else
  return ""; // TODO
  #endif
}

std::string Date::getDateTimeElapsed(const std::string& isoBegin, const std::string& isoEnd, bool bShort) {
  BUG(BugUtilities,"Date::getDateTimeElapsed");
  #ifdef HAVE_QT
  QDate date  = QDate::fromString(QString::fromStdString(isoBegin), ISO_DATE_FORMAT);
  QDateTime datetime  = QDateTime::fromString(QString::fromStdString(isoBegin), ISO_DATE_FORMAT);
  QTime time  = QTime::fromString(QString::fromStdString(isoBegin), ISO_DATE_FORMAT);
  QDate dateEnd  = QDate::fromString(QString::fromStdString(isoEnd), ISO_DATE_FORMAT);
  QDateTime datetimeEnd  = QDateTime::fromString(QString::fromStdString(isoEnd), ISO_DATE_FORMAT);
  QTime timeEnd  = QTime::fromString(QString::fromStdString(isoEnd), ISO_DATE_FORMAT);

  if (time.isValid() && timeEnd.isValid()) {
    int msecs = timeEnd.msecsTo(time);
    int secs = timeEnd.secsTo(time);
    QTime res((abs(secs)/3600)%24,(abs(secs)/60)%60,abs(secs)%60, abs(msecs)%1000);
    BUG_MSG(" ===> Result valid["<<res.isValid()<<"] ["<<res.toString("hh:mm:ss.zzz").toStdString());
    std::string pStr;
    if (msecs<0)
      pStr = "-";
    if (bShort) {
      if (abs(msecs) < 1000)
        return pStr + res.toString("zzz").toStdString() + " msecs";
      else
        if (abs(secs) < 60)
          return pStr + res.toString("ss.zzz").toStdString() + " secs";
        else
          if (abs(secs) < 3600)
            return pStr + res.toString("mm:ss.zzz").toStdString() + " secs";
    }
    else
      return pStr + res.toString("hh:mm:ss.zzz").toStdString();
  }

  if (date.isValid() && dateEnd.isValid()) {
    int sign = (dateEnd > date) ? 1 : -1;
    int dyear  = sign * (dateEnd.year()-date.year());
    int dmonth = sign * (dateEnd.month()-date.month());
    int dday   = sign * (dateEnd.day()-date.day());
    if (dday < 0) {
      dday = date.daysInMonth() - dateEnd.day() + date.day();
      --dmonth;
    }
    if (dmonth < 0) {
      dmonth += 12;
      --dyear;
    }
    std::stringstream os;
    os << ((sign == -1) ? "-" : " ");
    os.width(4);os.fill('0');
    os << abs(dyear) << "-";
    os.width(2);
    os << dmonth << "-" << dday;
    BUG_MSG(" ===> Date Result dyear["<<dyear<<"] dmonth["<<dmonth<<"] dday["<<dday<<"] str["<<os.str()<<"]");
    return os.str();
  }
  return isoBegin;
  #else
  // TODO
  return "";
  #endif
}

std::string Date::durationAsString(int msecs, bool bAppendix, const std::string& label) {
  std::ostringstream os;
  if (msecs) {
    if (bAppendix)
      os << ", ";
    if (!label.empty())
      os << label;
    if (msecs >= 1000) {
      if (label.empty())
        os << "duration in secs: ";
      os << msecs/1000 << "." <<  std::setfill('0') << std::setw(3)  << msecs%1000 << "s";
    } else {
      if (label.empty())
        os << "duration in msecs: ";
      os << msecs  << "ms";
    }
  }
  return os.str();
}
