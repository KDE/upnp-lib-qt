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
#include "upnpeventsubscriber.h"

#include "upnpactiondescription.h"
#include "upnpstatevariabledescription.h"
#include "upnpservicedescription.h"

#include <QPointer>
#include <QBuffer>
#include <QIODevice>
#include <QXmlStreamWriter>
#include <QTimer>
#include <QMetaObject>
#include <QMetaProperty>

#include <QDebug>

class UpnpAbstractServicePrivate
{
public:

    UpnpAbstractServicePrivate()
        : mService(), mXmlDescription(), mSubscribers()
    {
    }

    UpnpServiceDescription mService;

    QPointer<QIODevice> mXmlDescription;

    QVector<QPointer<UpnpEventSubscriber> > mSubscribers;

};

UpnpAbstractService::UpnpAbstractService(QObject *parent) :
    QObject(parent), d(new UpnpAbstractServicePrivate)
{
}

UpnpAbstractService::~UpnpAbstractService()
{
}

QIODevice* UpnpAbstractService::buildAndGetXmlDescription()
{
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
        const auto &allActions = description().actions();
        for (const auto &itAction : allActions) {
            insertStream.writeStartElement(QStringLiteral("action"));
            insertStream.writeTextElement(QStringLiteral("name"), itAction.mName);
            insertStream.writeStartElement(QStringLiteral("argumentList"));
            for (const auto &itArgument : itAction.mArguments) {
                insertStream.writeStartElement(QStringLiteral("argument"));
                insertStream.writeTextElement(QStringLiteral("name"), itArgument.mName);
                insertStream.writeTextElement(QStringLiteral("direction"), (itArgument.mDirection == UpnpArgumentDirection::In) ? QStringLiteral("in") : QStringLiteral("out"));
                if (itArgument.mIsReturnValue) {
                    insertStream.writeEmptyElement(QStringLiteral("retval"));
                }
                insertStream.writeTextElement(QStringLiteral("relatedStateVariable"), itArgument.mRelatedStateVariable);
                insertStream.writeEndElement();
            }
            insertStream.writeEndElement();
            insertStream.writeEndElement();
        }
        insertStream.writeEndElement();

        insertStream.writeStartElement(QStringLiteral("serviceStateTable"));

        const auto &allStateVariables = description().stateVariables();
        for (const auto &itStateVariable : allStateVariables) {
            insertStream.writeStartElement(QStringLiteral("stateVariable"));
            if (itStateVariable.mEvented) {
                insertStream.writeAttribute(QStringLiteral("sendEvents"), QStringLiteral("yes"));
            } else {
                insertStream.writeAttribute(QStringLiteral("sendEvents"), QStringLiteral("no"));
            }
            insertStream.writeTextElement(QStringLiteral("name"), itStateVariable.mUpnpName);
            insertStream.writeTextElement(QStringLiteral("dataType"), itStateVariable.mDataType);
            if (itStateVariable.mDefaultValue.isValid()) {
                insertStream.writeTextElement(QStringLiteral("defaultValue"), itStateVariable.mDefaultValue.toString());
            }
            if (itStateVariable.mMinimumValue.isValid() && itStateVariable.mMaximumValue.isValid()
                && itStateVariable.mStep.isValid()) {
                insertStream.writeStartElement(QStringLiteral("allowedValueRange"));
                if (itStateVariable.mMinimumValue.isValid()) {
                    insertStream.writeTextElement(QStringLiteral("minimum"), itStateVariable.mMinimumValue.toString());
                }
                if (itStateVariable.mMaximumValue.isValid()) {
                    insertStream.writeTextElement(QStringLiteral("maximum"), itStateVariable.mMaximumValue.toString());
                }
                if (itStateVariable.mStep.isValid()) {
                    insertStream.writeTextElement(QStringLiteral("step"), itStateVariable.mStep.toString());
                }
                insertStream.writeEndElement();
            }
            if (!itStateVariable.mValueList.isEmpty()) {
                insertStream.writeStartElement(QStringLiteral("allowedValueList"));
                for (const auto &itValue : itStateVariable.mValueList) {
                    insertStream.writeTextElement(QStringLiteral("allowedValue"), itValue);
                }
                insertStream.writeEndElement();
            }
            insertStream.writeEndElement();
        }
        insertStream.writeEndElement();
        insertStream.writeEndElement();
        insertStream.writeEndDocument();

        d->mXmlDescription = newDescription;
    }

    d->mXmlDescription->seek(0);

    return d->mXmlDescription;
}

