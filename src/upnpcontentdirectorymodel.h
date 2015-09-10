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

#ifndef UPNPCONTENTDIRECTORYMODEL_H
#define UPNPCONTENTDIRECTORYMODEL_H

#include "upnpQt_export.h"

#include <QAbstractListModel>

class UpnpContentDirectoryModelPrivate;
class UpnpSsdpEngine;
class UpnpControlAbstractDevice;
class UpnpControlContentDirectory;
struct UpnpDiscoveryResult;

class UPNPQT_EXPORT UpnpContentDirectoryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString rootObjectID
               READ rootObjectID
               WRITE setRootObjectID
               NOTIFY rootObjectIDChanged)

    Q_PROPERTY(QString browseFlag
               READ browseFlag
               WRITE setBrowseFlag
               NOTIFY browseFlagChanged)

    Q_PROPERTY(QString filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)

    Q_PROPERTY(QString sortCriteria
               READ sortCriteria
               WRITE setSortCriteria
               NOTIFY sortCriteriaChanged)

    Q_PROPERTY(UpnpControlContentDirectory* contentDirectory
               READ contentDirectory
               WRITE setContentDirectory
               NOTIFY contentDirectoryChanged)

public:

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        ClassRole = TitleRole + 1,
        CountRole = ClassRole + 1,
    };

    explicit UpnpContentDirectoryModel(QObject *parent = 0);

    virtual ~UpnpContentDirectoryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    const QString& rootObjectID() const;

    void setRootObjectID(const QString &value);

    const QString& browseFlag() const;

    void setBrowseFlag(const QString &flag);

    const QString& filter() const;

    void setFilter(const QString &flag);

    const QString& sortCriteria() const;

    void setSortCriteria(const QString &criteria);

    UpnpControlContentDirectory* contentDirectory() const;

    void setContentDirectory(UpnpControlContentDirectory *directory);

    Q_INVOKABLE void forceRefresh(const QString &objectId);

    Q_INVOKABLE QString objectIdByRow(int row) const;

Q_SIGNALS:

    void rootObjectIDChanged();

    void browseFlagChanged();

    void filterChanged();

    void sortCriteriaChanged();

    void contentDirectoryChanged();

public Q_SLOTS:

    void browseFinished(const QString &result, int numberReturned, int totalMatches, int systemUpdateID, bool success);

private Q_SLOTS:

private:

    UpnpContentDirectoryModelPrivate *d;

};

#endif // UPNPCONTENTDIRECTORYMODEL_H

