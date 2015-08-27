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

    switch(role)
    {
    case ColumnsRoles::NameRole:
        return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->friendlyName();
    case ColumnsRoles::TypeRole:
        return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->deviceType();
    case ColumnsRoles::uuidRole:
        return d->mAllHostsUUID[index.row()];
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

void UpnpDeviceModel::newDevice(const UpnpDiscoveryResult &deviceDiscovery)
{
    const QString &deviceUuid = deviceDiscovery.mUSN.mid(5, 36);
    if (!d->mAllHostsUUID.contains(deviceUuid) && deviceDiscovery.mNT.startsWith(QStringLiteral("urn:schemas-upnp-org:device:"))) {
        beginInsertRows(QModelIndex(), d->mAllHostsUUID.size(), d->mAllHostsUUID.size());

        const QString &decodedUdn(deviceUuid);

        d->mAllHostsUUID.push_back(decodedUdn);
        d->mAllHosts[decodedUdn] = deviceDiscovery;
        d->mAllHostsDescription[decodedUdn] = new UpnpControlAbstractDevice;
        d->mAllHostsDescription[decodedUdn]->setUDN(decodedUdn);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::inError, this, &UpnpDeviceModel::deviceInError);

        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::UDNChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::UPCChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::deviceTypeChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::friendlyNameChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::manufacturerChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::manufacturerURLChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::modelDescriptionChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::modelNameChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::modelNumberChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::modelURLChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::serialNumberChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpControlAbstractDevice::URLBaseChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);

        endInsertRows();

        d->mAllHostsDescription[decodedUdn]->downloadAndParseDeviceDescription(QUrl(deviceDiscovery.mLocation));
    }
}

void UpnpDeviceModel::removedDevice(const UpnpDiscoveryResult &deviceDiscovery)
{
    genericRemovedDevice(deviceDiscovery.mUSN);
}

void UpnpDeviceModel::genericRemovedDevice(const QString &usn)
{
    auto deviceIndex = d->mAllHostsUUID.indexOf(usn);
    if (deviceIndex != -1) {
        beginRemoveRows(QModelIndex(), deviceIndex, deviceIndex);
        d->mAllHostsUUID.removeAt(deviceIndex);
        d->mAllHosts.remove(usn);
        d->mAllHostsDescription.remove(usn);
        endRemoveRows();
    }
}

void UpnpDeviceModel::deviceDescriptionChanged(const QString &uuid)
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);
    Q_EMIT dataChanged(index(deviceIndex), index(deviceIndex));
}

void UpnpDeviceModel::deviceInError()
{
    if (sender()) {
        UpnpControlAbstractDevice *device = qobject_cast<UpnpControlAbstractDevice*>(sender());
        if (device) {
            genericRemovedDevice(device->UDN());
        }
    }
}

#include "moc_upnpdevicemodel.cpp"
