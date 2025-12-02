#include <iostream>
#include <sstream>
#include <qmessagebox.h>
#include <qsettings.h>
#include <QAbstractButton>
#include <QLocale>
#include <QTimeZone>
#include <cstdlib> // needed for setenv

#include "jwt-cpp/jwt.h"
#include "operator/RestService.h"
#include "utils/base64.h"
#include "utils/StringUtils.h"
#include "streamer/Stream.h"
#include "streamer/JSONStreamParameter.h"
#include "gui/DialogUserPassword.h"
#include "gui/GuiFactory.h"
// TODO: Build dialog in dedicated class and make api independent from Qt
//#include "gui/qt/QtDialogInformation.h"
// #include "gui/qt/QtDialogInputExt.h"
#include "gui/qt/GuiQtManager.h"

#include "utils/gettext.h"
#include "utils/Date.h"
#include "utils/utils.h"
#include "utils/UrlConverter.h"
#include "utils/StringUtils.h"
#include "gui/GuiScrolledText.h"
#include "app/AppData.h"
#include "datapool/DataVector.h"
#include "datapool/DataReference.h"
#include "job/JobManager.h"

#if defined(__MINGW32__) || defined(__MINGW64__)
#include <windows.h>
#define sleep(time) Sleep(1000*time)
#endif  // __MINGW32__ || __MINGW64__

INIT_LOGGER();

bool RestService::s_userReload = false;
int  RestService::s_initialRetry = 15;

/*=============================================================================*/
/* UserPasswordDialogAdapter                                                   */
/*=============================================================================*/

class RestServiceUserPasswordListener : public UserPasswordListener {
private:
  //==============
  // Trigger
  //==============
  class Trigger : public JobStarter
  {
  public:
    Trigger(JobFunction *f)
      : JobStarter( f ){
    }
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult jobResult ) {}
  };
public:
  RestServiceUserPasswordListener( RestService &restService )
    : m_restService( restService ){
  }
  virtual ~RestServiceUserPasswordListener(){
  }

  virtual bool okButtonPressed( const std::string &baseUrl,
                                const std::string &username,
                                const std::string &password ){
    bool ok = m_restService.login(baseUrl, username, password) == JobElement::op_Ok;
    if(ok){
      // DESKTOP mode > run AFTER_DB_LOGON
      JobFunction *function = JobManager::Instance().getFunction("AFTER_DB_LOGON");
      if( function != 0 ){
        JobStarter *trigger = new Trigger(function);
        trigger->setName("RestService-Trigger-AFTER_DB_LOGON");
        trigger->setReason(JobElement::cll_Function);
        trigger->startJob(true);
      }
    }
    return ok;
  }

  virtual void cancelButtonPressed(){
    m_restService.loginCanceled();
  }

private:
  RestService &m_restService;
};



/* initialize */
RestService *RestService::s_instance = 0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

RestService::RestService()
  : m_loginCanceled(false)
  , m_envPasswordUsed(false)
  , m_msecsCurlPerform(0)
  , m_msecsJsonParse(0)
  , m_dataStream(0)
  , m_responseStream(0)
  , m_setDbTimestamp(false)
  , m_webApiCallingAction(0) {
  BUG_DEBUG("Constructor of RestService");

  m_curlHandle = curl_easy_init();

  // set curl headers, without Authorization
  clearAuthHeader();

  char *skipPeriod = getenv("REST_SERVICE_SKIP_PERIOD");
  if ( skipPeriod ) { // REST_SERVICE_SKIP_PERIOD is not empty
    m_skipPeriod = atoi(skipPeriod);
  } else {
    m_skipPeriod = 600;
  }
  BUG_INFO("skipPeriod = " << m_skipPeriod);

  // authorization
  m_authorization = GuiQtManager::Settings()->value
    ( "Intens/Authorization", "jwt").toString().toStdString();
  BUG_INFO("authorization = " << m_authorization);
}

RestService::~RestService() {
  if(m_curlHandle) {
    curl_easy_cleanup(m_curlHandle);
  }
}

/*=============================================================================*/
/* Singleton                                                                   */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

