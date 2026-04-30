
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include <qvalidator.h>
#include <qdatetime.h>

#include "gui/GuiElement.h"

/**
* Validates user-entered dates.
*/
class DateValidator : public QRegularExpressionValidator
{
public:
    DateValidator(GuiElement* elem, QWidget* parent=0, const char* name=0);
    virtual State  validate(QString&, int&) const;
    virtual void   fixup ( QString & input ) const;
    State          date(const QString&, QDate&) const;

private:
  QString     m_format;
  QString     m_regExString;
  GuiElement *m_elem;
};

#endif