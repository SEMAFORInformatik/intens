
#if !defined(GUI_QT_MESSAGELINE_INCLUDED_H)
#define GUI_QT_MESSAGELINE_INCLUDED_H

#include <qobject.h>

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiMessageLine.h"

class QStatusBar;

class GuiQtMessageLine : public GuiQtElement, public GuiMessageLine
{
  Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtMessageLine( GuiElement *parent )
    : GuiQtElement( parent )
    , m_statusbarwidget( 0 )
    , m_time( 0 )
    , m_delay( 0 ) {
  }
  virtual ~GuiQtMessageLine(){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Message; }
  virtual void create();
  virtual void manage();
  virtual bool destroy();
  virtual void getSize( int &x, int &y );
  virtual void update( UpdateReason );
  virtual QWidget* myWidget();
  virtual void printMessage( const std::string &, MessageType, time_t delay = 2 );
  virtual void clearMessage();
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual GuiElement *getElement() { return this; }

/*=============================================================================*/
/* private slots                                                               */
/*=============================================================================*/
  private slots:
    void slotMessageChanged(const QString &message);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QStatusBar*       m_statusbarwidget;
  time_t       m_time;
  time_t       m_delay;
};

#endif