RestService &RestService::Instance(){
  if( s_instance == 0 ){
    s_instance = new RestService();
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* Destroy --                                                                  */
/* --------------------------------------------------------------------------- */

void RestService::Destroy(){
  if( s_instance != 0 ){
    delete s_instance;
    s_instance = 0;
  }
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* checkCredentials --                                                         */
/* --------------------------------------------------------------------------- */

bool RestService::checkCredentials() {
  BUG_DEBUG("Begin of checkCredentials");

  if (DialogUserPassword::Instance() && DialogUserPassword::Instance()->isShown()) {
    DialogUserPassword::Instance()->updateDialog(m_errorMessage);
    BUG_DEBUG("End of checkCredentials: send errormessage to Dialog");
    return false;
  }

  clearLogonStatus();
  m_loginCanceled = false;
  while ( m_loginCanceled == false && AppData::Instance().OAuthToken().empty() &&
          (m_base.empty() || m_authHeader.empty())
          ) {
    BUG_DEBUG("m_base = " << m_base);
    BUG_DEBUG("m_authHeader = " << m_authHeader);
    // check environment variables
    if ( m_base.empty() ) {
      char *base = getenv("REST_SERVICE_BASE");
      if ( base ) { // REST_SERVICE_BASE is not empty
        setBase( base );
      }
    }

    setBaseList( m_base ); // keep m_base if set and found in list

    if ( !m_base.empty() ) {
      BUG_DEBUG(" -- base is '" << m_base << "' --");
      char *username = getenv("REST_SERVICE_USERNAME");
      if ( username ) { // REST_SERVICE_USERNAME is not empty
        if ( ! m_envPasswordUsed ) { // only use the environment password once (see ticket #530)
          BUG_DEBUG(" -- user is '" << username << "'");
          setUsername(username);
          m_envPasswordUsed = true;
          char *password = getenv("REST_SERVICE_PASSWORD");
          if ( password ) { // REST_SERVICE_PASSWORD is not empty
            BUG_DEBUG(" --password is '" << password << "'");
            if(m_authorization == "jwt") {
              BUG_DEBUG(" -- jwt => try to login --");
              JobElement::OpStatus status = login(m_base, m_username, password);
              if( status != JobElement::op_Ok ){
                BUG_DEBUG(" == login failed ==");
              }
            }
            else {
              BUG_DEBUG(" -- basic => set password --");
              setPassword(password);
            }
            break;
          }
        }
      }
    }

    // if test mode => exit
    if (AppData::Instance().hasTestModeFunc()) {
      std::cerr << "=== TestMode RestService : automatic login failed" << std::endl
                << "=== URL: '" << m_base << "'" << std::endl
                << "=== USERNAME: '" << m_username << "'" << std::endl
                << std::flush;
      exit(1);
    }

    showDialogUserPassword();
  }

  m_errorMessage.clear();

  if ( m_loginCanceled ) {
    writeLogMessage(_("Login canceled."));
    BUG_DEBUG("End of checkCredentials: -- canceled --");
    return false;
  }

  BUG_DEBUG("End of checkCredentials: -- ok --");
  if (m_username.empty() && !AppData::Instance().OAuthToken().empty()){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* showDialogUserPassword --                                                   */
/* --------------------------------------------------------------------------- */

void RestService::showDialogUserPassword(){
  BUG_DEBUG("Begin of showDialogUserPassword");

  if ( m_username.empty() && AppData::Instance().OAuth().empty()) { // use OS user
    m_username = AppData::Instance().UserName();
  }

  setUsernameList();
  clearAuthHeader();
  std::string& connect = m_base_list.empty() ? m_base : m_base_list;
  auto settingsUser = GuiQtManager::Settings()->value("Authentification/user", "").toString().toStdString();
  std::string& username = m_username_list.empty() ? settingsUser.empty() ? m_username : settingsUser : m_username_list;
  BUG_INFO("-- showDialogUserPassword --");

  GuiFactory::Instance()->showDialogUserPassword( connect,
                                                  username,
                                                  new RestServiceUserPasswordListener(*this),
                                                  0,
                                                  m_errorMessage );
  BUG_DEBUG("End of showDialogUserPassword");
}

/* --------------------------------------------------------------------------- */
/* getErrorMessage --                                                          */
/* --------------------------------------------------------------------------- */

std::string RestService::getErrorMessage() const {
  return m_errorMessage;
}

/* --------------------------------------------------------------------------- */
/* extendAndAppendErrorMessage --                                              */
/* --------------------------------------------------------------------------- */

void RestService::extendAndAppendErrorMessage( std::string& errorMessage,
                                               const BaseUrl& baseUrl ) {
  // extend errorMessage with baseUrl.name if more than one base urls are used
  if(m_baseUrls.size() > 1) {
    errorMessage += " (" + baseUrl.name + ")";
  }

  // append errorMessage to m_errorMessage
  if(!m_errorMessage.empty()) {
    m_errorMessage += "\n";
  }
  m_errorMessage += errorMessage;
}

/* --------------------------------------------------------------------------- */
/* setPath --                                                                  */
/* --------------------------------------------------------------------------- */

void RestService::setPath( const std::string& path ) {
  std::string s(path);
  trim(s);
  m_path = s;
  UrlConverter::urlQuote_extendedAscii( m_path );

  if ( m_path.empty() ) {
    return;
  }
  // remove '/' at begin
  if ( m_path.at( 0 ) == '/' ) {
    m_path.erase(0, 1);
  }
}

/* --------------------------------------------------------------------------- */
/* addFilter --                                                                */
/* --------------------------------------------------------------------------- */

void RestService::addFilter( std::string filterString ) {
  BUG_DEBUG( "addFilter: filterString = " << filterString );
  trim(filterString);
  if ( filterString.empty() ) {
    return;
  }

  Json::Value filter = parseJsonObject( filterString );
  if ( filter.isObject() ) {
    // build URL filter
    Json::Value::Members members = filter.getMemberNames();
    std::ostringstream os;
    for( size_t m=0; m<members.size(); ++m ) {
      if ( m ) os << "&";
      appendParameter( os, members[m], filter[members[m]] );
    }
    std::string filterUrl = os.str();
    if(filterUrl.empty()) {
      return;
    }

    // add filterUrl to m_path
    m_path += ( m_path.find('?') != std::string::npos ) ? '&' : '?';
    m_path += filterUrl;
  }
}

/* --------------------------------------------------------------------------- */
/* appendParameter --                                                          */
/* --------------------------------------------------------------------------- */

void RestService::appendParameter( std::ostringstream& os,
                                   const std::string& name,
                                   const Json::Value& value ) {
  os << name << "=";
  if ( value.isArray() ) {
    bool insert_comma = false;
    if ( value.size() > 1 ) os << "(";
    for( Json::ValueConstIterator i=value.begin(); i!=value.end(); ++i ) {
      if (insert_comma) os << ","; else insert_comma=true;
      appendParameterValue(os, *i);
    }
    if ( value.size() > 1 ) os << ")";
  } else { // not an array
    appendParameterValue(os, value);
  }
}

/* --------------------------------------------------------------------------- */
/* appendParameterValue --                                                     */
/* --------------------------------------------------------------------------- */

void RestService::appendParameterValue( std::ostringstream& os,
                                        const Json::Value& value ) {
  if ( value.isString() ) {
    std::string s = value.asString();
    UrlConverter::urlQuote( s );
    os << s;
  } else if ( value.isInt() ) {
    os << value.asInt();
  } else if ( value.isNumeric() ) {
    os << value.asDouble();
  } else {
    os << value;
  }
}

/* --------------------------------------------------------------------------- */
/* setDataStream --                                                            */
/* --------------------------------------------------------------------------- */

void RestService::setDataStream( Stream *dataStream ) {
  m_dataStream = dataStream;
}

/* --------------------------------------------------------------------------- */
/* setResponseStream --                                                        */
/* --------------------------------------------------------------------------- */

void RestService::setResponseStream( Stream *responseStream ) {
  m_responseStream = responseStream;
}

/* --------------------------------------------------------------------------- */
/* setSetDbTimestamp --                                                        */
/* --------------------------------------------------------------------------- */

void RestService::setSetDbTimestamp( bool setDbTimestamp ) {
  m_setDbTimestamp = setDbTimestamp;
}

/* --------------------------------------------------------------------------- */
/* setCallingAction --                                                         */
/* --------------------------------------------------------------------------- */

void RestService::setCallingAction(JobAction* action)  {
  m_webApiCallingAction = action;
}

/* --------------------------------------------------------------------------- */
/* get --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::get(bool autoRetry, bool silent ) {
  BUG_INFO( "Begin of get(" << autoRetry << ", " << silent << "): " << m_path );

  m_dataStream = 0;
  m_setDbTimestamp = false;

  curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, write_data ) ;
  curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, this ) ;

  // GET
  curl_easy_setopt( m_curlHandle, CURLOPT_CUSTOMREQUEST, NULL);
  curl_easy_setopt( m_curlHandle, CURLOPT_HTTPGET, 1L);

  JobElement::OpStatus rslt = perform("GET", autoRetry, silent );
  BUG_INFO( "End of get: Result=" << rslt);

  // error case "users/<user>"
  // no user data from workbook query
  // return user data from jwt token
  std::string path_search("users/" + m_username);
  if (m_path == path_search && rslt == JobElement::op_FatalError) {
    writeToResponseStream(ch_semafor_intens::JsonUtils::value2string(m_user_jwt));
    return JobElement::op_Ok;
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* httpdelete --                                                               */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::httpdelete() {
  BUG_INFO( "Begin of httpdelete: Path=" << m_path );
  m_dataStream = 0;
  m_setDbTimestamp = false;

  curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, write_data ) ;
  curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, this ) ;

  // DELETE
  curl_easy_setopt( m_curlHandle, CURLOPT_CUSTOMREQUEST, "DELETE");
  curl_easy_setopt( m_curlHandle, CURLOPT_UPLOAD, 0L);

  JobElement::OpStatus rslt = perform("DELETE", false, false);
  BUG_INFO( "End of httpdelete: Result=" << rslt);
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* put --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::put() {
  BUG_INFO( "Begin of put: Path=" << m_path );
  set_data();
  BUG_DEBUG( "Data: " << m_dataData );

  curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, write_data ) ;
  curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, this ) ;
  curl_easy_setopt( m_curlHandle, CURLOPT_READFUNCTION, read_data ) ;
  curl_easy_setopt( m_curlHandle, CURLOPT_READDATA, this ) ;

  // PUT
  curl_easy_setopt( m_curlHandle, CURLOPT_CUSTOMREQUEST, NULL);
  curl_easy_setopt( m_curlHandle, CURLOPT_POST, 0L);
  curl_easy_setopt( m_curlHandle, CURLOPT_UPLOAD, 1L);

  JobElement::OpStatus rslt = perform("PUT", false, false);
  BUG_INFO( "End of put: Result=" << rslt);
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* post --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::post() {
  BUG_INFO( "Begin of post: Path=" << m_path );
  set_data();
  BUG_DEBUG( "Data: " << m_dataData );

  set_post_curl_options();

  JobElement::OpStatus rslt = perform("POST", false, false);
  BUG_INFO( "End of post: Result=" << rslt);
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* login --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::login( const std::string &baseUrl,
                                         const std::string &username,
                                         const std::string &password ) {
  BUG_DEBUG("Begin of login(" << baseUrl << ", " << username << ", " << password << ")");
  if (getUsername().size()) {
    BUG_INFO("Already logged in as user '" << getUsername() << "', ignore this login("
             << baseUrl << ", " << username << ", ..." << ")");
    return JobElement::op_Ok;
  }

  clearLogonStatus();

  // oauth !!!
  if (username == "__token__" &&
      !AppData::Instance().OAuth().empty() &&
      AppData::Instance().OAuthToken().empty()) {
    BUG_INFO("Network Authentification");
    decode_token(password);
    showDialogUserPassword();
    if (!baseUrl.empty())
      setBase(baseUrl);
    return JobElement::op_Ok;
  }

  if (username == "__token__" || username == "__accesstoken__") {
    decode_token(password);
    return login(baseUrl, password);
  }


  if(username.empty()) {
    BUG_DEBUG("no user => show Login Dialog");
    // kein Benutzername -> Login Dialog anzeigen
    if ( checkCredentials() ) {
      BUG_DEBUG("End of login(): checkCredentials() ok");
      setLogonStatus("OK");
      if(!AppData::Instance().OAuthToken().empty()){
        decode_token(AppData::Instance().OAuthToken());
      }

      return JobElement::op_Ok;
    }
    else {
      BUG_DEBUG("End of login(): checkCredentials() failed");
      setLogonStatus("ABORT");
      return JobElement::op_Aborted;
    }
  }

  // backup m_path, m_dataStream, m_responseStream, m_setDbTimestamp
  std::string   path(m_path);
  Stream       *dataStream(m_dataStream);
  Stream       *responseStream(m_responseStream);
  bool          setDbTimestamp(m_setDbTimestamp);

  JobElement::OpStatus ret;
  if ( m_authorization == "jwt" ) {
    BUG_DEBUG("authorization is jwt");
    // get token (checks username / password)

    // prepare call of perform("LOGIN")
    setPath("login");
    m_dataStream = 0;  // not needed (as set_data is not called and m_setDbTimestamp is set to false), but does not hurt
    m_responseStream = 0;  // below perform must not clear and write to the previous responseStream
    m_setDbTimestamp = false;  // below perform must not do m_dataStream->setDbItemsNotModified

    setBase(baseUrl);

    // set m_dataData
    Json::Value data = Json::Value(Json::objectValue);
    data["username"] = username;
    m_username = username;
    data["password"] = password;
    // TODO JWT: urlQuote or urlQuote_extendedAscii?
    m_dataData = value2string(data);

    // amg 2023-01-18 to keep some applications autologon runnning TODO
    setPassword(password);

    set_post_curl_options();

    // use header_data (callback)
    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERFUNCTION, header_data ) ;
    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERDATA, this ) ;

    ret = perform("LOGIN", /*autoRetry=*/false, /*silent=*/false);

    // don't use header_data (callback)
    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERFUNCTION, NULL ) ;
    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERDATA, 0L ) ;
  }
  else {
    BUG_DEBUG("authorization is basic");
    // prepare call of get (below): set m_path, m_responseStream
    // m_dataStream and m_setDbTimestamp are set inside get
    setPath("users/" + username);
    m_responseStream = 0;  // below get must not clear and write to the previous responseStream

    if (!AppData::Instance().HeadlessWebMode()) {
      s_initialRetry = 0;
    }
    do {
      setBase(baseUrl);
      setUsername(username);
      setPassword(password);
      BUG_DEBUG(" -- call get() --");
      ret = get(--s_initialRetry > 0);
      BUG_DEBUG(" -- get() returned " << ret << " --");
      if (ret != JobElement::op_Ok && s_initialRetry > 0){
        sleep(1);
      }
    }
    while (ret != JobElement::op_Ok && s_initialRetry > 0);
    s_initialRetry = 0;
  }

  if( ret == JobElement::op_Ok ){
    ret = executeVersionControl();
    if( ret != JobElement::op_Ok ){
      setBaseList( m_base );
      showDialogUserPassword();
    }
  }

  // restore m_path, m_dataStream, m_responseStream, m_setDbTimestamp
  m_path = path;
  m_dataStream = dataStream;
  m_responseStream = responseStream;
  m_setDbTimestamp = setDbTimestamp;

  if( ret == JobElement::op_Ok ){
    BUG_DEBUG("End of login(): success");
    setLogonStatus("OK");
    afterDbManagerLogin();
  }
  else{
    BUG_DEBUG("End of login(): result is " << ret );
    setLogonStatus("ABORT");
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/* decode_token                                                                */
/* --------------------------------------------------------------------------- */

void RestService::decode_token( const std::string &jwt){
  try {
    auto decoded = jwt::decode(jwt);
    for(auto& e : decoded.get_payload_json()) {
      BUG_INFO(">>" << e.first << " = " << e.second);
      if(e.first == "preferred_username"){
        setUsername(e.second.to_str());
        m_user_jwt["username"] = e.second.to_str();
      }
      if(e.first == "family_name"){
        m_user_jwt["lastName"] = e.second.to_str();
      }
      if(e.first == "given_name"){
        m_user_jwt["firstName"] = e.second.to_str();
      }
      if(e.first == "active_group"){
        std::string s(e.second.to_str());
        removeString(s, "/");
        m_user_jwt["active_group"] = s;
      }
      if(e.first == "groups"){
        Json::Value jsonAry = Json::Value(Json::arrayValue);
        picojson::array list = e.second.get<picojson::array>();
        for (picojson::array::iterator iter = list.begin(); iter != list.end(); ++iter) {
          Json::Value group;
          std::string group_name((*iter).get<std::string>().c_str());
          removeString(group_name, "/");
          BUG_DEBUG("Add Group: " << group_name);
          group["name"] = group_name;
          jsonAry.append(group);
        }
        m_user_jwt["groups"] = jsonAry;
      }
    }
    BUG_INFO("User data: " << ch_semafor_intens::JsonUtils::value2string(m_user_jwt));
    BUG_INFO("preferred_username: " << decoded.get_payload_claim("preferred_username").as_string());
    // setUsername(decoded.get_payload_claim("preferred_username").as_string());
  } catch( const std::exception &e ) {
    BUG_INFO("RestService::jwtParse Error: " << e.what());
  }
}

/* --------------------------------------------------------------------------- */
/* login -- (JWT)                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::login( const std::string &baseUrl,
                                         const std::string &jwt ) {
  setHeaders("Bearer " + jwt);
  setBase(baseUrl);

  return JobElement::op_Ok;
}
/* --------------------------------------------------------------------------- */
/* logoff --                                                                   */
/* --------------------------------------------------------------------------- */

bool RestService::logoff(const bool clearErrorMessage) {
  BUG_DEBUG("Begin of logoff");

  clearAuthHeader();
  m_username.clear();
  if(clearErrorMessage) {
    m_errorMessage.clear();
  }

  // Delete RESTUSERNAME and RESTBASE in Datapool
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "RESTUSERNAME" );
  if ( ref != 0 ) {
    // ref->clearDataItem(); not possible because RESTUSERNAME is global and cannot be cleared
    std::string empty("");
    ref->SetValue( empty );
    delete ref;
  }
  ref = DataPoolIntens::Instance().getDataReference( "RESTBASE" );
  if ( ref != 0 ) {
    // ref->clearDataItem(); not possible because RESTBASE is global and cannot be cleared
    std::string empty("");
    ref->SetValue( empty );
    delete ref;
  }
  BUG_DEBUG("End of logoff");
  return true;
}

/* --------------------------------------------------------------------------- */
/* recall --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::recall( const std::string& method ) {
  BUG_DEBUG("Begin of recall(" << method << ")");
  if (method == "GET")
    return get();
  if (method == "DELETE")
    return httpdelete();
  if (method == "PUT")
    return put();
  if (method == "POST")
    return post();
  if (method == "LOGIN")
    return JobElement::op_Warning;  // TODO JWT

  // this should not happen
  JobElement::OpStatus rslt = perform( method, false, false );
  BUG_INFO( "End of recall: Bad Result=" << rslt);
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* perform (all baseUrl) --                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::perform( const std::string& method,
                                           bool autoRetry,
                                           bool silent ) {
  BUG_DEBUG("Begin of perform(" << method << ", " << autoRetry << ", " << silent << ")");

  m_errorMessage.clear();
  // loop all baseUrls
  for( BaseUrls::iterator baseUrl = m_baseUrls.begin();
       baseUrl != m_baseUrls.end();
       ++baseUrl ) {
    BUG_DEBUG("perform: name = " << baseUrl->name << ", lockedUntil = " << baseUrl->lockedUntil);

    // is this baseUrl locked?
    if(baseUrl->lockedUntil > DateAndTime()) {  // also works for not locked (empty string)
      BUG_DEBUG("perform:  locked, continue");
      continue;
    }

    bool curl_ok(true);
    JobElement::OpStatus ret = perform(*baseUrl, method, silent, curl_ok);

    BUG_INFO("perform:  curl_ok: " << curl_ok
             << Date::durationAsString(m_startTime.msecsTo(QTime::currentTime()))
             << Date::durationAsString(m_msecsCurlPerform, true, "Curl: ")
             << Date::durationAsString(m_msecsJsonParse, true, "JsonParse: "));
    if(curl_ok) {
      BUG_DEBUG("End of perform: curl_ok");
      return ret;
    }

    // this baseUrl failed, lock it for skipPeriod and try next baseUrl
    baseUrl->lockedUntil = DateAndTime(m_skipPeriod);
    BUG_WARN("perform: name = " << baseUrl->name << " failed, lockedUntil = " << baseUrl->lockedUntil);
  }

  // all baseUrls locked: unlock all to retry next time
  for( BaseUrls::iterator baseUrl = m_baseUrls.begin();
       baseUrl != m_baseUrls.end();
       ++baseUrl ) {
    baseUrl->lockedUntil = "";
  }

  logoff(false);  // false: preserve errorMessage
  if(m_errorMessage.empty()) {
    m_errorMessage = compose(_("Cannot connect to Database(s), BaseUrl: %1"), m_base);
    writeLogMessage(m_errorMessage);
    BUG_INFO("perform: " << m_errorMessage);
  }

  // autoRetry, abort here
  if (autoRetry) {
    BUG_DEBUG("End of perform: Warning = Auto Retry");
    return JobElement::op_Warning;
  }

  // retry login
  if ( checkCredentials() ) {
    return recall(method);
  }
  BUG_DEBUG("End of perform: Warning");
  return JobElement::op_Warning;
}

/* --------------------------------------------------------------------------- */
/* perform (one baseUrl) --                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::perform( BaseUrl& baseUrl,
                                           const std::string& method,
                                           bool silent,
                                           bool& curl_ok ) {
  BUG_DEBUG("perform(" << baseUrl << ", " << method << ", " << silent << ")");

  m_startTime = QTime::currentTime();
  m_msecsCurlPerform = 0;
  m_msecsJsonParse = 0;

  m_dataSent = 0; // nothing sent yet
  m_responseData.clear();

  // http://curl.haxx.se/libcurl/c/https.html
#define SKIP_PEER_VERIFICATION
#define SKIP_HOSTNAME_VERIFICATION
  std::string https="https:";
  if(baseUrl.name.substr(0,https.size()) == https) {
#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
    curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */
    curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
  }

  // baseUrl.name may have changed (i.E. login window shown twice
  //   because something was wrong the first time, ...)
  std::string url;

  std::string http = "http";
  if(m_path.substr(0, http.size()) == http) {
    // absolute path
    url = m_path;
  } else if(m_authorization == "jwt" && method != "LOGIN") {
    url = baseUrl.name + "services/rest/" + m_path;
  } else {
    url = baseUrl.name + m_path;
  }
  BUG_DEBUG( "perform: " << url << ", method: " << method);

  curl_easy_setopt( m_curlHandle, CURLOPT_URL, url.c_str() ) ;
  updateHeaders();

  QTime startTime = QTime::currentTime();
  const CURLcode rc = curl_easy_perform( m_curlHandle ) ;
  m_msecsCurlPerform = startTime.msecsTo(QTime::currentTime());
  BUG_DEBUG( "perform: response = " << m_responseData );

  if( CURLE_OK != rc ){
    // default message
    std::string errorMessage_org = compose(_("Error from CURL: %1, BaseUrl: %2"), curl_easy_strerror(rc), m_base);
    // get message from application, if set
    std::string errorMessage(errorMessage_org);
    DataReference::Ptr ref = DataReference::newDataReference("REST_SERVICE.MESSAGE_CURLERROR");
    assert( ref->isValid() );
    ref->GetValue(errorMessage, 0);
    if(errorMessage == errorMessage_org) {
      errorMessage_org.clear();
    }

    extendAndAppendErrorMessage(errorMessage, baseUrl);
    writeLogMessage(errorMessage);
    BUG_WARN("perform: " << errorMessage);
    if(!errorMessage_org.empty()) {
      writeLogMessage(errorMessage_org);
      BUG_WARN("perform: " << errorMessage_org);
    }

    curl_ok = false;
    return JobElement::op_Aborted;  // not used, could be anything!
  }
  else {
    long statLong;
    std::string title = "REST Service";
    curl_easy_getinfo( m_curlHandle , CURLINFO_HTTP_CODE , &statLong );
    BUG_INFO( "HTTPStatusCode = " << HTTPStatusCode(statLong) << " " << statLong );

    switch ( HTTPStatusCode(statLong) ) {
    case OK:
      BUG_DEBUG("method: " << method
                << ", m_authorization: " << m_authorization
                << ", m_authHeader: " << m_authHeader
                << ", m_responseData: " << m_responseData);
      if(method == "LOGIN" &&
         m_authorization == "jwt" &&
         m_authHeader.empty()) {
        setHeaders("Bearer " + m_responseData);
      } else {
        writeToResponseStream(m_responseData);
      }
      mysetenv( "REST_SERVICE_BASE", m_base.c_str() );  // for applications that use this env
      // set db timestamp in m_dataStream if requested (PUT with SET_DB_TIMESTAMP)
      if ( m_setDbTimestamp && m_dataStream ) {
        m_dataStream->setDbItemsNotModified();
      }
      return JobElement::op_Ok;

    case NoContent: {
      // clear response stream
      m_responseData.clear();
      writeToResponseStream(m_responseData);
      return JobElement::op_Ok;
    }

    case Unauthorized: {
      // default message
      std::string errorMessage_org = _("Bad credentials");
      // get message from application, if set
      std::string errorMessage(errorMessage_org);
      DataReference::Ptr ref = DataReference::newDataReference("REST_SERVICE.MESSAGE_UNAUTHORIZED");
      assert( ref->isValid() );
      ref->GetValue(errorMessage, 0);
      if(errorMessage == errorMessage_org) {
        errorMessage_org.clear();
      }

      extendAndAppendErrorMessage(errorMessage, baseUrl);
      writeLogMessage(errorMessage);
      BUG_INFO(errorMessage);
      if(!errorMessage_org.empty()) {
        writeLogMessage(errorMessage_org);
        BUG_INFO(errorMessage_org);
      }

      // clear headers (curl, local, env) and user (local, env)
      logoff(false);

      // WebReply Aufruf, do WebQuery
      if (m_webApiCallingAction && AppData::Instance().HeadlessWebMode()) {
        BUG_WARN("Unauthorized: " << errorMessage);
        return JobElement::op_Warning;
      }

      if ( checkCredentials() ) {
        return recall(method);
      }
      return JobElement::op_Warning;
    }

    case Conflict: {
      return conflict();
    }

    case BadRequest:
      title = "Error";

    default:
      if( silent ){
        BUG_INFO("silent mode: no dialog");
        return JobElement::op_Aborted;
      }

      // show dialog
      std::ostringstream logmsg;
      trim(m_responseData);
      if(m_responseData.empty()) {
        logmsg << "empty response" << std::endl;
        char *http_proxy = getenv("http_proxy");
        if(http_proxy) {
          logmsg << "http_proxy = " << http_proxy << std::endl;
        }
        char *https_proxy = getenv("https_proxy");
        if(https_proxy) {
          logmsg << "https_proxy = " << https_proxy << std::endl;
        }
      } else {
        logmsg << m_responseData << std::endl;
      }
      std::string errmsg(compose(_("HTTP response code: %1"), statLong));
      BUG_ERROR(errmsg);
#if DEBUG
      logmsg << errmsg;
#endif
      if(HTTPStatusCode(statLong) == BadRequest){
        GuiFactory::Instance()->showDialogInformation(0, getDialogTitle(title), logmsg.str(), 0);
      }
      logmsg << "URL: " << m_base << m_path << std::endl;
      writeLogMessage(logmsg.str());
    } // switch

  }

  BUG_INFO("FatalError");
  return JobElement::op_Warning; //old: FatalError;
}

