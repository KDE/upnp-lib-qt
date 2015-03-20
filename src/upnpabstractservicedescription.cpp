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

#include "upnpabstractservicedescription.h"
#include "upnphttpserver.h"
#include "upnpservereventobject.h"

#include <KDSoapClient/KDSoapClientInterface.h>
#include <KDSoapClient/KDSoapMessage.h>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QDnsLookup>
#include <QtNetwork/QNetworkInterface>

#include <QtCore/QBuffer>
#include <QtCore/QTextStream>

#include <QtXml/QDomDocument>

enum class UpnpArgumentDirection
{
    In,
    Out,
};

class UpnpActionArgumentDescription
{
public:

    QString mName;

    UpnpArgumentDirection mDirection;

    bool mIsReturnValue;

    QString mRelatedStateVariable;
};

class UpnpActionDescription
{
public:

    QString mName;

    QList<UpnpActionArgumentDescription> mArguments;
};

class UpnpAbstractServiceDescriptionPrivate
{
public:

    UpnpAbstractServiceDescriptionPrivate()
        : mNetworkAccess(), mServiceType(), mServiceId(), mBaseURL(), mSCPDURL(), mControlURL(),
          mEventSubURL(), mActions(), mInterface(nullptr), mEventServer()
    {
    }

    QNetworkAccessManager mNetworkAccess;

    QVariant mServiceType;

    QVariant mServiceId;

    QVariant mBaseURL;

    QVariant mSCPDURL;

    QVariant mControlURL;

    QVariant mEventSubURL;

    QMap<QString, UpnpActionDescription> mActions;

    KDSoapClientInterface *mInterface;

    UpnpHttpServer mEventServer;

    QHostAddress mPublicAddress;
};

UpnpAbstractServiceDescription::UpnpAbstractServiceDescription(QObject *parent)
    : QObject(parent), d(new UpnpAbstractServiceDescriptionPrivate)
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpAbstractServiceDescription::finishedDownload);

    d->mEventServer.setService(this);

    d->mEventServer.listen(QHostAddress::Any);

    const QList<QHostAddress> &list = QNetworkInterface::allAddresses();
    for (auto address = list.begin(); address != list.end(); ++address) {
        if (!address->isLoopback()) {
            if (address->protocol() == QAbstractSocket::IPv4Protocol) {
                d->mPublicAddress = *address;
                break;
            }
        }
    }
}

UpnpAbstractServiceDescription::~UpnpAbstractServiceDescription()
{
    delete d->mInterface;
    delete d;
}

void UpnpAbstractServiceDescription::setBaseURL(const QVariant &newBaseURL)
{
    d->mBaseURL = newBaseURL;
    Q_EMIT baseURLChanged();
}

const QVariant &UpnpAbstractServiceDescription::baseURL() const
{
    return d->mBaseURL;
}

void UpnpAbstractServiceDescription::setServiceType(const QVariant &newServiceType)
{
    d->mServiceType = newServiceType;
    Q_EMIT serviceTypeChanged();
}

const QVariant &UpnpAbstractServiceDescription::serviceType() const
{
    return d->mServiceType;
}

void UpnpAbstractServiceDescription::setServiceId(const QVariant &newServiceId)
{
    d->mServiceId = newServiceId;
    Q_EMIT serviceIdChanged();
}

const QVariant &UpnpAbstractServiceDescription::serviceId() const
{
    return d->mServiceId;
}

void UpnpAbstractServiceDescription::setSCPDURL(const QVariant &newSCPDURL)
{
    d->mSCPDURL = newSCPDURL;
    Q_EMIT SCPDURLChanged();
}

const QVariant &UpnpAbstractServiceDescription::SCPDURL() const
{
    return d->mSCPDURL;
}

void UpnpAbstractServiceDescription::setControlURL(const QVariant &newControlURL)
{
    d->mControlURL = newControlURL;
    Q_EMIT controlURLChanged();
}

const QVariant &UpnpAbstractServiceDescription::controlURL() const
{
    return d->mControlURL;
}

