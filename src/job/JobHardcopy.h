
#if !defined(JOB_MESSAGE_INCLUDED_H)
#define JOB_MESSAGE_INCLUDED_H

#include "job/JobController.h"
#include "app/ReportGen.h"
#include "gui/GuiPrinterDialog.h"

class JobEngine;
class GuiElement;

class JobHardcopy : public JobAction
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobHardcopy();
  virtual ~JobHardcopy();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  void setElement( GuiElement *el );
  void setHardCopyListener( HardCopyListener *hcl );
  void setMode( ReportGen::Mode mode );

  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction();
  virtual void stopJobAction();
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
  HardCopyListener              *m_hcl;
  GuiElement                    *m_element;
  GuiPrinterDialog::MyEventData *m_event;
};

#endif
