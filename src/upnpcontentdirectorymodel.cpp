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

#include "upnpcontentdirectorymodel.h"
#include "upnpcontrolcontentdirectory.h"

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtCore/QUrl>

class UpnpContentDirectoryModelPrivate
{
public:

    UpnpControlContentDirectory *mContentDirectory;

    QString mRootObjectID;

    QString mBrowseFlag;

    QString mFilter;

    QString mSortCriteria;

    QList<QString> mDataIds;

    QHash<QString, QHash<QString, QVariant> > mData;

};

UpnpContentDirectoryModel::UpnpContentDirectoryModel(QObject *parent)
    : QAbstractListModel(parent), d(new UpnpContentDirectoryModelPrivate)
{
    d->mContentDirectory = nullptr;
}

UpnpContentDirectoryModel::~UpnpContentDirectoryModel()
{
    delete d;
}

int UpnpContentDirectoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.parent().isValid()) {
        return 0;
    } else {
        return d->mData.size();
    }
}

QHash<int, QByteArray> UpnpContentDirectoryModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::ItemClassRole)] = "itemClass";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";

    return roles;
}

Qt::ItemFlags UpnpContentDirectoryModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.column() != 0) {
        return Qt::NoItemFlags;
    }

    if (index.row() < 0 || index.row() > d->mData.size() - 1) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant UpnpContentDirectoryModel::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid()) {
        return QVariant();
    }

    if (index.column() != 0) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() > d->mData.size() - 1) {
        return QVariant();
    }

    switch(role)
    {
    case ColumnsRoles::TitleRole:
        return d->mData[d->mDataIds[index.row()]][QStringLiteral("title")];
    case ColumnsRoles::ItemClassRole:
        return d->mData[d->mDataIds[index.row()]][QStringLiteral("class")];
    case ColumnsRoles::CountRole:
        return d->mData[d->mDataIds[index.row()]][QStringLiteral("count")];
    case ColumnsRoles::DurationRole:
        return d->mData[d->mDataIds[index.row()]][QStringLiteral("duration")];
    case ColumnsRoles::RatingRole:
        return 0;
    case ColumnsRoles::ArtistRole:
        if (!d->mData[d->mDataIds[index.row()]][QStringLiteral("artist")].toString().isEmpty()) {
            return d->mData[d->mDataIds[index.row()]][QStringLiteral("artist")];
        } else {
            return d->mData[d->mDataIds[index.row()]][QStringLiteral("creator")];
        }
    case ColumnsRoles::ImageRole:
        switch (d->mData[d->mDataIds[index.row()]][QStringLiteral("class")].value<int>())
        {
        case UpnpContentDirectoryModel::Album:
            if (!d->mData[d->mDataIds[index.row()]][QStringLiteral("albumArt")].toString().isEmpty()) {
                return d->mData[d->mDataIds[index.row()]][QStringLiteral("albumArt")].toUrl();
            } else {
                return QUrl(QStringLiteral("image://icon/media-optical-audio"));
            }
        case UpnpContentDirectoryModel::Container:
            return QUrl(QStringLiteral("image://icon/folder"));
        case UpnpContentDirectoryModel::AudioTrack:
            return QUrl(QStringLiteral("image://icon/audio-x-generic"));
        }
    }

    return QVariant();
}

const QString &UpnpContentDirectoryModel::rootObjectID() const
{
    return d->mRootObjectID;
}

void UpnpContentDirectoryModel::setRootObjectID(const QString &value)
{
    d->mRootObjectID = value;
    Q_EMIT rootObjectIDChanged();
}

const QString &UpnpContentDirectoryModel::browseFlag() const
{
    return d->mBrowseFlag;
}

void UpnpContentDirectoryModel::setBrowseFlag(const QString &flag)
{
    d->mBrowseFlag = flag;
    Q_EMIT browseFlagChanged();
}

const QString &UpnpContentDirectoryModel::filter() const
{
    return d->mFilter;
}

void UpnpContentDirectoryModel::setFilter(const QString &flag)
{
    d->mFilter = flag;
    Q_EMIT filterChanged();
}

const QString &UpnpContentDirectoryModel::sortCriteria() const
{
    return d->mSortCriteria;
}

void UpnpContentDirectoryModel::setSortCriteria(const QString &criteria)
{
    d->mSortCriteria = criteria;
    Q_EMIT sortCriteriaChanged();
}

UpnpControlContentDirectory *UpnpContentDirectoryModel::contentDirectory() const
{
    return d->mContentDirectory;
}

void UpnpContentDirectoryModel::setContentDirectory(UpnpControlContentDirectory *directory)
{
    Q_ASSERT(directory);
    if (d->mContentDirectory) {
        disconnect(d->mContentDirectory, &UpnpControlContentDirectory::browseFinished, this, &UpnpContentDirectoryModel::browseFinished);
    }

    d->mContentDirectory = directory;
    Q_EMIT contentDirectoryChanged();

    connect(d->mContentDirectory, &UpnpControlContentDirectory::browseFinished, this, &UpnpContentDirectoryModel::browseFinished);
}

void UpnpContentDirectoryModel::forceRefresh(const QString &objectId)
{
    d->mRootObjectID = objectId;
    Q_EMIT rootObjectIDChanged();
    d->mContentDirectory->browse(d->mRootObjectID, d->mBrowseFlag, d->mFilter, 0, 0, d->mSortCriteria);
}

QString UpnpContentDirectoryModel::objectIdByRow(int row) const
{
    return d->mDataIds.at(row);
}

QVariant UpnpContentDirectoryModel::getUrl(int row) const
{
    return d->mData[d->mDataIds[row]][QStringLiteral("resource")];
}

