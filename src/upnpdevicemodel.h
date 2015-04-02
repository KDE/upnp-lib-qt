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

#ifndef UPNPDEVICEMODEL_H
#define UPNPDEVICEMODEL_H

#include <QAbstractListModel>

class UpnpDeviceModelPrivate;
class UpnpSsdpEngine;
class UpnpControlAbstractDevice;
struct UpnpDiscoveryResult;

class UpnpDeviceModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(UpnpSsdpEngine *listenner
               READ listenner
               WRITE setListenner)

public:

    enum ColumnsRoles {
        NameRole = Qt::UserRole + 1,
        TypeRole,
        uuidRole,
    };

    explicit UpnpDeviceModel(QObject *parent = 0);

    virtual ~UpnpDeviceModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    UpnpSsdpEngine* listenner() const;

    void setListenner(UpnpSsdpEngine *listenner);

    Q_INVOKABLE UpnpControlAbstractDevice* getDeviceDescription(const QString &uuid)const;

Q_SIGNALS:

public Q_SLOTS:

private Q_SLOTS:

    void newDevice(const UpnpDiscoveryResult &deviceDiscovery);

    void removedDevice(const UpnpDiscoveryResult &deviceDiscovery);

    void deviceDescriptionChanged(const QString &uuid);

    void deviceInError();

private:

    void genericRemovedDevice(const QString &usn);

    UpnpDeviceModelPrivate *d;

};

#endif // UPNPDEVICEMODEL_H
