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

#include "mediaplaylist.h"

#include <QtCore/QPersistentModelIndex>
#include <QtCore/QList>
#include <QtCore/QDebug>

class MediaPlayListPrivate
{
public:

    QList<QPersistentModelIndex> mData;
};

MediaPlayList::MediaPlayList(QObject *parent) : QAbstractListModel(parent), d(new MediaPlayListPrivate)
{
}

MediaPlayList::~MediaPlayList()
{
    delete d;
}

int MediaPlayList::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->mData.size();
}

QVariant MediaPlayList::data(const QModelIndex &index, int role) const
{
    qDebug() << "MediaPlayList::data" << index << role;

    if (!index.isValid()) {
        qDebug() << "nothing 1";
        return QVariant();
    }

    if (index.row() < 0 || index.row() > d->mData.size()) {
        qDebug() << "nothing 3";
        return QVariant();
    }

    qDebug() << d->mData[index.row()].data(role);
    return d->mData[index.row()].data(role);
}

QHash<int, QByteArray> MediaPlayList::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::ItemClassRole)] = "itemClass";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";

    return roles;
}

void MediaPlayList::enqueue(const QModelIndex &newTrack)
{
    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
    d->mData.push_back(newTrack);
    endInsertRows();
}

#include "moc_mediaplaylist.cpp"
