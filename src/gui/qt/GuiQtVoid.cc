#if HAVE_QT
#include <QApplication>
#include <QScreen>
#endif

#include "gui/qt/GuiQtVoid.h"
#include "gui/qt/GuiQtForm.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
GuiQtVoid::GuiQtVoid(GuiElement* parent)
  : GuiQtElement(parent)
  , m_widget(0)
{}

GuiQtVoid::GuiQtVoid( const GuiQtVoid &v )
  : GuiQtElement(v), GuiVoid(v)
  , m_widget(0)
{}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtVoid::create() {
  if (!m_widget) {
    QWidget* _widget = new QWidget();
    _widget->setMinimumSize(getWidth(), getHeight());
    m_widget = _widget;
    if (getQtElement())
      getQtElement()->updateWidgetProperty();

    // set class name
    if (!Class().empty()){
      _widget->setProperty("class", QString::fromStdString(Class()));
      BUG_DEBUG("Set class property to: " << Class()
                << "(Type: " << Type() << ")");
    }
  }
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */
QWidget* GuiQtVoid::myWidget() {
  return m_widget;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtVoid::update(UpdateReason reason) {
  if (m_widget && recalcSize()) {
    reinterpret_cast<QWidget*>(m_widget)->setMinimumSize(getWidth(), getHeight());
  }
}

/* --------------------------------------------------------------------------- */
/* recalcSize --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtVoid::recalcSize( ) {
  if (getDisplayPercentHeight() || getDisplayPercentWidth()) {
    QWidget *wi = reinterpret_cast<QWidget*>(m_widget);
    QRect drect = QGuiApplication::primaryScreen()->geometry();
    drect = getMyForm()->getElement()->getQtElement()->myWidget()->geometry();
    int w = floor(0.5 + getDisplayPercentWidth()/100. * drect.width());
    int h = floor(0.5 + getDisplayPercentHeight()/100. * drect.height());
    if (w != getWidth() || h != getHeight()) {
      setWidth(w);
      setHeight(h);
      BUG_DEBUG("Void New Size w["<<w<<"] h["<<h<<"]");
      return true;
    }
  }
  return false;
}
