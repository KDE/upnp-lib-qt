/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPSERVEREVENTOBJECT_H
#define UPNPSERVEREVENTOBJECT_H

#include "upnplibqt_export.h"

#include <KDSoapServer/KDSoapServerCustomVerbRequestInterface.h>
#include <KDSoapServer/KDSoapServerObjectInterface.h>

#include <QObject>

#include <memory>

class UpnpControlAbstractService;
class UpnpServerEventObjectPrivate;

class UpnpServerEventObject : public QObject, public KDSoapServerCustomVerbRequestInterface
{
    Q_OBJECT

    Q_INTERFACES(KDSoapServerCustomVerbRequestInterface)

public:
    explicit UpnpServerEventObject(QObject *parent = nullptr);

    ~UpnpServerEventObject() override;

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

    void setService(UpnpControlAbstractService *service);

private:
    std::unique_ptr<UpnpServerEventObjectPrivate> d;
};

#endif // UPNPSERVEREVENTOBJECT_H