/* --------------------------------------------------------------------------- */
/* conflict --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::conflict() {
  Json::Value response = parseJsonObject(m_responseData);
  if ( ! response.isObject() ) {
    writeLogMessage(_("recevied Conflict with unexpected data."));
    return JobElement::op_FatalError;
  }
  std::string status = response.get("status", "").asString();

  GuiElement::ButtonType buttonClicked;
  std::string reason;
  if ( status == "new" ) {
    // get new revision
    Json::Value modifications = response.get("modifications", Json::Value(Json::objectValue));
    int newRevision = modifications.get("rev", -2).asInt();

    std::ostringstream os;
    os << _("New") << " " << componentType() << std::endl
       << componentIdentification(newRevision);
    std::string question(compose(_("Do you want to create this %1?"),componentType()));
    std::map<GuiElement::ButtonType, std::string> buttonText;
    buttonText[GuiElement::button_Save] = _("Save");
    buttonText[GuiElement::button_Discard] = _("Discard");
    buttonText[GuiElement::button_Cancel] = _("Cancel");
    BUG_INFO("conflict question["<<os.str() + "\n\n" + question<<"]");
    buttonClicked = GuiFactory::Instance()->showDialogConfirmation
      ( 0
        , ""
        , os.str() + "\n\n" + question
        , 0, buttonText, true );
    BUG_INFO("conflict buttonClicked["<<buttonClicked<<"]");
    reason = "Component created.";

  } else if ( status == "modified" ) {
    Json::Value modifications = response.get("modifications", Json::Value(Json::objectValue));
    Json::Value::Members members = modifications.getMemberNames();
    std::ostringstream os;

    if ( members.size() == 1 &&
         modifications.isMember( "approval" ) &&
         modifications[members[0]].isArray() &&
         modifications[members[0]].size() == 2
       ) {
      // only approval changed :
      // - don't ask for but create a reason
      // - set buttonClicked to Save (update component)
      os << _("Approval State")
         << " : "
         << modifications[members[0]][(Json::Value::ArrayIndex)(0)]
         << " -> "
         << modifications[members[0]][(Json::Value::ArrayIndex)(1)]
        ;
      reason = os.str();
      buttonClicked = GuiElement::button_Save;

    } else {
      // values changed :
      // - show modifications
      // - ask for a reason
      // - set buttonClicked
      // TODO: use templates to adapt to customer needs
      int newRevision = response.get("rev", -1).asInt(); // -1: use rev of data if no rev found in response
      os << componentIdentification(newRevision) << "<br/>" << "<br/>" << std::endl;
      writeModifications(os, modifications, members, "");

      // show dialog
      bool ok;

      GuiEventLoopListener *loopcontrol = new GuiEventLoopListener( false );
      GuiManager::Instance().attachEventLoopListener( loopcontrol );

      std::string compType = componentType();
      // Hack for some applications (modifiy dialog w/o New Button)
      // TODO: move it to Intens-Db-Service, or let the app decide
      if ( compType == "Variant" or compType == "Project" or
           response.get("useNameRevisionAsIdentifier", false).asBool() // only one component with same name/rev is allowed -> don't show new button
           ) {
        // modify or cancel
        os << std::endl << "<br/>" << _("Comment:");
        reason = GuiFactory::Instance()->showDialogTextInput(0,
                                                             _("Save component"),
                                                             os.str(), "",
                                                             ok,
                                                             0);
        if (ok) {
          buttonClicked = GuiElement::button_Save;
        } else {
          buttonClicked = GuiElement::button_Cancel;
        }

      } else {
        // modify, new or cancel
        std::map<GuiElement::ButtonType, std::string> buttonText;
        buttonText[GuiElement::button_Yes] = _("Modify");
        buttonText[GuiElement::button_Apply] = _("New");
        buttonText[GuiElement::button_Cancel] = _("Cancel");
        reason = GuiFactory::Instance()->showDialogTextInputExt(0,
                                                                compose(_("Save %1"), compType),
                                                                os.str(),
                                                                _("Modify comment:"),
                                                                buttonText,
                                                                buttonClicked,
                                                                0);
        BUG_INFO("Got reason["<<reason<<"] button_clicked["<<buttonClicked<<"]");
        switch (buttonClicked) {
        case GuiElement::button_Yes:
          buttonClicked = GuiElement::button_Save;
          break;
        case GuiElement::button_Apply:
          buttonClicked = GuiElement::button_Apply;
          break;
        case GuiElement::button_Cancel:
        default:
          buttonClicked = GuiElement::button_Cancel;
          break;
        }
      }

      loopcontrol->goAway();

    }

  } else if ( status == "upToDateConflict" ) {
    std::string type     = response.get("type", "").asString();
    std::string name     = response.get("name", "").asString();
    int         rev      = response.get("rev", 0).asInt();
    std::string username = response.get("username", "").asString();
    std::string changed  = response.get("changed", "").asString();

    std::ostringstream element;
    element << type << " " << name << ", Rev. " << rev;
    std::ostringstream os;
    os << "<html>"

       << "<p>"
       << compose(_("While trying to save <center>%1</center><br/>an up-to-date conflict occurred."),
                  element.str())
       << "</p>"

       << "<p>"
      // TODO: restore
      //<< _("Possible reasons:")
       << _("Possible reason:")
       << "<ul>"
      // TODO: restore
       // <<   "<li>"
       // <<     compose(_("Attempt to create a new %1 with an existing name."), type)
       // <<   "</li>"
       <<   "<li>"
       <<     compose(_("Attempt to update %1 that has been updated by someone else since you loaded it."),
                      element.str())
       <<   "</li>"
       << "</ul>"
       << "</p>"

       << "<p>"
       << _("Latest changes") << ": " << username << _(" at ") << changed << "."
       << "</p>"

       << "<p>"
       << _("You have the following options:")
       << "<ul>"
       <<   "<li>"
       <<     compose(_("<b>Overwrite</b> the %1 with your data."), type) << "<br/>"
       <<   "</li>"
       <<   "<li>"
       <<     "" << _("<b>Load</b> the latest changes.") << "<br/>"
       <<     _("WARNING: Your changes will be lost.") << "<br/>"
       <<   "</li>"
       <<   "<li>"
       <<     "" << _("<b>Cancel</b> the save operation.") << "<br/>"
       <<     "   " << _("Maybe you want to save it with a different name.")
       <<   "</li>"
       << "</ul>"
       << "</p>"

       << "</html>";
    std::string question(compose(_("Do you want to create this %1?"),componentType()));
    std::map<GuiElement::ButtonType, std::string> buttonText;
    buttonText[GuiElement::button_Save] = _("Overwrite");
    buttonText[GuiElement::button_Open] = _("Load");
    buttonText[GuiElement::button_Cancel] = _("Cancel");
    buttonClicked = GuiFactory::Instance()->showDialogConfirmation
      ( 0
        , type + _(": Up-to-date conflict")
        , os.str()
        , 0, buttonText, true );

    switch (buttonClicked) {
    case GuiElement::button_Save: { // Overwrite was clicked
      int version  = response.get("version", 0).asInt();
      // add/update version to data
      Json::Value data = parseJsonObject(m_dataData);
      if ( data.isObject() ) {
        data["version"] = version;
      }
      m_dataData = value2string(data);
      // repeat put
      return perform("PUT", false, false);
    }
    case GuiElement::button_Open: { // Load was clicked
      int id = response.get("id", 0).asInt();
      std::ostringstream os;
      os << "/" << id;
      m_path += os.str();
      // get the data
      s_userReload = true;
      JobElement::OpStatus ret = get();
      s_userReload = false;
      return ret;
    }
    case GuiElement::button_Cancel:  // Cancel was clicked
    default:
      break;
    }

  } else if ( status == "error" ) {
    std::string text = response.get("text", "").asString();
    GuiFactory::Instance()->showDialogInformation(0, getDialogTitle("Error"), text, 0);
    writeLogMessage(text);
    return JobElement::op_Aborted;

  } else {

    std::string text = std::string("Unexpected error : conflict with unexpected status '")
      + status + "'. response : " + m_responseData;

    GuiFactory::Instance()->showDialogInformation(0, getDialogTitle(), text, 0);
    std::ostringstream os;
    os << text << std::endl
       << "path = " << m_base << m_path << std::endl
       << "data = " << m_dataData << std::endl
       << "response = " << m_responseData << std::endl
      ;
    writeLogMessage(os.str());
    return JobElement::op_FatalError;
  }

  // new or modified
  // evaluate buttonClicked
  switch (buttonClicked) {
  case GuiElement::button_Yes:
  case GuiElement::button_Save: { // Save was clicked
    // add reason to data
    Json::Value data = parseJsonObject(m_dataData);
    if ( data.isObject() ) {
      data["interactiveReason"] = reason;
    }
    m_dataData = value2string(data);
    // repeat put
    return perform("PUT", false, false);
  }
  case GuiElement::button_Apply: { // Save as new was clicked
    // add/update version to data
    Json::Value data = parseJsonObject(m_dataData);
    if ( data.isObject() ) {
      data.removeMember("id");
      data["interactiveReason"] = _("Element created.");
    }
    m_dataData = value2string(data);
    // repeat put
    return perform("PUT", false, false);
  }
  case GuiElement::button_No:
  case GuiElement::button_Discard: // Don't Save was clicked
    return JobElement::op_Ok;

  case GuiElement::button_Cancel: // Cancel was clicked
    writeLogMessage(_("Cancel button pressed."));
    return JobElement::op_Aborted;

  default: // should never be reached
    writeLogMessage(_("Unexpected button pressed."));
    return JobElement::op_FatalError;
  }
  return JobElement::op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* set_data --                                                                 */
