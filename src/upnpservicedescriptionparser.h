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

#ifndef UPNPSERVICEDESCRIPTIONPARSER_H
#define UPNPSERVICEDESCRIPTIONPARSER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>

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
