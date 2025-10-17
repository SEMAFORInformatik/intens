/**********************************************************************
 **
 ** $Id: KNumValidator.cpp,v 1.9 2006/03/15 09:22:44 amg Exp $
 **
 ** KIntValidator, KFloatValidator:
 **   Copyright (C) 1999 Glen Parker <glenebob@nwlink.com>
 ** KDoubleValidator:
 **   Copyright (c) 2002 Marc Mutz <mutz@kde.org>
 **
 ** This library is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Library General Public
 ** License as published by the Free Software Foundation; either
 ** version 2 of the License, or (at your option) any later version.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Library General Public License for more details.
 **
 ** You should have received a copy of the GNU Library General Public
 ** License along with this library; if not, write to the Free
 ** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 **
 *****************************************************************************/

#include <qwidget.h>
#include <qstring.h>

#include <iostream>
#include <assert.h>
#include <limits>

#include "KNumValidator.h"

#ifdef _WIN32
#include <sstream>
#include <windows.h>
#include <qapplication.h>
#else
#include <qapplication.h>
#endif

#include "xfer/Scale.h"
#include "xfer/XferConverter.h"
#include "app/AppData.h"

class MyLocale {
private:
  MyLocale() {

    _decimalSymbol = QString(RealConverter::decimalPoint());

    _negativeSign  = '-';
    //std::use_facet< std::moneypunct<char, true> >( loc ).negative_sign()[0];
    //     _positiveSign  = '+';
    _thousandsSeparator = '\'';
#if defined(HAVE_QT) && QT_VERSION > 0x050000
    _thousandsSeparator = QGuiApplication::inputMethod()->locale().system().groupSeparator();
#endif
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
  QString positiveSign()  const { return _positiveSign; }
  QString thousandsSeparator() const { return _thousandsSeparator; }
private:
  QString _decimalSymbol;
  QString _negativeSign;
  QString _positiveSign;
  QString _thousandsSeparator;
static  MyLocale* s_instance;
};
MyLocale* MyLocale::s_instance = 0;

