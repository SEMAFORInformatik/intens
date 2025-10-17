#include "gui/qt/GuiQtManager.h"
#include <QSettings>

#include "job/JobIncludes.h"

#include "job/JobCodeRestService.h"
#include "job/JobEngine.h"
#include "xfer/XferDataItem.h"
#include "streamer/Stream.h"

#include "operator/RestService.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "utils/gettext.h"
#include "utils/Date.h"
#include "utils/base64.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* JobCodeRestServiceGet::execute --                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRestServiceGet::execute( JobEngine* eng ){
  BUG(BugJobCode,"JobCodeRestServiceGet::execute");

  if ( RestService::Instance().checkCredentials() ) {
    QTime startTime(QTime::currentTime());
    if ( m_pathXfer ) {
      m_pathXfer->getValue( m_path );
    }
    else if ( m_pathStream ) {
      std::ostringstream os;
      m_pathStream->write( os );
      m_path = os.str();
    }
    RestService::Instance().setPath( m_path );

    if ( m_filterStream ) {
      std::ostringstream os;
      m_filterStream->write( os );
      RestService::Instance().addFilter( os.str() );
    }

    RestService::Instance().setResponseStream( m_responseStream );
    RestService::Instance().setCallingAction(eng->getFunction()->getBaseCallingAction());

    JobElement::OpStatus ret = RestService::Instance().get();
    eng->pushTrue( ret == op_Ok );
    BUG_INFO("End of RestServiceGet "
             << Date::durationAsString(startTime.msecsTo(QTime::currentTime())));
    return ret;
  }

  std::ostringstream logmsg;
  logmsg << DateAndTime() << ": "
         << RestService::Instance().getErrorMessage()
         << std::endl;
  GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  eng->pushTrue( false );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* JobCodeRestServiceDelete::execute --                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRestServiceDelete::execute( JobEngine* eng ){
  BUG(BugJobCode,"JobCodeRestServiceDelete::execute");

  if ( RestService::Instance().checkCredentials() ) {
    if ( m_pathXfer ) {
      m_pathXfer->getValue( m_path );
    }
    else if ( m_pathStream ) {
      std::ostringstream os;
      m_pathStream->write( os );
      m_path = os.str();
    }
    RestService::Instance().setPath( m_path );

    if ( m_filterStream ) {
      std::ostringstream os;
      m_filterStream->write( os );
      RestService::Instance().addFilter( os.str() );
    }

    RestService::Instance().setResponseStream( m_responseStream );
    RestService::Instance().setCallingAction(eng->getFunction()->getBaseCallingAction());

    JobElement::OpStatus ret = RestService::Instance().httpdelete();
    eng->pushTrue( ret == op_Ok );
    return ret;
  }

  eng->pushTrue( false );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* JobCodeRestServicePut::execute --                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRestServicePut::execute( JobEngine* eng ){
  BUG(BugJobCode,"JobCodeRestServicePut::execute");

  if ( RestService::Instance().checkCredentials() ) {
    if ( m_pathXfer ) {
      m_pathXfer->getValue( m_path );
    }
    else if ( m_pathStream ) {
      std::ostringstream os;
      m_pathStream->write( os );
      m_path = os.str();
    }
    RestService::Instance().setPath( m_path );

    RestService::Instance().setDataStream( m_dataStream );
    RestService::Instance().setResponseStream( m_responseStream );
    RestService::Instance().setCallingAction(eng->getFunction()->getBaseCallingAction());
    RestService::Instance().setSetDbTimestamp( m_setDbTimestamp );

    JobElement::OpStatus ret = RestService::Instance().put();
    eng->pushTrue( ret == op_Ok );
    return ret;
  }

  std::ostringstream logmsg;
  logmsg << DateAndTime() << ": "
         << RestService::Instance().getErrorMessage()
         << std::endl;
  GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  eng->pushTrue( false );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* JobCodeRestServicePost::execute --                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRestServicePost::execute( JobEngine* eng ){
  BUG(BugJobCode,"JobCodeRestServicePost::execute");

  if ( RestService::Instance().checkCredentials() ) {
    if ( m_pathXfer ) {
      m_pathXfer->getValue( m_path );
    }
    else if ( m_pathStream ) {
      std::ostringstream os;
      m_pathStream->write( os );
      m_path = os.str();
    }
    RestService::Instance().setPath( m_path );

    if ( m_filterStream ) {
      std::ostringstream os;
      m_filterStream->write( os );
      RestService::Instance().addFilter( os.str() );
    }

    RestService::Instance().setDataStream( m_dataStream );
    RestService::Instance().setResponseStream( m_responseStream );
    RestService::Instance().setCallingAction(eng->getFunction()->getBaseCallingAction());
    RestService::Instance().setSetDbTimestamp( m_setDbTimestamp );

    JobElement::OpStatus ret = RestService::Instance().post();
    eng->pushTrue( ret == op_Ok );
    return ret;
  }

  std::ostringstream logmsg;
  logmsg << DateAndTime() << ": "
         << RestService::Instance().getErrorMessage()
         << std::endl;
  GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  eng->pushTrue( false );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* JobCodeRestServiceLogon::execute --                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRestServiceLogon::execute( JobEngine* eng ){
  BUG(BugJobCode,"JobCodeRestServiceLogon::execute");
  RestService& restService = RestService::Instance();

  std::string baseUrl, username, password;
  if ( m_baseUrl ) {
    m_baseUrl->getValue ( baseUrl );
  }

  if ( m_username ) {
    m_username->getValue( username );
  }

  if ( m_password ) {
    m_password->getValue( password );
  }

  // Autologin: empty login, read setting data
  std::string restuser;
  if (restService.getAuthorization() == "jwt" &&
      baseUrl.empty() && username.empty() && password.empty()) {
    const char *base = getenv("REST_SERVICE_BASE");
    baseUrl = GuiQtManager::Settings()->value("Authentification/url", base).toString().toStdString();
    password = GuiQtManager::Settings()->value("Authentification/token", "").toString().toStdString();
    restuser = GuiQtManager::Settings()->value("Authentification/user", "").toString().toStdString();
    if (!password.empty()) {
      username = "__token__";
    }
    if (restuser.empty()) {
      restuser = GuiQtManager::Settings()->value("Resource/RESTUSERNAME", "").toString().toStdString();
    }

    // decode password
    size_t outsize = password.size();
    char *output = new char[outsize];
    if( base64decode(password, reinterpret_cast< unsigned char *>( output ), outsize ) ){
      password = std::string(output, outsize);
      trim(password);
    }
    delete[] output;
    BUG_INFO("Read Authentification from Settins, urL: " << baseUrl);
    m_baseUrl->setValue( baseUrl );
    m_username->setValue( username );
    m_password->setValue( password );
  }

  RestService::Instance().setResponseStream( m_responseStream );
  RestService::Instance().setCallingAction(eng->getFunction()->getBaseCallingAction());

  JobElement::OpStatus ret = restService.login( baseUrl, username, password );
  if (restService.getUsername().empty() && !restuser.empty()) {
    restService.setUsername(restuser);   // RESTUSERNAME
  }
  eng->pushTrue( ret == op_Ok );
  if ( m_message ) {
    m_message->setValue(RestService::Instance().getErrorMessage());
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* JobCodeRestServiceJwtLogon::execute --                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRestServiceJwtLogon::execute( JobEngine* eng ){
  BUG_DEBUG("JobCodeRestServiceJwtLogon::execute");

  std::string baseUrl, jwt;
  if ( m_baseUrl ) {
    m_baseUrl->getValue ( baseUrl );
  }

  if ( m_jwt ) {
    m_jwt->getValue( jwt );
  }

  RestService::Instance().setResponseStream( m_responseStream );
  RestService::Instance().setCallingAction(eng->getFunction()->getBaseCallingAction());

  JobElement::OpStatus ret = RestService::Instance().login( baseUrl, jwt );
  eng->pushTrue( ret == op_Ok );
  if ( m_message ) {
    m_message->setValue(RestService::Instance().getErrorMessage());
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* JobCodeRestServiceLogoff::execute --                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRestServiceLogoff::execute( JobEngine* eng ){
  BUG(BugJobCode,"JobCodeRestServiceLogoff::execute");

  RestService::Instance().logoff();
  eng->pushTrue( true );
  return op_Ok;
}
