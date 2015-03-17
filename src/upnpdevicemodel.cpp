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
#include "upnplistenner.h"
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

    QHash<QString, Upnp_Discovery> mAllHosts;

    QHash<QString, QPointer<UpnpDeviceDescription> > mAllHostsDescription;

    QList<QString> mAllHostsUUID;

    UpnpListenner *mListenner;
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
    }

    return QVariant();
}

UpnpListenner *UpnpDeviceModel::listenner() const
{
    return d->mListenner;
}

void UpnpDeviceModel::setListenner(UpnpListenner *listenner)
{
    if (d->mListenner) {
        disconnect(d->mListenner);
    }
    d->mListenner = listenner;
    connect(d->mListenner, &UpnpListenner::newService, this, &UpnpDeviceModel::newDevice);
    connect(d->mListenner, &UpnpListenner::removedService, this, &UpnpDeviceModel::removedDevice);
}

void UpnpDeviceModel::newDevice(const Upnp_Discovery &deviceDiscovery)
{
    const QString &deviceIdString(QString::fromUtf8(deviceDiscovery.DeviceId));

    if (!d->mAllHostsUUID.contains(deviceIdString)) {
        beginInsertRows(QModelIndex(), d->mAllHostsUUID.size(), d->mAllHostsUUID.size());

        d->mAllHostsUUID.push_back(deviceIdString);
        d->mAllHosts[deviceIdString] = deviceDiscovery;
        d->mAllHostsDescription[deviceIdString] = new UpnpDeviceDescription;
        d->mAllHostsDescription[deviceIdString]->setUUID(deviceIdString);

        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::UDNChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::UPCChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::deviceTypeChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::friendlyNameChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::manufacturerChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::manufacturerURLChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::modelDescriptionChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::modelNameChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::modelNumberChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::modelURLChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::serialNumberChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[deviceIdString].data(), &UpnpDeviceDescription::URLBaseChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);

        endInsertRows();

        d->mAllHostsDescription[deviceIdString]->downloadAndParseDeviceDescription(QUrl(QString::fromUtf8(deviceDiscovery.Location)));
    }
}

void UpnpDeviceModel::removedDevice(const Upnp_Discovery &deviceDiscovery)
{

}

void UpnpDeviceModel::deviceDescriptionChanged(const QString &uuid)
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);
    Q_EMIT dataChanged(index(deviceIndex), index(deviceIndex));
}

#include "moc_upnpdevicemodel.cpp"