/* --------------------------------------------------------------------------- */

void RestService::set_data() {
  // set m_dataData
  if ( m_dataStream ) {
    std::ostringstream os;
    // set HIDEtransient and unset HIDEhidden
    SerializableMask hideFlags_backup = m_dataStream->getHideFlags();
    SerializableMask hideFlags = (hideFlags_backup | HIDEtransient) & ~HIDEhidden;
    if (AppData::Instance().hasUnitManagerFeature()) {
      hideFlags |= APPLYdbUnit;
    }
    m_dataStream->setHideFlags( hideFlags );
    m_dataStream->write( os );
    // restore hideFlags
    m_dataStream->setHideFlags( hideFlags_backup );
    m_dataData = os.str();
    BUG_INFO("Send DataSize: " << runden(os.str().size()/1024., 0) << "kB");
  }
  else {
    m_dataData.clear();
  }
}

/* --------------------------------------------------------------------------- */
/* set_post_curl_options --                                                    */
/* --------------------------------------------------------------------------- */

void RestService::set_post_curl_options() {
  curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, write_data ) ;
  curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, this ) ;
  curl_easy_setopt( m_curlHandle, CURLOPT_READFUNCTION, read_data ) ;
  curl_easy_setopt( m_curlHandle, CURLOPT_READDATA, this ) ;

  // POST
  curl_easy_setopt( m_curlHandle, CURLOPT_CUSTOMREQUEST, NULL);
  curl_easy_setopt( m_curlHandle, CURLOPT_UPLOAD, 0L);
  curl_easy_setopt( m_curlHandle, CURLOPT_POST, 1L);
}

