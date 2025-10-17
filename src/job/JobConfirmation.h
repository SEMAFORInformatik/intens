
#if !defined(JOB_CONFIRMATION_INCLUDED_H)
#define JOB_CONFIRMATION_INCLUDED_H

#include "job/JobController.h"
#include "gui/ConfirmationListener.h"

class GuiElement;

class JobConfirmation : public JobAction
                      , public ConfirmationListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobConfirmation()
    : m_element( 0 ), m_cancelBtn(false) { setLogOff(); }
  virtual ~JobConfirmation(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  void setElement( GuiElement *el ) { m_element = el; }
  void setMessage( const std::string &msg ){ m_message = msg; }
  void addCancelButton(bool b) { m_cancelBtn = b; }
  void setButtonText(const std::map<GuiElement::ButtonType, std::string>& buttonText) { m_buttonText = buttonText; }

  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction();
  virtual void stopJobAction();
  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed();
  virtual void confirmCancelButtonPressed();
  virtual void serializeXML(std::ostream &os, bool recursive = false){}
/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  virtual void backFromJobController( JobResult rslt ){}

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiElement   *m_element;
  std::string   m_message;
  bool          m_cancelBtn;
  std::map<GuiElement::ButtonType, std::string> m_buttonText;
};

#endif
