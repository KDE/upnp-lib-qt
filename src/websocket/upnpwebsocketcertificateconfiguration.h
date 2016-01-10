/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef UPNPWEBSOCKETCERTIFICATECONFIGURATION_H
#define UPNPWEBSOCKETCERTIFICATECONFIGURATION_H

#include "upnpQtWebSocket_export.h"

#include <QtCore/QObject>
#include <QtCore/QString>

class UpnpSsdpCertificateConfigurationPrivate;
class QSslConfiguration;

class UPNPQTWEBSOCKET_EXPORT UpnpWebSocketCertificateConfiguration : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString certificateAuthorityFileName
               READ certificateAuthorityFileName
               WRITE setCertificateAuthorityFileName
               NOTIFY certificateAuthorityFileNameChanged)

    Q_PROPERTY(QString certificateFileName
               READ certificateFileName
               WRITE setCertificateFileName
               NOTIFY certificateFileNameChanged)

public:

    explicit UpnpWebSocketCertificateConfiguration(QObject *parent = 0);

    virtual ~UpnpWebSocketCertificateConfiguration();

    const QString& certificateAuthorityFileName() const;

    void setCertificateAuthorityFileName(const QString &value);

    const QString& certificateFileName() const;

    void setCertificateFileName(const QString &value);

    void initialize(QSslConfiguration *configuration);

Q_SIGNALS:

    void certificateAuthorityFileNameChanged();

    void certificateFileNameChanged();

public Q_SLOTS:

private:

    UpnpSsdpCertificateConfigurationPrivate *d;

};

#endif // UPNPSSDPCERTIFICATECONFIGURATION_H
