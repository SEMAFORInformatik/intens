
#include <QApplication>
#include <QMenu>
#include <QKeyEvent>
#include <QWheelEvent>
#include "utils/gettext.h"
#include "app/AppData.h"

#include "gui/GuiElement.h"
#include "gui/qt/GuiQtDialog.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/ArrowKeyLineEdit.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/KNumValidator.h"

bool ArrowKeyLineEdit::event(QEvent* e) {
  if (e->type() == QEvent::ShortcutOverride) {
    if (AppData::Instance().Undo())
      return true;
  }
  return QLineEdit::event(e);
}

/* --------------------------------------------------------------------------- */
/* ArrowKeyLineEdit                                                                 */
/* --------------------------------------------------------------------------- */
void ArrowKeyLineEdit::enterEvent ( QEnterEvent *e )
{
  if( m_textfield ) m_textfield->enterEvent( e );
  if(isEnabled())
    QLineEdit::enterEvent( e );
}

void ArrowKeyLineEdit::leaveEvent ( QEvent *e )
{
  if( m_textfield &&  m_textfield->hasHelptext())
    m_textfield->clearMessage();
  QLineEdit::leaveEvent( e );
}

/* --------------------------------------------------------------------------- */
/* ArrowKeyLineEdit::createPopupMenu                                                */
/* --------------------------------------------------------------------------- */
QMenu * ArrowKeyLineEdit::createPopupMenu (){
  QMenu *pm = QLineEdit::createStandardContextMenu();
  // set font
  QFont font = pm->font();
  pm->setFont( QtMultiFontString::getQFont( "@popup@", font ) );
  return pm;
}

/* --------------------------------------------------------------------------- */
/* ArrowKeyLineEdit::keyReleaseEvent                                           */
/* --------------------------------------------------------------------------- */
void ArrowKeyLineEdit::keyReleaseEvent(QKeyEvent* e) {
  if ( (e->key() != Qt::Key_Up && e->key() != Qt::Key_Down) ||
       m_saveInDataPool || e->isAutoRepeat() ) {
    QLineEdit::keyReleaseEvent(e);
    return;
  }
  m_saveInDataPool = false;
  if( m_textfield )
    m_textfield->FinalWork( text().toStdString() );
  else {
    if( m_listener ){
      m_listener -> valueChanged();
    }
  }
  QLineEdit::keyReleaseEvent(e);
}

/* --------------------------------------------------------------------------- */
/* ArrowKeyLineEdit::keyPressEvent                                             */
/* --------------------------------------------------------------------------- */
void ArrowKeyLineEdit::keyPressEvent(QKeyEvent* e) {
  m_saveInDataPool = false;  // new value is saved in datapool in keyReleaseEvent()
  if (m_textfield && !m_textfield->isEditable()) {
    e->ignore();
    return;
  }
  switch (e->key()) {
  case Qt::Key_Up:
    {
      if (m_textfield) {
        if (m_textfield->getLength() < text().size())
          break;
      }
      if ( processCursorStep(true) )
        e->accept();
      else
        e->ignore();
      return;
    }
  case Qt::Key_Down:
    {
      if (m_textfield) {
        if (m_textfield->getLength() < text().size())
          break;
      }
      if ( processCursorStep(false) )
        e->accept();
      else
        e->ignore();
      return;
    }
  case Qt::Key_Enter:
  case Qt::Key_Return:
    if( m_textfield ) {
      if (!makeValidatorCheck())
        return;
      e->accept();

      if (m_textfield) m_textfield->lostFocus();
      selectAll();
      // ignore a second lostFocus call through emit editingFinished signal
      // e.g when showing a consistency check dilaog
      if (m_textfield) m_textfield->m_textChanged = false;
      return;
    } else
      if( m_listener )
        m_listener -> valueChanged();
    selectAll();
    e->accept();
    return;
  }

  QLineEdit::keyPressEvent(e);
}

/* --------------------------------------------------------------------------- */
/* ArrowKeyLineEdit::wheelEvent                                                */
/* --------------------------------------------------------------------------- */
void ArrowKeyLineEdit::wheelEvent(QWheelEvent* e) {
  if (m_textfield && !m_textfield->isEditable() ||
      (!m_listener && m_textfield->Attr() && !m_textfield->Attr()->hasWheelEvent()) &&
      !AppData::Instance().GuiWheelEvent()) {
    e->ignore();
    return;
  }

  if (e->angleDelta().y() != 0) {
    int numStep = e->angleDelta().y() / 120;
    if (numStep > 0) {
      m_saveInDataPool = true;
      while(--numStep >= 0)
        processCursorStep(true);
    } else if (numStep < 0) {
      m_saveInDataPool = true;
      while(++numStep <= 0)
        processCursorStep(false);
    }
  }
  e->accept();
  setFocus();
  return;
}

