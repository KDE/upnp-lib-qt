/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPDEVICESOAPSERVEROBJECT_H
#define UPNPDEVICESOAPSERVEROBJECT_H

#include "upnplibqt_export.h"

#include <KDSoapServer/KDSoapServerCustomVerbRequestInterface.h>
#include <KDSoapServer/KDSoapServerObjectInterface.h>

#include <QList>
#include <QObject>

#include <memory>

class UpnpAbstractDevice;
class UpnpDeviceSoapServerObjectPrivate;

class UpnpDeviceSoapServerObject : public QObject, public KDSoapServerObjectInterface, public KDSoapServerCustomVerbRequestInterface
{
    Q_OBJECT

    Q_INTERFACES(KDSoapServerObjectInterface KDSoapServerCustomVerbRequestInterface)

public:
    explicit UpnpDeviceSoapServerObject(QList<UpnpAbstractDevice *> &devices, QObject *parent = nullptr);

    ~UpnpDeviceSoapServerObject() override;

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
    std::unique_ptr<QIODevice> downloadDeviceXmlDescription(UpnpAbstractDevice *device, QByteArray &contentType);

    std::unique_ptr<QIODevice> downloadServiceXmlDescription(UpnpAbstractDevice *device, const int serviceIndex, QByteArray &contentType);

    std::unique_ptr<UpnpDeviceSoapServerObjectPrivate> d;
};

#endif // UPNPDEVICESOAPSERVEROBJECT_H
