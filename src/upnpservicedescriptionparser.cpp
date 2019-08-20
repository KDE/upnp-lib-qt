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

#include "upnpservicedescriptionparser.h"

#include "upnpservicedescription.h"
#include "upnpactiondescription.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtXml/QDomDocument>

#include <QtCore/QDebug>

class UpnpServiceDescriptionParserPrivate
{
public:

    UpnpServiceDescriptionParserPrivate(QNetworkAccessManager *aNetworkAccess, UpnpServiceDescription &deviceDescription)
        : mNetworkAccess(aNetworkAccess), mServiceDescription(deviceDescription), mServiceURL()
    {
    }

    QNetworkAccessManager *mNetworkAccess;

    UpnpServiceDescription &mServiceDescription;

    QUrl mServiceURL;
};

UpnpServiceDescriptionParser::UpnpServiceDescriptionParser(QNetworkAccessManager *aNetworkAccess, UpnpServiceDescription &deviceDescription, QObject *parent)
    : QObject(parent), d(new UpnpServiceDescriptionParserPrivate(aNetworkAccess, deviceDescription))
{
}

UpnpServiceDescriptionParser::~UpnpServiceDescriptionParser()
{
}

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
            qDebug() << "UpnpAbstractServiceDescription::finishedDownload" << "error";
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
            qDebug() << "state variable name" << nameNode.toElement().text();
        }

        currentChild = currentChild.nextSibling();
    }
#endif

    Q_EMIT descriptionParsed(d->mServiceDescription.serviceId());
}


#include "moc_upnpservicedescriptionparser.cpp"
