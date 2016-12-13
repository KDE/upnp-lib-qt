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

#include "upnpwebsocketeventsubscriber.h"
//#include "upnpabstractservice.h"
//#include "upnpbasictypes.h"

#include "upnpstatevariabledescription.h"

#include <QtCore/QPointer>
#include <QtCore/QBuffer>
#include <QtCore/QUuid>

class UpnpWebSocketEventSubscriberPrivate
{
public:

    int mSecondTimeout = 1000;

    QUrl mCallback;

    QString mUuid;

    quint32 mSequenceCounter = 0;

    //QNetworkAccessManager mNetworkAccess;

    UpnpWebSocketPublisher *mUpnpService = nullptr;

    QPointer<QBuffer> mSentBuffer;

};

UpnpWebSocketEventSubscriber::UpnpWebSocketEventSubscriber(QObject *parent)
    : QObject(parent), d(new UpnpWebSocketEventSubscriberPrivate)
{
    const QString &uuidString(QUuid::createUuid().toString());
    d->mUuid = uuidString.mid(1, uuidString.length() - 2);
}

UpnpWebSocketEventSubscriber::~UpnpWebSocketEventSubscriber()
{
    delete d;
}

void UpnpWebSocketEventSubscriber::setSecondTimeout(int newValue)
{
    d->mSecondTimeout = newValue;
}

int UpnpWebSocketEventSubscriber::secondTimeout() const
{
    return d->mSecondTimeout;
}

void UpnpWebSocketEventSubscriber::setCallback(const QUrl &callbackAddress)
{
    d->mCallback = callbackAddress;
}

const QUrl &UpnpWebSocketEventSubscriber::callback() const
{
    return d->mCallback;
}

const QString &UpnpWebSocketEventSubscriber::uuid() const
{
    return d->mUuid;
}

void UpnpWebSocketEventSubscriber::setPublisher(UpnpWebSocketPublisher *publisher)
{
    d->mUpnpService = publisher;
}

void UpnpWebSocketEventSubscriber::sendEventNotification()
{
    /*QNetworkRequest newRequest(d->mCallback);
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

            const QVariant &propertyValue(d->mUpnpService->property(currentStateVariable.mPropertyName.constData()));
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
    connect(replyHandler, &QNetworkReply::finished, this, &UpnpWebSocketEventSubscriber::eventingFinished);
    connect(replyHandler, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(eventingInErrorFinished(QNetworkReply::NetworkError)));

    d->mSentBuffer = requestBody;*/
}

void UpnpWebSocketEventSubscriber::notifyPropertyChange(const QString &serviceId, const QByteArray &propertyName)
{
    Q_UNUSED(serviceId);
    Q_UNUSED(propertyName);

    /*QNetworkRequest newRequest(d->mCallback);
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
        if (currentStateVariable.mEvented && currentStateVariable.mPropertyName == propertyName) {
            insertStream.writeStartElement(QStringLiteral("urn:schemas-upnp-org:event-1-0"), QStringLiteral("property"));

            const QVariant &propertyValue(d->mUpnpService->property(currentStateVariable.mPropertyName.constData()));
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

    connect(replyHandler, &QNetworkReply::finished, this, &UpnpWebSocketEventSubscriber::eventingFinished);

    d->mSentBuffer = requestBody;
    d->mSentBuffer->seek(0);
    qDebug() << "UpnpWebSocketEventSubscriber::notifyPropertyChange" << d->mSentBuffer->data();*/
}

void UpnpWebSocketEventSubscriber::eventingFinished()
{
    d->mSentBuffer.clear();
}

#if 0
void UpnpWebSocketEventSubscriber::eventingInErrorFinished(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);

    d->mSentBuffer.clear();
}
#endif

#include "moc_upnpwebsocketeventsubscriber.cpp"
