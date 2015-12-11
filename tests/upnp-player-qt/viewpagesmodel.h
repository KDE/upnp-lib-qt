
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

#ifndef VIEWPAGESMODEL_H
#define VIEWPAGESMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QByteArray>

class ViewPagesModelPrivate;
class UpnpDiscoveryResult;
class UpnpControlContentDirectory;
class RemoteServerEntry;

class ViewPagesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool withPlaylist
               READ withPlaylist
               WRITE setWithPlaylist
               NOTIFY withPlaylistChanged)

public:

    enum ColumnsRoles {
        NameRole = Qt::UserRole + 1,
        UDNRole,
    };
    Q_ENUM(ColumnsRoles)

    ViewPagesModel(QObject *parent = 0);

    ~ViewPagesModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE RemoteServerEntry *remoteServer(int index) const;

    void setWithPlaylist(bool value);

    bool withPlaylist() const;

public Q_SLOTS:

    void newDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery);

    void removedDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery);

private Q_SLOTS:

    void deviceDescriptionChanged(const QString &uuid);

    void descriptionParsed(const QString &UDN);

Q_SIGNALS:

    void withPlaylistChanged();

private:

    ViewPagesModelPrivate *d;

};

#endif // VIEWPAGESMODEL_H
