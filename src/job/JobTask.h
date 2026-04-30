
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(JOB_TASK_INCLUDED_H)
#define JOB_TASK_INCLUDED_H

#include "job/JobFunction.h"
#include "gui/TimerTask.h"

class JobEngine;
class Timer;

/** Eine JobTask wird immer als selbständiger Job (TASK) mit execute() aufgerufen.

*/
class JobTask : public JobFunction
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobTask( const std::string &name );
  virtual ~JobTask();

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  virtual void printLogTitle( std::ostream &ostr );

  /** Diese Funktion gibt Auskunft, ob es sich um eine Task handelt.
      @return True: => Das Objekt ist eine Task.
  */
  virtual bool isTask() { return true; }

  /** Diese Funktion darf nicht aufgerufen werden (siehe JobFunction).
  */
  virtual void setData( XferDataItem *, JobStackData * ) { assert( false ); }

  void setLabel( const std::string &label );

  const std::string &getLabel();

  void setHelptext( const std::string &text ) { m_helptext = text; }

  const std::string &getHelptext() { return m_helptext; }

  void serializeXML(std::ostream &os, bool recursive=false);
/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  virtual void epilog( OpStatus op_status );

  /** Diese Funktion soll einen neuen GuiButtonListener erstellen.
      @return Pointer auf neuen GuiButtonListener.
  */
  virtual GuiButtonListener *createButtonListener();

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class JobTaskButtonListener : public JobActionButtonListener
  {
  public:
    JobTaskButtonListener( JobFunction *func )
      : JobActionButtonListener( func )
      , m_function( func ){}
    virtual ~JobTaskButtonListener() {}
    virtual void ButtonPressed();
  private:
    JobFunction  *m_function;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string     m_label;
  std::string     m_helptext;
  JobResult       m_result;
};

#endif
