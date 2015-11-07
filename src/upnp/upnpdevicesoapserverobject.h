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

#ifndef UPNPDEVICESOAPSERVEROBJECT_H
#define UPNPDEVICESOAPSERVEROBJECT_H

#include "upnpQt_export.h"

#include <KDSoapServer/KDSoapServerCustomVerbRequestInterface.h>
#include <KDSoapServer/KDSoapServerObjectInterface.h>

#include <QObject>
#include <QtCore/QList>

class UpnpAbstractDevice;
class UpnpDeviceSoapServerObjectPrivate;

class UpnpDeviceSoapServerObject : public QObject, public KDSoapServerObjectInterface, public KDSoapServerCustomVerbRequestInterface
{
    Q_OBJECT

    Q_INTERFACES(KDSoapServerObjectInterface KDSoapServerCustomVerbRequestInterface)

public:
    UpnpDeviceSoapServerObject(QList<UpnpAbstractDevice *> &devices, QObject *parent = 0);

    virtual ~UpnpDeviceSoapServerObject();

    void processRequest(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction) override;

    QIODevice* processFileRequest(const QString &path, QByteArray &contentType) override;

    void processRequestWithPath(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction, const QString &path) override;

    /**
     * Process a request made with a custom HTTP verb
     * @param requestType HTTP verb other than GET and POST
     * @param requestData is the content of the request
     * @param httpHeaders the map of http headers (keys have been lowercased since they are case insensitive)
     * @param customAnswer allow to send back the answer to the client if the request has been handled
     * @return true if the request has been handled and if customAnswer is valid and will be sent back to the client.
     */
    bool processCustomVerbRequest(const QByteArray &requestType, const QByteArray &requestData,
                                  const QMap<QByteArray, QByteArray> &httpHeaders, QByteArray &customAnswer) override;

private:

    QIODevice* downloadDeviceXmlDescription(UpnpAbstractDevice *device, QByteArray &contentType);

    QIODevice* downloadServiceXmlDescription(UpnpAbstractDevice *device, const int serviceIndex, QByteArray &contentType);

    UpnpDeviceSoapServerObjectPrivate *d;
};

#endif // UPNPDEVICESOAPSERVEROBJECT_H
