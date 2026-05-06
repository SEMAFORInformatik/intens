// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#if !defined(GUI_QT_INTEGERVALIDATOR_INCLUDED_H)
#define GUI_QT_INTEGERVALIDATOR_INCLUDED_H

#include <QIntValidator>

class QWidget;
class QString;
class Scale;

class IntegerValidator : public QIntValidator {

 public:
  IntegerValidator ( int bottom, int top, QObject * parent, Scale* scale);
  virtual ~IntegerValidator ();
  virtual void setRange ( int bottom, int top );

   private:
  Scale* m_scale;
};

#endif
