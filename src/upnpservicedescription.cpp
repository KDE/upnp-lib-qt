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

#include "upnpservicedescription.h"

#include <KDSoapClient/KDSoapClientInterface.h>
#include <KDSoapClient/KDSoapMessage.h>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

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

class UpnpServiceDescriptionPrivate
{
public:

    UpnpServiceDescriptionPrivate()
        : mNetworkAccess(), mServiceType(), mServiceId(), mBaseURL(), mSCPDURL(), mControlURL(), mEventSubURL(), mActions()
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
};

UpnpServiceDescription::UpnpServiceDescription(QObject *parent)
    : QObject(parent), d(new UpnpServiceDescriptionPrivate)
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpServiceDescription::finishedDownload);
}
UpnpServiceDescription::~UpnpServiceDescription()
{
    delete d;
}

void UpnpServiceDescription::setBaseURL(const QVariant &newBaseURL)
{
    d->mBaseURL = newBaseURL;
    Q_EMIT baseURLChanged();
}

const QVariant &UpnpServiceDescription::baseURL() const
{
    return d->mBaseURL;
}

void UpnpServiceDescription::setServiceType(const QVariant &newServiceType)
{
    d->mServiceType = newServiceType;
    Q_EMIT serviceTypeChanged();
}

const QVariant &UpnpServiceDescription::serviceType() const
{
    return d->mServiceType;
}

void UpnpServiceDescription::setServiceId(const QVariant &newServiceId)
{
    d->mServiceId = newServiceId;
    Q_EMIT serviceIdChanged();
}

const QVariant &UpnpServiceDescription::serviceId() const
{
    return d->mServiceId;
}

void UpnpServiceDescription::setSCPDURL(const QVariant &newSCPDURL)
{
    d->mSCPDURL = newSCPDURL;
    Q_EMIT SCPDURLChanged();
}

const QVariant &UpnpServiceDescription::SCPDURL() const
{
    return d->mSCPDURL;
}

void UpnpServiceDescription::setControlURL(const QVariant &newControlURL)
{
    d->mControlURL = newControlURL;
    Q_EMIT controlURLChanged();
}

const QVariant &UpnpServiceDescription::controlURL() const
{
    return d->mControlURL;
}

void UpnpServiceDescription::setEventSubURL(const QVariant &newEventSubURL)
{
    d->mEventSubURL = newEventSubURL;
    Q_EMIT eventSubURLChanged();
}

const QVariant &UpnpServiceDescription::eventSubURL() const
{
    return d->mEventSubURL;
}

void UpnpServiceDescription::callAction(const QString &action, const QList<QVariant> &arguments)
{
    auto itAction = d->mActions.find(action);
    if (itAction != d->mActions.end()) {
        QUrl controlUrl(d->mBaseURL.toUrl());
        controlUrl.setPath(d->mControlURL.toString());

        const UpnpActionDescription &actionDescription(itAction.value());

        KDSoapClientInterface clientInterface(controlUrl.toString(), d->mServiceType.toString());
        clientInterface.setSoapVersion(KDSoapClientInterface::SOAP1_1);
        clientInterface.setStyle(KDSoapClientInterface::RPCStyle);

        KDSoapMessage message;

        auto itArgumentName = actionDescription.mArguments.begin();
        auto itArgumentValue = arguments.begin();
        for (; itArgumentName != actionDescription.mArguments.end(); ++itArgumentName) {
            message.addArgument(itArgumentName->mName, itArgumentValue->toString());

            ++itArgumentValue;
            if (itArgumentValue == arguments.end()) {
                break;
            }
        }

        KDSoapMessage answer = clientInterface.call(action, message, d->mServiceType.toString() + QStringLiteral("#") + action);
    }
}

void UpnpServiceDescription::subscribeEvents()
{
    QUrl eventUrl(d->mBaseURL.toUrl());
    eventUrl.setPath(d->mEventSubURL.toString());

    QNetworkRequest myRequest(eventUrl);
    myRequest.setRawHeader("CALLBACK", "<http://127.0.0.1:42424>");
    myRequest.setRawHeader("NT", "upnp:event");
    myRequest.setRawHeader("TIMEOUT", "Second-infinite");

    d->mNetworkAccess.sendCustomRequest(myRequest, "SUBSCRIBE");
}

void UpnpServiceDescription::downloadAndParseServiceDescription(const QUrl &serviceUrl)
{
    qDebug() << "UpnpServiceCaller::downloadAndParseServiceDescription" << serviceUrl;
    d->mNetworkAccess.get(QNetworkRequest(serviceUrl));
}

void UpnpServiceDescription::finishedDownload(QNetworkReply *reply)
{
    qDebug() << "UpnpServiceCaller::finishedDownload" << reply->url();
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
#if 0
        qDebug() << "serviceId" << d->mServiceId;
        qDebug() << "serviceType" << d->mServiceType;
        qDebug() << "SCPDURL" << d->mSCPDURL;
        qDebug() << "controlURL" << d->mControlURL;
        qDebug() << "eventSubURL" << d->mEventSubURL;
#endif

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

#if 0
            qDebug() << "new service: " << d->mActions[actionName].mName;
            for (auto itArg = d->mActions[actionName].mArguments.begin(); itArg != d->mActions[actionName].mArguments.end(); ++itArg) {
                qDebug() << "argument: " << itArg->mName;
                qDebug() << "direction: " << (itArg->mDirection == UpnpArgumentDirection::In ? "In" : "Out");
                qDebug() << "state variable: " << itArg->mRelatedStateVariable;
                qDebug() << (itArg->mIsReturnValue ? "is return value" : "");
            }
            qDebug() << "\n";
#endif

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

#include "moc_upnpservicedescription.cpp"