/* --------------------------------------------------------------------------- */
/* writeToResponseStream --                                                    */
/* --------------------------------------------------------------------------- */

void RestService::writeToResponseStream(const std::string& response) {
  BUG_DEBUG("writeToResponseStream(" << response << ")");

  QTime startTime = QTime::currentTime();
  if ( m_responseStream ) {
    m_responseStream->clearRange();
    if ( !response.empty() ) {
      // set HIDEtransient
      SerializableMask hideFlags_backup = m_responseStream->getHideFlags();
      SerializableMask hideFlags = hideFlags_backup | HIDEtransient;
      m_responseStream->setHideFlags(hideFlags);
      if(s_userReload) {
        BUG_DEBUG("set reload Start");
        // add reload to response
        Json::Value data = parseJsonObject(response);
        if ( data.isObject() ) {
          data["restservice_reload_flag"] = 1;
        }
        BUG_DEBUG("set reload Done");
        std::istringstream is( value2string(data) );
        m_responseStream->read( is );
      } else {
        std::istringstream is( response );
        m_responseStream->read( is );
      }
      // restore hideFlags
      m_responseStream->setHideFlags(hideFlags_backup);
    }
    m_responseStream->setDbItemsNotModified();
  }
  m_msecsJsonParse = startTime.msecsTo(QTime::currentTime());
  BUG_INFO("Parse Response DataSize: " << runden(response.size()/1024., 0) << "kB");
}

/* --------------------------------------------------------------------------- */
/* componentIdentification --                                                  */
/* --------------------------------------------------------------------------- */

std::string RestService::componentIdentification(const int newRevision) const {
  std::ostringstream os;
  Json::Value data = parseJsonObject(m_dataData);
  if ( data.isObject() ) {
    int revision = newRevision == -1 ? data.get("rev", 0).asInt() : newRevision;
    os << data.get("name", "").asString();
    if ( revision >= 0 ) {
      os << ", Rev. "
	 << revision;
    }
  }
  return os.str();
}

/* --------------------------------------------------------------------------- */
/* componentType --                                                            */
/* --------------------------------------------------------------------------- */

