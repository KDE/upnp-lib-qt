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

class UPNPLIBQT_EXPORT UpnpControlAbstractServiceReply : public QObject
{

    Q_OBJECT

public:
    explicit UpnpControlAbstractServiceReply(const KDSoapPendingCall &soapAnswer, QObject *parent = nullptr);

    ~UpnpControlAbstractServiceReply() override;

    bool success() const;

    QVariantMap result() const;

    QString error() const;

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
