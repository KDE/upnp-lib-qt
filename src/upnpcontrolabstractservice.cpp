/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpcontrolabstractservice.h"

#include "upnplogging.h"

#include "upnpbasictypes.h"
#include "upnphttpserver.h"
#include "upnpservereventobject.h"

#include "upnpactiondescription.h"
#include "upnpservicedescription.h"

#include <KDSoapClient/KDSoapClientInterface.h>
#include <KDSoapClient/KDSoapMessage.h>

#include <QDnsLookup>
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkReply>

#include <QDomDocument>

#include <QBuffer>
#include <QPointer>
#include <QTextStream>
#include <QTimer>

#include <QLoggingCategory>

class UpnpAbstractServiceDescriptionPrivate
{
public:
    QNetworkAccessManager mNetworkAccess;

    std::unique_ptr<KDSoapClientInterface> mInterface;

    UpnpHttpServer mEventServer;

    QHostAddress mPublicAddress;

    std::unique_ptr<QTimer> mEventSubscriptionTimer;

    int mRealEventSubscriptionTimeout = 0;
};

UpnpControlAbstractService::UpnpControlAbstractService(QObject *parent)
    : UpnpAbstractService(parent)
    , d(std::make_unique<UpnpAbstractServiceDescriptionPrivate>())
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpControlAbstractService::finishedDownload);

    d->mEventServer.setService(this);

    d->mEventServer.listen(QHostAddress::Any);

    const QList<QHostAddress> &list = QNetworkInterface::allAddresses();
    for (const auto &address : list) {
        if (!address.isLoopback()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                d->mPublicAddress = address;
                break;
            }
        }
    }
}

UpnpControlAbstractService::~UpnpControlAbstractService() = default;

UpnpControlAbstractServiceReply *UpnpControlAbstractService::callAction(const QString &actionName, const QVector<QVariant> &arguments)
{
    KDSoapMessage message;

    const UpnpActionDescription &actionDescription(action(actionName));

    auto itArgumentName = actionDescription.mArguments.begin();
    auto itArgumentValue = arguments.begin();
    for (; itArgumentName != actionDescription.mArguments.end() && itArgumentValue != arguments.end(); ++itArgumentName) {
        message.addArgument(itArgumentName->mName, itArgumentValue->toString());

        ++itArgumentValue;
        if (itArgumentValue == arguments.end()) {
            break;
        }
    }

    if (!d->mInterface) {
        d->mInterface = std::make_unique<KDSoapClientInterface>(description().controlURL().toString(), description().serviceType());
        d->mInterface->setSoapVersion(KDSoapClientInterface::SOAP1_1);
        d->mInterface->setStyle(KDSoapClientInterface::RPCStyle);
    }

    return new UpnpControlAbstractServiceReply(d->mInterface->asyncCall(actionName, message, description().serviceType() + QStringLiteral("#") + actionName), this);
}

void UpnpControlAbstractService::subscribeEvents(int duration)
{
    QString webServerAddess(QStringLiteral("<http://"));

    if (!d->mPublicAddress.isNull()) {
        webServerAddess += d->mPublicAddress.toString();
    } else {
        webServerAddess += QStringLiteral("127.0.0.1");
    }

    webServerAddess += QStringLiteral(":") + QString::number(d->mEventServer.serverPort()) + QStringLiteral(">");

    QNetworkRequest myRequest(description().eventURL());
    myRequest.setRawHeader("CALLBACK", webServerAddess.toUtf8());
    myRequest.setRawHeader("NT", "upnp:event");
    QString timeoutDefinition(QStringLiteral("Second-"));
    timeoutDefinition += QString::number(duration);
    myRequest.setRawHeader("TIMEOUT", timeoutDefinition.toLatin1());

    d->mNetworkAccess.sendCustomRequest(myRequest, "SUBSCRIBE");
}

void UpnpControlAbstractService::handleEventNotification(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers)
{
    Q_UNUSED(headers)

    const QString &requestAnswer(QString::fromLatin1(requestData));

    QDomDocument requestDocument;
    requestDocument.setContent(requestAnswer);

    const QDomElement &eventRoot = requestDocument.documentElement();

    const QDomElement &propertysetRoot = eventRoot.firstChildElement();
    if (!propertysetRoot.isNull()) {
        const QDomElement &propertyNode = propertysetRoot.firstChildElement();
        if (!propertyNode.isNull()) {
            QDomNode currentChild = propertyNode.firstChild();
            if (!currentChild.isNull()) {
                parseEventNotification(propertyNode.tagName(), currentChild.toCharacterData().data());
            }
        }
    }
}