 //  Implementation of KIntValidator
 //

KIntValidator::KIntValidator ( QWidget * parent, int base, const char * name )
  : QValidator(parent)
{
  _base = base;
  if (_base < 2) _base = 2;
  if (_base > 36) _base = 36;

  _min = _max = 0;
}

KIntValidator::KIntValidator ( int bottom, int top, QWidget * parent, int base, const char * name )
  : QValidator(parent)
{
  _base = base;
  if (_base > 36) _base = 36;

  _min = bottom;
  _max = top;
}

KIntValidator::~KIntValidator ()
{}

QString KIntValidator::decimalSymbol() { return MyLocale::Instance().decimalSymbol(); }
QString KIntValidator::thousandsSeparator() { return MyLocale::Instance().thousandsSeparator(); }

QValidator::State KIntValidator::validate ( QString &str, int & ) const
{
  bool ok;
  int  val = 0;
  QString newStr;

  // empty inputs are Accepted
  if (str.trimmed().size() == 0) {
    return Acceptable;
  }
  newStr = str.trimmed();
  if (_base > 10)
    newStr = newStr.toUpper();

  if (newStr == QString::fromStdString("-")) // a special case
    if ((_min || _max) && _min >= 0)
      ok = false;
    else
      return QValidator::Acceptable;
  else if (newStr.length())
    val = newStr.toInt(&ok, _base);
  else {
    val = 0;
    ok = true;
  }

  if (! ok)
    return QValidator::Invalid;

  if ((! _min && ! _max) || (val >= _min && val <= _max))
    return QValidator::Acceptable;

  if (_max && _min >= 0 && val < 0)
    return QValidator::Invalid;

  if (_max && _min >= 0 && val > _max)
    return QValidator::Invalid;

  return QValidator::Intermediate;
}

void KIntValidator::fixup ( QString &str ) const
{
  int                dummy;
  int                val;
  QValidator::State  state;

  state = validate(str, dummy);

  if (state == QValidator::Invalid || state == QValidator::Acceptable)
    return;

  if (! _min && ! _max)
    return;

  val = str.toInt(0, _base);

  if (val < _min) val = _min;
  if (val > _max) val = _max;

  str.setNum(val, _base);
}

void KIntValidator::setRange ( int bottom, int top )
{
  _min = bottom;
  _max = top;

  if (_max < _min)
    _max = _min;
}

void KIntValidator::setBase ( int base )
{
  _base = base;
  if (_base < 2) _base = 2;
}

int KIntValidator::bottom () const
{
  return _min;
}

int KIntValidator::top () const
{
  return _max;
}

int KIntValidator::base () const
{
  return _base;
}


//  Implementation of KFloatValidator
//

class KFloatValidatorPrivate
{
public:
  KFloatValidatorPrivate()
  {
  }
  ~KFloatValidatorPrivate()
  {
  }
  bool acceptLocalizedNumbers;
};


KFloatValidator::KFloatValidator ( QWidget * parent, const char * name )
  : QValidator(parent)
{
  d = new KFloatValidatorPrivate;
  d->acceptLocalizedNumbers=false;
  _min = _max = 0;
}

KFloatValidator::KFloatValidator ( double bottom, double top, QWidget * parent, const char * name )
  : QValidator(parent)
{
  d = new KFloatValidatorPrivate;
  d->acceptLocalizedNumbers=false;
  _min = bottom;
  _max = top;
}

KFloatValidator::KFloatValidator ( double bottom, double top, bool localeAware, QWidget * parent, const char * name )
  : QValidator(parent)
{
  d = new KFloatValidatorPrivate;
  d->acceptLocalizedNumbers = localeAware;
  _min = bottom;
  _max = top;
}

KFloatValidator::~KFloatValidator ()
{
  delete d;
}

void KFloatValidator::setAcceptLocalizedNumbers(bool _b)
{
  d->acceptLocalizedNumbers=_b;
}

bool KFloatValidator::acceptLocalizedNumbers() const
{
  return d->acceptLocalizedNumbers;
}

QValidator::State KFloatValidator::validate ( QString &str, int & ) const
{
  bool    ok;
  double  val = 0;
  QString newStr;
  newStr = str.trimmed();

  if (newStr == QString::fromLatin1("-")) // a special case
    if ((_min || _max) && _min >= 0)
      ok = false;
    else
      return QValidator::Acceptable;
  else if (newStr == QString::fromLatin1(".") || (d->acceptLocalizedNumbers && newStr==MyLocale::Instance().decimalSymbol())) // another special case
    //  else if (newStr == QString::fromLatin1(".") || (d->acceptLocalizedNumbers && newStr==KGlobal::locale()->decimalSymbol())) // another special case
    return QValidator::Acceptable;
  else if (newStr.length())
    {
      val = newStr.toDouble(&ok);
      if(!ok && d->acceptLocalizedNumbers)
	assert( false ); // BAUSTELLE
	//      val= KGlobal::locale()->readNumber(newStr,&ok);
    }
  else {
    val = 0;
    ok = true;
  }

  if (! ok)
    return QValidator::Invalid;

  if (( !_min && !_max) || (val >= _min && val <= _max))
    return QValidator::Acceptable;

  if (_max && _min >= 0 && val < 0)
    return QValidator::Invalid;

  if ( (_min || _max) && (val < _min || val > _max))
    return QValidator::Invalid;

  return QValidator::Intermediate;
}

void KFloatValidator::fixup ( QString &str ) const
{
  int                dummy;
  double             val;
  QValidator::State  state;

  state = validate(str, dummy);

  if (state == QValidator::Invalid || state == QValidator::Acceptable)
    return;

  if (! _min && ! _max)
    return;

  val = str.toDouble();

  if (val < _min) val = _min;
  if (val > _max) val = _max;

  str.setNum(val);
}

void KFloatValidator::setRange ( double bottom, double top )
{
  _min = bottom;
  _max = top;

  if (_max < _min)
    _max = _min;
}

double KFloatValidator::bottom () const
{
  return _min;
}

double KFloatValidator::top () const
{
  return _max;
}




//  Implementation of KDoubleValidator
//

class KDoubleValidator::Private {
public:
  Private( bool accept=true ) : acceptLocalizedNumbers( accept ) {}

