
#if !defined(JOB_MESSAGE_INCLUDED_H)
#define JOB_MESSAGE_INCLUDED_H

#include "job/JobController.h"
#include "gui/InformationListener.h"

class JobEngine;
class GuiElement;

class JobMessage : public JobAction
                 , public InformationListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobMessage()
    : m_element( 0 ){ setLogOff(); }
  virtual ~JobMessage(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  void setElement( GuiElement *el );
  void setMessage( const std::string &msg ){ m_message = msg; }
  void setTitle( const std::string &title ){ m_title = title; }

  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction();
  virtual void stopJobAction();
  virtual void informOkButtonPressed();
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
  std::string   m_title;
};

#endif
