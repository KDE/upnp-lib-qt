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
#include "upnpabstractservice.h"
#include "upnpeventsubscriber.h"
#include "upnpbasictypes.h"

#include "upnpactiondescription.h"
#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include "KDSoapClient/KDSoapValue.h"
#include "KDSoapClient/KDSoapValue.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QSysInfo>
#include <QtCore/QPair>

class UpnpDeviceSoapServerObjectPrivate
{
public:

    explicit UpnpDeviceSoapServerObjectPrivate(QList<UpnpAbstractDevice *> &devices) : mDevices(devices)
    {
    }

    QList<UpnpAbstractDevice *> &mDevices;
};

UpnpDeviceSoapServerObject::UpnpDeviceSoapServerObject(QList<UpnpAbstractDevice *> &devices, QObject *parent)
    : QObject(parent), KDSoapServerObjectInterface(), d(new UpnpDeviceSoapServerObjectPrivate(devices))
{
}

UpnpDeviceSoapServerObject::~UpnpDeviceSoapServerObject()
{
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
    const QList<QString> &pathParts = path.split(QStringLiteral("/"));
    if (pathParts.count() == 3 && pathParts.last() == QStringLiteral("device.xml")) {
        const int deviceIndex = pathParts[1].toInt();
        if (deviceIndex >= 0 && deviceIndex < d->mDevices.count()) {
            return downloadDeviceXmlDescription(d->mDevices[deviceIndex], contentType);
        }
    } else if (pathParts.count() == 4 && pathParts.last() == QStringLiteral("service.xml")) {
        const int deviceIndex = pathParts[1].toInt();
        const int serviceIndex = pathParts[2].toInt();
        if (deviceIndex >= 0 && deviceIndex < d->mDevices.count()) {
            return downloadServiceXmlDescription(d->mDevices[deviceIndex], serviceIndex, contentType);
        }
    }

    return nullptr;
}

void UpnpDeviceSoapServerObject::processRequestWithPath(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction, const QString &path)
{
    Q_UNUSED(request);
    Q_UNUSED(response);
    Q_UNUSED(soapAction);

    qDebug() << "UpnpDeviceSoapServerObject::processRequestWithPath" << path << request.name();

    const QList<QString> &pathParts = path.split(QStringLiteral("/"));
    if (pathParts.count() != 4 || pathParts.last() != QStringLiteral("control")) {
        response.setFault(true);
        return;
    }

    const int deviceIndex = pathParts[1].toInt();
    const int serviceIndex = pathParts[2].toInt();

    if (deviceIndex < 0 || deviceIndex >= d->mDevices.count()) {
        response.setFault(true);
        return;
    }
    UpnpAbstractDevice *currentDevice = d->mDevices[deviceIndex];

    if (serviceIndex < 0 || serviceIndex >= currentDevice->services().count()) {
        response.setFault(true);
        return;
    }
    UpnpServiceDescription *currentService = currentDevice->serviceDescriptionByIndex(serviceIndex);

    const QList<QByteArray> &soapActionParts = soapAction.split('#');
    if (soapActionParts.size() != 2 || soapActionParts.first() != currentService->serviceType().toLatin1()) {
        response.setFault(true);
        return;
    }

    const QByteArray &actionName = soapActionParts.last();
    const QString &actionNameString = QString::fromLatin1(actionName);

    response = KDSoapValue(actionNameString + QStringLiteral("Response"), QVariant(), currentService->serviceType());

    const KDSoapValueList &allArguments(request.arguments());
    const UpnpActionDescription &currentAction = currentService->action(actionNameString);
    qDebug() << "allArguments" << allArguments << "action arguments" << currentAction.mNumberInArgument;

    QVector<QVariant> checkedArguments;
    bool argumentError = false;

    for (int argumentIndexMessage = 0, actionArgumentIndex = 0; !argumentError; ) {
        if (actionArgumentIndex >= currentAction.mArguments.size()) {
            break;
        }
        if (currentAction.mArguments[actionArgumentIndex].mDirection == UpnpArgumentDirection::Out) {
            ++actionArgumentIndex;
            continue;
        }
        if (argumentIndexMessage < allArguments.size() && argumentIndexMessage < currentAction.mNumberInArgument) {
            if (allArguments[argumentIndexMessage].name() == currentAction.mArguments[argumentIndexMessage].mName) {
                checkedArguments.push_back(allArguments[argumentIndexMessage].value());
            } else {
                qDebug() << "invalid argument";
                argumentError = true;
            }
        } else {
            if (argumentIndexMessage == currentAction.mNumberInArgument && argumentIndexMessage == allArguments.size()) {
                break;
            }

            if (argumentIndexMessage < currentAction.mNumberInArgument) {
                qDebug() << "missing arguments";
            }
            argumentError = true;
        }
        ++argumentIndexMessage;
        ++actionArgumentIndex;
    }

    if (argumentError) {
        qDebug() << "error about arguments";
        response.setFault(true);
        return;
    } else {
        bool actionCallIsInError = false;

        //const QList<QPair<QString, QVariant> > &returnedValues(currentService->invokeAction(actionNameString, checkedArguments, actionCallIsInError));

        if (actionCallIsInError) {
            response.setFault(true);
            return;
        } else {
            response.setFault(false);
        }

        response.setType(currentService->serviceType(), actionNameString + QStringLiteral("Response"));
        /*for (const QPair<QString, QVariant> &oneValue : returnedValues) {
            response.addArgument(oneValue.first, oneValue.second);
        }*/
    }
}