bool ArrowKeyLineEdit::makeValidatorCheck() {
  if (validator()) {
    int p=0;
    QString s = text();
    std::string sOld;
    if (m_textfield) m_textfield->m_param->getFormattedValue(sOld);
    QValidator::State ret = validator()->validate(s, p);
    if (s.size() == 0 || ret == QValidator::Acceptable) {
      return true;
    } else {
      setText( QString::fromStdString(sOld).trimmed() );
      if (m_textfield) m_textfield->printMessage( _("Conversion error."),
                                                  GuiElement::msg_Warning );
      return false;
    }
  }
  return true;
}
/* --------------------------------------------------------------------------- *
 * ArrowKeyLineEdit::focusInEvent                                              *
 * --------------------------------------------------------------------------- */
void ArrowKeyLineEdit::focusInEvent ( QFocusEvent *e) {
  m_textfield->startFocusFunction(true);
  QLineEdit::focusInEvent(e);
}
/* --------------------------------------------------------------------------- *
 * ArrowKeyLineEdit::focusOutEvent                                             *
 * --------------------------------------------------------------------------- */
void ArrowKeyLineEdit::focusOutEvent ( QFocusEvent *e) {
  // da das signal editingFinished nur ausgeloest wird, wenn die Validator-Methode
  // QValidator::Acceptable zurueckgibt, machen wir den fehlenden slot(lostFocus) selber
  // und wenn nicht, setzen wir den alten Wert wieder (Conversion error.)
  if( m_textfield ) {
    if (makeValidatorCheck())
        m_textfield->startFocusFunction(false);
  }
  QLineEdit::focusOutEvent(e);
}

/* --------------------------------------------------------------------------- *
 * ArrowKeyLineEdit::processCursorStep                                         *
 * --------------------------------------------------------------------------- */
