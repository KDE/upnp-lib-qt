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

#include "upnpdevicemodel.h"
#include "upnpssdpengine.h"
#include "upnpcontrolabstractdevice.h"
#include "upnpcontrolmediaserver.h"

#include "upnpdevicedescription.h"

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtCore/QUrl>

class UpnpDeviceModelPrivate
{
public:

    QHash<QString, UpnpDiscoveryResult> mAllHosts;

    QHash<QString, QPointer<UpnpControlAbstractDevice> > mAllHostsDescription;

    QList<QString> mAllHostsUUID;

    UpnpSsdpEngine *mListenner;
};

UpnpDeviceModel::UpnpDeviceModel(QObject *parent)
    : QAbstractListModel(parent), d(new UpnpDeviceModelPrivate)
{
    d->mListenner = nullptr;
}

UpnpDeviceModel::~UpnpDeviceModel()
{
    delete d;
}

int UpnpDeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.parent().isValid()) {
        return 0;
    } else {
        return d->mAllHosts.size();
    }
}

QHash<int, QByteArray> UpnpDeviceModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::NameRole)] = "name";
    roles[static_cast<int>(ColumnsRoles::TypeRole)] = "upnpType";
    roles[static_cast<int>(ColumnsRoles::uuidRole)] = "uuid";
    roles[static_cast<int>(ColumnsRoles::ViewNameRole)] = "viewName";
    roles[static_cast<int>(ColumnsRoles::MobileViewNameRole)] = "mobileViewName";

    return roles;
}

Qt::ItemFlags UpnpDeviceModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.column() != 0) {
        return Qt::NoItemFlags;
    }

    if (index.row() < 0 || index.row() > d->mAllHosts.size() - 1) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant UpnpDeviceModel::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid()) {
        return QVariant();
    }

    if (index.column() != 0) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() > d->mAllHosts.size() - 1) {
        return QVariant();
    }

    if (!d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]) {
        return QVariant();
    }

    switch(role)
    {
    case ColumnsRoles::NameRole:
        return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->device()->friendlyName();
    case ColumnsRoles::TypeRole:
        return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->device()->deviceType();
    case ColumnsRoles::uuidRole:
        return d->mAllHostsUUID[index.row()];
    case ColumnsRoles::ViewNameRole:
        return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->viewName();
    case ColumnsRoles::MobileViewNameRole:
        return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->mobileViewName();
    }

    return QVariant();
}

UpnpSsdpEngine *UpnpDeviceModel::listenner() const
{
    return d->mListenner;
}

void UpnpDeviceModel::setListenner(UpnpSsdpEngine *listenner)
{
    if (d->mListenner) {
        disconnect(d->mListenner);
    }
    d->mListenner = listenner;
    connect(d->mListenner, &UpnpSsdpEngine::newService, this, &UpnpDeviceModel::newDevice);
    connect(d->mListenner, &UpnpSsdpEngine::removedService, this, &UpnpDeviceModel::removedDevice);
}

UpnpControlAbstractDevice *UpnpDeviceModel::getDeviceDescription(const QString &uuid) const
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    if (deviceIndex == -1) {
        return nullptr;
    }

    return d->mAllHostsDescription[uuid];
}

QVariant UpnpDeviceModel::get(int row, const QString &roleName) const
{
    int role = Qt::DisplayRole;

    if (roleName == QStringLiteral("name")) {
        role = ColumnsRoles::NameRole;
    }
    if (roleName == QStringLiteral("upnpType")) {
        role = ColumnsRoles::TypeRole;
    }
    if (roleName == QStringLiteral("uuid")) {
        role = ColumnsRoles::uuidRole;
    }
    if (roleName == QStringLiteral("viewName")) {
        role = ColumnsRoles::ViewNameRole;
    }
    if (roleName == QStringLiteral("mobileViewName")) {
        role = ColumnsRoles::MobileViewNameRole;
    }

    return data(index(row), role);
}

void UpnpDeviceModel::newDevice(const UpnpDiscoveryResult &deviceDiscovery)
{
    const QString &deviceUuid = deviceDiscovery.mUSN.mid(5, 36);
    if (!d->mAllHostsUUID.contains(deviceUuid) && deviceDiscovery.mNT.startsWith(QStringLiteral("urn:schemas-upnp-org:device:"))) {
        beginInsertRows(QModelIndex(), d->mAllHostsUUID.size(), d->mAllHostsUUID.size());

        const QString &decodedUdn(deviceUuid);

        d->mAllHostsUUID.push_back(decodedUdn);
        d->mAllHosts[decodedUdn] = deviceDiscovery;
        if (deviceDiscovery.mNT == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1")) {
            d->mAllHostsDescription[decodedUdn] = new UpnpControlMediaServer;
        } else {
            d->mAllHostsDescription[decodedUdn] = new UpnpControlAbstractDevice;
        }
        d->mAllHostsDescription[decodedUdn]->device()->setUDN(decodedUdn);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::inError, this, &UpnpDeviceModel::deviceInError);

        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::UDNChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::UPCChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::deviceTypeChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::friendlyNameChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::manufacturerChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::manufacturerURLChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::modelDescriptionChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::modelNameChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::modelNumberChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::modelURLChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::serialNumberChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn]->device(), &UpnpDeviceDescription::URLBaseChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);

        endInsertRows();

        d->mAllHostsDescription[decodedUdn]->downloadDeviceDescription(QUrl(deviceDiscovery.mLocation));
    }
}

void UpnpDeviceModel::removedDevice(const UpnpDiscoveryResult &deviceDiscovery)
{
    genericRemovedDevice(deviceDiscovery.mUSN);
}

void UpnpDeviceModel::genericRemovedDevice(const QString &usn)
{
    const QString &deviceUuid = usn.mid(5, 36);
    auto deviceIndex = d->mAllHostsUUID.indexOf(deviceUuid);
    if (deviceIndex != -1) {
        beginRemoveRows(QModelIndex(), deviceIndex, deviceIndex);
        d->mAllHostsUUID.removeAt(deviceIndex);
        d->mAllHosts.remove(deviceUuid);
        d->mAllHostsDescription.remove(deviceUuid);
        endRemoveRows();
    }
}

void UpnpDeviceModel::deviceDescriptionChanged(const QString &uuid)
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    if (deviceIndex != -1) {
        Q_EMIT dataChanged(index(deviceIndex), index(deviceIndex));
    }
}

void UpnpDeviceModel::deviceInError()
{
    if (sender()) {
        UpnpControlAbstractDevice *device = qobject_cast<UpnpControlAbstractDevice*>(sender());
        if (device) {
            genericRemovedDevice(device->device()->UDN());
        }
    }
}

#include "moc_upnpdevicemodel.cpp"
