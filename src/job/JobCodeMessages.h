
#if !defined(JOB_CODE_MESSAGES_INCLUDED_H)
#define JOB_CODE_MESSAGES_INCLUDED_H

#include "job/JobCodeExec.h"

class JobEngine;
class JobDataReference;

/// Print Message
class JobCodePrint : public JobCodeExec
{
public:
  JobCodePrint(){}
  virtual ~JobCodePrint(){}
  virtual OpStatus execute( JobEngine * );
};

/// Set Error and Print Message
class JobCodeSetErrorMsg : public JobCodeExec
{
public:
  JobCodeSetErrorMsg(){}
  virtual ~JobCodeSetErrorMsg(){}
  virtual OpStatus execute( JobEngine * );
};

/// Set Error
class JobCodeSetError : public JobCodeExec
{
public:
  JobCodeSetError(){}
  virtual ~JobCodeSetError(){}
  virtual OpStatus execute( JobEngine * );
};

/// Reset Error
class JobCodeResetError : public JobCodeExec
{
public:
  JobCodeResetError(){}
  virtual ~JobCodeResetError(){}
  virtual OpStatus execute( JobEngine * );
};

/// Log Level Message
class JobCodeLog : public JobCodeExec
{
public:
  JobCodeLog(const std::string& level) : m_level(level) {}
  virtual ~JobCodeLog(){}
  virtual OpStatus execute( JobEngine * );
 private:
  std::string m_level;
};

/// Log Message
class JobCodeLogMsg : public JobCodeExec
{
public:
  JobCodeLogMsg() {}
  virtual ~JobCodeLogMsg(){}
  virtual OpStatus execute( JobEngine * );
};

#endif