bool ArrowKeyLineEdit::processCursorStep(bool up) {
  if ( !AppData::Instance().ArrowKeys() ) {
    return false;
  }
  if (text().isEmpty()) return false;
  if ( !m_textfield->isArrowKeyAllowed() ) {
    std::cerr << "WARNING::  Arrow Key are disabled!!!\n"<<std::flush;
    return false;
  }
  DataDictionary::DataType data_type = DataDictionary::type_Real;
  if( m_textfield )
    data_type = m_textfield->getDataType();

  if (data_type == DataDictionary::type_Real ||
      data_type == DataDictionary::type_Integer ) {
    long double diff_value;
    QString decPt( KIntValidator::decimalSymbol() );
    QString tsep( KIntValidator::thousandsSeparator() );
    QString qstr( text() );
    int tsepCnt = qstr.count(tsep);
    int tsepDiff = 2*tsepCnt;
    qstr.remove(tsep);

    if (qstr.contains('e') || qstr.contains('E')) {
      if( m_textfield )
        m_textfield->printMessage( _("Ignore Arrow Key at Exponential View."),
                                   GuiElement::msg_Warning );
      return false;
    }
    if ( tsepCnt ) {
      if( m_textfield )
        m_textfield->printMessage( _("Ignore Arrow Key at Thousand Separator View."),
                                   GuiElement::msg_Warning );
      return false;
    }

    int cur_pos = cursorPosition();
    if (cur_pos <=0) cur_pos = 0;

    // vielleicht eine Null vorher einfügen um abs(Num) zu erhoehen
    if ((cur_pos == 0  && up && qstr[0] != '-') ||
        (qstr.length() > 1 && cur_pos == 1  && !up && qstr[0] == '-')) {
      if (up)
        qstr.insert(0, '0');
      else
        qstr.insert(1, '0');
      ++cur_pos;
    } else
      if (cur_pos == 0 || (cur_pos < 2 && qstr[0] == '-') ) {
        return true;
      }
    int ppos =  qstr.indexOf(decPt);
    if (ppos < 0 ) ppos=qstr.length();

    // detect tsep => ...
    if (tsepCnt && ppos+tsepCnt >= cur_pos) {
      int di = (ppos+tsepCnt - cur_pos) /3;
      tsepDiff = tsepCnt - di;
      if ( (ppos - cur_pos)%3 == 2 ) { //TSep Position => return
        if( m_textfield )
          m_textfield->printMessage( _("Ignore Arrow Key at Thousand Separator Char."),
                                     GuiElement::msg_Warning );
        return false;
      }
    }

    if ((cur_pos-ppos) ==1) {
      if( m_textfield )
        m_textfield->printMessage( _("Ignore Arrow Key at Decimal Point."),
                                   GuiElement::msg_Warning );
      return false; // Position DecPoint ==> Return
    }

    // get diff value
    if (ppos>=cur_pos) {
      diff_value = pow(10, ppos-cur_pos);
    } else {
      diff_value = pow(10, 1+ppos-cur_pos);
    }

    double dbl_value;
    long lint_value;
    if (data_type == DataDictionary::type_Real) {

      if ((cur_pos-ppos) >1) {
        if( m_textfield && m_textfield->getPrecision() == -1) {

          if ( ( up && qstr.toStdString()[cur_pos-1] == '9' && cur_pos == qstr.length() ) ||
               (!up && qstr.toStdString()[cur_pos-1] == '1' && cur_pos == qstr.length() )   ) {
            m_textfield->printMessage( _("Restricted support for Arrow Keys if data format is not fix."),
                                       GuiElement::msg_Warning );
            return false;
          }
        }
      }

      if (  m_textfield &&
            m_textfield->getPrecision() > 0 && (cur_pos-ppos-1) >  m_textfield->getPrecision() ) {
        m_textfield->printMessage( _("Restricted support for Arrow Keys if data format is fix."),
                                   GuiElement::msg_Warning );
        return false;
      }
      if (decPt.compare(".") != 0) {  // makes Qt 3 happy (in future could be shit => QLocale)
        qstr.replace(decPt, ".");
      }
      dbl_value = qstr.toDouble();
      if (up)
        dbl_value +=  diff_value;
      else
        dbl_value -=  diff_value;
      int prec =  ((qstr.length()-ppos) > 0) ? qstr.length()-ppos-1 : 0;
      qstr.setNum(dbl_value, 'f', prec );
      if (decPt.compare(".") != 0) {  // makes Qt 3 happy (in future could be shit => QLocale)
        qstr.replace(".", decPt);
      }
    } else {
      lint_value = qstr.toLong();
      long diff_lval = (long) floor(diff_value+0.5);
      if (up) {
        long save_long = lint_value;
        lint_value +=  diff_lval;
        if (save_long > lint_value) {
          if( m_textfield )
            m_textfield->printMessage( _("Conversion error."),
                                       GuiElement::msg_Warning );
          return false;
        }
      } else {
        long save_long = lint_value;
        lint_value -=  diff_lval;
        if (save_long < lint_value) {
          if( m_textfield )
            m_textfield->printMessage( _("Conversion error."),
                                       GuiElement::msg_Warning );
          return false;
        }
      }
      qstr.setNum(lint_value);
    }
    XferParameter::InputStatus status = XferParameter::status_Changed;
    if( m_textfield )
      status = m_textfield->checkFormat( qstr.toStdString() );
    else {
      double x;
      std::istringstream input( qstr.toStdString() );
      if( m_converter -> readAll( input, x ) ){
        std::ostringstream output;
        m_converter -> write( x, output );
        qstr = QString::fromStdString(output.str()).trimmed();
      }
      else {
        status = XferParameter::status_Bad;
      }

    }
    if ( status != XferParameter::status_Bad) {
      if( m_textfield ) {
        if (m_textfield->getLength() < qstr.size())
          return false;
      }
      int p=0;
      // validator check
      if (validator()) {
	QValidator::State ret = validator()->validate(qstr, p);
	if (ret != QValidator::Acceptable) {
	  return false;
	}
      }
      setText( qstr );
      if (m_saveInDataPool) {
        if( m_textfield )
          m_textfield->FinalWork( qstr.toStdString() );
        else {
          if( m_listener ){
            m_listener -> valueChanged();
          }
        }
      }
      int diff = cur_pos - ppos;
      int curPosNew =   text().indexOf(decPt);
      if (curPosNew<0) // imagine decPt after the end
        curPosNew = text().length();
      curPosNew += diff;
      if (curPosNew<0) curPosNew=0;
      if (curPosNew > text().length())
        curPosNew = text().length();
      if (text().length() && curPosNew<=0 &&  text()[0] == '-') curPosNew=1;
      if (tsepCnt) curPosNew -= (tsepCnt-tsepDiff);
      if( m_textfield )
        m_textfield -> setRelativeCursorPos(diff);
      setCursorPosition(curPosNew);
      return true;
    } else {
      if( m_textfield )
        m_textfield->printMessage( _("Conversion error."),
                                   GuiElement::msg_Warning );
    }
  }
  return false;
}