std::string RestService::componentType() const {
  Json::Value data = parseJsonObject(m_dataData);
  if ( data.isObject() ) {
    return data.get("type", "").asString();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* parseJson --                                                                */
/* --------------------------------------------------------------------------- */

Json::Value RestService::parseJson(const std::string& jsonString) {
  Json::Value root;   // will contain the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse( jsonString, root );
  if ( !parsingSuccessful ) {
    // report to the user the failure and their locations in the document.
    std::cerr  << "Failed to parse json\n"
               << reader.getFormattedErrorMessages()
               << std::endl;
    return Json::Value();
  }
  return root;
}

/* --------------------------------------------------------------------------- */
/* parseJsonObject --                                                          */
/* --------------------------------------------------------------------------- */

Json::Value RestService::parseJsonObject(const std::string& jsonString){
  Json::Value root = parseJson(jsonString);
  if ( not root.isObject() ) {
    BUG_WARN("parseJsonObject: Not a Json object. jsonString starts with " << jsonString.substr(0, 10));
    return Json::Value();
  }
  return root;
}

/* --------------------------------------------------------------------------- */
/* value2string --                                                             */
/* --------------------------------------------------------------------------- */

std::string RestService::value2string(const Json::Value& value) {
  return Json::FastWriter().write(value);
}

/* --------------------------------------------------------------------------- */
/* writeLogMessage --                                                          */
/* --------------------------------------------------------------------------- */

void RestService::writeLogMessage(const std::string &message) {
  std::ostringstream logmsg;
  logmsg << DateAndTime() << ": REST SERVICE : " << message << std::endl;
  GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
}

/* --------------------------------------------------------------------------- */
/* write_data --                                                               */
/* --------------------------------------------------------------------------- */

size_t RestService::write_data(void *buffer,
                               size_t size,
                               size_t nmemb,
                               void *userp ) {
  RestService* restService = static_cast< RestService* >( userp ) ;
  const size_t bufferSize = size*nmemb;
  restService->m_responseData.append((char *)buffer, bufferSize);
  return bufferSize;
}

/* --------------------------------------------------------------------------- */
/* read_data --                                                                */
/* --------------------------------------------------------------------------- */

size_t RestService::read_data(char *bufptr,
                              size_t size,
                              size_t nitems,
                              void *userp ) {
  RestService* restService = static_cast< RestService* >( userp ) ;
  const size_t bufferSize = size*nitems;

  // everything sent ?
  if ( restService->m_dataSent >= restService->m_dataData.size() ) {
    return 0;
  }

  // send (some) data
  size_t numberOfBytes
    = std::min(bufferSize, // maximum amount of data
               restService->m_dataData.size() - restService->m_dataSent // remaining data size
               );
  memcpy(bufptr, &restService->m_dataData.c_str()[restService->m_dataSent], numberOfBytes);
  restService->m_dataSent += numberOfBytes;
  return numberOfBytes;
}

/* --------------------------------------------------------------------------- */
/* header_data --                                                              */
/* --------------------------------------------------------------------------- */

size_t RestService::header_data(char *buffer,
                                size_t size,
                                size_t nitems,
                                void *userp ) {
  /** callback function to read header fields received
      it is called for each header seperately
      Authorization field contains bearer token received by login
      -> call setHeaders with it
  */
  const size_t bufferSize = size*nitems;
  std::string header(buffer, bufferSize);
  // split header into key(elements[0]), value(elements[1])
  std::vector<std::string> elements = split(header, ": ");
  if(elements.size() == 2 &&
     lower(elements[0]) == "authorization") {
    trim(elements[1]);  // remove newline
    RestService* restService = static_cast< RestService* >( userp ) ;
    restService->setHeaders(elements[1]);
  }
  // add opentelemetry metadata
  if (AppData::Instance().OpenTelemetryMetadata()) {
    std::vector<std::string>::iterator it = std::find(elements.begin(), elements.end(), "Traceparent");
    if (it != elements.end()) {
      ++it;  // to value
      std::cout << " CURL Header received Traceparent data: " << (*it)<<"\n";
      DataReference *ref  =  DataPoolIntens::Instance().getDataReference( "OPENTELEMETRY_METADATA_OUT" );
      if (ref)
        ref->SetValue((*it));
    }
  }
  return bufferSize;
}

/* --------------------------------------------------------------------------- */
/* getBaseUrlLockedUntil --                                                    */
/* --------------------------------------------------------------------------- */

std::string RestService::getBaseUrlLockedUntil( const BaseUrls& baseUrls,
                                                const std::string& baseUrl ) {
  BUG_DEBUG("getBaseUrlLockedUntil: baseUrl = " << baseUrl);
  for(BaseUrls::const_iterator it = baseUrls.begin();
      it != baseUrls.end(); ++it) {
    BUG_DEBUG("getBaseUrlLockedUntil: name = " << it->name << ", lockedUntil = " << it->lockedUntil);
    if(it->name == baseUrl) {
      BUG_DEBUG("getBaseUrlLockedUntil: MATCHES");
      return it->lockedUntil;
    }
  }
  BUG_DEBUG("getBaseUrlLockedUntil: no match, not locked");
  return "";
}

/* --------------------------------------------------------------------------- */
/* setBase --                                                                  */
/* --------------------------------------------------------------------------- */

void RestService::setBase( const std::string& base ) {
  BUG_DEBUG("setBase(" << base << ")");
  m_base = base;
  trimUrl(m_base);

  // m_baseUrls
  BUG_DEBUG("m_baseUrls.size() = " << m_baseUrls.size());
  BaseUrls baseUrls(m_baseUrls);  // copy of previous baseUrls
  BUG_DEBUG("baseUrls.size() = " << baseUrls.size());
  m_baseUrls.clear();
  BUG_DEBUG("  m_baseUrls.size() = " << m_baseUrls.size());

  char delim = '|';
  std::stringstream ss(m_base);
  std::string baseUrl;
  while (std::getline(ss, baseUrl, delim)) {
    BUG_DEBUG("RestService::setBase: baseUrl = " << baseUrl);
    if ( trimUrl(baseUrl) ) {
      BUG_DEBUG("RestService::setBase: trimmed baseUrl = " << baseUrl);
      // keep lockedUntil if baseUrl is in previous baseUrls
      std::string lockedUntil = getBaseUrlLockedUntil(baseUrls, baseUrl);
      BUG_DEBUG("RestService::setBase: lockedUntil = " << lockedUntil);
      m_baseUrls.push_back(BaseUrl(baseUrl, lockedUntil));
    }
  }

  DataReference *ref = DataPoolIntens::Instance().getDataReference( "RESTBASE" );
  if ( ref != 0 ) {
    ref->SetValue( m_base );
    delete ref;
  }
}

/* --------------------------------------------------------------------------- */
/* setBaseList --                                                              */
/* --------------------------------------------------------------------------- */

void RestService::setBaseList( const std::string& selected_base ) {
  if( !m_base_list.empty() ){
    return;  // ok
  }

  char *base_list = getenv("REST_SERVICE_BASE_LIST");
  if( !base_list ){
    return;  // REST_SERVICE_BASE_LIST is empty
  }

  std::string base;
  char selected = '*';
  char delim = ';';
  std::stringstream ss(base_list);
  m_base_list.clear();

  std::string url;
  while (std::getline(ss, url, delim)) {
    if ( trimUrl(url) ) {
      // add delim if needed
      if ( !m_base_list.empty() ) {
        m_base_list += delim;
      }
      // select base
      if ( ( !selected_base.empty() && url == selected_base ) ||
           ( selected_base.empty() && m_base_list.empty() ) // select first entry when no selected_base is given
           ) {
        base = url;
        m_base_list += selected;
      }
      m_base_list += url;
    }
  }

  setBase(base);
}

/* --------------------------------------------------------------------------- */
/* setUsername --                                                              */
/* --------------------------------------------------------------------------- */

void RestService::setUsername( const std::string& username ) {
  BUG_INFO("username: " << username);
  std::string s(username);
  trim(s);
  m_username = s;
  if ( m_username.empty() ) {
    return;
  }

  DataReference *ref = DataPoolIntens::Instance().getDataReference( "RESTUSERNAME" );
  if ( ref != 0 ) {
    ref->SetValue( m_username );
    delete ref;
  }
}

/* --------------------------------------------------------------------------- */
/* getUsername --                                                              */
/* --------------------------------------------------------------------------- */

std::string RestService::getUsername() {
  std::string username;

  DataReference *ref = DataPoolIntens::Instance().getDataReference( "RESTUSERNAME" );
  if ( ref != 0 ) {
    ref->GetValue( username );
    delete ref;
  }
  return username;
}

/* --------------------------------------------------------------------------- */
/* setUsernameList --                                                          */
/* --------------------------------------------------------------------------- */

void RestService::setUsernameList( ) {
  // fill m_username_list from DATAPOOL variable RESTUSERNAMELIST
  // unset m_username if it is not found in the list

  m_username_list = "";

  DataReference *ref = DataPoolIntens::Instance().getDataReference( "RESTUSERNAMELIST" );
  if ( ref != 0 ) {
    char selected = '*';
    char delim = ';';
    bool first = true;

    DataVector vector;
    int id[1] = { -1 };
    ref->getDataVector( vector, 1, id );

    std::string username;
    for (DataVector::iterator it = vector.begin();it != vector.end(); ++it) {
      (*it)->getValue(username);
      if ( ! username.empty() ) {
        // add delim if needed
        if ( first ) {
          first = false;
        } else {
          m_username_list += delim;
        }
        // select m_username (if valid and found in list)
        if ( ( !m_username.empty() && username == m_username ) ||
             ( m_username.empty() && first ) // select first entry m_username is empty
             ) {
          m_username_list += selected;
        }
        m_username_list += username;
      }
    }

    delete ref;
  }
}

/* --------------------------------------------------------------------------- */
/* setPassword --                                                              */
/* --------------------------------------------------------------------------- */

void RestService::setPassword( const std::string& password ) {
  std::string realmPlain = m_username + ":" + password;
  std::string realmPlainBase64;
  base64encode(reinterpret_cast<const unsigned char*>(realmPlain.c_str()),
	       realmPlain.size(), realmPlainBase64, false);
  std::string authorization = "Basic " + realmPlainBase64;

  // to keep some applications autologon runnning TODO
  if ( m_authorization == "jwt" ) {
    mysetenv( "REST_SERVICE_AUTHHEADER", authorization.substr(6).c_str() );
  } else {
    setHeaders(authorization);
  }
}

/* --------------------------------------------------------------------------- */
/* updateHeaders --                                                            */
/* --------------------------------------------------------------------------- */

void RestService::updateHeaders() {
  BUG_INFO("Setting http headers");
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, m_authHeader.c_str());
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charsets: utf-8");

  // add opentelemtry metadata
  if (AppData::Instance().OpenTelemetryMetadata()) {
    std::string data;
    DataReference *refIn  =  DataPoolIntens::Instance().getDataReference( "OPENTELEMETRY_METADATA_IN" );
    if (refIn)
      refIn->GetValue(data);
    auto jsonElem = parseJsonObject(data);
    std::vector<std::string> members = jsonElem.getMemberNames();
    for (auto member : members) {
      if (!jsonElem[member].isString()) continue;
      auto header = member + ": " + jsonElem[member].asString();
      headers = curl_slist_append(headers, header.c_str());
    }

    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERFUNCTION, header_data ) ;
  }
  curl_easy_setopt( m_curlHandle, CURLOPT_HTTPHEADER, headers);
}

