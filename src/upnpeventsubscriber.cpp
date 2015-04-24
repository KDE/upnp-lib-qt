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

#include "upnpeventsubscriber.h"
#include "upnpabstractservice.h"
#include "upnpbasictypes.h"

#include <QtCore/QPointer>
#include <QtCore/QBuffer>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QUuid>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class UpnpEventSubscriberPrivate
{
public:

    UpnpEventSubscriberPrivate()
        : mSecondTimeout(1800), mCallback(), mUuid(), mSequenceCounter(0), mNetworkAccess(),
          mUpnpService(nullptr), mSentBuffer()
    {
    }

    int mSecondTimeout;

    QUrl mCallback;

    QString mUuid;

    quint32 mSequenceCounter;

    QNetworkAccessManager mNetworkAccess;

    UpnpAbstractService *mUpnpService;

    QPointer<QBuffer> mSentBuffer;

};

UpnpEventSubscriber::UpnpEventSubscriber(QObject *parent)
    : QObject(parent), d(new UpnpEventSubscriberPrivate)
{
    d-> mSequenceCounter = 0;

    const QString &uuidString(QUuid::createUuid().toString());
    d->mUuid = uuidString.mid(1, uuidString.length() - 2);
}

UpnpEventSubscriber::~UpnpEventSubscriber()
{
    delete d;
}

void UpnpEventSubscriber::setSecondTimeout(int newValue)
{
    d->mSecondTimeout = newValue;
}

int UpnpEventSubscriber::secondTimeout() const
{
    return d->mSecondTimeout;
}

void UpnpEventSubscriber::setCallback(const QUrl &callbackAddress)
{
    d->mCallback = callbackAddress;
}

const QUrl &UpnpEventSubscriber::callback() const
{
    return d->mCallback;
}

const QString &UpnpEventSubscriber::uuid() const
{
    return d->mUuid;
}

void UpnpEventSubscriber::setUpnpService(UpnpAbstractService *service)
{
    d->mUpnpService = service;
}

void UpnpEventSubscriber::sendEventNotification()
{
    QNetworkRequest newRequest(d->mCallback);
    newRequest.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("text/xml"));
    newRequest.setRawHeader("NT", "upnp:event");
    newRequest.setRawHeader("NTS", "upnp:propchange");
    QString sidHeader = QStringLiteral("uuid:") + d->mUuid;
    newRequest.setRawHeader("SID", sidHeader.toLatin1());
    newRequest.setRawHeader("SEQ", QByteArray::number(d->mSequenceCounter));

    QPointer<QBuffer> requestBody(new QBuffer);
    requestBody->open(QIODevice::ReadWrite);

    QXmlStreamWriter insertStream(requestBody.data());
    insertStream.setAutoFormatting(true);

    insertStream.writeStartDocument(QStringLiteral("1.0"));
    insertStream.writeNamespace(QStringLiteral("urn:schemas-upnp-org:event-1-0"), QStringLiteral("e"));
    insertStream.writeStartElement(QStringLiteral("urn:schemas-upnp-org:event-1-0"), QStringLiteral("propertyset"));
    const QList<QString> &allStateVariables(d->mUpnpService->stateVariables());
    for(const QString &itVariable : allStateVariables) {
        const UpnpStateVariableDescription &currentStateVariable(d->mUpnpService->stateVariable(itVariable));
        if (currentStateVariable.mEvented) {
            insertStream.writeStartElement(QStringLiteral("urn:schemas-upnp-org:event-1-0"), QStringLiteral("property"));

            const QVariant &propertyValue(d->mUpnpService->property(currentStateVariable.mPropertyName.toLatin1().constData()));
            if (propertyValue.canConvert<bool>()) {
                insertStream.writeTextElement(itVariable, propertyValue.toBool() ? QStringLiteral("1") : QStringLiteral("0"));
            } else {
                insertStream.writeTextElement(itVariable, propertyValue.toString());
            }

            insertStream.writeEndElement();
        }
    }
    insertStream.writeEndElement();
    insertStream.writeEndDocument();
    requestBody->seek(0);

    QNetworkReply *replyHandler = d->mNetworkAccess.sendCustomRequest(newRequest, "NOTIFY", requestBody.data());
    connect(replyHandler, &QNetworkReply::finished, this, &UpnpEventSubscriber::eventingFinished);

    d->mSentBuffer = requestBody;
}

void UpnpEventSubscriber::eventingFinished()
{
    d->mSentBuffer.clear();
}

#include "moc_upnpeventsubscriber.cpp"
