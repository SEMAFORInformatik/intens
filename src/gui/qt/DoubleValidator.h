// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#if !defined(GUI_QT_DOUBLEVALIDATOR_INCLUDED_H)
#define GUI_QT_DOUBLEVALIDATOR_INCLUDED_H

#include <QDoubleValidator>

class QWidget;
class QString;
class Scale;

class DoubleValidator : public QDoubleValidator {
public:
  DoubleValidator( QObject * parent, Scale* scale=0 );
  DoubleValidator( double bottom, double top, int decimals,
		    QObject * parent );
  virtual ~DoubleValidator();

  virtual QValidator::State validate( QString & input, int & pos ) const;
  virtual void setRange ( double bottom, double top );
  bool acceptLocalizedNumbers() const;
  void setAcceptLocalizedNumbers( bool accept );
  static QString decimalSymbol();
  static QString thousandsSeparator();


 private:
  typedef QDoubleValidator base;
  class Private;
  Scale*    m_scale;
  double _min;
  double _max;
};

#endif