/* --------------------------------------------------------------------------- */
/* setHeaders --                                                               */
/* --------------------------------------------------------------------------- */

void RestService::setHeaders( const std::string& authorization ) {
  if(authorization.empty()) {
    m_authHeader.clear();
  } else {
    m_authHeader = "Authorization: " + authorization;
  }
  updateHeaders();
  // !authorization.empty(): we know base, username and password
  // -> set environment variables
  // else:
  // -> clear environment variables
  if ( m_authorization == "jwt" ) {
    mysetenv( "REST_SERVICE_AUTHORIZATION", authorization.c_str() );
    if (!m_username.empty() && !authorization.empty() &&
        AppData::Instance().OAuthToken().empty()){
      setUsername(m_username);
    }else if (!AppData::Instance().OAuthToken().empty()){
      decode_token(authorization.substr(7));
    }
    // else: username is inside token
  } else {
    // deprecated, always set REST_SERVICE_AUTHORIZATION, including "Basic "
    mysetenv( "REST_SERVICE_AUTHHEADER",
              authorization.size() < 7 ? "" : authorization.substr(6).c_str() );
  }
  if(!m_base.empty() && !authorization.empty()) {
    mysetenv( "REST_SERVICE_BASE", m_base.c_str() );
  }
  // write settings
  if(authorization.size())      {
    mysetenv( "REST_SERVICE_BASE", m_base.c_str() );  // for applications that use this env

    std::string realmPlainBase64;
    std::string s(authorization.substr(7));
    base64encode(reinterpret_cast<const unsigned char*>(s.c_str()),
		 s.size(), realmPlainBase64, false);
    // write settings
    QSettings *settings = GuiQtManager::Settings();
    settings->beginGroup( QString::fromStdString("Authentification") );
    settings->setValue("token", realmPlainBase64.c_str());
    if (m_base.size()) {
      settings->setValue("url", m_base.c_str());
    }
    if (m_username.size()) {
      settings->setValue("user", m_username.c_str());
    }
    settings->endGroup();
  }
}

/* --------------------------------------------------------------------------- */
/* clearAuthHeader --                                                          */
/* --------------------------------------------------------------------------- */

void RestService::clearAuthHeader() {
  setHeaders("");  // set headers, without Authorization
}

