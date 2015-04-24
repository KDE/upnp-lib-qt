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

#ifndef UPNPSERVEREVENTOBJECT_H
#define UPNPSERVEREVENTOBJECT_H

#include <KDSoapServer/KDSoapServerObjectInterface.h>

#include <QObject>

class UpnpControlAbstractService;
class UpnpServerEventObjectPrivate;

class UpnpServerEventObject : public QObject, public KDSoapServerObjectInterface
{
    Q_OBJECT

    Q_INTERFACES(KDSoapServerObjectInterface)

public:
    UpnpServerEventObject(QObject *parent = 0);

    virtual ~UpnpServerEventObject();

    void processRequest(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction) Q_DECL_OVERRIDE;

    QIODevice* processFileRequest(const QString &path, QByteArray &contentType) Q_DECL_OVERRIDE;

    void processRequestWithPath(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction, const QString &path) Q_DECL_OVERRIDE;

    bool processCustomVerbRequest(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers, QByteArray &customAnswer) Q_DECL_OVERRIDE;

    void setService(UpnpControlAbstractService *service);

private:

    UpnpServerEventObjectPrivate *d;
};

#endif // UPNPSERVEREVENTOBJECT_H
