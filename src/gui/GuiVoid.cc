#if HAVE_QT
#include <QApplication>
#include <QScreen>
#endif
#include <math.h>

#include "gui/GuiVoid.h"
#include "gui/qt/GuiQtForm.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* QVoid / Class                                                               */
/*=============================================================================*/
class QVoid : public QWidget
{
public:
  QVoid(): QWidget(){}
};

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
GuiVoid::GuiVoid(GuiElement* parent)
  : GuiElement(parent)
  , m_width(10)
  , m_height(10)
  , m_displayPercentWidth(0)
  , m_displayPercentHeight(0)
#if HAVE_QT
  , m_widget(0)
#endif
{}

GuiVoid::GuiVoid( const GuiVoid &v )
  : GuiElement( v )
  , m_width(v.m_width)
  , m_height(v.m_height)
  , m_displayPercentWidth(v.m_displayPercentWidth)
  , m_displayPercentHeight(v.m_displayPercentHeight)
#if HAVE_QT
  , m_widget(0)
#endif
{}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiVoid::create() {
#if HAVE_QT
  if (!m_widget) {
    QWidget* _widget = new QVoid();
    _widget->setMinimumSize(m_width, m_height);
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
#endif
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */
void* GuiVoid::myWidget() {
  return m_widget;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiVoid::update(UpdateReason reason) {
#if HAVE_QT
  if (m_widget && recalcSize()) {
    reinterpret_cast<QWidget*>(m_widget)->setMinimumSize(m_width, m_height);
  }
#endif
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiVoid::serializeXML(std::ostream &os, bool recursive) {
  os << "<intens:Void/>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiVoid::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  if(onlyUpdated){
    return false;
  }
  jsonObj["type"] = "Void";
  jsonObj["width"] = m_width;
  jsonObj["height"] = m_height;
  jsonObj["colspan"] = getColSpan();
  return false;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiVoid::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  if(onlyUpdated){
    return false;
  }
  auto element = eles->add_voids();
  element->set_allocated_base(writeProtobufProperties());
  element->set_width(m_width);
  element->set_height(m_height);
  element->set_colspan(getColSpan());
  return false;
}
#endif

/* --------------------------------------------------------------------------- */
/* setDisplayPercentWidth --                                                   */
/* --------------------------------------------------------------------------- */
void GuiVoid::setDisplayPercentWidth( int w ) {
  m_displayPercentWidth = w;
}

/* --------------------------------------------------------------------------- */
/* setDisplayPercentHeight --                                                  */
/* --------------------------------------------------------------------------- */
void GuiVoid::setDisplayPercentHeight( int h ) {
  m_displayPercentHeight = h;
}

/* --------------------------------------------------------------------------- */
/* recalcSize --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiVoid::recalcSize( ) {
#if HAVE_QT
  if (m_displayPercentHeight || m_displayPercentWidth) {
    QWidget *wi = reinterpret_cast<QWidget*>(m_widget);
    QRect drect = QGuiApplication::primaryScreen()->geometry();
    drect = getMyForm()->getElement()->getQtElement()->myWidget()->geometry();
    int w = floor(0.5 + m_displayPercentWidth/100. * drect.width());
    int h = floor(0.5 + m_displayPercentHeight/100. * drect.height());
    if (w != m_width || h != m_height) {
      m_width = w;
      m_height = h;
      BUG_DEBUG("Void New Size w["<<w<<"] h["<<h<<"]");
      return true;
    }
#endif
  }
  return false;
}
