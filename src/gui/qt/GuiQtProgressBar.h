
#if !defined(GUI_QT_PROGRESSBAR_H)
#define GUI_QT_PROGRESSBAR_H

#include "gui/qt/GuiQtDataField.h"

class MessageQueuePublisher;

class GuiQtProgressBar : public GuiQtDataField
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  GuiQtProgressBar( GuiElement *parent, std::string name="" );
  GuiQtProgressBar( const GuiQtProgressBar & );
  virtual ~GuiQtProgressBar();

private:
  GuiQtProgressBar &operator=( const GuiQtProgressBar &i );

  /*=============================================================================*/
  /* public member functions of GuiElement                                       */
  /*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_ProgressBar; }
  virtual void create();
  virtual void manage();
  virtual void unmanage();
  virtual QWidget* myWidget();

  virtual GuiElement * getElement() { return this; }
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

  virtual void update( UpdateReason );
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();
  void timerEvent (QTimerEvent * event);

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed() {}
  virtual void confirmNoButtonPressed() {}

  /*=============================================================================*/
  /* private functions                                                           */
  /*=============================================================================*/
private:
  void publishData();
  /*=============================================================================*/
  /* private members                                                             */
  /*=============================================================================*/
private:
  QProgressBar *m_progressBar;
  std::vector<GuiQtProgressBar*> m_cloned;
  MessageQueuePublisher* m_publisher;
  int                    m_timerId;
};

#endif
