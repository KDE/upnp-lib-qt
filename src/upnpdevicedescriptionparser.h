/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPDEVICEDESCRIPTIONPARSER_H
#define UPNPDEVICEDESCRIPTIONPARSER_H

#include "upnplibqt_export.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QUrl>

#include <memory>

class QNetworkReply;
class QIODevice;
class QNetworkAccessManager;

class UpnpDeviceDescription;

class UpnpDeviceDescriptionParserPrivate;

class UPNPLIBQT_EXPORT UpnpDeviceDescriptionParser : public QObject
{
    Q_OBJECT

public:
    explicit UpnpDeviceDescriptionParser(QNetworkAccessManager *aNetworkAccess, UpnpDeviceDescription &deviceDescription, QObject *parent = nullptr);

    ~UpnpDeviceDescriptionParser() override;

Q_SIGNALS:

    void descriptionParsed(const QString &UDN);

    void deviceDescriptionInError(const QString &UDN);

public Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

    void downloadDeviceDescription(const QUrl &deviceUrl);

private Q_SLOTS:

    void serviceDescriptionParsed(const QString &upnpServiceId);

private:
    void parseDeviceDescription(QIODevice *deviceDescriptionContent, const QString &fallBackURLBase);

    std::unique_ptr<UpnpDeviceDescriptionParserPrivate> d;
};

#endif // UPNPDEVICEDESCRIPTIONPARSER_H
