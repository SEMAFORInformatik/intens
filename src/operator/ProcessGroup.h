
#if !defined(OPERATOR_PROCESSGROUP_H)
#define OPERATOR_PROCESSGROUP_H

#include <string>
#include <vector>
#include <map>

#include "streamer/Stream.h"

#include "operator/Worker.h"
#include "gui/GuiListenerController.h"
#include "gui/GuiButtonListener.h"
#include "job/JobAction.h"
#include "operator/OpProcess.h"

class GuiButton;
class GuiPulldownMenu;
class GuiInitWorker;


class ProcessGroup : public WorkerController
                   , public GuiListenerController
                   , public JobAction
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  ProcessGroup( const std::string &name="" );
  virtual ~ProcessGroup();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static void serializeAll( std::ostream &os );
  void serializeXML(std::ostream &os, bool recursive = false);
  void setUiUpdate( int interval ){ m_uiUpdateInterval = interval; }

  /// one-time initialization

  /// returns the ProcessGroup if it exists, 0 otherwise
  static ProcessGroup* find( const std::string &name );
  Process *appendProcess( const std::string &name );

  int getExitStatus()const { return m_ExitStatus; }
  void prepareStart();
  void stop();
  void start( GuiElement *parent = 0 );

  bool failed()const{ return m_failed; }

  virtual void startWorker();

  virtual JobAction::JobResult work();
  virtual void backFromWorker( JobAction::JobResult );

  virtual void printLogTitle( std::ostream &ostr ){
    ostr << "PROCESSGROUP";}
  virtual void startJobAction();
  virtual void stopJobAction();

  void setLabel( const std::string &label ) { m_label = label; }
  const std::string getLabel() { return m_label; }
  void setHelptext( const std::string &helptext ) { m_helptext = helptext; }
  const std::string getHelptext() { return m_helptext; }
  static void createProcessMenu( GuiPulldownMenu *menu );
  virtual void workCancelButtonPressed(){ assert( false ); }
/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  /** Diese Funktion soll einen neuen GuiButtonListener erstellen.
      @return Pointer auf neuen GuiButtonListener.
  */
  virtual GuiButtonListener *createButtonListener();

  virtual GuiToggleListener *createToggleListener() { return 0; }

  virtual void backFromJobController( JobResult rslt ){}

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  Process *first(){ return *(m_current=m_processes.begin()); }
  Process *next();
  Process *current(){ return m_current!=m_processes.end()?*m_current:0;}
  bool isRunning(){ return m_current!=m_processes.end()? (*m_current)->isRunning(): false; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int                    m_uiUpdateInterval;
  typedef std::map<std::string,ProcessGroup *> ProcessGroupMap;
  typedef std::vector<Process *> ProcessList;

  static ProcessGroupMap s_procgroupmap;

  std::string            m_label;
  std::string            m_helptext;
  ProcessList            m_processes;
  ProcessList::iterator  m_current;
  bool                   m_failed;

  bool                   m_stopped;

  bool                   m_inputCheckOk;
  int                    m_ExitStatus;
};

#endif
