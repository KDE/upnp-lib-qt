/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef UPNPDEVICEDESCRIPTIONPARSER_H
#define UPNPDEVICEDESCRIPTIONPARSER_H

#include "upnplibqt_export.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QSharedPointer>

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
