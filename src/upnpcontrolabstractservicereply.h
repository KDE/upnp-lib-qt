/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPCONTROLABSTRACTSERVICEREPLY_H
#define UPNPCONTROLABSTRACTSERVICEREPLY_H

#include "upnplibqt_export.h"

#include <QObject>
#include <QVariantMap>

#include <memory>

class UpnpControlAbstractServiceReplyPrivate;
class KDSoapPendingCall;

/**
 * @brief The UpnpControlAbstractServiceReply class is used to keep track of answers when calling actions from UPnP services
 *
 * It allows to know the status of the action call (.e. success or failure) and get the result or the returned error
 */
class UPNPLIBQT_EXPORT UpnpControlAbstractServiceReply : public QObject
{

    Q_OBJECT

public:
    explicit UpnpControlAbstractServiceReply(const KDSoapPendingCall &soapAnswer, QObject *parent = nullptr);

    ~UpnpControlAbstractServiceReply() override;

    [[nodiscard]] bool success() const;

    [[nodiscard]] QVariantMap result() const;

    [[nodiscard]] QString error() const;

Q_SIGNALS:

    void finished(UpnpControlAbstractServiceReply *self);

public Q_SLOTS:

    void callFinished();

protected:
    virtual void parseAnswer();

private:
    std::unique_ptr<UpnpControlAbstractServiceReplyPrivate> d;
};

#endif // UPNPCONTROLABSTRACTSERVICEREPLY_H
