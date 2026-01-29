
#ifdef HAVE_OAUTH

#include "app/oauthclient.h"
#include "app/AppData.h"
#include "job/JobAction.h"
#include "job/JobManager.h"
#include "job/InitialWorker.h"
#include "utils/Debugger.h"
#include "operator/RestService.h"
#include "gui/DialogUserPassword.h"

#include <QtCore>
#include <QtGui>
#include <QtNetworkAuth>
#include <QApplication>
#include <QWidget>
#include <qoauthhttpserverreplyhandler.h>
#include <qobject.h>

INIT_LOGGER();

OAuthClient::OAuthClient(const QString &clientEndpoint,
                         const QString &accessTokenEndpoint, QObject *parent)
    : QObject(parent) {
  // 2025-09 NOTE use port 3000 !!!
  // this makes mxe windows and QNetworkauth happy
  auto replyHandler = new QOAuthHttpServerReplyHandler(3000);
  oauth2.setReplyHandler(replyHandler);
  auto url = QUrl(clientEndpoint);
  BUG_DEBUG("clientEndpoint: " << clientEndpoint.toStdString());
  BUG_DEBUG("accessTokenEndpoint: " << accessTokenEndpoint.toStdString());

  auto scopesString = AppData::Instance().OAuthScopes();
#if QT_VERSION < 0x060900
  oauth2.setAccessTokenUrl(QUrl(accessTokenEndpoint));
  if (scopesString.empty()) {
    oauth2.setScope("profile roles");
  } else {
    oauth2.setScope(scopesString);
  }
#else
  oauth2.setTokenUrl(QUrl(accessTokenEndpoint));
  if (scopesString.empty()) {
    oauth2.setRequestedScopeTokens(QSet<QByteArray>{"openid", "profile", "roles"});
  } else {
    QSet<QByteArray> scopesSet;
    auto scopesList = QString::fromStdString(scopesString).split(" ");
    for (auto scope : scopesList) {
      scopesSet << QByteArray::fromStdString(scope.toStdString());
    }
    oauth2.setRequestedScopeTokens(scopesSet);
  }
#endif
  oauth2.setAuthorizationUrl(url.toDisplayString(QUrl::RemoveUserInfo));
  oauth2.setClientIdentifier(url.userName());

  QObject::connect(&oauth2, &QOAuth2AuthorizationCodeFlow::statusChanged,this,
                   [this](QAbstractOAuth::Status status) {
                     if (status == QAbstractOAuth::Status::Granted) {
                       BUG_INFO("OAuth login successful")
                         emit authenticated(oauth2.token());
                     }
                   });

  QObject::connect(
                   &oauth2, &QOAuth2AuthorizationCodeFlow::requestFailed,this,
      [this](const QAbstractOAuth2::Error _) { BUG_INFO("OAuth login failed"); });

#if 0
    QObject::connect(&oauth2, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
                     [](const QUrl &url) {
                       // mxe QOAuthHttpServerReplyHandler bug 2025-09
                       // replace redirect_uri to "http://127.0.0.1:3000/"
                       QString s(url.toString());
                       s.replace("redirect_uri=http:/&", "redirect_uri=http://127.0.0.1:3000/&");
                       std::cout << "Open browser from:" << url.toString().toStdString() << std::endl;
                       std::cout << "Open browser to  :" << s.toStdString() << std::endl;
                         QDesktopServices::openUrl(s);
                     });
#else
    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            &QDesktopServices::openUrl);
#endif
  connect(this, SIGNAL(authenticated(QString)), this, SLOT(post_authenticated(QString)));
}

/* --------------------------------------------------------------------------- */
/* grant --                                                                    */
/* --------------------------------------------------------------------------- */
void OAuthClient::grant() {
  BUG_INFO("oauth grant");

  // disable all widgets
  if (AppData::Instance().OAuth().size()) {
    const QWidgetList allWidgets = QApplication::allWidgets();
    for (QWidget *widget : allWidgets) {
      widget->setDisabled(true);
    }
  }
  // grant
  //oauth2.refreshAccessToken();
  oauth2.grant();
}

/* --------------------------------------------------------------------------- */
/* token --                                                                    */
/* --------------------------------------------------------------------------- */
std::string OAuthClient::token() {
  return oauth2.token().toStdString();
}

/* --------------------------------------------------------------------------- */
/* post_authenticated --                                                       */
/* --------------------------------------------------------------------------- */
void OAuthClient::post_authenticated(QString token) {
  BUG_INFO("oauth authenticated, token: " << token.toStdString());

  // set RestService settings (no db login)
  // to be prepared for db operation
  RestService::Instance().setHeaders("Bearer " + token.toStdString());

  // enable all widgets
  const QWidgetList allWidgets = QApplication::allWidgets();
  for (QWidget *widget : allWidgets) {
    widget->setDisabled(false);
  }

  // process
  if (m_listener) {
    m_listener->unmanageDialog();
    return;
  }
}

#endif  // HAVE_OAUTH