/* --------------------------------------------------------------------------- */
/* trimUrl --                                                                  */
/* --------------------------------------------------------------------------- */

 bool RestService::trimUrl(std::string &url) {
  trim(url);
  if ( url.empty() ) {
    return false;
  }

  // append '/' if missing
  if ( url[url.size()-1] != '/' ) {
    url += "/";
  }

  // remove '*' if there
  if ( url[0] == '*' ) {
    url.erase(0,1);
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* loginCanceled --                                                            */
/* --------------------------------------------------------------------------- */

void RestService::loginCanceled() {
  setLogonStatus("CANCELED");
  m_loginCanceled = true;
  logoff();
}

/* --------------------------------------------------------------------------- */
/* getInputItemLabel --                                                        */
/* --------------------------------------------------------------------------- */

std::string RestService::getInputItemLabel(const std::string &subvarname, bool inputLabel) {
  std::string ret;
  if (m_dataStream) {
    for (int i=0; i < m_dataStream->getParameterGroup()->size(); ++i) {
      JSONStreamParameter *jsonStreamParameter = \
        dynamic_cast<JSONStreamParameter*>((*m_dataStream->getParameterGroup()->getParameterList())[i]);
      if (jsonStreamParameter) {
        ret = jsonStreamParameter->getItemLabel(subvarname);
        if (ret.size())
          break;
      }
    }
  }
  // remove @RestService
  replaceAll(ret, "@RestService@", inputLabel ? "" : "RestService");
  return ret;
}

/* --------------------------------------------------------------------------- */
/* getInputItemGuiValue --                                                     */
/* --------------------------------------------------------------------------- */

std::string RestService::getInputItemGuiValue( const std::string &subvarname,
                                               const Json::Value& value ) {
  // if data item subvarname has a DataSet, return the input string that corresponds to value
  // return value (as std::string) otherwise

  // value to string
  std::ostringstream os1;
  os1 << value;
  std::string value_string(os1.str());

  if(m_dataStream) {
    for(int i=0; i < m_dataStream->getParameterGroup()->size(); ++i) {
      JSONStreamParameter *jsonStreamParameter = \
        dynamic_cast<JSONStreamParameter*>((*m_dataStream->getParameterGroup()->getParameterList())[i]);
      if(jsonStreamParameter) {
        std::string ret = jsonStreamParameter->getDataSetValue(subvarname, value_string);
        if(ret.size())
          return ret;
      }
    }
  }

  // if description file provides a mapping
  // e.g. RestServiceSemiconductor = {id=[12, 24], name=["twelve", "twentyfore"]}
  // replace id with name
  std::string varname = getInputItemLabel(subvarname, false);
  DataReference *refName = DataPoolIntens::Instance().getDataReference(varname + ".name");
  DataReference *refId = DataPoolIntens::Instance().getDataReference(varname + ".id");
  if ( refName != 0 && refId != 0) {
    DataVector vectorName, vectorId;
    int id[1] = { -1 };
    refName->getDataVector( vectorName, 1, id );
    refId->getDataVector( vectorId, 1, id );
    std::string name;
    int Id;
    for (DataVector::iterator it = vectorId.begin();it != vectorId.end(); ++it) {
      (*it)->getValue(Id);
      os1.str("");
      os1 << Id;
      if (os1.str() == value_string) {
        vectorName.getValue(it - vectorId.begin())->getValue(name);
        return name;
      }
    }
  }
  int maxChar(50);
  if (value_string.size() > maxChar){
    return compose("%1 ... %2", value_string.substr(0, maxChar/2),
                   value_string.substr(value_string.size()-maxChar/2));
  }
  return value_string;
}

/* --------------------------------------------------------------------------- */
/* executeVersionControl --                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus RestService::executeVersionControl(){
  BUG_DEBUG("Begin of executeVersionControl");

  if( getVersionIgnore() ){
    // Es werden sämtliche Angaben zur Version ignoriert.
    BUG_DEBUG("End of executeVersionControl: ignore");
    return JobElement::op_Ok;
  }

  int db_version_num;
  std::string db_version( getDbVersion( db_version_num ) );
  BUG_DEBUG("db_version: " << db_version << ", db_version_num: " << db_version_num);

  int DP_app_version_num;
  std::string DP_app_version( getDpVersion( "APP", DP_app_version_num ) );
  BUG_DEBUG("DP_app_version: " << DP_app_version << ", DP_app_version_num: " << DP_app_version_num);

  int DP_db_version_num;
  std::string DP_db_version( getDpVersion( "DB", DP_db_version_num ) );
  BUG_DEBUG("DP_db_version: " << DP_db_version << ", DP_db_version_num: " << DP_db_version_num);

  if( DP_app_version.empty() && DP_db_version.empty() && db_version.empty() ){
    // Weder auf der Datenbank noch im DataPool hat es Angaben.
    BUG_DEBUG("End of executeVersionControl: no version information available");
    return JobElement::op_Ok;
  }

  std::ostringstream errmsg;

  if( db_version.empty() ){
    // Sobald eine Versionsnummer auftaucht, muss auf der Datenbank zwingend
    // Eine Versionsnummer vorhanden sein.
    logoff();
    errmsg << _("Database-Version is missing");
    if( !DP_db_version.empty() ){
      errmsg << "<br/>=> " << compose(_("required at least %1"), DP_db_version);
    }
    errmsg << "<br/>" << _("Connect to a different database or press 'Cancel'.");
    BUG_DEBUG("End of executeVersionControl: " << errmsg.str() << " => failed");
    m_errorMessage = errmsg.str();
    setLogonStatus("OLD_DB", true);
    return JobElement::op_Aborted;
  }

  if( DP_app_version.empty() ){
    // Sobald auf der Datenbank eine Versionsnummer abgelegt ist, wird eine
    // Applikation ohne Versionsnummer nicht mehr akzeptiert.
    logoff();
    errmsg << _("Application-Version is missing")
           << "<br/>=> " << compose(_("required at least %1"), db_version);
    errmsg << "<br/>" << _("Connect to a different database or press 'Cancel'.");
    BUG_DEBUG("End of executeVersionControl: " << errmsg.str() << " => failed");
    m_errorMessage = errmsg.str();
    setLogonStatus("OLD_APP", true);
    return JobElement::op_Aborted;
  }

  if( db_version_num > DP_app_version_num ){
    // Die Versionsnummer der Applikation muss mindestens derjenigen auf der Datenbank
    // ensprechen oder höher sein.
    logoff();
    errmsg << _("Application-Version is older than required")
           << "<br/>=> " << compose(_("Application-Version is %1; required at least %2"),
                                   DP_app_version, db_version);
    errmsg << "<br/>" << _("Connect to a different database or press 'Cancel'.");
    BUG_DEBUG("End of executeVersionControl: " << errmsg.str() << " => failed");
    m_errorMessage = errmsg.str();
    setLogonStatus("OLD_APP", true);
    return JobElement::op_Aborted;
  }

  if( !DP_db_version.empty() ){
    if( db_version_num < DP_db_version_num ){
      logoff();
      errmsg << _("Database-Version is older than required")
             << "<br/>=> " << compose(_("Database-Version is %1; required at least %2"),
                                     db_version, DP_db_version);
      errmsg << "<br/>" << _("Connect to a different database or press 'Cancel'.");
      BUG_DEBUG("End of executeVersionControl: " << errmsg.str() << " => failed");
      m_errorMessage = errmsg.str();
      setLogonStatus("OLD_DB", true);
      return JobElement::op_Aborted;
    }
  }

  BUG_DEBUG("End of executeVersionControl: no control");
  return JobElement::op_Ok;
}
/* --------------------------------------------------------------------------- */
/* getVersionIgnore --                                                         */
/* --------------------------------------------------------------------------- */

bool RestService::getVersionIgnore(){
  DataReference::Ptr ref = DataReference::newDataReference("REST_SERVICE.DB_VERSION_IGNORE");
  assert( ref->isValid() );

  int ignore = 0;
  if( !ref->GetValue( ignore, 0 ) ){
    return false;
  }
  return ( ignore > 0 );
}

/* --------------------------------------------------------------------------- */
/* getDbVersion --                                                             */
/* --------------------------------------------------------------------------- */

std::string RestService::getDbVersion( int &version_num ){
  BUG_DEBUG("Begin of getDbVersion");

  setPath("components/type/AppVersionCtrl?_projection=(app_major,app_minor,app_patch)");
  JobElement::OpStatus rslt = get( false, true ); // no retry, but silent

  int major = -1;
  int minor = 0;
  int patch = 0;
  version_num = 0;

  if( rslt != JobElement::op_Ok ){
    BUG_DEBUG("End of getDbVersion: Component AppVersionCtrl is unknown");
    return "";
  }

  if(m_responseData.empty()) {
    BUG_DEBUG("End of getDbVersion: No Verson Data found");
    return "";
  }

  Json::Value response = parseJson(m_responseData);

  if( response.isArray() ){
    if( response.size() == 0 ){
      BUG_DEBUG("End of getDbVersion: No Version Object found");
      return "";
    }
    if( response.size() > 1 ){
      BUG_DEBUG("End of getDbVersion: " << response.size() << " Objects found");
      return "";
    }

    int i = 0;
    Json::Value &obj = response[i];
    if( !obj.isObject() ){
      BUG_DEBUG("End of getDbVersion: No Object in Array");
      return "";
    }
    major = obj.get("app_major", -1).asInt();
    minor = obj.get("app_minor", 0).asInt();
    patch = obj.get("app_patch", 0).asInt();
    BUG_DEBUG("Array [0]: major: " << major << ", minor: " << minor << ", patch: " << patch);
  }

  if ( response.isObject() ) {
    major = response.get("app_major", -1).asInt();
    minor = response.get("app_minor", 0).asInt();
    patch = response.get("app_patch", 0).asInt();
    BUG_DEBUG("Object: major: " << major << ", minor: " << minor << ", patch: " << patch);
  }

  if( major < 0 ){
    return "";
  }

  std::ostringstream version;
  version <<
    major << "." <<
    minor << "." <<
    patch;
  version_num = major * 1000000 + minor * 1000 + patch;
  return version.str();
}

/* --------------------------------------------------------------------------- */
/* getDpVersion --                                                             */
/* --------------------------------------------------------------------------- */

std::string RestService::getDpVersion( const std::string &prefix,
                                       int &version_num ){
  DataReference::Ptr ref = DataReference::newDataReference(std::string("REST_SERVICE." + prefix + "_VERSION_MAJOR").c_str());
  assert( ref->isValid() );

  int major = -1;
  int minor = 0;
  int patch = 0;
  version_num = 0;

  if( !ref->GetValue( major, 0 ) ){
      return "";
  }
  if( major < 0 ){
    return "";
  }

  ref = DataReference::newDataReference(std::string("REST_SERVICE." + prefix + "_VERSION_MINOR").c_str());
  assert( ref->isValid() );
  ref->GetValue( minor, 0 );
  ref = DataReference::newDataReference(std::string("REST_SERVICE." + prefix + "_VERSION_PATCH").c_str());
  assert( ref->isValid() );
  ref->GetValue( patch, 0 );

  std::ostringstream version;
  version <<
    major << "." <<
    minor << "." <<
    patch;
  version_num = major * 1000000 + minor * 1000 + patch;
  return version.str();
}

/* --------------------------------------------------------------------------- */
/* setLogonStatus --                                                            */
/* --------------------------------------------------------------------------- */

void RestService::setLogonStatus( const std::string &status,
                                 bool override ){
  BUG_DEBUG("===== Set Status " << status << " =====");
  DataReference::Ptr ref = DataReference::newDataReference("REST_SERVICE.LOGON_STATUS");
  assert( ref->isValid() );

  if( !override ){
    std::string current_status;
    if( ref->GetValue( current_status, 0 ) ){
      BUG_DEBUG("===== Keep old Status " << current_status << " =====");
      return;
    }
  }

  ref->SetValue(status);
}

/* --------------------------------------------------------------------------- */
/* clearLogonStatus --                                                          */
/* --------------------------------------------------------------------------- */

void RestService::clearLogonStatus(){
  BUG_DEBUG("===== Clear Status =====");
  DataReference::Ptr ref = DataReference::newDataReference("REST_SERVICE.LOGON_STATUS");
  assert( ref->isValid() );
  ref->SetItemInvalid( 0 );
}

/* --------------------------------------------------------------------------- */
/*afterDbManagerLogin --                                                       */
/* --------------------------------------------------------------------------- */

void RestService::afterDbManagerLogin(){
  BUG_DEBUG("===== afterDbManagerLogin =====");

  // get locale
  std::string stringValue;
  double doubleValue;
  int intValue;
  AppData::Instance().getResource("LC_ALL", stringValue, doubleValue, intValue);
  // e.g. "de" import info fails
  if (stringValue.size() >= 5) {
    BUG_INFO("set LC_ALL: [" << stringValue<<"]");
    // set QLocale
    QLocale locale = QLocale(stringValue.c_str());
    QLocale::setDefault(locale);
    RealConverter::resetDecimalPoint(); // very important

    // setlocale
    if (stringValue.find(".") == std::string::npos && stringValue.size() == 5) {
      stringValue += ".utf8";
    }
    setlocale(LC_ALL, stringValue.c_str());

    // timezone info
#if QT_VERSION >= 0x060000
    int country = locale.territory();
#else
    int country = locale.country();
#endif
    QList<QByteArray> tzList = QTimeZone::availableTimeZoneIds(country);
    for (int i=0; i<tzList.size(); ++i) {
      BUG_DEBUG("TIMEZONE: " << i <<". " << tzList[i].data());
    }
    std::string tz("UTC");
    if(tzList.size() > 0){
      // take first
      std::string tz(tzList[0].data());
    }
    AppData::Instance().setResource("TZ", tz);
    BUG_INFO("TimeZone: " << tz);
    mysetenv( "TZ", tz.c_str());
  }

  // set HOME
  if (AppData::Instance().HeadlessWebMode() && m_username.size()) {
    std::string home(compose(_("%1/%2"), getenv("HOME"), m_username));
    mysetenv( "HOME", home.c_str());
    AppData::Instance().setResource("HOME", home);
    BUG_INFO("!!! Override environment HOME: " << getenv("HOME"));
  }
}

/* --------------------------------------------------------------------------- */

std::string RestService::getDialogTitle(std::string fallback){
  std::string title;
  DataReference::Ptr ref = DataReference::newDataReference("REST_SERVICE.MESSAGE_DIALOG_TITLE");
  assert( ref->isValid() );
  if(ref->GetValue(title, 0)) {
    return title;
  }
  return fallback;
}

/* --------------------------------------------------------------------------- */
/*writeModifications --                                                        */
/* --------------------------------------------------------------------------- */

void RestService::writeModifications(std::ostream& os, Json::Value& modifications, Json::Value::Members& members, std::string lineprefix) {
  // build modifications text
  for( size_t m=0; m<members.size(); ++m ) {
    std::string label = getInputItemLabel(members[m]);
    os << lineprefix << (label.size() ? label : members[m]) << ": ";
    if ( modifications[members[m]].isArray() &&
         modifications[members[m]].size() == 2 /*&&
         ( modifications[members[m]][(Json::Value::ArrayIndex)(0)].isNumeric() ||
           modifications[members[m]][(Json::Value::ArrayIndex)(0)].isString() ) &&
         ( modifications[members[m]][(Json::Value::ArrayIndex)(1)].isNumeric() ||
         modifications[members[m]][(Json::Value::ArrayIndex)(1)].isString() )*/
         ) {
      os << "<b>" << getInputItemGuiValue(members[m], modifications[members[m]][(Json::Value::ArrayIndex)(0)]) << "</b>"
         << " → " << getInputItemGuiValue(members[m], modifications[members[m]][(Json::Value::ArrayIndex)(1)]);
    } else if ( modifications[members[m]].isObject()) {
      os << "<br/>" << std::endl;
      Json::Value::Members membersSub = modifications[members[m]].getMemberNames();
      std::string lineprefixSub(lineprefix + "&nbsp;&nbsp;");
      writeModifications(os, modifications[members[m]], membersSub, lineprefixSub);
    } else {
      os << _("modified");
    }
    os << "<br/>" << std::endl;
  }
};

/* --------------------------------------------------------------------------- */
/* operator << --                                                              */
/* --------------------------------------------------------------------------- */

std::ostream &operator<<( std::ostream &o, const RestService::BaseUrl &url ){
  o << url.name;
  return o;
}
