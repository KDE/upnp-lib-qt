/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef UPNPWEBSOCKETDEVICEMODEL_H
#define UPNPWEBSOCKETDEVICEMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QSharedPointer>

class UpnpWebSocketClient;
class UpnpDeviceDescription;

class UpnpWebSocketDeviceModel : public QAbstractListModel
{

    Q_OBJECT

    Q_PROPERTY(UpnpWebSocketClient* webSocketClient
               READ webSocketClient
               WRITE setWebSocketClient
               NOTIFY webSocketClientChanged)

public:

    enum ColumnsRoles {
        NameRole = Qt::UserRole + 1,
        UDNRole,
    };
    Q_ENUM(ColumnsRoles)

    explicit UpnpWebSocketDeviceModel(QObject *parent = 0);

    virtual ~UpnpWebSocketDeviceModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void setWebSocketClient(UpnpWebSocketClient* value);

    UpnpWebSocketClient *webSocketClient() const;

Q_SIGNALS:

    void webSocketClientChanged();

public Q_SLOTS:

    void newDevice(const QString &udn);

    void removedDevice(const QString &udn);

private:

    UpnpWebSocketClient *mWebSocketClient;

    QList<QString> mListDevices;

    QMap<QString, QSharedPointer<UpnpDeviceDescription>> mDevices;

};

#endif // UPNPWEBSOCKETDEVICEMODEL_H
