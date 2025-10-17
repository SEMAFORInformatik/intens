/**********************************************************************
 **
 ** $Id: KNumValidator.h,v 1.3 2005/02/01 09:43:15 amg Exp $
 **
 ** Copyright (C) 1999 Glen Parker <glenebob@nwlink.com>
 ** Copyright (C) 2002 Marc Mutz <mutz@kde.org>
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
 
#ifndef __KNUMVALIDATOR_H
#define __KNUMVALIDATOR_H
 
#include <qvalidator.h>
 
class QWidget;
class QString;
class Scale;
 
class KIntValidator : public QValidator {

 public:
  KIntValidator ( QWidget * parent, int base = 10, const char * name = 0 );
  KIntValidator ( int bottom, int top, QWidget * parent, int base = 10, const char * name = 0 );
  virtual ~KIntValidator ();
  virtual State validate ( QString &, int & ) const;
  virtual void fixup ( QString & ) const;
  virtual void setRange ( int bottom, int top );
     virtual void setBase ( int base );
     virtual int bottom () const;
     virtual int top () const;
     virtual int base () const;
 
  static QString decimalSymbol();
  static QString thousandsSeparator();

   private:
     int _base;
     int _min;
     int _max;
 
 };
 
class KFloatValidatorPrivate;

class KFloatValidator : public QValidator {
  
 public:
  KFloatValidator ( QWidget * parent, const char * name = 0 );
  KFloatValidator ( double bottom, double top, QWidget * parent, const char * name = 0 );
  KFloatValidator ( double bottom, double top, bool localeAware, QWidget * parent, const char * name = 0 );
  virtual ~KFloatValidator ();
  virtual State validate ( QString &, int & ) const;
  virtual void fixup ( QString & ) const;
  virtual void setRange ( double bottom, double top );
  virtual double bottom () const;
  virtual double top () const;
  void setAcceptLocalizedNumbers(bool b);
  bool acceptLocalizedNumbers() const;
 private:
  double _min;
  double _max;
 
  KFloatValidatorPrivate *d;
 };

class KDoubleValidator : public QDoubleValidator {
  /*   Q_OBJECT */
  Q_PROPERTY( bool acceptLocalizedNumbers READ acceptLocalizedNumbers WRITE setAcceptLocalizedNumbers )
 public:
  KDoubleValidator( QObject * parent, const char * name=0, Scale* scale=0 );
  KDoubleValidator( double bottom, double top, int decimals,
		    QObject * parent, const char * name=0 );
  virtual ~KDoubleValidator();
 
  virtual QValidator::State validate( QString & input, int & pos ) const;
  
  virtual void setRange ( double bottom, double top );
  bool acceptLocalizedNumbers() const;
  void setAcceptLocalizedNumbers( bool accept );

 private:
  typedef QDoubleValidator base;
  class Private;
  Private * d;
  Scale*    m_scale;
  double _min;
  double _max;
};

#endif
