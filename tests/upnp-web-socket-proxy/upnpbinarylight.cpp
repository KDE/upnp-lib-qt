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

#include "upnpbinarylight.h"

#include "upnpswitchpower.h"
#include "abstractrelayactuator.h"

#include "upnpwebsocketpublisher.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include <QtCore/QUuid>

class BinaryLightPrivate
{
public:

    QSharedPointer<UpnpDeviceDescription> mDevice;

    UpnpWebSocketPublisher* mWebSocketPublisher = nullptr;

    AbstractRelayActuator* mActuator = nullptr;

    QSharedPointer<UpnpSwitchPower> mSwitchPowerService;

};

UpnpBinaryLight::UpnpBinaryLight(int cacheDuration, QObject *parent)
    : QObject(parent), d(new BinaryLightPrivate)
{
    d->mDevice.reset(new UpnpDeviceDescription);

    d->mDevice->setDeviceType(QStringLiteral("urn:schemas-upnp-org:device:BinaryLight:1"));
    d->mDevice->setFriendlyName(QStringLiteral("Binary Light for Test"));
    d->mDevice->setManufacturer(QStringLiteral("Matthieu Gallien"));
    d->mDevice->setManufacturerURL(QUrl(QStringLiteral("https://gitlab.com/homeautomationqt/upnp-player-qt")));
    d->mDevice->setModelDescription(QStringLiteral("Test Device"));
    d->mDevice->setModelName(QStringLiteral("Automatiq Binary Light"));
    d->mDevice->setModelNumber(QStringLiteral("0.1"));
    d->mDevice->setModelURL(QUrl(QStringLiteral("https://gitlab.com/homeautomationqt/upnp-player-qt")));
    d->mDevice->setSerialNumber(QStringLiteral("test-0.1"));

    const QString &uuidString(QUuid::createUuid().toString());
    d->mDevice->setUDN(uuidString.mid(1, uuidString.length() - 2));
    d->mDevice->setUPC(QStringLiteral("test"));
    d->mDevice->setCacheControl(cacheDuration);

    d->mSwitchPowerService.reset(new UpnpSwitchPower);
    d->mDevice->addService(d->mSwitchPowerService->sharedDescription());
    const int serviceIndex = 0/*addService(switchPowerService)*/;

    const int deviceIndex = 1/*d->mServer.addDevice(this)*/;

    QUrl eventUrl /*= d->mServer.urlPrefix()*/;
    eventUrl.setPath(QStringLiteral("/") + QString::number(deviceIndex) + QStringLiteral("/") + QString::number(serviceIndex) + QStringLiteral("/event"));

    QUrl controlUrl /*= d->mServer.urlPrefix()*/;
    controlUrl.setPath(QStringLiteral("/") + QString::number(deviceIndex) + QStringLiteral("/") + QString::number(serviceIndex) + QStringLiteral("/control"));

    QUrl serviceDescriptionUrl /*= d->mServer.urlPrefix()*/;
    serviceDescriptionUrl.setPath(QStringLiteral("/") + QString::number(deviceIndex) + QStringLiteral("/") + QString::number(serviceIndex) + QStringLiteral("/service.xml"));

    /*switchPowerService->setControlURL(controlUrl);
    switchPowerService->setEventURL(eventUrl);
    switchPowerService->setSCPDURL(serviceDescriptionUrl);*/

    QUrl deviceDescriptionUrl /*= d->mServer.urlPrefix();
                                            d->mDevice->setURLBase(d->mServer.urlPrefix().toString())*/;
    deviceDescriptionUrl.setPath(QStringLiteral("/") + QString::number(deviceIndex) + QStringLiteral("/device.xml"));
    d->mDevice->setLocationUrl(deviceDescriptionUrl);
}

UpnpBinaryLight::~UpnpBinaryLight()
{
    delete d;
}

QSharedPointer<UpnpDeviceDescription> UpnpBinaryLight::description() const
{
    return d->mDevice;
}

void UpnpBinaryLight::setWebSocketPublisher(UpnpWebSocketPublisher *value)
{
    if (d->mWebSocketPublisher) {
        disconnect(d->mWebSocketPublisher, &UpnpWebSocketPublisher::actionCalled, this, &UpnpBinaryLight::actionCalled);
    }

    d->mWebSocketPublisher = value;

    if (d->mWebSocketPublisher) {
        connect(d->mWebSocketPublisher, &UpnpWebSocketPublisher::actionCalled, this, &UpnpBinaryLight::actionCalled);
    }

    Q_EMIT webSocketPublisherChanged();
}

UpnpWebSocketPublisher *UpnpBinaryLight::webSocketPublisher() const
{
    return d->mWebSocketPublisher;
}

void UpnpBinaryLight::setUdn(const QString &value)
{
    d->mDevice->setUDN(value);
    Q_EMIT udnChanged();
}

const QString &UpnpBinaryLight::udn() const
{
    return d->mDevice->UDN();
}

AbstractRelayActuator *UpnpBinaryLight::actuator() const
{
    return d->mActuator;
}

void UpnpBinaryLight::actionCalled(const QString &action, const QVariantMap &arguments, qint64 sequenceNumber, const QString &serviceId)
{
    Q_UNUSED(sequenceNumber);

    if (serviceId == QStringLiteral("urn:upnp-org:serviceId:SwitchPower")) {
        if (action == QStringLiteral("SetTarget")) {
            auto itArg = arguments.find(QStringLiteral("newTargetValue"));

            if (itArg == arguments.end()) {
                return;
            }

            Q_EMIT setTarget(itArg.value().toBool());
        }
        if (action == QStringLiteral("GetTarget")) {
            Q_EMIT getTarget();
        }
        if (action == QStringLiteral("GetStatus")) {
            Q_EMIT getStatus();
        }
    }
}

void UpnpBinaryLight::setActuator(AbstractRelayActuator *actuator)
{
    if (d->mActuator) {
        disconnect(this, &UpnpBinaryLight::setTarget, d->mActuator, &AbstractRelayActuator::switchRelay);
    }

    if (d->mActuator == actuator)
        return;

    d->mActuator = actuator;

    if (d->mActuator) {
        connect(this, &UpnpBinaryLight::setTarget, d->mActuator, &AbstractRelayActuator::switchRelay);
    }

    Q_EMIT actuatorChanged();
}

#include "moc_upnpbinarylight.cpp"