  bool acceptLocalizedNumbers;
};

KDoubleValidator::KDoubleValidator( QObject * parent, const char * name, Scale* scale )
  : QDoubleValidator( parent ), d( 0 ), m_scale( scale )
{
  d = new Private();
  _min = std::numeric_limits<double>::min();
  _max = std::numeric_limits<double>::max();
}

KDoubleValidator::KDoubleValidator( double bottom, double top, int decimals,
				    QObject * parent, const char * name )
  : QDoubleValidator( bottom, top, decimals, parent ), d( 0 ), m_scale(0)
{
  d = new Private();
  _min = bottom;
  _max = top;
}

KDoubleValidator::~KDoubleValidator()
{
  delete d;
}

bool KDoubleValidator::acceptLocalizedNumbers() const {
  return d->acceptLocalizedNumbers;
}

void KDoubleValidator::setAcceptLocalizedNumbers( bool accept ) {
  d->acceptLocalizedNumbers = accept;
}

QValidator::State KDoubleValidator::validate( QString & input, int & p ) const {
  QString s = input;
  // empty inputs are Accepted
  if (s.trimmed().size() == 0) {
    return Acceptable;
  }
  if ( acceptLocalizedNumbers() ) {
    //    are exactly three decimals between each separator):
    const MyLocale& l = MyLocale::Instance();
    QString d = l.decimalSymbol(),
      n = l.negativeSign(),
      p = l.positiveSign(),
      t = l.thousandsSeparator();

    // amg:: return Invalid if char '.' is not used(decimalSymbol, thousandsSeparator)
    //       and string has char '.'
    if ( (!d.isEmpty() && d.indexOf('.') == -1) &&
	 (t.isEmpty() || (!t.isEmpty() && t.indexOf('.') == -1) ) )
      if (s.indexOf( '.'  )  != -1)
	return Invalid;

    // amg:: return Invalid if char ',' is not used(decimalSymbol, thousandsSeparator)
    //       and string has char ','
    if ( (!d.isEmpty() && d.indexOf(',') == -1) &&
	 (t.isEmpty() || (!t.isEmpty() && t.indexOf(',') == -1) ) )
      if (s.indexOf( ','  )  != -1)
	return Invalid;

    // first, delete p's and t's:
    if ( !p.isEmpty() )
      for ( int idx = s.indexOf( p ) ; idx >= 0 ; idx = s.indexOf( p, idx ) )
	s.remove( idx, p.length() );

    if ( !t.isEmpty() )
      for ( int idx = s.indexOf( t ) ; idx >= 0 ; idx = s.indexOf( t, idx ) )
	s.remove( idx, t.length() );

    // then, replace the d's and n's
    if ( ( !n.isEmpty() && n.indexOf('.') != -1 ) ||
	 ( !d.isEmpty() && d.indexOf('-') != -1 ) ) {
      // make sure we don't replace something twice:
      std::cerr << "+++ DoubleValidator: decimal symbol contains '-' or "
	"negative sign contains '.' -> improve algorithm" << std::endl;
      return Invalid;
    }

#if QT_VERSION < 0x050000
    if ( !d.isEmpty() && d != "." )
      for ( int idx = s.indexOf( d ) ; idx >= 0 ; idx = s.indexOf( d, idx + 1 ) )
	s.replace( idx, d.length(), ".");
#endif

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
  }

  // amg:: we use of xfer Scale (get actual scale factor)
  if (m_scale &&
      (_min != std::numeric_limits<double>::min() || _max != std::numeric_limits<double>::max()) ) {
    bool    ok;
    double fac = m_scale->getValue();
    double min = fac * _min;
    double max = fac * _max;
    double value = input.trimmed().toDouble(&ok);

    if ( value >= min && value <= max) {
      return QValidator::Acceptable;
    } else
      if (value > max && value > 0.0)
	return QValidator::Invalid;
      else if (value < min && value < 0.0)
	return QValidator::Invalid;
      else
	return QValidator::Intermediate;
  }
  return base::validate( s, p );
}

void KDoubleValidator::setRange ( double bottom, double top ) {
  // xfer Scale ?
  if (m_scale) {
    _min = bottom;
    _max = top;

    if (_max < _min)
      _max = _min;
  } else {
    QDoubleValidator::setRange (bottom, top );
  }
}

//#include "knumvalidator.moc"