void UpnpControlAbstractService::downloadServiceDescription(const QUrl &serviceUrl)
{
    d->mNetworkAccess.get(QNetworkRequest(serviceUrl));
}

void UpnpControlAbstractService::finishedDownload(QNetworkReply *reply)
{
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        if (reply->url() == description().eventURL()) {
            if (reply->hasRawHeader("TIMEOUT")) {
                if (reply->rawHeader("TIMEOUT").startsWith("Second-")) {
                    d->mRealEventSubscriptionTimeout = reply->rawHeader("TIMEOUT").mid(7).toInt();

                    if (!d->mEventSubscriptionTimer) {
                        d->mEventSubscriptionTimer = std::make_unique<QTimer>();
                        connect(d->mEventSubscriptionTimer.get(), &QTimer::timeout, this, &UpnpControlAbstractService::eventSubscriptionTimeout);
                        d->mEventSubscriptionTimer->setInterval(1000 * (d->mRealEventSubscriptionTimeout > 60 ? d->mRealEventSubscriptionTimeout - 60 : d->mRealEventSubscriptionTimeout));
                        d->mEventSubscriptionTimer->start();
                    }
                }
            }
        } else {
            parseServiceDescription(reply);
        }
    } else if (reply->isFinished()) {
        qCDebug(orgKdeUpnpLibQtUpnp()) << "UpnpAbstractServiceDescription::finishedDownload"
                                       << "error";
    }
}

void UpnpControlAbstractService::eventSubscriptionTimeout()
{
    subscribeEvents(d->mRealEventSubscriptionTimeout);
}

void UpnpControlAbstractService::parseServiceDescription(QIODevice *serviceDescriptionContent)
{
    QDomDocument serviceDescriptionDocument;
    serviceDescriptionDocument.setContent(serviceDescriptionContent);

    const QDomElement &scpdRoot = serviceDescriptionDocument.documentElement();

    const QDomElement &actionListRoot = scpdRoot.firstChildElement(QStringLiteral("actionList"));
    QDomNode currentChild = actionListRoot.firstChild();
    while (!currentChild.isNull()) {
        const QDomNode &nameNode = currentChild.firstChildElement(QStringLiteral("name"));

        QString actionName;
        if (!nameNode.isNull()) {
            actionName = nameNode.toElement().text();
        }

        UpnpActionDescription newAction;

        newAction.mName = actionName;

        const QDomNode &argumentListNode = currentChild.firstChildElement(QStringLiteral("argumentList"));
        QDomNode argumentNode = argumentListNode.firstChild();
        while (!argumentNode.isNull()) {
            const QDomNode &argumentNameNode = argumentNode.firstChildElement(QStringLiteral("name"));
            const QDomNode &argumentDirectionNode = argumentNode.firstChildElement(QStringLiteral("direction"));
            const QDomNode &argumentRetvalNode = argumentNode.firstChildElement(QStringLiteral("retval"));
            const QDomNode &argumentRelatedStateVariableNode = argumentNode.firstChildElement(QStringLiteral("relatedStateVariable"));

            UpnpActionArgumentDescription newArgument;
            newArgument.mName = argumentNameNode.toElement().text();
            newArgument.mDirection = (argumentDirectionNode.toElement().text() == QStringLiteral("in") ? UpnpArgumentDirection::In : UpnpArgumentDirection::Out);
            newArgument.mIsReturnValue = !argumentRetvalNode.isNull();
            newArgument.mRelatedStateVariable = argumentRelatedStateVariableNode.toElement().text();

            newAction.mArguments.push_back(newArgument);

            addAction(newAction);

            argumentNode = argumentNode.nextSibling();
        }

        currentChild = currentChild.nextSibling();
    }

#if 0
    const QDomElement &serviceStateTableRoot = scpdRoot.firstChildElement(QStringLiteral("serviceStateTable"));
    currentChild = serviceStateTableRoot.firstChild();
    while (!currentChild.isNull()) {
        const QDomNode &nameNode = currentChild.firstChildElement(QStringLiteral("name"));
        if (!nameNode.isNull()) {
            qCDebug(orgKdeUpnpLibQtUpnp()) << "state variable name" << nameNode.toElement().text();
        }

        currentChild = currentChild.nextSibling();
    }
#endif
}

void UpnpControlAbstractService::parseEventNotification(const QString &eventName, const QString &eventValue)
{
    Q_UNUSED(eventName)
    Q_UNUSED(eventValue)
}

#include "moc_upnpcontrolabstractservice.cpp"