QPointer<UpnpEventSubscriber> UpnpAbstractService::subscribeToEvents(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers)
{
    Q_UNUSED(requestData);

    QPointer<UpnpEventSubscriber> newSubscriber(new UpnpEventSubscriber);

    const QByteArray &rawCallBackAddress = headers["callback"];
    newSubscriber->setCallback(QUrl(QString::fromLatin1(rawCallBackAddress.mid(1, rawCallBackAddress.length() - 2))));

    const QByteArray &rawTimeout = headers["timeout"];
    bool conversionIsOk = false;
    newSubscriber->setSecondTimeout(rawTimeout.right(rawTimeout.length() - 7).toInt(&conversionIsOk));
    if (!conversionIsOk || rawTimeout == "Second-infinite") {
        newSubscriber->setSecondTimeout(description().maximumSubscriptionDuration());
    }

    int signalIndex = -1;
    for (int i = 0; i < newSubscriber->metaObject()->methodCount(); ++i) {
        if (newSubscriber->metaObject()->method(i).name() == "notifyPropertyChange") {
            signalIndex = i;
            break;
        }
    }

    if (signalIndex != -1) {
        newSubscriber->setUpnpService(this);
        d->mSubscribers.push_back(newSubscriber);

        const auto &allStateVariables = description().stateVariables();
        for (const auto &currentStateVariable : allStateVariables) {
            if (currentStateVariable.mEvented) {
                connect(this, metaObject()->property(currentStateVariable.mPropertyIndex).notifySignal(), newSubscriber, newSubscriber->metaObject()->method(signalIndex));
            }
        }
    }

    sendEventNotification(newSubscriber);

    return newSubscriber;
}

void UpnpAbstractService::unsubscribeToEvents(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers)
{
    Q_UNUSED(headers);

    qDebug() << "UpnpAbstractService::unsubscribeToEvents" << requestData;
}

void UpnpAbstractService::addAction(const UpnpActionDescription &newAction)
{
    description().actions()[newAction.mName] = newAction;
}

const UpnpActionDescription &UpnpAbstractService::action(const QString &name) const
{
    return description().action(name);
}

QList<QString> UpnpAbstractService::actions() const
{
    return description().actions().keys();
}

void UpnpAbstractService::addStateVariable(const UpnpStateVariableDescription &newVariable)
{
    description().stateVariables()[newVariable.mUpnpName] = newVariable;
}

const UpnpStateVariableDescription &UpnpAbstractService::stateVariable(const QString &name) const
{
    return description().stateVariable(name);
}

QList<QString> UpnpAbstractService::stateVariables() const
{
    return description().stateVariables().keys();
}

QVector<QPair<QString, QVariant> > UpnpAbstractService::invokeAction(const QString &actionName, const QVector<QVariant> &arguments, bool &isInError)
{
    Q_UNUSED(actionName);
    Q_UNUSED(arguments);
    Q_UNUSED(isInError);

    return {};
}

void UpnpAbstractService::setDescription(const UpnpServiceDescription &value)
{
    d->mService = value;
    Q_EMIT descriptionChanged();
}

UpnpServiceDescription &UpnpAbstractService::description()
{
    return d->mService;
}

const UpnpServiceDescription &UpnpAbstractService::description() const
{
    return d->mService;
}
void UpnpAbstractService::sendEventNotification(const QPointer<UpnpEventSubscriber> &currentSubscriber)
{
    QTimer::singleShot(0, currentSubscriber.data(), &UpnpEventSubscriber::sendEventNotification);
}

#include "moc_upnpabstractservice.cpp"
