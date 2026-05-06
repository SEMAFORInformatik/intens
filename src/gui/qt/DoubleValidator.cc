// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#include <QApplication>
#include "xfer/Scale.h"
#include "xfer/XferConverter.h"
#include "app/AppData.h"
#include "utils/Debugger.h"
#include "gui/qt/DoubleValidator.h"

INIT_LOGGER();

class MyLocale {
private:
  MyLocale() {

    _decimalSymbol = QString(RealConverter::decimalPoint());

    _negativeSign  = '-';
    _thousandsSeparator = '\'';
    _thousandsSeparator = QGuiApplication::inputMethod()->locale().system().groupSeparator();
  }
public:
  static const MyLocale& Instance() {
    if ( !s_instance ) {
      s_instance = new MyLocale();
    }
    return *s_instance;
  }
  QString decimalSymbol() const { return _decimalSymbol; }
  QString negativeSign()  const { return _negativeSign; }
  QString thousandsSeparator() const { return _thousandsSeparator; }
private:
  QString _decimalSymbol;
  QString _negativeSign;
  QString _thousandsSeparator;
  static  MyLocale* s_instance;
};
MyLocale* MyLocale::s_instance = 0;

QString DoubleValidator::decimalSymbol() { return MyLocale::Instance().decimalSymbol(); }
QString DoubleValidator::thousandsSeparator() { return MyLocale::Instance().thousandsSeparator(); }

//  Implementation of DoubleValidator
//
DoubleValidator::DoubleValidator(QObject * parent, Scale* scale)
  : QDoubleValidator( parent ), m_scale( scale )
{
  _min = std::numeric_limits<double>::min();
  _max = std::numeric_limits<double>::max();
}

DoubleValidator::DoubleValidator(double bottom, double top, int decimals,
				    QObject * parent)
  : QDoubleValidator( bottom, top, decimals, parent ), m_scale(0)
{
  _min = bottom;
  _max = top;
}

DoubleValidator::~DoubleValidator()
{
}

QValidator::State DoubleValidator::validate( QString & input, int & p ) const {
  QString s = input;
  // empty inputs are Accepted
  if (s.trimmed().size() == 0) {
    return Acceptable;
  }
  //    are exactly three decimals between each separator):
  const MyLocale& l = MyLocale::Instance();
  QString d = l.decimalSymbol(),
    n = l.negativeSign(),
    t = l.thousandsSeparator();

  // return Invalid if char '.' is not used(decimalSymbol, thousandsSeparator)
  //       and string has char '.'
  if ( (!d.isEmpty() && d.indexOf('.') == -1) &&
       (t.isEmpty() || (!t.isEmpty() && t.indexOf('.') == -1) ) )
    if (s.indexOf( '.'  )  != -1)
      return Invalid;

  // return Invalid if char ',' is not used(decimalSymbol, thousandsSeparator)
  //       and string has char ','
  if ( (!d.isEmpty() && d.indexOf(',') == -1) &&
       (t.isEmpty() || (!t.isEmpty() && t.indexOf(',') == -1) ) )
    if (s.indexOf( ','  )  != -1)
      return Invalid;

  if ( !t.isEmpty() )
    for ( int idx = s.indexOf( t ) ; idx >= 0 ; idx = s.indexOf( t, idx ) )
      s.remove( idx, t.length() );

  // then, replace the d's and n's
  if ( ( !n.isEmpty() && n.indexOf('.') != -1 ) ||
       ( !d.isEmpty() && d.indexOf('-') != -1 ) ) {
    // make sure we don't replace something twice:
    BUG_WARN("+++ DoubleValidator: decimal symbol contains '-' or "
             "negative sign contains '.' -> improve algorithm");
    return Invalid;
  }

  if ( !n.isEmpty() && n != "-" )
    for ( int idx = s.indexOf( n ) ; idx >= 0 ; idx = s.indexOf( n, idx + 1 ) )
      s.replace( idx, n.length(), "-" );

  // Special Case KeypadDecimalPoint, Do replace of Decimal Point
  if (getenv("DECIMAL_POINT") != NULL ||
      AppData::Instance().KeypadDecimalPoint() &&
      AppData::Instance().isWindows()) {
    QString defDecPt(RealConverter::defaultDecimalPoint());
    s.replace(d, defDecPt );
  }
  return QDoubleValidator::validate( s, p );
}

void DoubleValidator::setRange ( double bottom, double top ) {
  if (m_scale){
    bool    ok;
    double fac = m_scale->getValue();
    bottom *= fac;
    top *= fac;
  }
  QDoubleValidator::setRange (bottom, top );
}