void UpnpContentDirectoryModel::browseFinished(const QString &result, int numberReturned, int totalMatches, int systemUpdateID, bool success)
{
    Q_UNUSED(numberReturned)
    Q_UNUSED(totalMatches)
    Q_UNUSED(systemUpdateID)

    if (!success) {
        return;
    }

    QDomDocument browseDescription;
    browseDescription.setContent(result);

    browseDescription.documentElement();

    QList<QString> newDataIds;
    QHash<QString, QHash<QString, QVariant> > newData;
    bool isValid = false;

    auto containerList = browseDescription.elementsByTagName(QStringLiteral("container"));
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            const QString &parentID = containerNode.toElement().attribute(QStringLiteral("parentID"));

            if (parentID != d->mRootObjectID) {
                continue;
            }

            isValid = true;

            const QString &id = containerNode.toElement().attribute(QStringLiteral("id"));
            newDataIds.push_back(id);

            const QString &childCount = containerNode.toElement().attribute(QStringLiteral("childCount"));
            newData[id][QStringLiteral("count")] = childCount;

            const QDomNode &titleNode = containerNode.firstChildElement(QStringLiteral("dc:title"));
            if (!titleNode.isNull()) {
                newData[id][QStringLiteral("title")] = titleNode.toElement().text();
            }

            const QDomNode &authorNode = containerNode.firstChildElement(QStringLiteral("dc:creator"));
            if (!authorNode.isNull()) {
                newData[id][QStringLiteral("creator")] = authorNode.toElement().text();
            }

            const QDomNode &resourceNode = containerNode.firstChildElement(QStringLiteral("res"));
            if (!resourceNode.isNull()) {
                newData[id][QStringLiteral("resource")] = resourceNode.toElement().text();
            }

            const QDomNode &classNode = containerNode.firstChildElement(QStringLiteral("upnp:class"));
            if (classNode.toElement().text().startsWith(QStringLiteral("object.item.audioItem"))) {
                newData[id][QStringLiteral("class")] = UpnpContentDirectoryModel::AudioTrack;
            } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container.album"))) {
                newData[id][QStringLiteral("class")] = UpnpContentDirectoryModel::Album;
            } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container"))) {
                newData[id][QStringLiteral("class")] = UpnpContentDirectoryModel::Container;
            }

            const QDomNode &albumArtNode = containerNode.firstChildElement(QStringLiteral("upnp:albumArtURI"));
            if (!albumArtNode.isNull()) {
                newData[id][QStringLiteral("albumArt")] = albumArtNode.toElement().text();
            }

            const QDomNode &storageUsedNode = containerNode.firstChildElement(QStringLiteral("upnp:storageUsed"));
            if (!storageUsedNode.isNull()) {
            }
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            const QString &parentID = itemNode.toElement().attribute(QStringLiteral("parentID"));

            if (parentID != d->mRootObjectID) {
                continue;
            }

            isValid = true;

            const QString &id = itemNode.toElement().attribute(QStringLiteral("id"));
            newDataIds.push_back(id);

            const QString &childCount = itemNode.toElement().attribute(QStringLiteral("childCount"));
            newData[id][QStringLiteral("count")] = childCount;

            const QDomNode &titleNode = itemNode.firstChildElement(QStringLiteral("dc:title"));
            if (!titleNode.isNull()) {
                newData[id][QStringLiteral("title")] = titleNode.toElement().text();
            }

            const QDomNode &authorNode = itemNode.firstChildElement(QStringLiteral("dc:creator"));
            if (!authorNode.isNull()) {
                newData[id][QStringLiteral("creator")] = authorNode.toElement().text();
            }

            const QDomNode &resourceNode = itemNode.firstChildElement(QStringLiteral("res"));
            if (!resourceNode.isNull()) {
                newData[id][QStringLiteral("resource")] = resourceNode.toElement().text();
                if (resourceNode.attributes().contains(QStringLiteral("duration"))) {
                    const QDomNode &durationNode = resourceNode.attributes().namedItem(QStringLiteral("duration"));
                    QString durationValue = durationNode.nodeValue();
                    if (durationValue.startsWith(QStringLiteral("0:"))) {
                        durationValue = durationValue.mid(2);
                    }
                    if (durationValue.contains(uint('.'))) {
                        durationValue = durationValue.split(QStringLiteral(".")).first();
                    }

                    newData[id][QStringLiteral("duration")] = durationValue;
                }
                if (resourceNode.attributes().contains(QStringLiteral("artist"))) {
                    const QDomNode &artistNode = resourceNode.attributes().namedItem(QStringLiteral("artist"));
                    newData[id][QStringLiteral("artist")] = artistNode.nodeValue();
                }
            }

            const QDomNode &classNode = itemNode.firstChildElement(QStringLiteral("upnp:class"));
            if (!classNode.isNull()) {
                if (classNode.toElement().text().startsWith(QStringLiteral("object.item.audioItem"))) {
                    newData[id][QStringLiteral("class")] = UpnpContentDirectoryModel::AudioTrack;
                } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container.album"))) {
                    newData[id][QStringLiteral("class")] = UpnpContentDirectoryModel::Album;
                } else if (classNode.toElement().text().startsWith(QStringLiteral("object.container"))) {
                    newData[id][QStringLiteral("class")] = UpnpContentDirectoryModel::Container;
                }
            }

            const QDomNode &storageUsedNode = itemNode.firstChildElement(QStringLiteral("upnp:storageUsed"));
            if (!storageUsedNode.isNull()) {
            }
        }
    }

    if (isValid) {
        beginResetModel();
        d->mData = newData;
        d->mDataIds = newDataIds;
        endResetModel();
    }
}

#include "moc_upnpcontentdirectorymodel.cpp"
