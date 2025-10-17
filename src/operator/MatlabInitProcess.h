
#ifndef MATLAB_INIT_PROCESS_H
#define MATLAB_INIT_PROCESS_H

#include "operator/OpProcess.h"
#include "gui/InformationListener.h"
#include "utils/IntensThread.h"

/*=============================================================================*/
/* MatlabInitProcess                                                           */
/*=============================================================================*/
class MatlabInitProcess : public Process
, public InformationListener, private ThreadListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
 public:
  static MatlabInitProcess &Instance(){
    if( s_instance == 0 )
      s_instance = new MatlabInitProcess();
    return *s_instance;
  }
  virtual ~MatlabInitProcess() {}

 private:
  MatlabInitProcess();
  MatlabInitProcess(MatlabInitProcess&);
  MatlabInitProcess& operator=(const MatlabInitProcess&);

/*=============================================================================*/
/* public member functions of Process                                          */
/*=============================================================================*/
 public:
  virtual void informOkButtonPressed();

/*=============================================================================*/
/* public member functions of Process                                          */
/*=============================================================================*/
 public:
  virtual Process *clone(){ return s_instance; }
  virtual bool start();
  virtual bool stop();
  virtual bool hasTerminated();
  virtual void workFailed(){}

/*=============================================================================*/
/* public member functions of StreamDestination                                */
/*=============================================================================*/
public:
  virtual void putValues( StreamParameter &dpar
			  , const std::vector<int>&dims
			  , const std::vector<double>&vals
			  , bool isCell
			  , const std::vector<int>&inds ){}

  virtual void putValues( StreamParameter &dpar
			  , const std::vector<int>&dims
			  , const std::vector<dComplex>&vals
			  , const std::vector<int>&inds ){}

  virtual void putValues( StreamParameter &dpar
			  , const std::vector<int>&dims
			  , const std::vector<std::string>&vals ){}

  virtual void putValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldList
			  , const std::vector<int> &dims ){}

/*=============================================================================*/
/* public member functions of StreamSource                                     */
/*=============================================================================*/
public:
  virtual void getValues( DataStreamParameter &dpar ){}
  virtual void getValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldlist ){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
  void init();
  bool initialized(){ return m_initialized; }
  void setSilentMode()   { m_silentMode=true; }
  void resetSilentMode() { m_silentMode=false; }
/*=============================================================================*/
/* private ThreadListener function                                             */
/*=============================================================================*/
 private:
  virtual void startThread();

/*=============================================================================*/
/* private GuiButtonListener class                                             */
/*=============================================================================*/
private:
  class ConnectionCloseMenuButtonListener : public GuiMenuButtonListener
  {
  public:
    virtual JobAction *getAction(){ return 0; }
    virtual void ButtonPressed();
  };

/*=============================================================================*/
/* private members                                                             */
/*=============================================================================*/
  bool                      m_initialized;
  bool                      m_terminated;
  bool                      m_dialog;
  bool                      m_silentMode;
  IntensThread              m_thread;
  static MatlabInitProcess *s_instance;

public:
  ConnectionCloseMenuButtonListener  m_matlabConnClose;
};

#endif
