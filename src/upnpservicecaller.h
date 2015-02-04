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

#ifndef UPNPSERVICECALLER_H
#define UPNPSERVICECALLER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariantList>
#include <QtCore/QUrl>

class UpnpServiceCallerPrivate;
class QNetworkReply;

class UpnpServiceCaller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString endPoint
               READ endPoint
               WRITE setEndPoint
               NOTIFY endPointChanged)

    Q_PROPERTY(QString messageNamespace
               READ messageNamespace
               WRITE setMessageNamespace
               NOTIFY messageNamespaceChanged)

public:
    explicit UpnpServiceCaller(const QString &endPoint, const QString &messageNamespace, QObject *parent = 0);

    explicit UpnpServiceCaller(QObject *parent = 0);

    ~UpnpServiceCaller();

    void setEndPoint(const QString &newEndPoint);

    const QString& endPoint() const;

    void setMessageNamespace(const QString &newMessageNamespace);

    const QString& messageNamespace() const;

Q_SIGNALS:

    void endPointChanged();

    void messageNamespaceChanged();

public Q_SLOTS:

    void downloadAndParseServiceDescription(const QUrl &serviceUrl);

    void asyncCallService(const QString &name, const QVariantList &parameters);

private Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

private:

    UpnpServiceCallerPrivate *d;
};

#endif // UPNPSERVICECALLER_H
