/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPSERVICEDESCRIPTIONPARSER_H
#define UPNPSERVICEDESCRIPTIONPARSER_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QUrl>

#include <memory>

class QNetworkReply;
class QIODevice;
class QNetworkAccessManager;

class UpnpServiceDescription;

class UpnpServiceDescriptionParserPrivate;

class UpnpServiceDescriptionParser : public QObject
{
    Q_OBJECT

public:
    explicit UpnpServiceDescriptionParser(QNetworkAccessManager *aNetworkAccess, UpnpServiceDescription &serviceDescription, QObject *parent = nullptr);

    ~UpnpServiceDescriptionParser() override;

Q_SIGNALS:

    void descriptionParsed(const QString &upnpServiceId);

    void ServiceDescriptionInError(const QString &upnpServiceId);

public Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

    void downloadServiceDescription(const QUrl &serviceUrl);

private:
    void parseServiceDescription(QIODevice *serviceDescriptionContent);

    std::unique_ptr<UpnpServiceDescriptionParserPrivate> d;
};

#endif // UPNPSERVICEDESCRIPTIONPARSER_H
