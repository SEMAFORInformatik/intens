
#if !defined(GUI_QT_PROGRESSBAR_H)
#define GUI_QT_PROGRESSBAR_H

#include "gui/DialogProgressBar.h"
#include "gui/qt/GuiQtElement.h"

class GuiQtProgressBar : public GuiQtElement, public DialogProgressBar
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  GuiQtProgressBar( GuiElement *parent, const std::string &name );
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

  virtual void update( UpdateReason );
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::Progressbar* element, bool onlyUpdated = false);
#endif
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();

/*=============================================================================*/
/* public DialogProgressBar functions                                          */
/*=============================================================================*/
  virtual GuiElement* getGuiElement() { return this; }
  virtual void execute_abort(bool callAbortedFunc=true) {}

  /*=============================================================================*/
  /* private members                                                             */
  /*=============================================================================*/
private:
  QProgressBar *m_progressBar;
  std::vector<GuiQtProgressBar*> m_cloned;
};

#endif
