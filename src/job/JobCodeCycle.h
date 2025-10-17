
#if !defined(JOB_CODE_CYCLE_INCLUDED_H)
#define JOB_CODE_CYCLE_INCLUDED_H

#include "job/JobCodeExec.h"

class JobEngine;


class JobCodeCycle : public JobCodeExec
{
public:
  JobCodeCycle(){}
  virtual ~JobCodeCycle(){}
protected:
  OpStatus run( const std::string &name, JobEngine *eng );
};

class JobCodeNewCycle : public JobCodeExec
{
public:
  JobCodeNewCycle(){}
  virtual ~JobCodeNewCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeDeleteCycle : public JobCodeCycle
{
public:
  JobCodeDeleteCycle(){}
  virtual ~JobCodeDeleteCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeClearCycle : public JobCodeCycle
{
public:
  JobCodeClearCycle(){}
  virtual ~JobCodeClearCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeFirstCycle : public JobCodeCycle
{
public:
  JobCodeFirstCycle(){}
  virtual ~JobCodeFirstCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeLastCycle : public JobCodeCycle
{
public:
  JobCodeLastCycle(){}
  virtual ~JobCodeLastCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeNextCycle : public JobCodeCycle
{
public:
  JobCodeNextCycle(){}
  virtual ~JobCodeNextCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeGoCycle : public JobCodeCycle
{
public:
  JobCodeGoCycle(){}
  virtual ~JobCodeGoCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeGetCycle : public JobCodeExec
{
public:
  JobCodeGetCycle(){}
  virtual ~JobCodeGetCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeMaxCycle : public JobCodeExec
{
public:
  JobCodeMaxCycle(){}
  virtual ~JobCodeMaxCycle(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeGetCycleName : public JobCodeExec
{
public:
  JobCodeGetCycleName(){}
  virtual ~JobCodeGetCycleName(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeSetCycleName : public JobCodeExec
{
public:
  JobCodeSetCycleName(){}
  virtual ~JobCodeSetCycleName(){}
  virtual OpStatus execute( JobEngine * );
};

#endif
