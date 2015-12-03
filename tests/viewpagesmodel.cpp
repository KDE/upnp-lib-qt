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

#include "viewpagesmodel.h"

#include "upnpdevicedescription.h"
#include "upnpdiscoveryresult.h"
#include "upnpdevicedescriptionparser.h"
#include "upnpcontrolcontentdirectory.h"

#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <QtCore/QScopedPointer>
#include <QtCore/QDebug>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class ViewPagesModelPrivate
{
public:

    QList<QSharedPointer<UpnpDiscoveryResult> > mAllDevices;

    QHash<QString, QSharedPointer<UpnpDiscoveryResult> > mAllDeviceDiscoveryResults;

    QHash<QString, QSharedPointer<UpnpDeviceDescription> > mAllHostsDescription;

    QHash<QString, QSharedPointer<UpnpDeviceDescriptionParser> > mDeviceDescriptionParsers;

    QList<QString> mAllHostsUUID;

    QNetworkAccessManager mNetworkAccess;
};

ViewPagesModel::ViewPagesModel(QObject *parent)
    : QAbstractListModel(parent), d(new ViewPagesModelPrivate)
{
}

ViewPagesModel::~ViewPagesModel()
{
    delete d;
}

int ViewPagesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->mAllDevices.size() + 1;
}

QVariant ViewPagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= d->mAllDevices.size() + 1) {
        return QVariant();
    }

    if (role < ColumnsRoles::NameRole || role > ColumnsRoles::UDNRole) {
        return QVariant();
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    if (index.row() < d->mAllDevices.size()) {
        switch(convertedRole)
        {
        case ColumnsRoles::NameRole:
            return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->friendlyName();
        case ColumnsRoles::UDNRole:
            return d->mAllHostsUUID[index.row()];
        }
    } else {
        switch(convertedRole)
        {
        case ColumnsRoles::NameRole:
            return QStringLiteral("Play List");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QHash<int, QByteArray> ViewPagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::NameRole)] = "name";
    roles[static_cast<int>(ColumnsRoles::UDNRole)] = "udn";

    return roles;
}

QVariant ViewPagesModel::udn(int rowIndex) const
{
    return data(index(rowIndex), ColumnsRoles::UDNRole);
}

UpnpControlContentDirectory* ViewPagesModel::service(int index) const
{
    QScopedPointer<UpnpControlContentDirectory> newService(new UpnpControlContentDirectory());

    const auto &deviceUuid = d->mAllDevices[index]->usn().mid(5, 36);
    qDebug() << "ViewPagesModel::service" << index << deviceUuid;

    const auto &realDevice = d->mAllHostsDescription[deviceUuid];

    const auto &allServices = realDevice->servicesName();
    qDebug() << allServices;
    auto serviceIndex = allServices.indexOf(QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1"));
    if (serviceIndex == -1) {
        return nullptr;
    }

    newService->setDescription(realDevice->serviceByIndex(serviceIndex).data());
    return newService.take();
}

void ViewPagesModel::newDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery)
{
    if (serviceDiscovery->nt() == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1")) {
        qDebug() << "nt" << serviceDiscovery->nt();
        qDebug() << "usn" << serviceDiscovery->usn();

        const QString &deviceUuid = serviceDiscovery->usn().mid(5, 36);
        if (!d->mAllDeviceDiscoveryResults.contains(deviceUuid)) {
            qDebug() << "ViewPagesModel::newDevice" << deviceUuid;

            //d->mAllDevices.append(serviceDiscovery);
            d->mAllDeviceDiscoveryResults[deviceUuid] = serviceDiscovery;

            const QString &decodedUdn(deviceUuid);

            d->mAllHostsUUID.push_back(decodedUdn);
            d->mAllHostsDescription[decodedUdn].reset(new UpnpDeviceDescription);
            d->mAllHostsDescription[decodedUdn]->setUDN(decodedUdn);

            connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::friendlyNameChanged, this, &ViewPagesModel::deviceDescriptionChanged);

            d->mDeviceDescriptionParsers[decodedUdn].reset(new UpnpDeviceDescriptionParser(&d->mNetworkAccess, d->mAllHostsDescription[decodedUdn]));

            connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::finishedDownload);
            connect(d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::descriptionParsed, this, &ViewPagesModel::descriptionParsed);

            d->mDeviceDescriptionParsers[decodedUdn]->downloadDeviceDescription(QUrl(serviceDiscovery->location()));
        }
    }
}

void ViewPagesModel::removedDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery)
{
    if (serviceDiscovery->nt() == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1")) {
        qDebug() << "nt" << serviceDiscovery->nt();
        qDebug() << "usn" << serviceDiscovery->usn();
    }
}

void ViewPagesModel::deviceDescriptionChanged(const QString &uuid)
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    if (deviceIndex != -1) {
        Q_EMIT dataChanged(index(deviceIndex), index(deviceIndex));
    }
}

void ViewPagesModel::descriptionParsed(const QString &UDN)
{
    qDebug() << "ViewPagesModel::descriptionParsed" << UDN;
    d->mDeviceDescriptionParsers.remove(UDN.mid(5));
    QString realUDN = UDN.mid(5);
    for (int i = 0; i < d->mAllHostsUUID.size(); ++i) {
        if (d->mAllHostsUUID[i] == realUDN) {
            beginInsertRows(QModelIndex(), d->mAllDevices.size(), d->mAllDevices.size());
            d->mAllDevices.append(d->mAllDeviceDiscoveryResults[realUDN]);
            qDebug() << "ViewPagesModel::descriptionParsed" << i;
            endInsertRows();
            break;
        }
    }
}


#include "moc_viewpagesmodel.cpp"