void UpnpAbstractServiceDescription::setEventSubURL(const QVariant &newEventSubURL)
{
    d->mEventSubURL = newEventSubURL;
    Q_EMIT eventSubURLChanged();
}

const QVariant &UpnpAbstractServiceDescription::eventSubURL() const
{
    return d->mEventSubURL;
}

KDSoapPendingCall UpnpAbstractServiceDescription::callAction(const QString &action, const QList<QVariant> &arguments)
{
    QUrl controlUrl(d->mBaseURL.toUrl());
    controlUrl.setPath(d->mControlURL.toString());

    KDSoapMessage message;

    auto itAction = d->mActions.find(action);
    if (itAction != d->mActions.end()) {
        const UpnpActionDescription &actionDescription(itAction.value());

        auto itArgumentName = actionDescription.mArguments.begin();
        auto itArgumentValue = arguments.begin();
        for (; itArgumentName != actionDescription.mArguments.end() && itArgumentValue != arguments.end(); ++itArgumentName) {
            message.addArgument(itArgumentName->mName, itArgumentValue->toString());

            ++itArgumentValue;
            if (itArgumentValue == arguments.end()) {
                break;
            }
        }
    }

    if (!d->mInterface) {
        d->mInterface = new KDSoapClientInterface(controlUrl.toString(), d->mServiceType.toString());
        d->mInterface->setSoapVersion(KDSoapClientInterface::SOAP1_1);
        d->mInterface->setStyle(KDSoapClientInterface::RPCStyle);
    }

    return d->mInterface->asyncCall(action, message, d->mServiceType.toString() + QStringLiteral("#") + action);
}

void UpnpAbstractServiceDescription::subscribeEvents()
{
    QUrl eventUrl(d->mBaseURL.toUrl());
    eventUrl.setPath(d->mEventSubURL.toString());

    QString webServerAddess(QStringLiteral("<http://"));

    if (!d->mPublicAddress.isNull()) {
        webServerAddess += d->mPublicAddress.toString();
    } else {
        webServerAddess += QStringLiteral("127.0.0.1");
    }

    webServerAddess += QStringLiteral(":") + QString::number(d->mEventServer.serverPort()) + QStringLiteral(">");

    QNetworkRequest myRequest(eventUrl);
    myRequest.setRawHeader("CALLBACK", webServerAddess.toUtf8());
    myRequest.setRawHeader("NT", "upnp:event");
    myRequest.setRawHeader("TIMEOUT", "Second-600");

    d->mNetworkAccess.sendCustomRequest(myRequest, "SUBSCRIBE");
}

void UpnpAbstractServiceDescription::handleEventNotification(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers)
{
    qDebug() << "UpnpServiceDescription::handleEventNotification";
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

void UpnpAbstractServiceDescription::downloadAndParseServiceDescription(const QUrl &serviceUrl)
{
    d->mNetworkAccess.get(QNetworkRequest(serviceUrl));
}

void UpnpAbstractServiceDescription::finishedDownload(QNetworkReply *reply)
{
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        QDomDocument serviceDescriptionDocument;
        serviceDescriptionDocument.setContent(reply);

        const QDomElement &scpdRoot = serviceDescriptionDocument.documentElement();

        const QDomElement &actionListRoot = scpdRoot.firstChildElement(QStringLiteral("actionList"));
        QDomNode currentChild = actionListRoot.firstChild();
        while (!currentChild.isNull()) {
            const QDomNode &nameNode = currentChild.firstChildElement(QStringLiteral("name"));

            QString actionName;
            if (!nameNode.isNull()) {
                actionName = nameNode.toElement().text();
            }

            d->mActions[actionName].mName = actionName;

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

                d->mActions[actionName].mArguments.push_back(newArgument);

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
                qDebug() << "state variable name" << nameNode.toElement().text();
            }

            currentChild = currentChild.nextSibling();
        }
#endif
    }
}

void UpnpAbstractServiceDescription::parseEventNotification(const QString &eventName, const QString &eventValue)
{
    qDebug() << "one variable" << eventName << eventValue;
}

#include "moc_upnpabstractservicedescription.cpp"
