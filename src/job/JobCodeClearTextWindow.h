
#if !defined(JOB_CODE_CLEAR_TEXT_WINDOW_H)
#define JOB_CODE_CLEAR_TEXT_WINDOW_H

#include "job/JobCodeExec.h"

class JobEngine;
class GuiScrolledText;

///
class JobCodeClearTextWindow : public JobCodeExec
{
public:
  JobCodeClearTextWindow( GuiScrolledText *tw): _tw(tw){}
  virtual ~JobCodeClearTextWindow(){}
  /** Die Funktion löscht das TextWindow
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiScrolledText *_tw;
};


#endif
