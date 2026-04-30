
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(JOB_STACK_PROCESS_INCLUDED_H)
#define JOB_STACK_PROCESS_INCLUDED_H

#include "job/JobStackItem.h"

class WorkerListener;

class JobStackProcess : public JobStackItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackProcess( WorkerListener *worker )
    : m_worker( worker ){
  }
  virtual ~JobStackProcess(){}

/*=============================================================================*/
/* public Interfaces                                                           */
/*=============================================================================*/
public:
  virtual StackType Type() { return stck_Process; }
  WorkerListener *getProcess() { return m_worker; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  WorkerListener  *m_worker;

};

#endif