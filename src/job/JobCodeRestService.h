
#if !defined(JOB_CODE_RESTSERVICE_INCLUDED_H)
#define JOB_CODE_RESTSERVICE_INCLUDED_H

#include "job/JobCodeExec.h"

class JobEngine;
class Stream;

/** send a get request to a rest service
 */
class JobCodeRestServiceGet : public JobCodeExec
{
public:
  JobCodeRestServiceGet( const std::string &path,
                         XferDataItem *pathXfer,
                         Stream *pathStream,
                         Stream *filterStream,
                         Stream *responseStream)
  : m_path(path)
  , m_pathXfer(pathXfer)
  , m_pathStream(pathStream)
  , m_filterStream(filterStream)
  , m_responseStream(responseStream) {
  }
  virtual ~JobCodeRestServiceGet(){}
  virtual OpStatus execute( JobEngine * );

private:
  std::string   m_path;
  XferDataItem *m_pathXfer;
  Stream       *m_pathStream;
  Stream       *m_filterStream;
  Stream       *m_responseStream;
};

/** send a delete request to a rest service
 */
class JobCodeRestServiceDelete : public JobCodeExec
{
public:
  JobCodeRestServiceDelete( const std::string &path,
                         XferDataItem *pathXfer,
                         Stream *pathStream,
                         Stream *filterStream,
                         Stream *responseStream)
  : m_path(path)
  , m_pathXfer(pathXfer)
  , m_pathStream(pathStream)
  , m_filterStream(filterStream)
  , m_responseStream(responseStream) {
  }
  virtual ~JobCodeRestServiceDelete(){}
  virtual OpStatus execute( JobEngine * );

private:
  std::string   m_path;
  XferDataItem *m_pathXfer;
  Stream       *m_pathStream;
  Stream       *m_filterStream;
  Stream       *m_responseStream;
};

/** send a put request to a rest service
 */
class JobCodeRestServicePut : public JobCodeExec
{
public:
  JobCodeRestServicePut( const std::string &path,
                         XferDataItem *pathXfer,
                         Stream *pathStream,
                         Stream *dataStream,
                         Stream *responseStream,
                         bool setDbTimestamp )
    : m_path(path)
    , m_pathXfer(pathXfer)
    , m_pathStream(pathStream)
    , m_dataStream(dataStream)
    , m_responseStream(responseStream)
    , m_setDbTimestamp(setDbTimestamp) {
  }
  virtual ~JobCodeRestServicePut(){}
  virtual OpStatus execute( JobEngine * );

private:
  std::string   m_path;
  XferDataItem *m_pathXfer;
  Stream       *m_pathStream;
  Stream       *m_dataStream;
  Stream       *m_responseStream;
  bool          m_setDbTimestamp;
};

/** send a post request to a rest service
 */
class JobCodeRestServicePost : public JobCodeExec
{
public:
  JobCodeRestServicePost( const std::string &path,
                          XferDataItem *pathXfer,
                          Stream *pathStream,
                          Stream *filterStream,
                          Stream *dataStream,
                          Stream *responseStream,
                          bool setDbTimestamp )
    : m_path(path)
    , m_pathXfer(pathXfer)
    , m_pathStream(pathStream)
    , m_filterStream(filterStream)
    , m_dataStream(dataStream)
    , m_responseStream(responseStream)
    , m_setDbTimestamp(setDbTimestamp) {
  }
  virtual ~JobCodeRestServicePost(){}
  virtual OpStatus execute( JobEngine * );

private:
  std::string   m_path;
  XferDataItem *m_pathXfer;
  Stream       *m_pathStream;
  Stream       *m_filterStream;
  Stream       *m_dataStream;
  Stream       *m_responseStream;
  bool          m_setDbTimestamp;
};

/** logon to rest service
 */
class JobCodeRestServiceLogon : public JobCodeExec
{
public:
  JobCodeRestServiceLogon( XferDataItem *baseUrl,
                           XferDataItem *username,
                           XferDataItem *password,
                           XferDataItem *message,
                           Stream *responseStream)
    : m_baseUrl(baseUrl)
    , m_username(username)
    , m_password(password)
    , m_message(message)
    , m_responseStream(responseStream) {
  }
  virtual ~JobCodeRestServiceLogon(){}
  virtual OpStatus execute( JobEngine * );

private:
  XferDataItem *m_baseUrl;
  XferDataItem *m_username;
  XferDataItem *m_password;
  XferDataItem *m_message;
  Stream       *m_responseStream;
};

/** logon to rest service jwt
 */
class JobCodeRestServiceJwtLogon : public JobCodeExec
{
public:
  JobCodeRestServiceJwtLogon(XferDataItem *baseUrl,
			     XferDataItem *jwt,
			     XferDataItem *message,
			     Stream *responseStream)
    : m_baseUrl(baseUrl)
    , m_jwt(jwt)
    , m_message(message)
    , m_responseStream(responseStream) {
  }
  virtual ~JobCodeRestServiceJwtLogon(){}
  virtual OpStatus execute( JobEngine * );

private:
  XferDataItem *m_baseUrl;
  XferDataItem *m_jwt;
  XferDataItem *m_message;
  Stream       *m_responseStream;
};

/** logoff to rest service
 */
class JobCodeRestServiceLogoff : public JobCodeExec
{
public:
  JobCodeRestServiceLogoff() {}
  virtual ~JobCodeRestServiceLogoff(){}
  virtual OpStatus execute( JobEngine * );
};
#endif