bool UpnpDeviceSoapServerObject::processCustomVerbRequest(const QByteArray &requestType, const QByteArray &requestData,
                                                          const QMap<QByteArray, QByteArray> &httpHeaders, QByteArray &customAnswer)
{
    if (requestType == "SUBSCRIBE") {
        const QString &path = QString::fromLatin1(httpHeaders.value("_path"));
        const QList<QString> &pathParts = path.split(QStringLiteral("/"));
        if (pathParts.count() == 4 && pathParts.last() == QStringLiteral("event")) {
            const int deviceIndex = pathParts[1].toInt();
            const int serviceIndex = pathParts[2].toInt();
            if (deviceIndex >= 0 && deviceIndex < d->mDevices.count()) {
                UpnpAbstractDevice *currentDevice = d->mDevices[deviceIndex];
                if (serviceIndex >= 0 && serviceIndex < currentDevice->services().count()) {
                    //QPointer<UpnpEventSubscriber> newSubscriber = currentDevice->serviceByIndex(serviceIndex)->subscribeToEvents(requestData, httpHeaders);

                    customAnswer  = "HTTP/1.1 200 OK\r\n";
                    customAnswer += "DATE: " + QDateTime::currentDateTime().toString(QStringLiteral("ddd, d MMM yyyy HH:mm:ss t")).toLatin1() + "\r\n";
                    //customAnswer += "SID: uuid:" + newSubscriber->uuid().toLatin1() + "\r\n";
                    customAnswer += "SERVER: " + QSysInfo::kernelType().toLatin1() + " " + QSysInfo::kernelVersion().toLatin1() + " UPnP/1.0 test/1.0\r\n";
                    //customAnswer += "TIMEOUT:Second-" + QByteArray::number(newSubscriber->secondTimeout()) + "\r\n";
                    customAnswer += "\r\n";

                    return true;
                }
            }
        }
    } else if (requestType == "UNSUBSCRIBE") {
        const QString &path = QString::fromLatin1(httpHeaders.value("_path"));
        const QList<QString> &pathParts = path.split(QStringLiteral("/"));
        if (pathParts.count() == 4 && pathParts.last() == QStringLiteral("event")) {
            const int deviceIndex = pathParts[1].toInt();
            const int serviceIndex = pathParts[2].toInt();
            if (deviceIndex >= 0 && deviceIndex < d->mDevices.count()) {
                UpnpAbstractDevice *currentDevice = d->mDevices[deviceIndex];
                if (serviceIndex >= 0 && serviceIndex < currentDevice->services().count()) {
                    //currentDevice->serviceByIndex(serviceIndex)->unsubscribeToEvents(requestData, httpHeaders);

                    customAnswer  = "HTTP/1.1 200 OK\r\n";
                    customAnswer += "DATE: " + QDateTime::currentDateTime().toString(QStringLiteral("ddd, d MMM yyyy HH:mm:ss t")).toLatin1() + "\r\n";
                    //customAnswer += "SID: uuid:" + newSubscriber->uuid().toLatin1() + "\r\n";
                    customAnswer += "SERVER: " + QSysInfo::kernelType().toLatin1() + " " + QSysInfo::kernelVersion().toLatin1() + " UPnP/1.0 test/1.0\r\n";
                    //customAnswer += "TIMEOUT:Second-" + QByteArray::number(newSubscriber->secondTimeout()) + "\r\n";
                    customAnswer += "\r\n";

                    return true;
                }
            }
        }
    } else {
        qDebug() << "UpnpDeviceSoapServerObject::processCustomVerbRequest" << requestData << httpHeaders;
    }

    return false;
}

QIODevice *UpnpDeviceSoapServerObject::downloadDeviceXmlDescription(UpnpAbstractDevice *device, QByteArray &contentType)
{
    qDebug() << "UpnpDeviceSoapServerObject::downloadDeviceXmlDescription" << device->description()->UDN();

    contentType = "text/xml";

    return device->buildAndGetXmlDescription();
}

QIODevice *UpnpDeviceSoapServerObject::downloadServiceXmlDescription(UpnpAbstractDevice *device, const int serviceIndex, QByteArray &contentType)
{
    qDebug() << "UpnpDeviceSoapServerObject::downloadServiceXmlDescription" << device->description()->UDN() << serviceIndex;

    contentType = "text/xml";

    return nullptr /*device->serviceByIndex(serviceIndex)->buildAndGetXmlDescription()*/;
}

#include "moc_upnpdevicesoapserverobject.cpp"
