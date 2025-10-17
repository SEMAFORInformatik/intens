
#if !defined(REST_SERVICE_H)
#define REST_SERVICE_H

#include <vector>
#include <string>
#include <curl/curl.h>
#include <QTime>

#include "jsoncpp/json/json.h"
#include "job/JobElement.h"

class Stream;
class ConnectionListener;

class RestService
{
 public:
  class BaseUrl {
  public:
    BaseUrl( const std::string &baseUrl,
             const std::string &_lockedUntil )
    : name(baseUrl)
    , lockedUntil(_lockedUntil) {
    }

    std::string name;
    std::string lockedUntil;
  };

  typedef std::vector<BaseUrl> BaseUrls;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  RestService();
public:
  virtual ~RestService();

/*=============================================================================*/
/* Singleton                                                                   */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Pointer auf das Singleton-Objekt DbManager.
   */
  static RestService &Instance();

  /** Die Funktion löscht das Singleton-Objekt DbManager.
   */
  static void Destroy();
private:
  static RestService   *s_instance;

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** get authorization method
   */
  const std::string& getAuthorization() {  return m_authorization; }

  /** Are base, username and password known?
   *  If not, open 'Login' dialog to get them
   */
  bool checkCredentials();

  /** get error message
   */
  std::string getErrorMessage() const;

private:
  /** if more than one baseUrls are used, extend
   *     the given error message with the baseUrl.name
   *  append the (possibly extended) given error message to m_errorMessage
   */
  void extendAndAppendErrorMessage(std::string& errorMessage, const BaseUrl& baseUrl);

public:
  /** login canceled by user */
  void loginCanceled();

  /** set RestService path for next operation
   */
  void setPath( const std::string& path );

  /** add RestService filter for next operation
   *  the filter is the last part of the url
   *  and is therefore added to m_path
   */
  void addFilter( std::string filterString );

private:
  static void appendParameter( std::ostringstream& os, const std::string& name, const Json::Value& value );
  static void appendParameterValue( std::ostringstream& os, const Json::Value& value );

public:
  /** set RestService output Stream for next operation
   */
  void setDataStream( Stream *dataStream );

  /** set RestService input Stream for next operation
   */
  void setResponseStream( Stream *responseStream );

  /** set RestService input Stream for next operation
   */
  void setSetDbTimestamp( bool setDbTimestamp );

  /** set RestService calling JobAction (used by WebApi)
   */
  void setCallingAction(JobAction* action);

  /** Sends a GET request.
      Write response to m_responseStream
   */
  JobElement::OpStatus get(bool autoRetry=false, bool silent=false);

  /** Sends a DELETE request.
      Write response to m_responseStream
   */
  JobElement::OpStatus httpdelete();

  /** Sends a PUT request.
      Write response to m_responseStream
   */
  JobElement::OpStatus put();

  /** Sends a POST request.
      Write response to m_responseStream
   */
  JobElement::OpStatus post();

  /** Login
   */
  JobElement::OpStatus login(const std::string &dbname,
                             const std::string &user, const std::string &pw);
  /** Login JWT
   */
  JobElement::OpStatus login(const std::string &dbname,
                             const std::string &jwt);

  /** Logoff
   */
  bool logoff(const bool clearErrorMessage=true);

private:
  /** recall get, delete, put or post */
  JobElement::OpStatus recall( const std::string& method );
  /** call curl_easy_perform and analize response (all baseUrls) */
  JobElement::OpStatus perform( const std::string& method,
                                bool autoRetry,
                                bool silent );
  /** call curl_easy_perform and analize response (one baseUrl) */
  JobElement::OpStatus perform( BaseUrl& baseUrl,
                                const std::string& method,
                                bool silent,
                                bool& curl_ok );
  /** process conflict received from rest service */
  JobElement::OpStatus conflict();

  /** set m_dataData from m_dataStream */
  void set_data();

  /** set curl options for post */
  void set_post_curl_options();

  /** write response to m_responseStream */
  void writeToResponseStream(const std::string& response);

  /** return type, name and revision string of component (m_dataData) */
  std::string componentIdentification(const int newRevision=-1) const;

  /** return type of component (m_dataData) */
  std::string componentType() const;

  static Json::Value parseJson(const std::string &jsonString);
  static Json::Value parseJsonObject(const std::string &jsonString);
  static std::string value2string(const Json::Value &value);

  void writeLogMessage(const std::string &message);

  JobElement::OpStatus executeVersionControl();
  bool getVersionIgnore();
  std::string getDbVersion( int &version_num );
  std::string getDpVersion( const std::string &prefix, int &version_num );

  void afterDbManagerLogin();

public:
  /** write_data : libcurl function to receive data */
  static size_t write_data(void *buffer,
                           size_t size,
                           size_t nmemb,
                           void *userp );

  /** write_data : libcurl function to write data */
  static size_t read_data(char *bufptr,
                          size_t size,
                          size_t nitems,
                          void *userp );

  /** header_data : libcurl function to receive header data */
  static size_t header_data(char *buffer,
                            size_t size,
                            size_t nitems,
                            void *userp );
  /** set RESTUSERNAME datapool variable */
  void setUsername( const std::string& username );
  std::string getUsername();
private:
  void setBase( const std::string& base );
  void updateHeaders();
  void setPassword( const std::string& password );
  void setHeaders( const std::string& authorization );
  void clearAuthHeader();
  void showDialogUserPassword();
  void setLogonStatus( const std::string &status, bool override=false );
  void clearLogonStatus();
  std::string getDialogTitle(std::string fallback = "REST Service");

  std::string   getBaseUrlLockedUntil(const BaseUrls& baseUrls, const std::string& baseUrl);
  void setBaseList( const std::string& selected_base="" );
  void setUsernameList( );
  bool trimUrl( std::string &url );
  std::string  getInputItemLabel(const std::string &subvarname, bool inputLabel=true);
  std::string   getInputItemGuiValue(const std::string &subvarname, const Json::Value& value);
  void decode_token( const std::string &jwt);
  void writeModifications(std::ostream& os, Json::Value& modifications, Json::Value::Members& members, std::string lineprefix);

  std::string   m_base_list;
  std::string   m_base;
  BaseUrls      m_baseUrls;
  int           m_skipPeriod;
  std::string   m_authorization;  // basic or jwt
  std::string   m_username_list;
  std::string   m_username;
  std::string   m_authHeader;
  bool          m_loginCanceled;
  bool          m_envPasswordUsed;
  QTime         m_startTime;
  int           m_msecsCurlPerform;
  int           m_msecsJsonParse;

  std::string   m_path;
  Stream       *m_dataStream;
  Stream       *m_responseStream;
  bool          m_setDbTimestamp; // set db timestamp of dataStream after successful PUT

  std::string   m_errorMessage;
  std::string   m_dataData; // data to send with put
  size_t        m_dataSent; // bytes sent
  std::string   m_responseData;  // data received (put and get)
  JobAction*    m_webApiCallingAction;

  CURL         *m_curlHandle;
  Json::Value   m_user_jwt;

  enum HTTPStatusCode {
    OK=200,
    NoContent=204,
    Unauthorized=401,
    BadRequest=400,
    Conflict=409
  };

  static bool s_userReload;
  static int  s_initialRetry;
  friend class OAuthClient;
};

std::ostream &operator<<( std::ostream &o, const RestService::BaseUrl &url );

#endif  // REST_SERVICE_H
