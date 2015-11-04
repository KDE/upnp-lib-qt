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

#include <QtCore/QPointer>
#include <QtCore/QBuffer>
#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QTimer>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>

#include <QtCore/QDebug>

class UpnpAbstractServicePrivate
{
public:

    UpnpAbstractServicePrivate()
        : mBaseURL(), mServiceType(), mServiceId(), mSCPDURL(), mControlURL(),
          mEventURL(), mXmlDescription(), mActions(), mStateVariables(),
          mSubscribers(), mMaximumSubscriptionDuration(3600)
    {
    }

    QString mBaseURL;

    QString mServiceType;

    QString mServiceId;

    QUrl mSCPDURL;

    QUrl mControlURL;

    QUrl mEventURL;

    QPointer<QIODevice> mXmlDescription;

    QMap<QString, UpnpActionDescription> mActions;

    QMap<QString, UpnpStateVariableDescription> mStateVariables;

    QList<QPointer<UpnpEventSubscriber> > mSubscribers;

    int mMaximumSubscriptionDuration;
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

void UpnpAbstractService::setMaximumSubscriptionDuration(int newValue)
{
    d->mMaximumSubscriptionDuration = newValue;
    Q_EMIT maximumSubscriptionDurationChanged(d->mServiceId);
}

int UpnpAbstractService::maximumSubscriptionDuration() const
{
    return d->mMaximumSubscriptionDuration;
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
            insertStream.writeTextElement(QStringLiteral("name"), itStateVariable->mUpnpName);
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
        newSubscriber->setSecondTimeout(d->mMaximumSubscriptionDuration);
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
        for (const UpnpStateVariableDescription &currentStateVariable : d->mStateVariables) {
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
    qDebug() << "UpnpAbstractService::unsubscribeToEvents" << requestData;
}

void UpnpAbstractService::addAction(const UpnpActionDescription &newAction)
{
    d->mActions[newAction.mName] = newAction;
}

const UpnpActionDescription &UpnpAbstractService::action(const QString &name) const
{
    return d->mActions[name];
}

QList<QString> UpnpAbstractService::actions() const
{
    return d->mActions.keys();
}

void UpnpAbstractService::addStateVariable(const UpnpStateVariableDescription &newVariable)
{
    d->mStateVariables[newVariable.mUpnpName] = newVariable;
}

const UpnpStateVariableDescription &UpnpAbstractService::stateVariable(const QString &name) const
{
    return d->mStateVariables[name];
}

QList<QString> UpnpAbstractService::stateVariables() const
{
    return d->mStateVariables.keys();
}

QList<QPair<QString, QVariant> > UpnpAbstractService::invokeAction(const QString &actionName, const QList<QVariant> &arguments, bool &isInError)
{
    Q_UNUSED(actionName);
    Q_UNUSED(arguments);
    Q_UNUSED(isInError);

    return {};
}

void UpnpAbstractService::sendEventNotification(const QPointer<UpnpEventSubscriber> &currentSubscriber)
{
    QTimer::singleShot(0, currentSubscriber.data(), SLOT(sendEventNotification()));
}

#include "moc_upnpabstractservice.cpp"
