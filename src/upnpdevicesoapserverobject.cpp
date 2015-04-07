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

#include "upnpdevicesoapserverobject.h"

#include "upnpabstractdevice.h"

#include <QtCore/QDebug>

class UpnpDeviceSoapServerObjectPrivate
{
public:

    UpnpDeviceSoapServerObjectPrivate(QMap<QString, UpnpAbstractDevice *> &devices) : mDevices(devices)
    {
    }

    QMap<QString, UpnpAbstractDevice *> &mDevices;
};

UpnpDeviceSoapServerObject::UpnpDeviceSoapServerObject(QMap<QString, UpnpAbstractDevice *> &devices, QObject *parent)
    : QObject(parent), KDSoapServerObjectInterface(), d(new UpnpDeviceSoapServerObjectPrivate(devices))
{
}

UpnpDeviceSoapServerObject::~UpnpDeviceSoapServerObject()
{
    delete d;
}

void UpnpDeviceSoapServerObject::processRequest(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction)
{
    Q_UNUSED(request);
    Q_UNUSED(response);
    Q_UNUSED(request);
    Q_UNUSED(soapAction);

    qDebug() << "UpnpDeviceSoapServerObject::processRequest" << request.name();
}

QIODevice *UpnpDeviceSoapServerObject::processFileRequest(const QString &path, QByteArray &contentType)
{
    if (path.startsWith(QStringLiteral("/device.xml?"))) {
        const QString &deviceUuid(path.right(path.length() - 12));
        if (d->mDevices.contains(deviceUuid)) {
            return downloadDeviceXmlDescription(d->mDevices[deviceUuid], contentType);
        }
    }

    return nullptr;
}

void UpnpDeviceSoapServerObject::processRequestWithPath(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction, const QString &path)
{
    Q_UNUSED(request);
    Q_UNUSED(response);
    Q_UNUSED(soapAction);
    Q_UNUSED(path);

    qDebug() << "UpnpDeviceSoapServerObject::processRequestWithPath" << path << request.name();
}

bool UpnpDeviceSoapServerObject::processCustomVerbRequest(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers)
{
    Q_UNUSED(requestData);
    Q_UNUSED(headers);

    return false;
}

QIODevice *UpnpDeviceSoapServerObject::downloadDeviceXmlDescription(UpnpAbstractDevice *device, QByteArray &contentType)
{
    qDebug() << "UpnpDeviceSoapServerObject::downloadDeviceXmlDescription" << device->UDN();

    contentType = "text/xml";

    return device->buildAndGetXmlDescription();
}

#include "moc_upnpdevicesoapserverobject.cpp"
