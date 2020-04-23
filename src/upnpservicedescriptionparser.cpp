/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpservicedescriptionparser.h"

#include "upnplogging.h"

#include "upnpactiondescription.h"
#include "upnpservicedescription.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QDomDocument>

#include <QLoggingCategory>

class UpnpServiceDescriptionParserPrivate
{
public:
    UpnpServiceDescriptionParserPrivate(QNetworkAccessManager *aNetworkAccess, UpnpServiceDescription &serviceDescription)
        : mNetworkAccess(aNetworkAccess)
        , mServiceDescription(serviceDescription)
        , mServiceURL()
    {
    }

    QNetworkAccessManager *mNetworkAccess;

    UpnpServiceDescription &mServiceDescription;

    QUrl mServiceURL;
};

UpnpServiceDescriptionParser::UpnpServiceDescriptionParser(QNetworkAccessManager *aNetworkAccess, UpnpServiceDescription &deviceDescription, QObject *parent)
    : QObject(parent)
    , d(std::make_unique<UpnpServiceDescriptionParserPrivate>(aNetworkAccess, deviceDescription))
{
}

UpnpServiceDescriptionParser::~UpnpServiceDescriptionParser() = default;

void UpnpServiceDescriptionParser::downloadServiceDescription(const QUrl &serviceUrl)
{
    d->mServiceURL = serviceUrl;
    d->mNetworkAccess->get(QNetworkRequest(serviceUrl));
}

void UpnpServiceDescriptionParser::finishedDownload(QNetworkReply *reply)
{
    if (reply->url() == d->mServiceURL) {
        if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
            parseServiceDescription(reply);
        } else if (reply->isFinished()) {
            qCDebug(orgKdeUpnpLibQtUpnp()) << "UpnpAbstractServiceDescription::finishedDownload"
                                           << "error";
        }
    }
}

void UpnpServiceDescriptionParser::parseServiceDescription(QIODevice *serviceDescriptionContent)
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

            d->mServiceDescription.addAction(newAction);

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

    Q_EMIT descriptionParsed(d->mServiceDescription.serviceId());
}

#include "moc_upnpservicedescriptionparser.cpp"
