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

#include "upnpabstractservice.h"

#include <QtCore/QPointer>
#include <QtCore/QBuffer>
#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>

#include <QtCore/QDebug>

class UpnpAbstractServicePrivate
{
public:

    QString mBaseURL;

    QString mServiceType;

    QString mServiceId;

    QUrl mSCPDURL;

    QUrl mControlURL;

    QUrl mEventURL;

    QPointer<QIODevice> mXmlDescription;

};

UpnpAbstractService::UpnpAbstractService(QObject *parent) :
    QObject(parent), d(new UpnpAbstractServicePrivate)
{
}

UpnpAbstractService::~UpnpAbstractService()
{
    delete d;
}

void UpnpAbstractService::setBaseURL(const QString &newBaseURL)
{
    d->mBaseURL = newBaseURL;
}

const QString &UpnpAbstractService::baseURL() const
{
    return d->mBaseURL;
}

void UpnpAbstractService::setServiceType(const QString &newServiceType)
{
    d->mServiceType = newServiceType;
}

const QString &UpnpAbstractService::serviceType() const
{
    return d->mServiceType;
}

void UpnpAbstractService::setServiceId(const QString &newServiceId)
{
    d->mServiceId = newServiceId;
}

const QString &UpnpAbstractService::serviceId() const
{
    return d->mServiceId;
}

void UpnpAbstractService::setSCPDURL(const QUrl &newSCPDURL)
{
    d->mSCPDURL = newSCPDURL;
}

const QUrl &UpnpAbstractService::SCPDURL() const
{
    return d->mSCPDURL;
}

void UpnpAbstractService::setControlURL(const QUrl &newControlURL)
{
    d->mControlURL = newControlURL;
}

const QUrl &UpnpAbstractService::controlURL() const
{
    return d->mControlURL;
}

void UpnpAbstractService::setEventURL(const QUrl &newEventURL)
{
    d->mEventURL = newEventURL;
}

const QUrl &UpnpAbstractService::eventURL() const
{
    return d->mEventURL;
}

QIODevice* UpnpAbstractService::buildAndGetXmlDescription()
{
    qDebug() << "UpnpAbstractService::buildAndGetXmlDescription";

    if (!d->mXmlDescription) {
        QPointer<QBuffer> newDescription(new QBuffer);

        newDescription->open(QIODevice::ReadWrite);

        QXmlStreamWriter insertStream(newDescription.data());
        insertStream.setAutoFormatting(true);

        insertStream.writeStartDocument();
        insertStream.writeStartElement(QStringLiteral("scpd"));
        insertStream.writeAttribute(QStringLiteral("xmlns"), QStringLiteral("urn:schemas-upnp-org:service-1-0"));
        insertStream.writeStartElement(QStringLiteral("specVersion"));
        insertStream.writeTextElement(QStringLiteral("major"), QStringLiteral("1"));
        insertStream.writeTextElement(QStringLiteral("minor"), QStringLiteral("0"));
        insertStream.writeEndElement();
        insertStream.writeStartElement(QStringLiteral("actionList"));
        insertStream.writeEndElement();
        insertStream.writeStartElement(QStringLiteral("serviceStateTable"));
        insertStream.writeEndElement();
        insertStream.writeEndElement();

        d->mXmlDescription = newDescription;

        d->mXmlDescription->seek(0);

        qDebug() << newDescription->data();
    }

    d->mXmlDescription->seek(0);

    return d->mXmlDescription;
}

#include "moc_upnpabstractservice.cpp"
