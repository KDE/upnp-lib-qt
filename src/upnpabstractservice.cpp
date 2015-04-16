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
#include "upnpbasictypes.h"

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

    QMap<QString, UpnpActionDescription> mActions;

    QMap<QString, UpnpStateVariableDescription> mStateVariables;

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
        for (auto itAction = d->mActions.begin(); itAction != d->mActions.end(); ++itAction) {
            insertStream.writeStartElement(QStringLiteral("action"));
            insertStream.writeTextElement(QStringLiteral("name"), itAction->mName);
            insertStream.writeStartElement(QStringLiteral("argumentList"));
            for (auto itArgument = itAction->mArguments.begin(); itArgument != itAction->mArguments.end(); ++itArgument) {
                insertStream.writeStartElement(QStringLiteral("argument"));
                insertStream.writeTextElement(QStringLiteral("name"), itArgument->mName);
                insertStream.writeTextElement(QStringLiteral("direction"), (itArgument->mDirection == UpnpArgumentDirection::In) ? QStringLiteral("in") : QStringLiteral("out"));
                if (itArgument->mIsReturnValue) {
                    insertStream.writeEmptyElement(QStringLiteral("retval"));
                }
                insertStream.writeTextElement(QStringLiteral("relatedStateVariable"), itArgument->mRelatedStateVariable);
                insertStream.writeEndElement();
            }
            insertStream.writeEndElement();
            insertStream.writeEndElement();
        }
        insertStream.writeEndElement();

        insertStream.writeStartElement(QStringLiteral("serviceStateTable"));
        for (auto itStateVariable = d->mStateVariables.begin(); itStateVariable != d->mStateVariables.end(); ++itStateVariable) {
            insertStream.writeStartElement(QStringLiteral("stateVariable"));
            if (itStateVariable->mEvented) {
                insertStream.writeAttribute(QStringLiteral("sendEvents"), QStringLiteral("yes"));
            } else {
                insertStream.writeAttribute(QStringLiteral("sendEvents"), QStringLiteral("no"));
            }
            insertStream.writeTextElement(QStringLiteral("name"), itStateVariable->mName);
            insertStream.writeTextElement(QStringLiteral("dataType"), itStateVariable->mDataType);
            if (itStateVariable->mDefaultValue.isValid()) {
                insertStream.writeTextElement(QStringLiteral("defaultValue"), itStateVariable->mDefaultValue.toString());
            }
            if (itStateVariable->mMinimumValue.isValid() && itStateVariable->mMaximumValue.isValid()
                && itStateVariable->mStep.isValid()) {
                insertStream.writeStartElement(QStringLiteral("allowedValueRange"));
                if (itStateVariable->mMinimumValue.isValid()) {
                    insertStream.writeTextElement(QStringLiteral("minimum"), itStateVariable->mMinimumValue.toString());
                }
                if (itStateVariable->mMaximumValue.isValid()) {
                    insertStream.writeTextElement(QStringLiteral("maximum"), itStateVariable->mMaximumValue.toString());
                }
                if (itStateVariable->mStep.isValid()) {
                    insertStream.writeTextElement(QStringLiteral("step"), itStateVariable->mStep.toString());
                }
                insertStream.writeEndElement();
            }
            if (!itStateVariable->mValueList.isEmpty()) {
                insertStream.writeStartElement(QStringLiteral("allowedValueList"));
                for (auto itValue = itStateVariable->mValueList.begin(); itValue != itStateVariable->mValueList.end(); ++itValue) {
                    insertStream.writeTextElement(QStringLiteral("allowedValue"), *itValue);
                }
                insertStream.writeEndElement();
            }
            insertStream.writeEndElement();
        }
        insertStream.writeEndElement();
        insertStream.writeEndElement();

        d->mXmlDescription = newDescription;

        d->mXmlDescription->seek(0);

        qDebug() << newDescription->data();
    }

    d->mXmlDescription->seek(0);

    return d->mXmlDescription;
}

void UpnpAbstractService::addAction(const UpnpActionDescription &newAction)
{
    d->mActions[newAction.mName] = newAction;
}

const UpnpActionDescription &UpnpAbstractService::action(const QString &name) const
{
    return d->mActions[name];
}

void UpnpAbstractService::addStateVariable(const UpnpStateVariableDescription &newVariable)
{
    d->mStateVariables[newVariable.mName] = newVariable;
}

const UpnpStateVariableDescription &UpnpAbstractService::stateVariable(const QString &name) const
{
    return d->mStateVariables[name];
}

#include "moc_upnpabstractservice.cpp"
