/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpeventsubscriber.h"

#include "upnplogging.h"

#include "upnpabstractservice.h"
#include "upnpbasictypes.h"

#include "upnpstatevariabledescription.h"

#include <QBuffer>
#include <QPointer>
#include <QUuid>
#include <QXmlStreamWriter>

#include <QNetworkReply>
#include <QNetworkRequest>

#include <QLoggingCategory>

class UpnpEventSubscriberPrivate
{
public:
    UpnpEventSubscriberPrivate()
        : mSecondTimeout(1800)
        , mCallback()
        , mUuid()
        , mSequenceCounter(0)
        , mNetworkAccess()
        , mUpnpService(nullptr)
        , mSentBuffer()
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
    : QObject(parent)
    , d(new UpnpEventSubscriberPrivate)
{
    d->mSequenceCounter = 0;

    const QString &uuidString(QUuid::createUuid().toString());
    d->mUuid = uuidString.mid(1, uuidString.length() - 2);
}

UpnpEventSubscriber::~UpnpEventSubscriber()
{
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
    const auto &allStateVariables(d->mUpnpService->stateVariables());
    for (const QString &itVariable : allStateVariables) {
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
    connect(replyHandler, &QNetworkReply::finished, this, &UpnpEventSubscriber::eventingFinished);
    connect(replyHandler, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &UpnpEventSubscriber::eventingInErrorFinished);

    d->mSentBuffer = requestBody;
}

void UpnpEventSubscriber::notifyPropertyChange(const QString &serviceId, const QByteArray &propertyName)
{
    Q_UNUSED(serviceId);

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
    const auto &allStateVariables(d->mUpnpService->stateVariables());
    for (const QString &itVariable : allStateVariables) {
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

    connect(replyHandler, &QNetworkReply::finished, this, &UpnpEventSubscriber::eventingFinished);

    d->mSentBuffer = requestBody;
    d->mSentBuffer->seek(0);
    qCDebug(orgKdeUpnpLibQtUpnp()) << "UpnpEventSubscriber::notifyPropertyChange" << d->mSentBuffer->data();
}

void UpnpEventSubscriber::eventingFinished()
{
    d->mSentBuffer.clear();
}

void UpnpEventSubscriber::eventingInErrorFinished(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);

    d->mSentBuffer.clear();
}

#include "moc_upnpeventsubscriber.cpp"
