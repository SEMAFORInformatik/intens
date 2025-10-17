// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef OAUTHCLIENT_H
#define OAUTHCLIENT_H

#include <QtCore>
#include <QtNetwork>
#include <QString>

#include <QOAuth2AuthorizationCodeFlow>
class UserPasswordListener;

class OAuthClient : public QObject
{
    Q_OBJECT

public:
    OAuthClient(const QString &clientEndpoint, const QString &accessTokenEndpoint, QObject *parent = nullptr);
    std::string token();
    void setListener(UserPasswordListener* listener) { m_listener = listener; }

public slots:
    void grant();
    void post_authenticated(QString token);

signals:
    void authenticated(QString token);

private:
    QOAuth2AuthorizationCodeFlow oauth2;
    bool permanent = false;
    UserPasswordListener* m_listener = 0;
};

#endif // REDDITWRAPPER_H
