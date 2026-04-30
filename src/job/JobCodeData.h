
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#if !defined(JOB_CODE_DATA_INCLUDED_H)
#define JOB_CODE_DATA_INCLUDED_H

#include "job/JobCodeItem.h"

class JobEngine;

class JobCodeData : public JobCodeItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobCodeData(){}
  virtual ~JobCodeData(){}

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:

/*=============================================================================*/
/* public Interfaces                                                           */
/*=============================================================================*/
public:
  virtual OpStatus execute( JobEngine * ) { return op_FatalError; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

};

#endif